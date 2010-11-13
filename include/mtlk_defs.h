#ifndef _MT2LK_DEFS_H
#define _MT2LK_DEFS_H

/* Key definitions */

#define LKKEY_UP 
#define LKKEY_DOWN 
#define LKKEY_LEFT 
#define LKKEY_RIGHT 

#define LKKEY_0
#define LKKEY_1
#define LKKEY_2
#define LKKEY_3
#define LKKEY_4
#define LKKEY_5
#define LKKEY_6
#define LKKEY_7
#define LKKEY_8
#define LKKEY_9

#define LKKEY_ENTER
#define LKKEY_BACK

#define LKKEY_A
#define LKKEY_B
#define LKKEY_C
#define LKKEY_D



/* Module command definitions */

#define LKCMD_SET_POS "\xfe\x47%c%c" // Row (1-4), Col (1-20)
#define LKCMD_CLR_SRC "\xfe\x58"

#define LKCMD_GPO_ON  "\xfe\x57%c" // nGPO (1-6)
#define LKCMD_GPO_OFF "\xfe\x56%c" // nGPO (1-6)

#define LKCMD_BACKLIGHT_ON  "\xfe\x42"
#define LKCMD_BACKLIGHT_OFF "\xfe\x46"
#define LKCMD_SET_CONTRAST  "\xfe\x50%c" // Contrast (0-255)

#define LKCMD_AUTO_REPEAT_ON  "\xfe\x7e%c" // 0: 200ms repeat, 1: Up/down sent
#define LKCMD_AUTO_REPEAT_OFF "\xfe\x60"
#define LKCMD_AUTO_SCROLL_ON  "\xfe\x51"
#define LKCMD_AUTO_SCROLL_OFF "\xfe\x52"
#define LKCMD_AUTO_LINEWRAP_ON  "\xfe\x43"
#define LKCMD_AUTO_LINEWRAP_OFF "\xfe\x44"

#define LKCMD_CURSOR_BLOCK_ON  "\xfe\x53"
#define LKCMD_CURSOR_BLOCK_OFF "\xfe\x54"
#define LKCMD_CURSOR_UNDER_ON  "\xfe\x4a"
#define LKCMD_CURSOR_UNDER_OFF "\xfe\x4b"

#define LKCMD_VBAR_INIT  "\xfe\x76"
#define LKCMD_VBAR2_INIT "\xfe\x73"
#define LKCMD_HBAR_INIT  "\xfe\x68"
#define LKCMD_VBAR_DRAW  "\xfe\x3d%c%c" // Col (1-20), Length (0-20)
#define LKCMD_HBAR_DRAW  "\xfe\x7c%c%c%c%c" // Col (1-20),
                                            // Row (1-4),
                                            // Dir (0: right, 1: left)
                                            // Length (0-100)
#endif
