#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#include "debug.h"
#include "displayer_api.h"


displayer_t *displayer_create(char *name, struct ev_loop* loop,
			      display_handler_t *dhandler,
			      keypad_handler_t *khandler)
{
  char *filename;
  displayer_init_func init_func;
  displayer_t *tmp = malloc(sizeof(displayer_t));

  if (!tmp || !name || !loop)
    return NULL;

  memset(tmp, 0, sizeof(displayer_t));
  tmp->name = strdup(name);
  tmp->loop = loop;

  if (asprintf(&filename, "libmtdisplayer_%s.so", name) == -1)
    goto error;

  if (!tmp->name)
    goto error;

  DBG("Opening %s", filename);
  tmp->dl_handle = dlopen(filename, RTLD_LAZY);

  if (!tmp->dl_handle)
    goto error;

  tmp->dhandler = dhandler;
  tmp->khandler = khandler;

  init_func = dlsym(tmp->dl_handle, "displayer_initialize");
  (*init_func)(tmp);

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
  int r;

  if (disp == NULL)
    return -1;
  else {
    if (displayer_is_running(disp))
      displayer_stop(disp);
    if (disp->dl_handle)
      r = dlclose(disp->dl_handle);
    if (disp->name)
      free(disp->name);
    if (disp->data)
      free(disp->data);
    free(disp);
    return r;
  }
}


int displayer_activate(displayer_t *disp)
{
  if (disp) {
    disp->status |= DISPLAYER_STATUS_ACTIVE;
    DBG("Activate: Displayer (%ld) status: %d", (long)disp, disp->status);
    return 0;
  } else {
    return -1;
  }
}

int displayer_deactivate(displayer_t *disp)
{
  if (disp) {
    disp->status &= ~DISPLAYER_STATUS_ACTIVE;
    DBG("Deactivate: Displayer (%ld) status: %d", (long)disp, disp->status);
    return 0;
  } else {
    return -1;
  }
}


int displayer_start(displayer_t *disp)
{
  displayer_start_func func;
  int r;
  if (displayer_is_running(disp))
    return -1;

  func = dlsym(disp->dl_handle, "displayer_start");
  r = (*func)(disp);
  if (r == 0)
    disp->status |= DISPLAYER_STATUS_RUNNING;

  DBG("Start: Displayer (%ld) status: %d", (long)disp, disp->status);
  return r;
}

int displayer_stop(displayer_t *disp)
{
  displayer_stop_func func;
  int r;
  if (!displayer_is_running(disp))
    return -1;

  func = dlsym(disp->dl_handle, "displayer_stop");
  r = (*func)(disp);
  if (r == 0)
    disp->status &= ~DISPLAYER_STATUS_RUNNING;

  DBG("Stop: Displayer (%ld) status: %d", (long)disp, disp->status);
  return r;
}


int displayer_is_running(displayer_t *disp){
  if (disp->status && DISPLAYER_STATUS_RUNNING > 0)
    return 1;
  else
    return 0;
}

int displayer_is_active(displayer_t *disp){
  if (disp->status && DISPLAYER_STATUS_ACTIVE > 0)
    return 1;
  else
    return 0;
}
