#include <stdlib.h>

#include "displayer_api.h"

#define DEBUG

#include "debug.h"

#define TIMER_TIME 1.0
#define TIMER_REPEAT 1.0


int displayer_initialize(displayer_t *disp);
int displayer_start(displayer_t *disp);
int displayer_stop(displayer_t *disp);
int displayer_key_callback(displayer_t *disp, char key);


struct test_disp_data {
  int counter;
};


static void test_runner(struct ev_loop *l, ev_timer *w, int revents)
{
  displayer_t* disp = (displayer_t *)w;
  struct test_disp_data *data = (struct test_disp_data*)disp->data;
  data->counter++;
  DBG("Test timer: %d", data->counter);
}


int displayer_initialize(displayer_t *disp)
{
  DBG("Initing test displayer");
  ev_timer_init(&disp->disp_watcher, test_runner, TIMER_TIME, TIMER_REPEAT);

  if (disp->data)
    free(disp->data);
  disp->data = malloc(sizeof(struct test_disp_data));

  if (!disp->data)
    return -1;

  ((struct test_disp_data*)(disp->data))->counter = 0;
  return 0;
}


int displayer_deinitialize(displayer_t *disp)
{
  // Nothing to do
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
