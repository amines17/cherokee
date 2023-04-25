//vu qu'on a pas le droit a une lib, on va utiliser ca pour l'instant

#ifndef HTTP_HEADER_PARSER_H
#define HTTP_HEADER_PARSER_H

#include <stddef.h>
#include <stdlib.h>
 
typedef struct  {
	char * lien ;
	char * method;
	char * http_version;
	char * content_type;
	char * content_length;
	char * body;
} http_parse_header_t;

http_parse_header_t * create_struct_parse();

void destroy_parse_header(http_parse_header_t ** h);

//renvoie la list qui contient tous les donnees comme dans un map
http_parse_header_t * get_header_with_data(const char * , size_t);

void parse_first_line(http_parse_header_t*, const char* first_line);

void parse_line_header(http_parse_header_t*, const char * line);

#endif