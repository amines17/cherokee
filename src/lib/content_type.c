#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../headers/content_type.h"


tree_t * content_type_tree = NULL;

void init_global_content_struct()
{
	printf("init content type \n");
    
    //creer avec la base
    content_type_tree = tree_create(".txt", "text/plain");		
    
    //html-css-js	
    tree_add(content_type_tree, ".html", "text/html");	
    tree_add(content_type_tree, ".htm", "text/html");		
    tree_add(content_type_tree, ".js", "application/javascript");		
    tree_add(content_type_tree, ".css", "text/css");		
    
    //json 	
    tree_add(content_type_tree, ".json", "application/json");		
    
    //image	
    tree_add(content_type_tree, ".apng", "image/apng");	
    tree_add(content_type_tree, ".png", "image/png");	
    tree_add(content_type_tree, ".webp", "image/webp");	
    tree_add(content_type_tree, ".svg", "image/svg+xml");	
    tree_add(content_type_tree, ".jpg", "image/jpeg");	
    tree_add(content_type_tree, ".jpeg", "image/jpeg");	
    tree_add(content_type_tree, ".gif", "image/gif");	
    tree_add(content_type_tree, ".avif", "image/avif");		
    tree_add(content_type_tree, ".ico", "image/x-icon");
}

const char * get_content_type_from_extension(const char * extension)
{
	if(extension)
    {    
        const char * val = tree_get(content_type_tree, extension);

        if(val)
            return val;
    }     

    return "text/plain";
}

const char * get_content_type_from_filename(const char* filename) 
{
    if(!filename)
    {
        return "text/plain";
    }

    //recuperer extension
    size_t filename_size = strlen(filename);
    const char * filename_end = filename + filename_size;
    const char * index_dernier_point;
    bool found = false;

    for(index_dernier_point = filename_end; index_dernier_point != filename; index_dernier_point--)
    {
        if(*index_dernier_point == '.')
        {
            found = true;
            break;
        }
    }

    const char * extension = NULL;
    
    if(found)
        extension = index_dernier_point;

    if(!content_type_tree)
        init_global_content_struct();
        
    const char * extension_type = get_content_type_from_extension(extension);

    return extension_type ;
}
 
void delete_global_content_struct()
{
    if(content_type_tree)
    {
        tree_delete(&content_type_tree);
    }
}