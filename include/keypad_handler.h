#ifndef _KEYPAD_HANDLER_H
#define _KEYPAD_HANDLER_H


typedef struct keypad_handler keypad_handler_t:

typedef void (*key_received_callback_t) (void*);

struct keypad_handler {
  char *dev_name;
  int dev_fd;
  key_received_callback_t key_callback;
};

int keypad_handler_set_received_key_cb();

int keypad_handler_get_column(char key);
int keypad_handler_get_row(char key);

int keypad_handler_key_is_down(char key);
int keypad_handler_key_is_up(char key);

#endif
