#ifndef _MT2LK_DEFS_H
#define _MT2LK_DEFS_H

#define KEY_UP 
#define KEY_DOWN 
#define KEY_LEFT 
#define KEY_RIGHT 

#define LKCMD_SET_POS "\xfe\x47%d%d" // Row (1-4), Col (1-20)
#define LKCMD_CLR_SRC "\xfe\x58"

#define LKCMD_GPO_ON  "\xfe\x57%d" // nGPO (1-6)
#define LKCMD_GPO_OFF "\xfe\x56%d" // nGPO (1-6)

#define LKCMD_BACKLIGHT_ON  "\xfe\x42"
#define LKCMD_BACKLIGHT_OFF "\xfe\x46"
#define LKCMD_SET_CONTRAST  "\xfe\x50%d" // Contrast (0-255)

#define LKCMD_AUTO_REPEAT_ON  "\xfe\x7e%d" // 0: 200ms repeat, 1: Up/down sent
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
#define LKCMD_VBAR_DRAW  "\xfe\x3d%d%d" // Col (1-20), Length (0-20)
#define LKCMD_HBAR_DRAW  "\xfe\x7c%d%d%d%d" // Col (1-20),
                                            // Row (1-4),
                                            // Dir (0: right, 1: left)
                                            // Length (0-100)
#endif
