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

#define FORECA_UPDATE_INTERVAL 300
#define FORECA_FETCH_URL "http://www.foreca.fi"
#define FORECA_READ_BYTES 10000

#define READ 0
#define WRITE 1


int displayer_initialize(displayer_t *disp);
int displayer_start(displayer_t *disp);
int displayer_stop(displayer_t *disp);
int displayer_key_callback(displayer_t *disp, char key);


struct forecast {
  int wind_amount;
  char wind_direction[2];
  int temperature;
  struct forecast *next;
};

struct foreca_raw_data {
  ev_io data_fetcher;
  char *data;
  int len;
};

struct foreca_data {
  struct forecast **forecasts;
  int tested;
  struct foreca_raw_data raw_data;
};


//static void parse_forecasts(char *data);


static void foreca_raw_read(struct ev_loop *l, ev_io *w, int revents)
{
  struct foreca_raw_data *raw = (struct foreca_raw_data *)w;
  ev_io_stop(l, w);
  DBG("In raw_read");
  char rdata[FORECA_READ_BYTES+1];
  int r;

  rdata[FORECA_READ_BYTES] = 0;

  DBG("Reading %d bytes...", FORECA_READ_BYTES);

  r = read(w->fd, rdata, FORECA_READ_BYTES);
  DBG("Starting while loop, r=%d", r);

  while (r > 0) {
    char *ptr = realloc(raw->data, raw->len+r);
    if (ptr) {
      raw->data = ptr;
      memcpy(&raw->data[raw->len], rdata, r);
      raw->len += r;
      //DBG("Read: %s", rdata);
    }
    r = read(w->fd, rdata, FORECA_READ_BYTES);
    DBG("In while loop, r=%d", r);
  }

  if (r <= 0) {
    /*if (errno == EAGAIN) {
      DBG("Error: %s", strerror(errno));
      DBG("Restarting watcher");
      ev_io_start(l, w);
      } else {*/
    DBG("Closing fd");
    close(w->fd);
    //}
  }
}


static void set_nonblock(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


static void foreca_updater(struct ev_loop *l, ev_timer *w, int revents)
{
  displayer_t* disp = (displayer_t *)w;
  struct foreca_data *data = (struct foreca_data*)disp->data;

  ev_timer_stop(l, w);

  DBG("Starting updater function");

  pid_t pid;
  int piperead[2];
  //int pipewrite[2];
  pipe(piperead);
  //pipe(pipewrite);

  DBG("Forking");
  pid = fork();

  if (pid == 0) { // Child
    char *cmd[4];
    cmd[0] = "/usr/bin/curl";
    cmd[1] = "-q";
    cmd[2] = "www.foreca.fi";
    cmd[3] = NULL;
    dup2(piperead[WRITE], STDOUT_FILENO);
    close(piperead[READ]);
    close(piperead[WRITE]);

    //DBG("Child executing");
    execv(cmd[0], cmd);
    DBG("Execv exited, (%s), exiting...", strerror(errno));
    exit(0);
  } else if (pid > 0) { // Parent
    set_nonblock(piperead[READ]);

    if (data->raw_data.data) {
      free(data->raw_data.data);
      data->raw_data.data = NULL;
      data->raw_data.len = 0;
    }

    DBG("Starting io_watcher for raw read");
    ev_io_init(&data->raw_data.data_fetcher, foreca_raw_read,
	       piperead[READ], EV_READ);
    sleep(1);
    ev_io_start(l, &data->raw_data.data_fetcher);
  } else { // Error

  }

}


int displayer_initialize(displayer_t *disp)
{
  DBG("Initing foreca displayer");
  ev_timer_init(&disp->disp_watcher, foreca_updater,
		2, FORECA_UPDATE_INTERVAL);

  if (disp->data)
    free(disp->data);
  disp->data = malloc(sizeof(struct foreca_data));

  if (!disp->data)
    return -1;

  bzero(disp->data, sizeof(struct foreca_data));

  //((struct test_disp_data*)(disp->data))->counter = 0;
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
