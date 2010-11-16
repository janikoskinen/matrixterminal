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

#define DISPLAYER_STATUS_ACTIVE      1
#define DISPLAYER_STATUS_RUNNING     2


typedef struct displayer displayer_t;

typedef void (*displayer_init_func) (displayer_t *);
typedef int (*displayer_start_func) (displayer_t *);
typedef int (*displayer_stop_func) (displayer_t *);


struct displayer {
  int status;
  int oper_msecs;

  display_handler_t *dhandler; // Separate for each displayer
  keypad_handler_t *khandler; // Common for all displayers

  struct ev_loop *loop;
  ev_timer disp_watcher;

  void *dl_handle; // Handler for lib

  //disp_oper displayer_operate_callback;
  //disp_key displayer_key_callback;

  char *name; // Displayer name
  void *data; // General data pointer to something
};



/* Globally defined */
displayer_t *displayer_create(char *name,
			      struct ev_loop *loop,
			      display_handler_t *dhandler,
			      keypad_handler_t *khandler);
int displayer_close(displayer_t *disp);

int displayer_start(displayer_t *);
int displayer_stop(displayer_t *);
int displayer_activate(displayer_t *);
int displayer_deactivate(displayer_t *);

int displayer_is_running(displayer_t *);
int displayer_is_active(displayer_t *);

#endif
