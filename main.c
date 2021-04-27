#include "term.h"
#include "snake.h"
#include "timeutil.h"
#include "linkstack.h"
/*
 *Default defines
 */

#define STAT_WON 1
#define STAT_LOSE 2
#define STAT_PAUSE 3

#define WIN_MSG "You won !"
#define LOSE_MSG "Aborted0x90"
#define MSG_COLOR "\e[7m"

#define MAX_MODS 50

//mode functions
int mod_init();
void mod_loop();
void mod_input(int k);
void mod_clean();
//game functions
int game_init();
void game_loop();
void game_input(int k);
void game_clean();
//menu functions
int menu_init();
void menu_loop();
void menu_input(int k);
//message screen functions
int msg_init();
void msg_input(int k);

int width = 10;//default term size
int height = 10;

snake* p_snake;//snake
ms_t m_timeout;
ms_t d_move;//last move delta time
ms_t movement_speed = 200;//wait time before moving again
ms_t default_movement_speed = 200;
level* cur_lvl;
short sel_lvl = 0;

typedef struct {
	ms_t start;
	ms_t passed;
} dtime;

dtime delta_time;

int points;//points collected

int game_status;

char** mods;//mods names (also used to change path when loading mod)
short sel_mod;//current selected mod
int n_mods;//mods count

node* collectibles = NULL;
int nr_collectibles = 0 ;

object* walls;
int nr_walls = 0;

void goto_mod(){
  start_game(&mod_init, &mod_loop, &mod_input);
}

void goto_menu(){
  end_game(&game_clean);
  free_level(cur_lvl);
  start_game(&menu_init, &menu_loop, &menu_input);
}

void goto_msg(){
  start_game(&msg_init, NULL, &msg_input);
}

void goto_game(){
  cur_lvl = getlvl(sel_lvl);
  start_game(&game_init, &game_loop, &game_input);
}

int main(int argc, char **argv){

  argv++;
  argc--;
  sel_mod = -1;
  sel_lvl = -1;
  while(argc > 0){
    if(strcmp(*argv, "--speed") == 0 || strcmp(*argv, "-s") == 0){
      argv++;
      argc--;
      default_movement_speed = 1000 - atoi(*argv);
      argv++;
      argc--;
    }else if(strcmp(*argv, "--level") == 0 || strcmp(*argv, "-l") == 0){
      argv++;argc--;
      sel_lvl = atoi(*argv);
      argv++;argc--;
    }else if(strcmp(*argv, "--mod") == 0 || strcmp(*argv, "-m") == 0){
      argv++;argc--;
      sel_mod = atoi(*argv);
      argv++;argc--;
    }
  }
  
  init_term();
  //first create buffer
  create_buffer(getWinWidth(), getWinHeight());
  //set width & height
  width = getWinWidth();
  height = getWinHeight();
  //select mod
  goto_mod();
  dispose_term();
  free_buffer();
  return 0;
}

void game_clean(){
  if(walls)
    free(walls);
  nr_walls = 0;
  if(p_snake)
    free_snake(p_snake);
  delete_list(&collectibles);
}

int game_init(){
  //Init status and buffer
  game_status = STAT_PAUSE;
  width = getWinWidth();
  height = getWinHeight();
  create_buffer(width-2, height);
  p_snake = create_snake();
  points = 0;
  movement_speed = default_movement_speed;
  //Init level objects (walls/collectibles)
  walls =malloc(sizeof(object) * cur_lvl->len);
  int j = 0;
  for(int i = 0; i < cur_lvl->len; i++){
    object o = cur_lvl->objs[i];
    //if wall
    if(o.type == OBJ_WALL)
      walls[j++] = o;
    //if collectible
    if(o.type == OBJ_COLLECTIBLE){
      if(is_empty(collectibles))
	collectibles = create_node(NULL, o);
      else
	collectibles = push_node(collectibles, o);
    }
  }
  walls = realloc(walls, sizeof(object) * j);
  nr_walls = j;
  //Init time
  delta_time.start = gettom();
  delta_time.passed = 0;
  m_timeout = delta_time.start;
  d_move = m_timeout;
  int i;
  GOTO(0, 3);
  for(i = 0; i < width; i++)
    printf("%c", '.');
  GOTO(0, height);
  for(i = 0; i < width; i++)
    printf("%c", '.');
  for(int y = 3; y < height; y++){
    GOTO(0, y);
    printf("%c", '.');
    GOTO(width, y);
    printf("%c", '.');
    }
  return 0;
}

