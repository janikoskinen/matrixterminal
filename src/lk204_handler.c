
#define _GNU_SOURCE

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG
#include "debug.h"

#include "mt2lk_defs.h"
#include "display_handler.h"
#include "keypad_handler.h"


/* TODO list
   - all buffer operation to loop
   - page aomunt can be specified (in init)

*/


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


static int write_to_lk(int fd, const char *cmd, ...)
{
  char *cmdstr = NULL;
  va_list ap;
  int r, w;

  va_start(ap, cmd);
  r = vasprintf(&cmdstr, cmd, ap);
  va_end(ap);

  if (r < 0)
    return -1;

  // TODO: to ev_write ?
  w = 0;
  while (w < strlen(cmdstr)) {
    w += write(fd, &cmdstr[w], strlen(cmdstr)-w);
  }
  return 0;
}


static void display_scroller_cb(struct ev_loop *l, ev_timer *w, int revents)
{
  char *tmp = NULL;
  struct display_scroller *ds = (struct display_scroller *)w;

  tmp = malloc(ds->width+1);
  if (tmp) {
    if (ds->width > strlen(&ds->text[ds->pos])) { // Do in one or two parts
      int w = sprintf(tmp, "%s", &ds->text[ds->pos]);
      snprintf(&tmp[w], ds->width-w+1, "%s", ds->text);
    } else {
      if (snprintf(tmp, ds->width+1, "%s", &ds->text[ds->pos]) < 0)
	goto out;
    }

    if (display_handler_write_to(ds->dhandler, ds->row, ds->column, tmp) == 0)
      ds->pos++;
  } else {
    DBG("Memory error when scrolling!");
  }

  if (ds->pos > strlen(ds->text))
    ds->pos -= strlen(ds->text);

  display_handler_dump_buffer(ds->dhandler, 0);

 out:
  if (tmp)
    free(tmp);
}


