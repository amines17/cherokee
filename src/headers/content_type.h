#ifndef CONTENT_TYPE_H
#define CONTENT_TYPE_H

#include "container_implementation.h"

void init_global_content_struct(); 

void delete_global_content_struct(); 

const char * get_content_type_from_extension(const char * extension); 

const char * get_content_type_from_filename(const char* filename);

#endif