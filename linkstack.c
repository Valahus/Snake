#include "linkstack.h"

node* create_node(node* next, object obj){

  node * n_node = malloc(sizeof(node));
  if(!n_node){
    perror("Failed to create new node");
    exit(1);
  }

  n_node->data = obj;
  n_node->nxt = next;

  return n_node;  

}

node* push_node(node* hd, object obj){
  node* node = create_node(hd, obj);
  hd = node;
  return hd;
}

node* pop_node(node* hd, object * obj){
  if(!hd)
    return NULL;
  node* t_node = hd;
  if(obj)
    *obj = hd->data;
  hd = hd->nxt;
  free(t_node);
  return hd;
}

void iterate(node* hd, callback f){
  node* c_node = hd;
  while(c_node){
    f(c_node);
    c_node = c_node->nxt;
  }
}

void delete_list(node** hd){
  while(*hd)
    *hd = pop_node(*hd, NULL);
}

int is_empty(node* hd){
  return hd ? 0 : 1;
}
