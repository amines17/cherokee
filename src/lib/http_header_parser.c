#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../headers/http_header_parser.h"


http_parse_header_t * create_struct_parse()
{
	http_parse_header_t * res = malloc(sizeof(http_parse_header_t));

	res->lien = NULL;
	res->method = NULL;
	res->http_version = NULL;
	res->content_type = NULL;
	res->content_length = NULL;
	res->body = NULL;

	return res;
}

void destroy_parse_header(http_parse_header_t ** h) 
{
	if(!h || !*h)
		return;
    
	http_parse_header_t * header = *h;

	free(header->lien); 
	free(header->method);
	free(header->http_version);
	free(header->content_type);
	free(header->content_length);
	free(header->body);

	free(header);

	header = NULL;
}

void insert_body(http_parse_header_t* header, const char * data, size_t data_len) 
{  
    if(!header)
        return;
    
    header->body = malloc(sizeof(char) * data_len + 1);
    memcpy(header->body, data, data_len);
    
    header->body[data_len] = '\0';
}

void parse_line_header(http_parse_header_t * header, const char * line)
{
    
    if(!header)
    {
        return;
    }
		
    char * copy_line = strdup(line);
    char * save_ptr;
    char * val[2] = {0, 0};
    char separators[][2] = {": ", "\r\n"};
    char * token = strtok_r(copy_line, separators[0], &save_ptr);
    int i;

	for(i = 0 ; i < 2 ; i++)
    {
	    int len_read = strlen(token);
	
	    val[i] = malloc(sizeof(char) * len_read + 1);
	        
        memcpy(val[i], token, len_read);

        val[i][len_read] = '\0';

        if(i == 0)
            token = strtok_r(NULL, separators[i+1], &save_ptr);
    }

    if(strcasecmp(val[0] , "Content-Length") == 0)
        header->content_length = val[1];

    else if(strcasecmp(val[0] , "Content-Type") == 0)
        header->content_type = val[1]; 

    else 
    {
        free(val[1]);
    }

    free(val[0]);
	free(copy_line);
}

void parse_first_line(http_parse_header_t* header, const char* first_line)
{
    if(!header)
    {    
        return;
    }

	char * copy_first_line = strdup(first_line);
	
	char** res = malloc(sizeof(char*) * 3);
	
    char * save_ptr;

	/**************************************/
	/******** obtenir method ici **********/
	
    char * token  = strtok_r(copy_first_line, " ", &save_ptr);
    
    for(int i = 0 ; i < 2 ; i++)
    {
        int a = strlen(token);
        res[i] = malloc(sizeof(char) * a + 1);
        res[i] = memcpy(res[i], token, a);
        res[i][a] = '\0';

        if(i == 0)
	        token  = strtok_r(NULL, " ", &save_ptr);
    }
		
	token = strtok_r( NULL, "\r\n", &save_ptr );

	/******** verification http/1.1 ici ********/
    size_t a = strlen(token);
	
	res[2] = malloc(sizeof(char) * a + 1);
	
	for(size_t i = 0; i < a; i++)
	{
		res[2][i] = token[i];
	}
	
	res[2][a] = '\0';

    free(copy_first_line);
	
	header->lien = res[1];
	header->method = res[0];
	header->http_version = res[2];

    free(res);
}

http_parse_header_t * get_header_with_data(const char * data_client, size_t data_size) 
{
    char * body = strstr(data_client, "\r\n\r\n");

    if(!body)
    {
        return NULL;
    }
    
    http_parse_header_t * res = create_struct_parse();
    
    char * save_ptr;

    //on se charge du header ici
    size_t header_size = body - data_client;
    
    char * header = malloc(sizeof(char) * (header_size + 1));
	
	memcpy(header, data_client, header_size);

    header[header_size] = '\0';

    char * token = strtok_r(header, "\r\n", &save_ptr);

    //la premiere ligne est le header
    parse_first_line(res, token);

	size_t body_length = 0;

    //les autres lignes sont le reste
    while((token = strtok_r(NULL, "\r\n", &save_ptr)))
    {
        parse_line_header(res, token);
    }

    if(res->content_length)
    {    
        sscanf(res->content_length, "%zu", &body_length);	
    }

	if(body_length)
	{	
    	insert_body(res, data_client + data_size - body_length , body_length);
	}

    free(header);

    return res;
}
