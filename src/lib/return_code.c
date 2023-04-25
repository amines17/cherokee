#include <stdbool.h>
#include <stdio.h>

#include "../headers/return_code.h"


tree_t * return_code_tree = NULL;  

void init_return_code_global_struct()
{
    printf("init return code \n");
    
    //creer avec la base
    return_code_tree = tree_create("200", "OK");		

    //texte 	 	
    tree_add(return_code_tree, "201", "Created");		
    tree_add(return_code_tree, "204", "No Content");		
    tree_add(return_code_tree, "400", "Bad Request");		
    tree_add(return_code_tree, "403", "Forbidden");		
    tree_add(return_code_tree, "404", "Not Found");		
    tree_add(return_code_tree, "500", "Internal Server Error");		
    tree_add(return_code_tree, "505", "HTTP Version not supported");		
}

void delete_return_code_global_struct()
{
    if(return_code_tree)
    {
        tree_delete(&return_code_tree);
    }
}

const char * get_return_code_string(int return_code) 
{
    if(!return_code_tree)
        init_return_code_global_struct();

    char code[4];

    snprintf(code ,4,"%d" ,return_code);

    return tree_get(return_code_tree, code);
}