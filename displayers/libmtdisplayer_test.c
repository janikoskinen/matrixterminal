#include "displayer_api.h"

#define DEBUG

#include "debug.h"

#define TIMER_TIME 1.0
#define TIMER_REPEAT 1.0


int displayer_initialize(displayer_t *disp);
int displayer_start(displayer_t *disp);
int displayer_stop(displayer_t *disp);


static void test_runner(struct ev_loop *l, ev_timer *w, int revents)
{
  DBG("Test timer...");
}


int displayer_initialize(displayer_t *disp)
{
  DBG("Initing test displayer");
  ev_timer_init(&disp->disp_watcher, test_runner, TIMER_TIME, TIMER_REPEAT);
  return -1;
}


int displayer_start(displayer_t *disp)
{
  DBG("Start test displayer");
  if ((disp->status & DISPLAYER_STATUS_RUNNING) != 0) {
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
  if ((disp->status & DISPLAYER_STATUS_RUNNING) == 0) {
    return -1;
  } else {
    disp->status &= !DISPLAYER_STATUS_RUNNING;
    return 0;
  }
}
