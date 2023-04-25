#ifndef CONTAINER_IMPLEMENTATION_H
#define CONTAINER_IMPLEMENTATION_H

#include <stdlib.h>
#include <string.h>
 

// db
typedef struct 
{
    int index;
    char * title;
    char * image;
    char * subtitle;
} db_main_content_t;

//vector
typedef struct 
{
    /* data */
    size_t capacity;
    size_t size; 
    db_main_content_t ** data; 
}vector_t;

vector_t*  vect_init();

void vect_push_back(vector_t * v, db_main_content_t * data);

// on retire de la liste sans le detruire
db_main_content_t * vect_pop_back(vector_t* v);

db_main_content_t * vect_get_element_index(vector_t* v, size_t index);

void vect_free_content(vector_t* v );

void vect_delete_list(vector_t ** v);

//tree  
typedef struct tree{
	
	char * extension;
	char * value;
	struct tree * fils_gauche;
	struct tree * fils_droite;
} tree_t;

tree_t * tree_create(const char * ext, const char * v);

void tree_add(tree_t * t, const char * ext, const char * v);

//delete
void tree_delete(tree_t ** t);

void tree_delete_v1(tree_t * t);

const char * tree_get(tree_t * t, const char * ext); 

#endif