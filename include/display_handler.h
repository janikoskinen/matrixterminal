#ifndef _DISPLAYER_HANDLER_H
#define _DISPLAYER_HANDLER_H

#include <ev.h>


/* Only buffer supported so far */
#define DISPLAY_TARGET_SCREEN 1
#define DISPLAY_TARGET_BUF1   2
#define DISPLAY_TARGET_BUF2   4 // Not yet implemented

#define DISPLAY_MAX_BUFFERS   2
#define DISPLAY_MAX_SCROLLERS 4

#define DISPLAY_BARS_NONE   0
#define DISPLAY_BARS_HLEFT  1
#define DISPLAY_BARS_HRIGHT 2
#define DISPLAY_BARS_VTHIN  3
#define DISPLAY_BARS_VTHICK 4
#define DISPLAY_BARS_H      DISPLAY_BARS_HRIGHT
#define DISPLAY_BARS_V      DISPLAY_BARS_VTHICK

typedef struct display_handler display_handler_t;

struct display_scroller {
  ev_timer watcher;
  display_handler_t *dhandler;
  int running;

  char *text;
  int row;
  int column;
  int width;
  int pos;
};

struct display_buffer {
  int row;
  int column;
  char data[4][21];
};

struct display_handler {
  struct ev_loop *loop;
  int disp_fd;

  int write_targets;
  int gpo_value;
  int bars_initialized;
  int row;
  int column;


  struct display_buffer buffer[DISPLAY_MAX_BUFFERS];

  struct display_scroller scrollers[DISPLAY_MAX_SCROLLERS];
};


/* *** API Definition ***

All command routines return 0 on success and -1 on error
*/

display_handler_t *display_handler_init(struct ev_loop*, int);
void display_handler_close(display_handler_t *);

int display_handler_set_write_target(display_handler_t *, int);

int display_handler_write(display_handler_t *, char *);
int display_handler_write_to(display_handler_t *, int, int, char *);
int display_handler_go_to(display_handler_t *, int, int);
int display_handler_go_home(display_handler_t *);


int display_handler_clear_display(display_handler_t *);
int display_handler_clear_line(display_handler_t *, int);

/* Scroller commands */
int display_handler_scroller_init(display_handler_t *,
				  int, int, int, int, char *, int);
int display_handler_scroller_close(display_handler_t *, int);
int display_handler_scroller_start(display_handler_t *, int);
int display_handler_scroller_stop(display_handler_t *, int);

/* GPO routines affect only to lk screen */
int display_handler_write_gpo(display_handler_t *, int);
int display_handler_set_gpo(display_handler_t *, int);
int display_handler_reset_gpo(display_handler_t *, int);

/* Bar drawing */
int display_handler_use_bars(display_handler_t *, int);
int display_handler_draw_bar_horizontal(display_handler_t *, int, int, int);
int display_handler_draw_bar_vertical(display_handler_t *, int, int);

// Dumps given buffer page with DBG
void display_handler_dump_buffer(display_handler_t *, int);

#endif
