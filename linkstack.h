#ifndef __LINKSTACK__
#define __LINKSTACK__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "snake.h"

typedef struct node{
  object data;
  struct node * nxt;
}node;

typedef void (*callback)(node*);

node* create_node(node*, object);
node* push_node(node*, object);
node* pop_node(node*, object*);
void iterate(node*, callback);
void delete_list(node**);
int is_empty(node*);

#endif
