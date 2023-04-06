#ifndef CONSOLE_COLOR_H
#define CONSOLE_COLOR_H

//控制台文字OFF
#define NONE "\e[0m"

#define CONSOLE_END "\e[0m"

//控制台文字高亮
#define L_HIGHLIGHT "\e[1;1m"
//斜体
#define L_ITALIC "\e[1;3m"
//控制台文字下划线
#define L_UNDERLINE "\e[1;4m"
//控制台文字闪烁
#define L_BLINK  "\e[1;5m"
//反白
#define L_REVERSE   "\e[1;7m"
//不可见
#define L_HIDE      "\e[1;8m"
//删除线
#define L_STRIKETHROUGH "\e[1;9m"


//控制台文字颜色代码
#define L_BLACK "\e[1;30m"
#define L_RED "\e[1;31m"
#define L_GREEN "\e[1;32m"
#define L_YELLOW "\e[1;33m"
#define L_BLUE "\e[1;34m"
#define L_PURPLE "\e[1;35m"
#define L_CYAN "\e[1;36m"
#define L_WHITE "\e[1;37m"

#define BL_RED "\e[1;41m"

#define BLACK_CODE              0
#define RED_CODE                1
#define GREEN_CODE              2
#define YELLOW_CODE             3
#define BLUE_CODE               4
#define PURPLE_CODE             5
#define CYAN_CODE               6
#define WHITE_CODE              7

#define FONT_COLOR_BASE         30
#define BACKGROUND_COLOR_BASE   40

#define HIGHLIGHT_CODE          1
#define ITALIC_CODE             3
#define UNDERLINE_CODE          4
#define BLINK_CODE              5
#define REVERSE_CODE            7
#define HIDE_CODE               8
#define STRIKETHROUGH_CODE      9

#endif //CONSOLE_COLOR_H