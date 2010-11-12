#include <unistd.h>

#include "displayer_api.h"


displayer_t *displayer_create(char *name) {
  return NULL;
}


int displayer_close(displayer_t *disp) {
  if (disp == NULL)
    return -1;
  else
    return 0;
}


int displayer_activate(displayer_t *disp)
{

  return -1;
}


int displayer_deactivate(displayer_t *disp)
{

  return -1;
}