/*
************************************************************
*                                                          *
*                       Display                            *
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
  dh->loop = loop;
  for (i = 0 ; i < DISPLAY_MAX_SCROLLERS ; i++) {
    dh->scrollers[i].dhandler = dh;
    dh->scrollers[i].text = NULL;
    dh->scrollers[i].running = 0;
  }

  dh->write_targets = DISPLAY_TARGET_BUF1;
  dh->gpo_value = 0;
  dh->bars_initialized = DISPLAY_BARS_NONE;

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
				     int targets_mask) {
  if (targets_mask < 0 || targets_mask > 7)
    return -1;
  else {
    dhandler->write_targets = targets_mask;
    return 0;
  }
}


int display_handler_write(display_handler_t *dhandler, char *str)
{
  int target = 0;

  if (!dhandler)
    return -1;

  // Write to screen
  if (dhandler->write_targets & DISPLAY_TARGET_SCREEN) {
    write_to_lk(dhandler->disp_fd, str);
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

  if (!dhandler)
    return -1;

  if (row < 1 || row > 4 || column < 1 || column > 20)
    return -1;

  if (dhandler->write_targets & DISPLAY_TARGET_SCREEN) {
    ret |= write_to_lk(dhandler->disp_fd, LKCMD_SET_POS, row, column);
  }

  // TODO: buf loop
  if (dhandler->write_targets & DISPLAY_TARGET_BUF1) {
    dhandler->buffer[0].row = row;
    dhandler->buffer[0].column = column;
  }

  return ret;
}


int display_handler_go_home(display_handler_t *dhandler)
{
  return display_handler_go_to(dhandler, 1, 1);
}


int display_handler_clear_display(display_handler_t *dhandler)
{
  int ret = 0;

  if (!dhandler)
    return -1;

  if (dhandler->write_targets & DISPLAY_TARGET_SCREEN) {
    ret = write_to_lk(dhandler->disp_fd, LKCMD_CLR_SRC);
  }

  if (dhandler->write_targets & DISPLAY_TARGET_BUF1) {
    for (int i = 1 ; i <= 4 ; i++)
      ret |= display_handler_clear_line(dhandler, i);
    ret |= display_handler_go_home(dhandler);
  }

  if (ret != 0)
    return -1;
  return 0;
}


int display_handler_clear_line(display_handler_t *dhandler, int row)
{
  int ret = 0;

  if (!dhandler)
    return -1;

  ret |= display_handler_go_to(dhandler, row, 1);
  ret |= display_handler_write(dhandler, "                    ");

  if (ret != 0)
    return -1;
  return 0;
}


int display_handler_scroller_init(display_handler_t *dhandler,
				  int scroller,
				  int row,
				  int column,
				  int width,
				  char *scroll_text,
				  int interval_msecs)
{
  float repeat_time = 0.0;

  if (!dhandler || scroller < 0 || scroller > DISPLAY_MAX_SCROLLERS)
    return -1;

  // TODO: check params
  if ((strlen(scroll_text) < width) ||
      (row < 1 || row > 4 || column < 1 || column > 20) ||
      (width < 1 || interval_msecs < 10))
    return 1;

  if (dhandler->scrollers[scroller].running)
    display_handler_scroller_close(dhandler, scroller);

  repeat_time = interval_msecs / 1000.0;
  dhandler->scrollers[scroller].pos = 0;
  dhandler->scrollers[scroller].row = row;
  dhandler->scrollers[scroller].column = column;
  dhandler->scrollers[scroller].width = width;
  dhandler->scrollers[scroller].running = 0;
  dhandler->scrollers[scroller].text = strdup(scroll_text);
  ev_timer_init(&dhandler->scrollers[scroller].watcher,
		display_scroller_cb, 0, repeat_time);
  return 0;
}


int display_handler_scroller_close(display_handler_t *dhandler, int scroller)
{
  if (!dhandler || scroller < 0 || scroller > DISPLAY_MAX_SCROLLERS)
    return -1;

  if (dhandler->scrollers[scroller].running) {
    display_handler_scroller_stop(dhandler, scroller);

    if (dhandler->scrollers[scroller].text) {
      free(dhandler->scrollers[scroller].text);
      dhandler->scrollers[scroller].text = NULL;
    }
  }
  return 0;
}


int display_handler_scroller_start(display_handler_t *dhandler, int scroller)
{
  if (!dhandler || scroller < 0 || scroller > DISPLAY_MAX_SCROLLERS)
    return -1;

  if (dhandler->scrollers[scroller].text == NULL ||
      dhandler->scrollers[scroller].running != 0)
    return -1;

  ev_timer_start(dhandler->loop, &dhandler->scrollers[scroller].watcher);
  return 0;
}


int display_handler_scroller_stop(display_handler_t *dhandler, int scroller)
{
  if (!dhandler || scroller < 0 || scroller > DISPLAY_MAX_SCROLLERS)
    return -1;

  if (dhandler->scrollers[scroller].text == NULL ||
      dhandler->scrollers[scroller].running == 0)
    return -1;

  ev_timer_stop(dhandler->loop, &dhandler->scrollers[scroller].watcher);
  return 0;
}


int display_handler_write_gpo(display_handler_t *dhandler, int new_gpo_value)
{
  int ret = 0;

  if (!dhandler)
    return -1;

  if (dhandler->write_targets & DISPLAY_TARGET_SCREEN) {
    dhandler->gpo_value = new_gpo_value;
    for (int i = 0 ; i < 6 ; i++) {
      if ((new_gpo_value >> i) & 1)
	ret |= display_handler_set_gpo(dhandler, i+1);
      else
	ret |= display_handler_reset_gpo(dhandler, i+1);
    }
    if (ret)
      return -1;
  } else {
    return -1;
  }
  return 0;
}


int display_handler_set_gpo(display_handler_t *dhandler, int ngpo)
{
  if (!dhandler || ngpo < 1 || ngpo > 6)
    return -1;

  return write_to_lk(dhandler->disp_fd, LKCMD_GPO_ON, ngpo);
}


int display_handler_reset_gpo(display_handler_t *dhandler, int ngpo)
{
  if (!dhandler || ngpo < 1 || ngpo > 6)
    return -1;

  return write_to_lk(dhandler->disp_fd, LKCMD_GPO_OFF, ngpo);
}


int display_handler_use_bars(display_handler_t *dhandler, int type)
{
  int ret = -1;

  if (!dhandler || type < 0 || type > 4)
    return -1;

  switch (type) {
  case DISPLAY_BARS_HRIGHT:
  case DISPLAY_BARS_HLEFT:
    ret = write_to_lk(dhandler->disp_fd, LKCMD_HBAR_INIT);
    break;
  case DISPLAY_BARS_VTHICK:
    ret = write_to_lk(dhandler->disp_fd, LKCMD_VBAR_INIT);
    break;
  case DISPLAY_BARS_VTHIN:
    ret = write_to_lk(dhandler->disp_fd, LKCMD_VBAR2_INIT);
    break;
  default:
    break;
  }
  dhandler->bars_initialized = type;

  return ret;
}


int display_handler_draw_bar_horizontal(display_handler_t *dhandler,
					int row, int column, int length)
{
  if (!dhandler || row < 1 || row > 4 || column < 1 || column > 20
      || length < 0 || length > 20)
    return -1;

  if (dhandler->bars_initialized != DISPLAY_BARS_HLEFT &&
      dhandler->bars_initialized != DISPLAY_BARS_HRIGHT)
    display_handler_use_bars(dhandler, DISPLAY_BARS_H);

  if (dhandler->bars_initialized == DISPLAY_BARS_HRIGHT)
    return write_to_lk(dhandler->disp_fd,
		       LKCMD_HBAR_DRAW, column, row, 0, length);
  else
    return write_to_lk(dhandler->disp_fd,
		       LKCMD_HBAR_DRAW, column, row, 1, length);
  return 0;
}


int display_handler_draw_bar_vertical(display_handler_t *dhandler,
				      int column, int height)
{
  if (!dhandler || column < 1 || column > 20 || height < 0 || height > 20)
    return -1;

  if (dhandler->bars_initialized != DISPLAY_BARS_V &&
      dhandler->bars_initialized != DISPLAY_BARS_VTHIN)
    display_handler_use_bars(dhandler, DISPLAY_BARS_V);

  return write_to_lk(dhandler->disp_fd, LKCMD_VBAR_DRAW, column, height);
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


