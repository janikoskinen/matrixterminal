#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "displayer_api.h"


displayer_t *displayer_create(char *name)
{
  char *filename;
  displayer_t *tmp = malloc(sizeof(displayer_t));

  if (!tmp || !name)
    return NULL;

  memset(tmp, 0, sizeof(displayer_t));
  tmp->name = strdup(name);

  if (asprintf(&filename, "%s%s", "mt_displayer_", name) == -1)
    goto error;

  if (!tmp->name)
    goto error;

  tmp->dl_handle = dlopen(filename, RTLD_LAZY);

  if (!tmp->dl_handle)
    goto error;

  //tmp->displayer_operate_callback = func_oper;
  //tmp->displayer_key_callback = func_key;

  return tmp;

 error:
  if (tmp) {
    if (tmp->name)
      free(tmp->name);
    free(tmp);
  }
  return NULL;
}


int displayer_close(displayer_t *disp)
{
  if (disp == NULL)
    return -1;
  else {
    if (disp->dl_handle)
      dlclose(disp->dl_handle);
    if (disp->name)
      free(disp->name);
    if (disp->data)
      free(disp->data);
    free(disp);
    return 0;
  }
}


int displayer_activate(displayer_t *disp)
{

  return -1;
}


int displayer_deactivate(displayer_t *disp)
{

  return -1;
}

