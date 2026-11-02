// NAME : taylormci
// DATE : 2026-09-12
// PURP : ciao.h -> Input/Output With Ansi

#ifndef CIAO_H
#define CIAO_H

//Beep
#define BEEP        "\a"

//Text Color
#define BLK 	    "\e[30m"
#define RED 	    "\e[31m"
#define GRN 	    "\e[32m"
#define YEL 	    "\e[33m"
#define BLU 	    "\e[34m"
#define MAG 	    "\e[35m"
#define CYN 	    "\e[36m"
#define WHT 	    "\e[37m"

//Background
#define BG_BLK      "\e[40m"
#define BG_RED      "\e[41m"
#define BG_GRN      "\e[42m"
#define BG_YEL      "\e[43m"
#define BG_BLU      "\e[44m"
#define BG_MAG      "\e[45m"
#define BG_CYN      "\e[46m"
#define BG_WHT      "\e[47m"

//High Intensty Background 
#define HBG_BLK     "\e[100m"
#define HBG_RED     "\e[101m"
#define HBG_GRN     "\e[102m"
#define HBG_YEL     "\e[103m"
#define HBG_BLU     "\e[104m"
#define HBG_MAG     "\e[105m"
#define HBG_CYN     "\e[106m"
#define HBG_WHT     "\e[107m"

//Text Styles
#define BOLD        "\e[1m"
#define DIM         "\e[2m"
#define ITALIC      "\e[3m"
#define UNDER       "\e[4m"
#define BLINK       "\e[5m"
#define INVERSE     "\e[7m"
#define HIDDEN      "\e[8m"
#define SCORE       "\e[9m"
#define OVER        "\e[53m"
#define BOLD_OFF    "\e[22m"
#define DIM_OFF     "\e[22m"
#define ITALIC_OFF  "\e[23m"
#define UNDER_OFF   "\e[24m"
#define BLINK_OFF   "\e[25m"
#define INVERSE_OFF "\e[27m"
#define HIDDEN_OFF  "\e[28m"
#define SCORE_OFF   "\e[29m"
#define OVER_OFF    "\e[55m"

//Reset
#define RESET       "\e[0m"

//Cursor
#define CURSOR_SAVE "\e[s"
#define CURSOR_REST "\e[u"
#define CURSOR_HIDE "\e[?25l"
#define CURSOR_SHOW "\e[?25h"
#define CURSOR_GOTO "\e[%d;%dH"
#define CURSOR_UP   "\e[%dA"
#define CURSOR_DOWN "\e[%dB"
#define CURSOR_FORW "\e[%dC"
#define CURSOR_BACK "\e[%dD"
#define CURSOR_NEXT "\e[%dE"
#define CURSOR_PREV "\e[%dF"

//Screen
#define SCREEN_SAVE "\e[?47h"
#define SCREEN_REST "\e[?47l"
#define BUFFER_MAIN "\e[?1049l"
#define BUFFER_ALT  "\e[?1049h"

//Clear
#define CLR_INDISP  "\e[0J"
#define CLR_OUTDISP "\e[1J"
#define CLR_DISP    "\e[2J"
#define CLEAR	    "\e[2J"
#define CLR_INLINE  "\e[0K"
#define CLR_OUTLINE "\e[1K"
#define CLR_LINE    "\e[2K"

//Window
#define DEC_MODE    "\e(0"
#define ASCII_MODE  "\e(B"
#define TITLE_TAB   "\e]0;%s\a"
#define TITLE	    "\e]1;%s\a"
#define TAB	    "\e]2;%s\a"

#endif
