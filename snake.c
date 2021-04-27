#include "snake.h"
#include "term.h"

char **print_buffer;
int b_width, b_height;//buffer dimensions

int n_lvls;
int n_titles;
int n_objects;

char* title;
char** lvl_titles;
level** levels;
int*    map;

void create_buffer(int width, int height){
  free_buffer();
  print_buffer = malloc(sizeof(char*) * height);
  for(int i = 0; i < height; i++){
    print_buffer[i] = malloc(width);
    memset(print_buffer[i], ' ', width);
  }
  b_width = width;
  b_height = height;
}

int snake_safe(snake* s){
  if(s->length < 2)
    return 0;
  part* p = s->tail;
  while(p->tail){
    p = p->tail;
    if(p->pos.x == s->tail->pos.x && p->pos.y == s->tail->pos.y)
      return 0;
  }
  return 1;
}

snake* create_snake(){
  snake* snk = malloc(sizeof(snake));
  part* p = malloc(sizeof(part));
  p->pos.x = b_width/2;
  p->pos.y = b_height/2;
  p->tail = NULL;
  snk->dir.x = 0;
  snk->dir.y = -1;
  snk->length = 1;
  snk->tail = p;
  for(int i = 0; i < 5; i++)
    increase_snake(snk);
  return snk;
}

void increase_snake(snake* s){
  part* p = s->tail;
  while(p->tail != NULL)
    p = p->tail;
  p->tail = malloc(sizeof(part));
  p->tail->pos.x = p->pos.x;
  p->tail->pos.y = p->pos.y;
  p->tail->tail = NULL;
  s->length++;
}
void load(char *path){
  n_lvls = 0;
  n_objects = 0;
  char fnames[100][2];//file names array max lenght = 2 (ex: 00, 01, 02...99) => max 100 files
  int ns = 0;//names stored
  if(chdir(path) == -1){
    fprintf(stderr, "Failed to change directory to selected mod: %s", path);
    exit(1);
  }
  DIR *dir = opendir(MOD_DER_PATH);
  struct dirent *entry;
 
  ns = 0;
  dir = opendir(LVLS_PATH);
  if(dir){
    while((entry = readdir(dir)))
      if(entry->d_type == DT_REG){
	if(ns == 99){
	  perror("Too many levels added to current mod used (max: 100 supported)");
	  exit(1);
	}
	if(strlen(entry->d_name) > 2){
	  perror("Filename is too long, max supported filename lenght is 2");
	  exit(1);
	}
	strncpy(fnames[ns], entry->d_name, 2);
	ns++;
      }
    closedir(dir);
    levels = malloc(sizeof(level) * ns);
    int offset = sizeof(LVLS_PATH);
    char* path = malloc(offset+2);
    strncpy(path, LVLS_PATH, offset);
    for(int i = ns-1; i > -1; i--){
      strncpy(path+(offset-1), fnames[i], 2);
      strncpy(path+(offset+1), "\0", 1);
      n_lvls++;
      load_lvls(path);
    }
    free(path);
  }else{
    perror("Could not open levels directory in current mod");
    exit(1);
  }
  load_der();
}


void load_lvls(char path[]){
  FILE* f = fopen(path, "r");
  int index = atoi(path + (strlen(path)-2));
  if(!f){
    fprintf(stderr, "Could not open level file: %s ", path);
    exit(1);
  }
  level* lvl = (level*)malloc(sizeof(level));
  int r = fscanf(f, "%d\n", &lvl->len);
  if(r != 1){
    perror("Wrong file format used to specify a level");
    exit(1);
  }
  lvl->objs = malloc(sizeof(object) * lvl->len);
  object obj;
  for(int i = 0; i < lvl->len; i++){

    int x, y;
    r = fscanf(f, "%c %d %d\n", &obj.type, &x, &y);
    if(r != 3){
      fprintf(stderr, "Error occured when trying to read obj line from file: %s", path);
      exit(1);
    }
    obj.pos.x = x;
    obj.pos.y = y;
    lvl->objs[i] = obj;
  }
  fclose(f);
  levels[index] = lvl;
}

void load_der(){
  FILE* f = fopen(MOD_DER_PATH, "r");
  char buff[1024];
  fscanf(f, "%[^\n]\n", buff);
  title = malloc(strlen(buff)+1);//+1 for string null terminator '\0'
  strcpy(title, buff);
  lvl_titles = malloc(100 * sizeof(char*));
  map = malloc(100 * sizeof(int));
  int rd = 1;
  int index;
  for(int i = 0; rd != EOF ; i++){
    rd = fscanf(f, "%d", &index); 
    rd = fscanf(f, "%[^\n]\n", buff);
    if(rd == EOF)
      break;
    lvl_titles[i] = malloc(strlen(buff)+1);//+1 for string null terminator '\0'
    strcpy(lvl_titles[i], buff);
    map[i] = index;
    n_titles++;
  }
  lvl_titles = realloc(lvl_titles, sizeof(char*) * n_titles);
  map = realloc(map, sizeof(int) * n_titles);
  fclose(f);
}


