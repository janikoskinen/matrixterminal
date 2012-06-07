#include <curl/curl.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "displayer_api.h"

#define DEBUG

#include "debug.h"

#define WEATHER_UPDATE_INTERVAL 30
//#define WEATHER_FETCH_URL "http://printer.wunderground.com/cgi-bin/findweather/getForecast?query=turku"
#define WEATHER_FETCH_URL "http://www.foreca.fi/Finland/Turku/tenday"
#define WEATHER_READ_BYTES 5000

#define READ 0
#define WRITE 1


int displayer_key_callback(displayer_t *disp, char key);


struct forecast_data {
  int wind_amount;
  char wind_direction[3];
  int temperature_day;
  int temperature_night;
  char *status;
  struct forecast_data *next;
};

struct weather_raw_data {
  ev_io data_fetcher;
  char *data;
  int len;
};

struct weather_data {
  struct weather_raw_data raw_data;
  struct forecast_data *forecasts;
  displayer_t *disp;
};


static void free_forecasts(struct forecast_data *fdata)
{
  if (fdata == NULL)
    return;

  free_forecasts(fdata->next);
  if (fdata->status != NULL)
    free(fdata->status);
  free(fdata);
}


static struct forecast_data *alloc_forecast()
{
  struct forecast_data *fdata = malloc(sizeof(struct forecast_data));

  if (fdata == NULL) {
    DBG("Malloc error");
    return NULL;
  } else {
    fdata->wind_amount = 0;
    fdata->wind_direction[0] = ' ';
    fdata->wind_direction[1] = ' ';
    fdata->wind_direction[2] = '\0';
    fdata->temperature_day = 0;
    fdata->temperature_night = 0;
    fdata->status = NULL;
    fdata->next = NULL;
    return fdata;
  }
}


static void forecast_debug_print(struct forecast_data *fdata)
{
  DBG("Forecast data (%ld):", (long)fdata);
  if (fdata != NULL) {
    DBG(" Status: %s", fdata->status);
    DBG(" DayTmp: %d", fdata->temperature_day);
    DBG(" NgtTmp: %d", fdata->temperature_night);
    DBG(" WndAmn: %d", fdata->wind_amount);
    DBG(" WndDir: %s", fdata->wind_direction);
    DBG("   Next: %ld", (long)fdata->next);
  }
}


static struct forecast_data *parse_forecasts(char *data)
{
  DBG("In parse forecast");
  //DBG("Data received ****:\n%s", data);
  //DBG("*** End of data ***");

  char *a, *b;
  int datacount = strlen(data);
  //DBG("Datacount: %d", datacount);
  int day, tmp;
  struct forecast_data* fdata = NULL;
  //  struct forecast_data* ftemp = NULL;

  // Check for errors
  int errors = 0;
  if (strstr(data, "Execvp exited") != NULL) {
    if (strstr(data, "No such file") != NULL)
      errors = 1;
  }
  if (strstr(data, "Unable to") != NULL &&
      strstr(data, "remote host") != NULL) {
    errors = 2;
  }

  if (datacount < 512) {
    switch(errors) {
    case 1:
      DBG("Not enough data and Execvp exited: Lynx installed??");
      goto error;
    case 2:
      DBG("Cannot find server");
      goto error;
    default:
      DBG("Little data received without apparent reason!");
      break;
    }
  }

  if (errors == 0) {
    a = strstr(data, "Turku\n\n10");
    if (a == NULL) {
      errors = 4;
      goto error;
    }
    a = strstr(a, " ennuste");
    if (a == NULL) {
      errors = 4;
      goto error;
    }
  }

  DBG("Valid data received");
  a = &a[10];

  fdata = alloc_forecast();
  if (fdata == NULL) {
    errors = 8;
    DBG("Could not allocate memory for forecast data");
    goto error;
  }

  for (day = 0 ; day < 1 ; day++) {

    DBG("Day %d:", day);
    // Day and cloud status
    a = strstr(a, "]") + 1;
    b = strstr(&a[1], "\n");
    *b = '\0';
    fdata->status = malloc(strlen(a)+1);
    if (fdata->status == NULL) {
      DBG("Malloc error when allocing status string");
    } else {
      strcpy(fdata->status, a);
    }
    DBG("Status: %s", a);

    // Day temperature
    a = &b[13];
    b = strstr(&a[1], "\xc2");
    *b = '\0';
    tmp = strtol(a, NULL, 10);
    b = strstr(&b[1], "\n");
    fdata->temperature_day = tmp;
    DBG("Day temp: %d", tmp);

    // Night temperature
    a = &b[9];
    b = strstr(&a[1], "\xc2");
    *b = '\0';
    tmp = strtol(a, NULL, 10);
    b = strstr(&b[1], "\n");
    fdata->temperature_night = tmp;
    DBG("Ngt temp: %d", tmp);

    // Wind
    a = skip_whites(&b[1], 20);
    //a = &b[4];
    b = strstr(&a[1], " ");
    *b = '\0';
    fdata->wind_direction[0] = a[0];
    fdata->wind_direction[1] = a[1];
    fdata->wind_direction[2] = '\0';
    DBG("Wind dir: %s", a);
    a = &b[1];
    b = strstr(&a[1], " ");
    *b = '\0';
    tmp = strtol(a, NULL, 10);
    fdata->wind_amount = tmp;
    DBG("Wind amt: %d", tmp);
  }

