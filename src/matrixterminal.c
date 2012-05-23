
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG

#include "debug.h"

#include "socket-handler.h"

#include "displayer_api.h"

#include "display_handler.h"
#include "keypad_handler.h"

#define DEF_PORT 6100

#define MAX_DISPLAYERS 3

/* LOCAL FUNCTIONS */

static void set_active_displayer(displayer_t **disps, displayer_t *active)
{
  for( int i = 0 ; i < MAX_DISPLAYERS-1 ; i++)
    displayer_deactivate(disps[i]);
  displayer_activate(active);
}

static displayer_t *get_active_displayer(displayer_t **disps)
{
  displayer_t *found = NULL;
  for( int i = 0 ; i < MAX_DISPLAYERS-1 && !found ; i++) {
    if (displayer_is_active(disps[i]))
      found = disps[i];
  }
  return found;
}


/* CALLBACK FUNCTIONS */

static void key_received_cb(keypad_handler_t *kh, char key)
{
  DBG("Got key '%c'", key);

  // Check for exclusive mt-keys first (keys forbidden from displayers)
  if (key == MTKEY_MAINMENU) {
    // Deactivate displayers and refresh main menu
  } else {
    if (0) {
      // Mt-keys
    } else {
      displayer_t *active = get_active_displayer(kh->key_callback_data);
      if (active) {
	// Send key to active displayer after mt handling
	
      }
    }
  }
}


static void connection_accepted_cb(socket_handler_t *sh, int fd)
{
  DBG("Remote connection accepted. FD: %d", fd);
}


static void message_received_cb(socket_handler_t *sh, int fd,
				message_t *msg, void *cb_data)
{
  DBG("Message received from FD %d", fd);
  DBG("  content: '%s'", msg->data);
}


static void message_sent_cb(socket_handler_t *sh, int fd,
			    void *id, void *cb_data)
{
  DBG("Message sent to FD %d", fd);
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

  display_handler_t *dhandlers[3];
  displayer_t *displayers[3];

  socket_handler_t shandler;

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

  // Set main key event handler
  keypad_handler_set_key_received_callback(khandler, key_received_cb);

  display_handler_set_write_target(dhandler, DISPLAY_TARGET_BUF1);
  display_handler_write_to(dhandler, 1, 3, "MatrixTerminal2");

  display_handler_write_to(dhandler, 3, 3, "Initializing...");
  display_handler_dump_buffer(dhandler, 0);


  // ev_signal
  ev_signal_init(&sigint_watcher, sigint_cb, SIGINT);
  ev_signal_start(loop, &sigint_watcher);


  // Set displayers
  dhandlers[0] = display_handler_init(loop, display_fd);
  dhandlers[1] = NULL;
  dhandlers[2] = NULL;

  displayers[0] = displayer_create("weather", loop, dhandlers[0], khandler);
  displayers[1] = NULL;
  displayers[2] = NULL;
  keypad_handler_set_data(khandler, displayers);

  DBG("Disp[0] = %ld", (long)displayers[0]);

  // Set remote access
  socket_handler_init(&shandler, loop);
  socket_handler_start_listen_inet(&shandler, DEF_PORT,
				   connection_accepted_cb,
				   message_received_cb,
				   message_sent_cb);

  // Start displayers
  displayer_start(displayers[0]);
  set_active_displayer(displayers, displayers[0]);

  DBG("Beginning loop");

  ev_loop(loop, 0);

  // Free stuff

  DBG("Freeing stuff");
  socket_handler_stop_listen_inet(&shandler);
  DBG("Close returned: %d", displayer_close(displayers[0]));
  displayer_close(displayers[1]);
  displayer_close(displayers[2]);
  display_handler_close(dhandlers[0]);
  display_handler_close(dhandlers[1]);
  display_handler_close(dhandlers[2]);
  keypad_handler_close(khandler);

  return 0;
}