void update_snake(){
  move_snake(p_snake);
  if(!snake_safe(p_snake))
    game_status = STAT_LOSE;
  for(int i = 0; i < nr_walls; i++)
    if(collide(p_snake, &walls[i]))
      game_status = STAT_LOSE;

  node* tmp = NULL;
  while(!is_empty(collectibles)){
    object o;
    collectibles = pop_node(collectibles, &o);
    if(collide(p_snake, &o)){
      increase_snake(p_snake);
      if(movement_speed >= 25)
	movement_speed -= 25;
      points++;
    }
    else
      if(is_empty(tmp))
	tmp = create_node(NULL, o);
      else
	tmp = push_node(tmp, o);
  }
  collectibles = tmp;
  
}

void update(){
  if(game_status == STAT_PAUSE){
    delta_time.start = gettom();
    delta_time.passed = 0;
    return;
  }
  ms_t ctime = gettom();
  delta_time.passed = ctime - delta_time.start;
  delta_time.start = ctime;

  //because movement speed is unsigned it can never be negative
  //only upped boundary are needed
  if(movement_speed > 500)
    movement_speed = 500;
  if((delta_time.start - d_move) > movement_speed){
    update_snake();
    d_move = delta_time.start;
  }
}

void p_collectible(node* n){
  print_object(n->data);
}

void game_loop(){
  update();
  for(int i=  0; i < nr_walls; i++)
    print_object(walls[i]);
  iterate(collectibles, &p_collectible);
  print_snake(*p_snake);
  flush_buffer(2, 4);
  GOTO(2, 2);
  if(game_status == STAT_PAUSE)
    printf("\e[7m Press arrow keys to start \033[0m");
  else
    printf("\e[7mLength: %d X: %d Y: %d   Points: %d \033[0m", p_snake->length, p_snake->tail->pos.x, p_snake->tail->pos.y, points);
  if(p_snake->length == 1)
    game_status = STAT_LOSE;
  if(game_status != 0 && game_status != STAT_PAUSE)
    goto_msg();
}

void unpause(){
  if(game_status == STAT_PAUSE)
    game_status = 0;
}

void game_input(int k){
  if(p_snake->length > 0){
    switch(k){
    case K_A:case K_LEFT:
      p_snake->dir.x = -1;
      p_snake->dir.y = 0;
      m_timeout = delta_time.start;
      unpause();
      break;
    case K_D:case K_RIGHT:
      p_snake->dir.x = 1;
      p_snake->dir.y = 0;
      m_timeout = delta_time.start;
      unpause();
      break;
    case K_W:case K_UP:
      p_snake->dir.x = 0;
      p_snake->dir.y = -1;
      m_timeout = delta_time.start;
      unpause();
      break;
    case K_S:case K_DOWN:
      p_snake->dir.x = 0;
      p_snake->dir.y = 1;
      m_timeout = delta_time.start;
      unpause();
      break;
    }
  }
  if(k == 'q' || k == 'Q'){
    sel_lvl = -1;
    goto_menu();
  }
}

int menu_init(){
  if(sel_lvl > -1 && sel_lvl < c_lvls()){
    goto_game();
    return 1;
  }else{
    sel_lvl = 0;
  }
  GOTO(width/2 - (int)strlen(get_title())/2, 4);
  fprintf(stdout, "\e[37m\e[41m%s\033[0m", get_title());
  return 0;
}

void menu_loop(){
  
  for(int i = 0; i < c_titles(); i++)
    if(get_lvl_title(i)){
      GOTO(width/2 - (int)strlen(get_lvl_title(i)+2)/2, 12 + (i*2));
      if(sel_lvl == i)
	fprintf(stdout, COLOR_INV);
      fprintf(stdout, " %s \033[0m ", get_lvl_title(i));
      fflush(stdout);
    }
  fflush(stdout);
}

void menu_input(int k){
  switch(k){
  case K_DOWN:
  case K_S:
    if(sel_lvl < c_lvls()-1)
      sel_lvl++;
    break;
  case K_UP:
  case K_W:
    if(sel_lvl > 0)
      sel_lvl--;
    break;
  case K_SPACE:
  case K_RET:
    goto_game();
    break;
  case K_ESC:
  case K_Q:
    end_game(NULL);
    break;
  }
}

