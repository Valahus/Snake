#include "term.h"

#define SPEED B230400

struct termios oterm;
struct termios iterm;

int running = 0;

int output = STDOUT_FILENO;
int input = STDIN_FILENO;
struct winsize w;

int getWinWidth(){
	return w.ws_col;
}

int getWinHeight(){
	return w.ws_row;
}

void init_term(){
  
  ioctl(output, TIOCGWINSZ, &w);
  //term settup
  struct termios nterm;
  cfmakeraw(&nterm);
  if(tcgetattr(output, &nterm) != 0)
    exit(1);
  memcpy(&oterm, &nterm, sizeof(struct termios));
  //echo off, non-canonical
  nterm.c_lflag &= ~(ICANON | ECHO);
  cfsetispeed(&nterm, SPEED);
  cfsetospeed(&nterm, SPEED);
  tcsetattr(output, TCSANOW, &nterm);
  
  if(tcgetattr(input, &nterm) != 0)
    exit(1);
  memcpy(&iterm, &nterm, sizeof(struct termios));
  cfmakeraw(&nterm);
  nterm.c_cc[VMIN] = 0;
  if(tcsetattr(input, TCSANOW, &nterm) != 0)
    exit(1);
}

void start_game(int (*init)(void), void (*gameloop)(), void (*input_proc)(int)){
  running = 1;
  CLS();
  HIDEC();
  int rd = 0;
  if(init())
    return;
  struct pollfd fd;
  fd.fd = STDIN_FILENO;
  fd.events = POLLIN;
  while(running){
    if(gameloop)
      gameloop();
    poll(&fd, 1, 50);//50ms fastest update speed
    char buffer[3];
    if((rd = read(input, &buffer, 3)) > 0){
      if(buffer[0] == K_ESC && rd == 3)
	input_proc(buffer[2]);
      else
	input_proc(buffer[0]);
    }
  }
}

void end_game(void (*clean)(void)){
  running = 0;
  if(clean)
    clean();
}

void dispose_term(){
  CLS();
  SHOWC();
  GOTO(0, 0);
  tcsetattr(input, TCSANOW, &iterm);
  tcsetattr(output, TCSANOW, &oterm);
}

unsigned char file_type(struct dirent* dir){
  #ifdef _DIRENT_HAVE_DT_TYPE
  if(dir->d_type != DT_UNKNOWN && dir->d_type != DT_LNK){
    return dir->d_type == DT_DIR ? FILE_DIR : dir->d_type == DT_REG ? FILE_REG : FILE_UNK;
  }
  #endif
  struct stat fstat;
  if(stat(dir->d_name, &fstat) == 0){
    return S_ISDIR(fstat.st_mode) ? FILE_DIR : S_ISREG(fstat.st_mode) ? FILE_REG : FILE_UNK;
  }
  return FILE_UNK;
}
