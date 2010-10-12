
#include <ev.h>
#include <stdio.h>

#define DEBUG

#include "debug.h"


static void sigint_cb(struct ev_loop *l, ev_signal *w, int revents)
{
  DBG("\nSigInt received, exiting");
  ev_unloop(l, EVUNLOOP_ALL);
}

int main (int argc, char **argv)
{

  struct ev_loop *loop = NULL;
  ev_signal sigint_watcher;


  // Get opts


  // Load configuration


  /* *** Set ev */
  loop = ev_default_loop(0);

  // ev_signal
  ev_signal_init(&sigint_watcher, sigint_cb, SIGINT);
  ev_signal_start(loop, &sigint_watcher);

  ev_loop(loop, 0);

  return 0;
}
