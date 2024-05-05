#ifndef CONSOLE_H
#define CONSOLE_H

#include "comm/types.h"
#include "dev/tty.h"
#include "ipc/mutex.h"

// https://wiki.osdev.org/Printing_To_Screen
#define CONSOLE_VIDEO_BASE			0xb8000		
#define CONSOLE_DISP_ADDR           0xb8000
#define CONSOLE_DISP_END			(0xb8000 + 32*1024)	
#define CONSOLE_ROW_MAX				25			
#define CONSOLE_COL_MAX				80			

#define ASCII_ESC                   0x1b                 

#define	ESC_PARAM_MAX				10			

typedef enum _cclor_t {
    COLOR_Black			= 0,
    COLOR_Blue			= 1,
    COLOR_Green			= 2,
    COLOR_Cyan			= 3,
    COLOR_Red			= 4,
    COLOR_Magenta		= 5,
    COLOR_Brown			= 6,
    COLOR_Gray			= 7,
    COLOR_Dark_Gray 	= 8,
    COLOR_Light_Blue	= 9,
    COLOR_Light_Green	= 10,
    COLOR_Light_Cyan	= 11,
    COLOR_Light_Red		= 12,
    COLOR_Light_Magenta	= 13,
    COLOR_Yellow		= 14,
    COLOR_White			= 15
}cclor_t;

typedef union {
	struct {
		char c;						
		char foreground : 4;		
		char background : 3;		
	};

	uint16_t v;
}disp_char_t;

typedef struct _console_t {
	disp_char_t * disp_base;	

    enum {
        CONSOLE_WRITE_NORMAL,			
        CONSOLE_WRITE_ESC,				
        CONSOLE_WRITE_SQUARE,          
    }write_state;

    int cursor_row, cursor_col;		
    int display_rows, display_cols;	
    int old_cursor_col, old_cursor_row;	
    cclor_t foreground, background;	

    int esc_param[ESC_PARAM_MAX];	
    int curr_param_index;

    mutex_t mutex;                  
}console_t;

int console_init (int idx);
int console_write (tty_t * tty);
void console_close (int dev);
void console_select(int idx);
void console_set_cursor(int idx, int visiable);
#endif /* SRC_UI_TTY_WIDGET_H_ */
