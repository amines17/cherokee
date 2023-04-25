#include <stdlib.h>
#include <stdio.h>

#include "../headers/container_implementation.h"


vector_t* vect_init() 
{
    vector_t * vector = malloc(sizeof(vector_t));
    vector->capacity = 10;
    vector->size = 0;
    vector->data = malloc( sizeof(*vector->data) * 10);

    return vector;
}

void vect_push_back(vector_t * v, db_main_content_t * data_db) 
{
    if(v->size == v->capacity)
    {
        if(!v->capacity)
        {    
            v->capacity = 5;
        }

        v->capacity *= 2;
        v->data = realloc(v->data, sizeof(*v->data) * v->capacity);
    }

    v->data[v->size] = data_db;

    v->size++;
}

//on retire de la liste sans le detruire
db_main_content_t * vect_pop_back(vector_t* v)
{
    if(!v)
        return NULL;
    
    if(v->size)
    {
        db_main_content_t * res = v->data[v->size-1];
        
        v->data[v->size - 1] = NULL;
        v->size--; 

        return res;
    }

    return NULL;
} 

db_main_content_t * vect_get_element_index(vector_t* v, size_t index) 
{
    if(index < v->size)
        return v->data[index];

    return NULL;
}

void vect_free_content(vector_t* v) 
{
    for(int i = 0 ; i < v->size ; i++)
    {
        free(v->data[i]->image);
        free(v->data[i]->subtitle);
        free(v->data[i]->title);
        free(v->data[i]);
    }

    v->capacity = 0;
    v->size = 0;

    free(v->data);
    v->data = NULL;
}

void vect_delete_list(vector_t ** v)
{
    if(!*v)
        return;
    
    vect_free_content(*v);
    free(*v);

    *v = NULL;
}

//linked list
tree_t * tree_create(const char * ext, const char * v)
{
	tree_t * res = malloc(sizeof(tree_t));
	
	res->extension = strdup(ext);
	res->value = strdup(v);
	
	res->fils_gauche = NULL;
	res->fils_droite = NULL;
	
	return res;
}

void tree_add(tree_t * t, const char * ext, const char * v) 
{
	if(t == NULL)
		return;

	if(strcmp(t->extension, ext) > 0)
	{
		if(t->fils_gauche == NULL)
			t->fils_gauche = tree_create(ext,v);
		
		else
			tree_add(t->fils_gauche, ext, v);
	}
	
	else
	{
		if(t->fils_droite == NULL)
			t->fils_droite = tree_create(ext,v);
		
		else
			tree_add(t->fils_droite, ext, v);
	}
}

//remove
void tree_delete(tree_t ** t)
{
    if(!*t)
    {    
        return;
    }

	tree_delete(&(*t)->fils_gauche);
	tree_delete(&(*t)->fils_droite);

	free((*t)->extension);
	free((*t)->value);
	free(*t);

    *t = NULL;
}

void tree_delete_v1(tree_t * t)
{
    if(!t)
    {    
        return;
    }
    
	tree_delete_v1(t->fils_gauche);
	tree_delete_v1(t->fils_droite);

	free(t->extension);
	free(t->value);
	free(t);

    t = NULL;
}

const char * tree_get(tree_t * t, const char * ext)
{
	if(!t || !t->extension)
		return NULL;
		
	int gauche_ou_droite = strcmp(t->extension, ext);
	
	if(!gauche_ou_droite)
		return t->value;
	
	else if (gauche_ou_droite > 0)
		return tree_get(t->fils_gauche, ext);
		
	else
		return tree_get(t->fils_droite, ext);
} 

 