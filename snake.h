#ifndef __SNAKE__
#define __SNAKE__

#include "timeutil.h"

#define OBJ_WALL '#'
#define OBJ_COLLECTIBLE '*'

#define KHR '<'
#define KHL '>'
#define KHU '^'
#define KHD 'v'

//Resources
#define MOD_DER_PATH "./deroulement"
#define COLLECTIBLES "./collectibles/"
#define LVLS_PATH "./niveaux/"

typedef struct {
  int x, y;
} vec2;

typedef struct part {
  vec2 pos;
  struct part* tail;//pointer to next tail part
} part;

typedef struct {
  int length;//length of snake
  vec2 dir;//direction of the head
  part* tail;//tail part
} snake;

typedef struct {
  char type;//object type: ex. wall, collectible
  vec2 pos;//obejct position
} object;


typedef struct {
  int len;
  object* objs;
} level;

void create_buffer(int, int);

//Printing functions
void print_snake(snake);
void print_object(object);
void print_level(level);
//Create/Modify snake functions
snake* create_snake();
void increase_snake(snake*);

//Loading functions
void load();
void load_snake();
void load_collectibles();
void load_lvls();
void load_der();
vec2 ptob_pos();

//Count functions
int c_lvls();//number of levels currently loaded
int c_titles();//number of titles
int c_objs();//number of objects currently loaded

//Get functions
level* getlvl(int);

//Movement function
void move_snake(snake*);

//snake collision and misc functions
int snake_safe(snake*);

//Collision function(s)
int collide(const snake*, const object*);

//Info functions
char* get_title();
char* get_lvl_title(int);

//Buffer functions
void flush_buffer(int, int);

//Free functions
void free_buffer();
void free_snake(snake*);
void free_object(object*);
void free_level(level*);
void free_levels();
void free_resources();
#endif
