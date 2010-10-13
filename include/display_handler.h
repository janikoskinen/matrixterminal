#ifndef _DISPLAYER_HANDLER_H
#define _DISPLAYER_HANDLER_H

#include <ev.h>

typedef struct display_handler display_handler_t;
typedef struct display_scroller display_scroller_t;

struct display_scroller {
  io_watcher watcher;
  display_scroller_t *next;
}

struct display_handler {
  char *disp_name;
  int disp_fd;

  display_scroller_t *scrollers;
};


/* *** API Definition *** */


int display_handler_clear_display();
int display_handler_write(char *);
int display_handler_write_to(int, int, char *);
int display_handler_go_to(int, int);

int start_scroller();
int stop_scroller();

#endif
