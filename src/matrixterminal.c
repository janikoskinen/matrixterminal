
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG

#include "debug.h"

#include "displayer_api.h"

#include "display_handler.h"
#include "keypad_handler.h"



/* CALLBACK FUNCTIONS */

static void key_received_cb(keypad_handler_t *kh, char key, int downtime)
{
  DBG("Got key '%c'", key);
}



static void sigint_cb(struct ev_loop *l, ev_signal *w, int revents)
{
  DBG("\nSigInt received, exiting");
  ev_unloop(l, EVUNLOOP_ALL);
}


/* MAIN */

int main (int argc, char **argv)
{

  struct ev_loop *loop = NULL;
  ev_signal sigint_watcher;

  display_handler_t *dhandler;
  keypad_handler_t *khandler;

  int display_fd = STDOUT_FILENO;
  int keypad_fd = STDIN_FILENO;

  // Get opts


  // Load configuration


  /* *** Set ev */
  loop = ev_default_loop(0);
  if (!loop)
    return -1;

  // Set handlers
  dhandler = display_handler_init(loop, display_fd);
  khandler = keypad_handler_init(loop, keypad_fd);
  if (!dhandler || !khandler)
    return 1;

  keypad_handler_set_key_received_callback(khandler, key_received_cb);

  display_handler_set_write_target(dhandler, DISPLAY_TARGET_BUF1);
  display_handler_write_to(dhandler, 1, 3, "MatrixTerminal2");

  display_handler_write_to(dhandler, 3, 3, "Initializing...");
  display_handler_dump_buffer(dhandler, 0);


  // ev_signal
  ev_signal_init(&sigint_watcher, sigint_cb, SIGINT);
  ev_signal_start(loop, &sigint_watcher);


  // Set displayers
  displayer_t* displayers[3];

  displayers[0] = displayer_create("test", loop,
				   display_handler_init(loop, display_fd),
				   khandler);
  displayers[1] = NULL;
  displayers[2] = NULL;

  DBG("Disp[0] = %ld", (long)displayers[0]);

  // Start displayers
  displayer_start(displayers[0]);


  DBG("Beginning loop");

  ev_loop(loop, 0);

  // Free stuff

  DBG("Freeing stuff");
  displayer_close(displayers[0]);
  displayer_close(displayers[1]);
  displayer_close(displayers[2]);

  return 0;
}
