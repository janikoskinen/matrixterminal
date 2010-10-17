#ifndef _DISPLAYER_HANDLER_H
#define _DISPLAYER_HANDLER_H

#include <ev.h>


/* Only buffer supported so far */
#define DISPLAY_TARGET_SCREEN 1
#define DISPLAY_TARGET_BUF1   2
#define DISPLAY_TARGET_BUF2   4 // Not yet implemented


typedef struct display_handler display_handler_t;
typedef struct display_scroller display_scroller_t;

struct display_scroller {
  ev_io watcher;
  display_scroller_t *next;
};

struct display_buffer {
  int row;
  int column;
  char data[4][21];
};

struct display_screen {
  int row;
  int column;
};

struct display_handler {
  struct ev_loop *loop;
  int disp_fd;

  int write_targets;
  int gpo_value;

  struct display_buffer buffer[2];
  struct display_screen screen;

  display_scroller_t *scrollers;
};


/* *** API Definition *** */

display_handler_t *display_handler_init(struct ev_loop*, int);
void display_handler_close(display_handler_t *);

int display_handler_set_write_target(display_handler_t *, int);

int display_handler_write(display_handler_t *, char *);
int display_handler_write_to(display_handler_t *, int, int, char *);
int display_handler_go_to(display_handler_t *, int, int);

int display_handler_clear_display(display_handler_t *);
int display_handler_clear_line(display_handler_t *, int);

int display_handler_start_scroller(display_handler_t *);
int display_handler_stop_scroller(display_handler_t *);

int display_handler_write_gpo(display_handler_t *, int);
int display_handler_set_gpo(display_handler_t *, int);
int display_handler_reset_gpo(display_handler_t *, int);

void display_handler_dump_buffer(display_handler_t *, int);

#endif