vec2 ptob_pos(int x, int y){
  float px = ((float)b_width/100.f) * (float)x;
  float py = ((float)b_height/100.f) * (float)y;
  vec2 res;
  res.x = (int)px;
  res.y = (int)py;
  return res;
}

int c_lvls(){
  return n_lvls;
}

int c_titles(){
  return n_titles;
}

int c_objs(){
  return n_objects;
}

level* getlvl(int index){
  if(index > n_titles || index < 0)
    index = 0;
  index = map[index];
  level* lvl = malloc(sizeof(level));
  lvl->len = levels[index]->len;
  int objs_size = sizeof(object) * lvl->len;
  lvl->objs = malloc(objs_size);
  memcpy(lvl->objs, levels[index]->objs, objs_size);
  return lvl;
}

void print_part(const part* p, char c){
  if(p->pos.x >= 0 && p->pos.y + 1 < b_height && p->pos.x >= 0 && p->pos.x + 1 < b_width)
    print_buffer[p->pos.y][p->pos.x] = c;
}

void print_snake(snake s){
  char p;
  switch(s.dir.x){
  case -1:
    p = KHR;
    break;
  case 1:
    p = KHL;
    break;
  }
  switch(s.dir.y){
  case -1:
    p = KHU;
    break;
  case 1:
    p = KHD;
    break;
  }
  part* part = s.tail;
  while((part = part->tail) != NULL)
    print_part(part, '@');
  print_part(s.tail, p);
}

void print_object(object o){
  int y = o.pos.y;
  int x = o.pos.x;
    if(y >= 0 && y < b_height && x >= 0 && x < b_width)
      print_buffer[y][x] = o.type;
}

void print_level(level l){
  for(int i = 0; i < l.len; i++)
    print_object(l.objs[i]);
}


void move_part(part* p, vec2 pos){
  if(p->tail && (p->tail->pos.x != p->pos.x || p->tail->pos.y != p->pos.y))
    move_part(p->tail, p->pos);
  p->pos.x = pos.x;
  p->pos.y = pos.y;
  if(p->pos.x < 1)
    p->pos.x = b_width-2;
  if(p->pos.y < 1)
    p->pos.y = b_height-1;
  if(p->pos.x > b_width-2)
    p->pos.x = 0;
  if(p->pos.y > b_height-1)
    p->pos.y= 0;
}

void move_snake(snake* s){
  vec2 pos;
  pos.x = s->tail->pos.x + s->dir.x;
  pos.y = s->tail->pos.y + s->dir.y;
  move_part(s->tail, pos);
}

int collide_p(const part* p, const object* o){
  if(!p || !o)
    return 0;
  return (p->pos.x == o->pos.x &&
	  p->pos.y == o->pos.y);
}

int collide(const snake* s, const object* o){
  for(part* p = s->tail; p != NULL; p = p->tail)
    if(collide_p(p, o))
      return 1;
  return 0;
}

char* get_title(){
  return title;
}

char* get_lvl_title(int index){
  return lvl_titles[index];
}

void flush_buffer(int x, int y){
  for(int i = y; i < b_height; i++){
    GOTO(x, i);
    fflush(stdout);
    write(STDOUT_FILENO, print_buffer[i], b_width);
    memset(print_buffer[i], ' ', b_width);
  }
}

void free_buffer(){
  if(!print_buffer)
    return;
  for(int i = 0; i < b_height; i++)
    free(print_buffer[i]);
  free(print_buffer);
  b_height = 0;
  b_width = 0;
}

void free_snake(snake* s){
  part* p = s->tail;
  while(p){
    part* tmp = p;
    p = p->tail;
    free(tmp);
  }
  free(s);
}

void free_object(object *obj){
  if(!obj)
    return;
  free(obj);
  obj = NULL;
}

void free_der(){
  free(title);
  for(int i = 0; i < n_titles; i++)
    free(lvl_titles[i]);
  free(lvl_titles);
}

void free_level(level* l){
  if(!l)
    return;
  free(l->objs);
  free(l);
}

void free_levels(){
  for(int i = 0; i < c_lvls(); i++)
    free_level(levels[i]);
  n_lvls = 0;
  free(levels);
}

void free_resources(){
  free_der();
  free_levels();
  free(map);
}
