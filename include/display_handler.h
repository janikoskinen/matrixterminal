#ifndef _DISPLAYER_HANDLER_H
#define _DISPLAYER_HANDLER_H

#include <ev.h>


/* Only buffer supported so far */
#define DISPLAY_TARGET_SCREEN 1
#define DISPLAY_TARGET_BUF1   2
#define DISPLAY_TARGET_BUF2   4 // Not yet implemented

#define DISPLAY_MAX_BUFFERS   2
#define DISPLAY_MAX_SCROLLERS 4

#define DISPLAY_BARS_NONE   0
#define DISPLAY_BARS_HLEFT  1
#define DISPLAY_BARS_HRIGHT 2
#define DISPLAY_BARS_VTHIN  3
#define DISPLAY_BARS_VTHICK 4
#define DISPLAY_BARS_H      DISPLAY_BARS_HRIGHT
#define DISPLAY_BARS_V      DISPLAY_BARS_VTHICK

typedef struct display_handler display_handler_t;

struct display_scroller {
  ev_timer watcher;
  display_handler_t *dhandler;
  int running;

  char *text;
  int row;
  int column;
  int width;
  int pos;
};

struct display_buffer {
  int row;
  int column;
  char data[4][21];
};

struct display_handler {
  struct ev_loop *loop;
  int disp_fd;

  int write_targets;
  int gpo_value;
  int bars_initialized;
  int row;
  int column;


  struct display_buffer buffer[DISPLAY_MAX_BUFFERS];

  struct display_scroller scrollers[DISPLAY_MAX_SCROLLERS];
};


/* *** API Definition ***

All command routines return 0 on success and -1 on error
*/


/* Init / Close
    init:  init(ev_loop*, display_fd)
           Initializes display handler with given ev_loop struct and file
	   descriptor for actual display port.

    close: close(dh*)
           Frees resources of given display handler.
*/
display_handler_t *display_handler_init(struct ev_loop*, int);
void display_handler_close(display_handler_t *);


/* Screen write commands
    set_write_target: set_write_target(dh*, target_mask)
                      Sets targets where write and cursor move commands will
		      affect. Bit 0 is actual display and following bits are
		      following virtual pages.

    write:            write(dh*, text)
                      Writes 'text' to current location.

    write_to:         write_to(dh*, row, column, text)
                      Writes 'text' to given location.

    go_to:            go_to(dh*, row, column)
                      Moves cursor to given location.

    go_home:          go_home(dh*)
                      Moves cursor to upper left corner.

    clear_display:    clear_display(dh*)
                      Clears display and moves cursor to top left corner.

    clear_line:       clear_line(dh*, row)
                      Clears given line and leaves cursor after the row.
*/
int display_handler_set_write_target(display_handler_t *, int);
int display_handler_write(display_handler_t *, char *);
int display_handler_write_to(display_handler_t *, int, int, char *);
int display_handler_go_to(display_handler_t *, int, int);
int display_handler_go_home(display_handler_t *);
int display_handler_clear_display(display_handler_t *);
int display_handler_clear_line(display_handler_t *, int);


/* Scroller commands
    scroller_init:  scroller_init(dh*, nbr, row, col, width, text, interval)
                    Initializes scroller number 'nbr' with scroll text 'text'.
		    Scroller is started from location defined by 'row' and
		    'column' and covers 'width' chars from display. Interval
		    defines scroll speed in milliseconds.

    scroller_close: scroller_close(dh*, nbr)
                    Frees the given scroller resources.

    scroller_start: scroller_start(dh*, nbr)
                    Starts the given scroller.

    scroller_stop:  scroller_stop(dh*, nbr)
                    Stops the given scroller.
*/
int display_handler_scroller_init(display_handler_t *,
				  int, int, int, int, char *, int);
int display_handler_scroller_close(display_handler_t *, int);
int display_handler_scroller_start(display_handler_t *, int);
int display_handler_scroller_stop(display_handler_t *, int);


/* GPO routines - affect only to lk screen
    write_gpo: write_gpo(dh*, gpo_value)
               Sets gpos as defined by bits of 'gpo_value'. Bit 0 is GPO 1
	       and so on. 'gpo_value' is therefore in range 0-63.

    set_gpo:   set_gpo(dh*, gpo_number)
               Sets given gpo on. 'gpo_number' is in range 1-6.

    reset_gpo: reset_gpo(dh*, gpo_number)
               Sets given gpo off. 'gpo_number' is in range 1-6.
*/
int display_handler_write_gpo(display_handler_t *, int);
int display_handler_set_gpo(display_handler_t *, int);
int display_handler_reset_gpo(display_handler_t *, int);


/* Bar drawing
    use_bars:            use_bars(dh*, type)
                         Initializes bar usage. 'type' is in range 0-4.

    Draw_bar_horizontal: draw_bar_horizontal(dh*, row, column, length)
                         Draws horizontal bar from position defined by 'row'
			 and 'column' with length of 'length'.

    draw_bar_vertical:   draw_bar_vertical(dh*, column, height)
                         Draws vertical bar from bottom of display to 'height'
			 to given column.
*/
int display_handler_use_bars(display_handler_t *, int);
int display_handler_draw_bar_horizontal(display_handler_t *, int, int, int);
int display_handler_draw_bar_vertical(display_handler_t *, int, int);


// Dumps given buffer page with DBG
void display_handler_dump_buffer(display_handler_t *, int);

#endif
