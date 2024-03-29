#define _SVID_SOURCE 1

#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <jansson.h>

#define DEBUG

#include "logger.h"
//#include "debug.h"

#include "socket-handler.h"

#include "displayer_api.h"

#include "display_handler.h"
#include "keypad_handler.h"

#define DEF_PORT 6100

#define MAX_DISPLAYERS 3


/* STRUCTS */

struct matrixterminal_data {
  display_handler_t *dhandler;
  keypad_handler_t *khandler;

  display_handler_t *dhandlers[MAX_DISPLAYERS];
  displayer_t *displayers[MAX_DISPLAYERS];
};


static struct matrixterminal_data *mt_data_alloc()
{
  struct matrixterminal_data *tmp = malloc(sizeof(struct matrixterminal_data));
  if (tmp == NULL) {
    DBG("Mt_data malloc failed");
    return NULL;
  } else {
    tmp->dhandler = NULL;
    tmp->khandler = NULL;
    return tmp;
  }
}


static void mt_data_free(struct matrixterminal_data *mt_data)
{
  if (mt_data != NULL) {
    if (mt_data->dhandler != NULL) {
      display_handler_close(mt_data->dhandler);
      mt_data->dhandler = NULL;
    }
    if (mt_data->khandler != NULL) {
      keypad_handler_close(mt_data->khandler);
      mt_data->khandler = NULL;
    }
    free(mt_data);
  }
}


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

  struct matrixterminal_data *mtdata = (struct matrixterminal_data *)cb_data;

  // Verify json
  const char *cmdstr = NULL;
  json_t *cmd = NULL;
  json_t *jroot = json_loads(msg->data, 0, NULL);

  if (jroot == NULL) {
    DBG("No JSON received");
  } else {
    DBG("Valid JSON received");
    cmd = json_object_get(jroot, "command");
    cmdstr = json_string_value(cmd);
    DBG(" Command: '%s'", cmdstr);

    if (strcmp(cmdstr, "request") == 0) {
      json_t *item = json_object_get(jroot, "parameter");
      if (item != NULL) {
	const char *itemstr = json_string_value(item);
	if (strcmp(itemstr, "screen") == 0) {
	  // TODO: CB_DATA ensin kuntoon ja sieltä dh_buf_to_str()
	  char *msgtemplate = "screen: \"%s\"";
	  char *bufdata = display_handler_buf_to_str(mtdata->dhandlers[0], 1);
	  int msgstrlen = strlen(msgtemplate) + strlen(bufdata) + 1;
	  char *msgstr = malloc(msgstrlen+1);
	  snprintf(msgstr, msgstrlen, msgtemplate, bufdata);
	  message_t *msg = message_create_from(msgstr);
	  socket_handler_reply_message(sh, fd, msg);
	} else {
	  DBG("Unknown request");
	  message_t *msg = message_create_response_error("Unknown request");
	  socket_handler_reply_message(sh, fd, msg);
	}
      }
    } else if (strcmp(cmdstr, "key") == 0) {
      DBG("Key-commands not yet supported");
      message_t *msg = message_create_response_error("Key-commands not yet supported");
      socket_handler_reply_message(sh, fd, msg);
    } else {
      DBG("Command not recognized");
    }
    json_decref(jroot);
  }

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

  // DisplayHandler for MT
  //display_handler_t *dhandler = NULL;
  //keypad_handler_t *khandler = NULL;

  // Struct for MT stuff
  struct matrixterminal_data *mt_data = mt_data_alloc();
  if (mt_data == NULL)
    goto out;

  int display_fd = STDOUT_FILENO;
  int keypad_fd = STDIN_FILENO;

  // Handlers for displayers
  //display_handler_t *dhandlers[3] = {NULL, NULL, NULL};
  //displayer_t *displayers[3] = {NULL, NULL, NULL};

  socket_handler_t shandler;

  // Get opts


  // Load configuration


  /* *** Set ev */
  loop = ev_default_loop(0);
  if (!loop)
    return -1;

  // Set handlers
  mt_data->dhandler = display_handler_init(loop, display_fd);
  mt_data->khandler = keypad_handler_init(loop, keypad_fd);
  if (mt_data->dhandler == NULL || mt_data->khandler == NULL)
    return 1;

  // Set main key event handler
  keypad_handler_set_key_received_callback(mt_data->khandler, key_received_cb);

  display_handler_set_write_target(mt_data->dhandler, DISPLAY_TARGET_BUF1);
  display_handler_write_to(mt_data->dhandler, 1, 3, "MatrixTerminal2");

  display_handler_write_to(mt_data->dhandler, 3, 3, "Initializing...");
  display_handler_dump_buffer(mt_data->dhandler, 0);

  // ev_signal
  ev_signal_init(&sigint_watcher, sigint_cb, SIGINT);
  ev_signal_start(loop, &sigint_watcher);

  // Set displayers
  mt_data->dhandlers[0] = display_handler_init(loop, display_fd);
  mt_data->dhandlers[1] = NULL;
  mt_data->dhandlers[2] = NULL;

  mt_data->displayers[0] = displayer_create("weather", loop, mt_data->dhandlers[0], mt_data->khandler);
  //  displayers[0] = displayer_create("test", loop, dhandlers[0], mt_data->khandler);
  mt_data->displayers[1] = NULL;
  mt_data->displayers[2] = NULL;

  keypad_handler_set_data(mt_data->khandler, mt_data->displayers);

  DBG("Disp[0] = %ld", (long)mt_data->displayers[0]);

  // Set remote access
  DBG("Starting remote access, listening to port %d.", DEF_PORT);
  socket_handler_init(&shandler, loop);

  socket_handler_start_listen_inet(&shandler, DEF_PORT,
				   connection_accepted_cb,
				   message_received_cb,
				   message_sent_cb);
  socket_handler_set_received_cb_default_data(&shandler, (void*)mt_data);

  // Start displayers
  displayer_start(mt_data->displayers[0]);
  set_active_displayer(mt_data->displayers, mt_data->displayers[0]);

  DBG("Beginning loop");

  ev_loop(loop, 0);

  // Free stuff
 out:
  DBG("Freeing stuff");
  socket_handler_stop_listen_inet(&shandler);
  DBG("Close returned: %d", displayer_close(mt_data->displayers[0]));
  displayer_close(mt_data->displayers[1]);
  displayer_close(mt_data->displayers[2]);
  display_handler_close(mt_data->dhandlers[0]);
  display_handler_close(mt_data->dhandlers[1]);
  display_handler_close(mt_data->dhandlers[2]);

  mt_data_free(mt_data);

  return 0;
}
