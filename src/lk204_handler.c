#ifndef _LK204_HANDLER_H
#define _LK204_HANDLER_H

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG
#include "debug.h"

#include "display_handler.h"
#include "keypad_handler.h"



/* General functions */

static void set_nonblock(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    DBG("Error when setting fd %d to nonblock mode", fd);
}


/* *** Display api routines *** */

display_handler_t *display_handler_init(struct ev_loop *loop, int disp_fd) {
  display_handler_t *dh = NULL;

  dh = malloc(sizeof(display_handler_t));

  if (!dh)
    return NULL;

  set_nonblock(disp_fd);
  dh->disp_fd = disp_fd;
  dh->scrollers = NULL;
  dh->loop = loop;

  return dh;
}



/*
*** Keypad local routines ***
*/

static void device_read_cb(struct ev_loop *l, ev_io *w, int revents)
{
  int count;
  char key;

  ev_io_stop(l, w);

  keypad_handler_t *kh = (keypad_handler_t *)w;

  count = read(w->fd, &key, 1);
  DBG("Got count %d", count);

  if (count == 1) {
    if (kh->key_callback)
      kh->key_callback(kh, key);
  } else {
    DBG("Read error");
  }

  ev_io_start(l, w);
}


/*
*** Keypad api routines ***
*/


keypad_handler_t *keypad_handler_init(struct ev_loop *loop, int dev_fd)
{
  keypad_handler_t *kh = NULL;

  if (!loop || dev_fd < 0)
    return NULL;

  kh = malloc(sizeof(keypad_handler_t));

  DBG("Got fd %d", dev_fd);

  if (!kh)
    return NULL;

  set_nonblock(dev_fd);
  kh->dev_fd = dev_fd;
  kh->key_callback = NULL;
  kh->loop = loop;

  ev_io_init(&kh->dev_watcher, device_read_cb, dev_fd, EV_READ);
  ev_io_start(kh->loop, &kh->dev_watcher);
  DBG("Keypad init complete");
  return kh;
}


int keypad_handler_set_key_received_callback(keypad_handler_t *kh,
					     key_received_callback_t callback)
{
  DBG("Setting callback");
  kh->key_callback = callback;
  return 0;
}

int keypad_handler_get_column(char key)
{ return -1; }

int keypad_handler_get_row(char key)
{ return -1; }

int keypad_handler_key_is_down(char key)
{ return -1; }

int keypad_handler_key_is_up(char key)
{ return -1; }



#endif
