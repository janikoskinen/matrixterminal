#ifndef _DISPLAYER_API_H
#define _DISPLAYER_API_H


#include "display_handler.h"
#include "keypad_handler.h"


/* Displayer states:
   - Running
     Displayer is doing normal data operations.
   - Active
     Displayer draws to display.
*/

#define DISPLAYER_STATUS_RUNNING     1
#define DISPLAYER_STATUS_ACTIVE      2


typedef struct displayer displayer_t;

typedef void (*disp_oper) (int, int);
typedef void (*disp_key) (int, int);

struct displayer {
  int status;
  int oper_msecs;

  display_handler_t dhandler; // Separate for each displayer
  keypad_handler_t khandler; // Common for all displayers

  ev_timer disp_watcher;

  disp_oper displayer_operate_callback;
  disp_key displayer_key_callback;
};


int displayer_start(displayer_t *);
int displayer_stop(displayer_t *);


/* Globally defined */
displayer_t *displayer_create(char *name);
int displayer_close(displayer_t *disp);

int displayer_activate(displayer_t *);
int displayer_deactivate(displayer_t *);


#endif
