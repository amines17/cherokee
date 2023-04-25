#ifndef RETURN_CODE_H
#define RETURN_CODE_H

#include "container_implementation.h"

void init_return_code_global_struct();

void delete_return_code_global_struct();

const char * get_return_code_string(int return_code);

#endif