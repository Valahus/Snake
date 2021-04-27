#ifndef __TERM__
#define __TERM__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

//colors
#define COLOR_NRM "\x1B[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GRN "\x1B[32m"
#define COLOR_YEL "\x1B[33m"
#define COLOR_BLU "\x1B[34m"
#define COLOR_INV "\e[7m"
#define COLOR_BUL "\e[1m"

/*
 *Key defines
 */
#define K_A	97
#define K_D	100
#define K_S	115
#define K_W	119
#define K_LEFT	68
#define K_RIGHT 67
#define K_UP	65
#define K_DOWN	66
#define K_SPACE	' '
#define K_RET   '\r'
#define K_ESC   27
#define K_Q     113

//file types:
#define FILE_UNK 0 //unkown file type
#define FILE_DIR 1 //directory
#define FILE_REG 2 //regular file

#define HIDEC() printf("\x1b[?25l");
#define SHOWC() printf("\x1b[?25h");
#define CLS()   printf("\033[2J");
#define GOTO(x,y)   printf("\033[%d;%dH",(y),(x));

void start_game(int (*init)(void), void (*gameloop)(void), void (*input_proc)(int));
void end_game();
void init_term();
void dispose_term();
int getWinHeight();
int getWinWidth();
//portable way to check file type
unsigned char file_type(struct dirent*);

#endif 
