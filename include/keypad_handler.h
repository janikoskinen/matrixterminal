#ifndef _KEYPAD_HANDLER_H
#define _KEYPAD_HANDLER_H


#include "mt2lk_defs.h"

typedef struct keypad_handler keypad_handler_t;

typedef void (*key_received_callback_t) (keypad_handler_t *dhandler,
					 char key, int downtime);


struct keypad_handler {
  ev_io dev_watcher;
  struct ev_loop *loop;
  int dev_fd;
  key_received_callback_t key_callback;
};


/* API */

keypad_handler_t *keypad_handler_init(struct ev_loop *, int);
void keypad_handler_close(keypad_handler_t*);


int keypad_handler_set_key_received_callback(keypad_handler_t *,
					     key_received_callback_t);

int keypad_handler_get_column(char key);
int keypad_handler_get_row(char key);

int keypad_handler_key_is_down(char key);
int keypad_handler_key_is_up(char key);

#endif
