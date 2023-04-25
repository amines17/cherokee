// server_db.sqlite3 

#ifndef DB_MAN_H
#define DB_MAN_H

#include <stdbool.h>
#include <sqlite3.h> 

#include "../headers/container_implementation.h"


void init_db();

bool db_ajouter_content(const char* title, const char * image, const char * subtitle);

vector_t * db_get_list_content();

db_main_content_t * db_get_content_by_id(int id);

bool update_content(int id, const char* title, const char * image, const char * subtitle);

bool db_delete_content(int id); 

int db_callback_sql_response(void *data, int column, char **column_data, char **column_name);

void close_db();

#endif