int msg_init(){
  CLS();
  fflush(stdout);
  char * buff = malloc(width);
  memset(buff, '#', width);
  GOTO(0, height/2 - 6);
  write(STDOUT_FILENO, MSG_COLOR, strlen(MSG_COLOR));
  fflush(stdout);
  write(STDOUT_FILENO, buff, width);
  memset(buff, ' ', width);
  buff[0] = '#';
  buff[width-1] = '#';
  for(int i = 1; i < 6; i++){
    GOTO(0, height/2 - i);
    fflush(stdout);
    write(STDOUT_FILENO, buff, width);
    fflush(stdout);
  }
  if(game_status == STAT_WON){
    GOTO(width/2-(int)strlen(WIN_MSG)/2, height/2-4);
    fflush(stdout);
    write(STDOUT_FILENO, WIN_MSG, strlen(WIN_MSG));
    GOTO(width/2-3, height/2-2);
    fflush(stdout);
    write(STDOUT_FILENO, COLOR_NRM, strlen(COLOR_NRM));
    write(STDOUT_FILENO, " next ", 6);
    write(STDOUT_FILENO, MSG_COLOR, strlen(MSG_COLOR));
  }else{
    GOTO(width/2-(int)strlen(LOSE_MSG)/2, height/2-4);
    fflush(stdout);
    write(STDOUT_FILENO, LOSE_MSG, strlen(LOSE_MSG));
    GOTO(width/2-2, height/2-2);
    fflush(stdout);
    write(STDOUT_FILENO, COLOR_NRM, strlen(COLOR_NRM));
    write(STDOUT_FILENO, " ok ", 4);
    write(STDOUT_FILENO, MSG_COLOR, strlen(MSG_COLOR));
  }
  memset(buff, '#', width);
  GOTO(0, height/2);
  fflush(stdout);
  write(STDOUT_FILENO, buff, width);
  write(STDOUT_FILENO, COLOR_NRM, strlen(COLOR_NRM));
  fflush(stdout);
  free(buff);
  return 0;
}

void msg_input(int k){
  if(k == K_RET){
    if(game_status == STAT_WON && sel_lvl < c_titles()-1){
      sel_lvl++;
      end_game(&game_clean);
      free_level(cur_lvl);
      goto_game();
    }else{
      sel_lvl = -1;
      goto_menu();
    }
  }
  if(k == 'q' || k == 'Q'){
    sel_lvl = -1;
    goto_menu();
  }
}
//mod selection screen

int is_mod(char* path){
  char buff[4096];
  getcwd(buff, 4096);
  chdir(path);
  int valid_mod = 0;
  int fd;
  if((fd = open(MOD_DER_PATH, O_RDONLY)) != -1){
    valid_mod = 1;
    close(fd);
  }
  chdir(buff);
  return valid_mod;
}

int mod_init(){
  CLS();
  DIR* dir = opendir("./");
  struct dirent *entry;
  mods = malloc(sizeof(char*) * MAX_MODS);
  n_mods = 0;
  if(dir){
    while((entry= readdir(dir))){
      if(file_type(entry) == FILE_DIR && is_mod(entry->d_name)){
	mods[n_mods] = malloc(sizeof(entry->d_name));
	strcpy(mods[n_mods++], entry->d_name);
      }
    }
  }
  closedir(dir);
  mods = realloc(mods, sizeof(char*) * n_mods);
  if(sel_mod > -1 && sel_mod < n_mods){
    load(mods[sel_mod]);
    end_game(mod_clean);
    goto_menu();
    free_resources();
    return 1;
  }else{
    sel_mod = 0;
  }
  GOTO(width/2-22, 3);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, ".___  ___.   ______    _______       _______.", COLOR_NRM);
  GOTO(width/2-22, 4);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|   \\/   |  /  __  \\  |       \\     /       |", COLOR_NRM);
  GOTO(width/2-22, 5);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|  \\  /  | |  |  |  | |  .--.  |   |   (----`", COLOR_NRM);
  GOTO(width/2-22, 6);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|  |\\/|  | |  |  |  | |  |  |  |    \\   \\    ", COLOR_NRM);
  GOTO(width/2-22, 7);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|  |  |  | |  `--'  | |  '--'  |.----)   |   ", COLOR_NRM);
  GOTO(width/2-22, 8);
  fflush(stdout);
  fprintf(stdout, "%s%s%s", COLOR_BUL, "|__|  |__|  \\______/  |_______/ |_______/    ", COLOR_NRM);
  return 0;
}

void mod_loop(){
  int y = 15;
  for(int i = 0; i < n_mods; i++){
    GOTO(width/2 - (int)strlen(mods[i])/2, y+=2);
    fflush(stdout);
    fprintf(stdout, "%s%s%s", sel_mod == i ? COLOR_INV : COLOR_NRM, mods[i], COLOR_NRM);
  }
  fflush(stdout);
}

void mod_input(int k){
  if(k == K_UP || k == K_W){
    if(sel_mod > 0)
      sel_mod--;
  }
  if(k == K_DOWN || k == K_S){
    if(sel_mod < n_mods-1)
      sel_mod++;
  }
  if(k == K_RET || k == K_SPACE){
    //menu
    load(mods[sel_mod]);
    end_game(mod_clean);
    goto_menu();
    free_resources();
  }else if(k == K_Q || k == K_ESC){
    end_game(mod_clean);
  }
}

void mod_clean(){
  for(int i = 0; i < n_mods; i++)
    free(mods[i]);
  free(mods);
  n_mods = 0;
}
