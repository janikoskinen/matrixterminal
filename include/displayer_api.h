#ifndef _DISPLAYER_API_H
#define _DISPLAYER_API_H


/* *** API Definition *** */


int write(char *);
int write_to(int, int, char *);

int start_scroller();
int stop_scroller();

#endif
