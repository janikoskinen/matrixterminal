
#define _GNU_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG
#include "debug.h"

#include "mt2lk_defs.h"
#include "display_handler.h"
#include "keypad_handler.h"



/* General functions */

static void set_nonblock(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    DBG("Error when setting fd %d to nonblock mode", fd);
}

static int min(int a, int b)
{
  if (a > b)
    return b;
  else
    return a;
}



/*
************************************************************
*                                                          *
*                       DISPLAY                            *
*                                                          *
************************************************************
*/


/*
*** Display local routines ***
*/



/*
*** Display api routines ***
*/

display_handler_t *display_handler_init(struct ev_loop *loop, int disp_fd)
{
  display_handler_t *dh = NULL;
  int i;

  dh = malloc(sizeof(display_handler_t));

  if (!dh)
    return NULL;

  set_nonblock(disp_fd);
  dh->disp_fd = disp_fd;
  dh->scrollers = NULL;
  dh->loop = loop;

  dh->write_targets = DISPLAY_TARGET_BUF1;
  dh->gpo_value = 0;

  // Clear buffers;
  for (i = 0 ; i < 4 ; i++)
    snprintf(dh->buffer[0].data[i], 21, "                    ");
  for (i = 0 ; i < 4 ; i++)
    snprintf(dh->buffer[1].data[i], 21, "                    ");

  return dh;
}


void display_handler_close(display_handler_t *dhandler)
{

}


int display_handler_set_write_target(display_handler_t *dhandler,
				     int targets) {
  if (targets < 0 || targets > 7)
    return -1;
  else {
    dhandler->write_targets = targets;
    return 0;
  }
}


int display_handler_write(display_handler_t *dhandler, char *str)
{
  int target = 0;

  if (!dhandler)
    return -1;

  // Write to screen -> Not supported (TODO)
  if (dhandler->write_targets & DISPLAY_TARGET_SCREEN) {
    return -2;
  }

  // Buffers here, TODO: in loop

  if ((dhandler->write_targets >> (target+1)) & 1) {
    int str_pos = 0;
    int str_len = strlen(str);
    int to_write = 0;

    while (str_pos < str_len) {
      to_write = min((str_len-str_pos), 21-dhandler->buffer[target].column);

      memcpy(&dhandler->buffer[target].data[dhandler->buffer[target].row-1][dhandler->buffer[target].column-1], &str[str_pos], to_write);

      str_pos += to_write;
      dhandler->buffer[target].column += to_write;

      if (dhandler->buffer[target].column > 20) {
	dhandler->buffer[target].column = 1;
	dhandler->buffer[target].row++;
      }
      if (dhandler->buffer[target].row > 4)
	dhandler->buffer[target].row = 1;
    }
  }
  return 0;
}


int display_handler_write_to(display_handler_t *dhandler,
			     int row, int column, char *str)
{
  int ret = -1;

  if (!dhandler)
    return ret;

  if (display_handler_go_to(dhandler, row, column) == 0) {
    ret = display_handler_write(dhandler, str);
  }
  return ret;
}


int display_handler_go_to(display_handler_t *dhandler, int row, int column)
{
  int ret = 0;

  if (row < 1 || row > 4 || column < 1 || column > 20)
    return -1;

  char *cmd = NULL;

  if (dhandler->write_targets & DISPLAY_TARGET_SCREEN) {
    if (asprintf(&cmd, LKCMD_SET_POS, row, column) < 0)
      ret = -1;
  }

  // TODO: buf loop
  if (dhandler->write_targets & DISPLAY_TARGET_BUF1) {
    dhandler->buffer[0].row = row;
    dhandler->buffer[0].column = column;
  }

  return ret;
}



void display_handler_dump_buffer(display_handler_t *dhandler, int page)
{
  if (page < 0 || page > 1) {
    DBG("Error");
  } else {
    DBG("+--------------------+");
    DBG("|%s|", dhandler->buffer[page].data[0]);
    DBG("|%s|", dhandler->buffer[page].data[1]);
    DBG("|%s|", dhandler->buffer[page].data[2]);
    DBG("|%s|", dhandler->buffer[page].data[3]);
    DBG("+--------------------+");
    DBG("Row: %d, col: %d",
	dhandler->buffer[page].row,
	dhandler->buffer[page].column);
  }
}

/*
************************************************************
*                                                          *
*                        KEYPAD                            *
*                                                          *
************************************************************
*/

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