  forecast_debug_print(fdata);

  return fdata;

 error:
  free_forecasts(fdata);
  return NULL;
}


static void weather_raw_read(struct ev_loop *l, ev_io *w, int revents)
{
  struct forecast_data *fdata;
  struct weather_data *wdata;
  char rdata[WEATHER_READ_BYTES+1];
  int r;
  char *ptr;
  struct weather_raw_data *raw = (struct weather_raw_data *)w;

  ev_io_stop(l, w);
  //DBG("In raw_read, fd=%d", w->fd);

  rdata[WEATHER_READ_BYTES] = 0;

  //DBG("Reading %d bytes...", WEATHER_READ_BYTES);

  r = read(w->fd, rdata, WEATHER_READ_BYTES);
  //DBG("  got: %d", r);

  if (r > 0) {
    ptr = realloc(raw->data, raw->len+r+1);
    if (ptr) {
      raw->data = ptr;
      strncpy(&raw->data[raw->len], rdata, r);
      raw->len += r;
    }
  }

  if (r == WEATHER_READ_BYTES) {
    ev_io_start(l, w);
  } else {
    if (r > 0) {
      DBG("All hopefully received");
      close(w->fd);
      // Parse received data
      fdata = parse_forecasts(raw->data);
      if (fdata) {
	wdata = (struct weather_data *)w;
	if (wdata->forecasts)
	  free_forecasts(wdata->forecasts);
	wdata->forecasts = fdata;
      }
    } else {
      DBG("Error: %s", strerror(errno));
    }
  }
}


static void set_nonblock(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


static void weather_updater(struct ev_loop *l, ev_timer *w, int revents)
{
  displayer_t* disp = (displayer_t *)w;
  struct weather_data *data = (struct weather_data*)disp->data;

  ev_timer_stop(l, w);

  DBG("Starting updater function");

  pid_t pid;
  int piperead[2];
  pipe(piperead);

  //DBG("Forking");
  pid = fork();

  if (pid == 0) { // Child
    char *cmd[4];
    cmd[0] = "lynx";
    cmd[1] = "-dump";
    cmd[2] = WEATHER_FETCH_URL;
    cmd[3] = NULL;
    dup2(piperead[WRITE], STDOUT_FILENO);
    close(piperead[READ]);
    close(piperead[WRITE]);

    DBG("Child executing");
    execvp(cmd[0], cmd);
    DBG("Execvp exited, (%s), exiting...", strerror(errno));
    exit(0);
  } else if (pid > 0) { // Parent
    set_nonblock(piperead[READ]);

    if (data->raw_data.data) {
      free(data->raw_data.data);
      data->raw_data.data = NULL;
      data->raw_data.len = 0;
    }

    DBG("Starting io_watcher for raw read");
    ev_io_init(&data->raw_data.data_fetcher, weather_raw_read,
	       piperead[READ], EV_READ);
    sleep(1);
    ev_io_start(l, &data->raw_data.data_fetcher);
  } else { // Error
    DBG("Error in fork!");
  }

  ev_timer_start(l, w);
}


int displayer_initialize(displayer_t *disp)
{
  DBG("Initing foreca displayer");
  ev_timer_init(&disp->disp_watcher, weather_updater,
		2, WEATHER_UPDATE_INTERVAL);

  if (disp->data)
    free(disp->data);
  disp->data = malloc(sizeof(struct weather_data));

  if (!disp->data)
    return -1;

  bzero(disp->data, sizeof(struct weather_data));

  ((struct weather_data*)(disp->data))->disp = disp;
  return 0;
}


int displayer_deinitialize(displayer_t *disp)
{
  if (disp->data) {
    struct weather_data *tmp = (struct weather_data*)disp->data;
    if (tmp->raw_data.data != NULL) {
      DBG("Free rawdata string");
      free(tmp->raw_data.data);
      tmp->raw_data.data = NULL;
    }
  }
  return 0;
}


int displayer_start(displayer_t *disp)
{
  DBG("Start test displayer");
  if (displayer_is_running(disp)) {
    return -1;
  } else {
    DBG("Calling ev_timer_start");
    ev_timer_start(disp->loop, &disp->disp_watcher);
    DBG("Setting status");
    disp->status |= DISPLAYER_STATUS_RUNNING;
    return 0;
  }
}


int displayer_stop(displayer_t *disp)
{
  if (!displayer_is_running(disp)) {
    return -1;
  } else {
    disp->status &= ~DISPLAYER_STATUS_RUNNING;
    return 0;
  }
}
