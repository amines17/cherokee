#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdbool.h>
#include <unistd.h>

#include "http_header_parser.h"


/* initialisation du serveur */
int create_and_launch_server();

// recevoir client
void traiter_demande_client(int);

void traitement(int socket, http_parse_header_t *);

char* get_recv_data(int client_socket, ssize_t *);

// GET HEAD
void http_server_get_page(int cliend_fd, const char* link, bool is_method_get);

// DELETE 
void http_server_delete_page(int cliend_fd, const char*);

// PUT POST 
void http_server_post_page(int cliend_fd, const char* lien, const char* content, int content_lenght, bool is_method_post);

void http_server_send_code_page(int cliend_fd, int error_code); 

void http_send_file_to_client(int client_fd, const char* filename );

void http_send_directory_to_client(int client_socket, const char* filename); 

ssize_t http_send_header_to_client(int client_fd, int return_code, long content_length, const char* content_type); 

void http_server_create(int client_fd, const char * data, bool);

void http_server_read(int client_fd, int index, bool);

void http_server_read_all(int client_fd, bool, int );

void http_server_update(int client_fd, int index, const char* data, bool);

void http_server_delete(int client_fd, int index);

#endif