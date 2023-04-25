#include <stdio.h>
#include <string.h>

#include "db_man.h"


sqlite3 *database;

void init_db()
{
    int rc = sqlite3_open("db/server.db", &database);
    
    if( rc ) 
    {
        printf("Can't open database: %s\n", sqlite3_errmsg(database));
        
        // db_is_open = true;
        return;
    }

    else 
    {
        printf("Opened database successfully\n");
    }

    char * create_main_table = "CREATE TABLE IF NOT EXISTS my_table ( id integer PRIMARY KEY not null , title TEXT , image TEXT, subtitle TEXT); ";

    char * error_message ; 

    /* Execute SQL statement */
    // rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    rc = sqlite3_exec(database, create_main_table, 0, 0, &error_message);
   
    if( rc != SQLITE_OK )
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
    } 
    
    else 
    {
        printf("Table created successfully\n");
    }
}

bool db_ajouter_content(const char *title, const char *image, const char *subtitle)
{    
    size_t s1 = strlen(title); 
    size_t s2 = strlen(image); 
    size_t s3 = strlen(subtitle); 

    size_t text_lenght =  s1 + s2 + s3 + 100;
    char * sql_command = malloc( text_lenght );
    
    snprintf(sql_command, text_lenght, "insert into my_table(title,image,subtitle) values(\"%s\",\"%s\",\"%s\")", title, image, subtitle);

    char * error_message;
    
    int rc = sqlite3_exec(database, sql_command, 0, 0, &error_message);
   

    if(rc != SQLITE_OK) 
    {
        printf("SQL error: %s\n", error_message);
        printf("SQL command: %s\n", sql_command);
        sqlite3_free(error_message);
        
        free(sql_command);
        
        return false;
    } 
   
    free(sql_command);
    return true;
}

vector_t * db_get_list_content()
{
    vector_t * res = vect_init();
    
    char * sql_command = "select id, title, image, subtitle from my_table";

    char * error_message;
    
    int rc = sqlite3_exec(database, sql_command, db_callback_sql_response, (void*)res, &error_message);
   
    if(rc != SQLITE_OK) 
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        
        return NULL;
    } 
    
    return res;
}

db_main_content_t * db_get_content_by_id(int id)
{
    char sql_command[200];

    snprintf( sql_command, 200, "select id, title, image, subtitle from my_table where id = %d", id );
    
    vector_t * res = vect_init();

    char * error_message;

    int rc = sqlite3_exec(database, sql_command, db_callback_sql_response, (void*)res, &error_message);
   
    if(rc != SQLITE_OK) 
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        
        return NULL;
    } 
   
    db_main_content_t * return_val = vect_pop_back(res);

    vect_free_content(res);
    
    free(res);

    return return_val;
}

bool db_delete_content(int id)
{
    char sql_command [100];  
 
    snprintf(sql_command, 100, "delete from my_table where id = %d", id );

    char * error_message;
    
    int rc = sqlite3_exec(database, sql_command, 0, 0, &error_message);
   
    if(rc != SQLITE_OK) 
    {
        printf("SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        
        return false;
    } 
   
    return true;
}

bool update_content(int id, const char* title, const char * image, const char * subtitle) 
{
    size_t s1 = strlen(title); 
    size_t s2 = strlen(image); 
    size_t s3 = strlen(subtitle); 

    size_t text_lenght =  s1 + s2 + s3 + 100;
    char * sql_command = malloc( text_lenght);
    
    snprintf(sql_command, text_lenght, "insert or replace into my_table(id,title,image,subtitle) values(%d, \"%s\",\"%s\",\"%s\")", id, title, image, subtitle);

    char * error_message;
    
    int rc = sqlite3_exec(database, sql_command, 0, 0, &error_message);

    if(rc != SQLITE_OK) 
    {
        printf("SQL error: %s\n", error_message);
        printf("SQL command: %s\n", sql_command);
        sqlite3_free(error_message);
        
        free(sql_command);
        return false;
    } 
   
    free(sql_command);
    
    return true;
}

int db_callback_sql_response(void *data, int column, char **column_data, char **column_name)
{  
    vector_t * current_vect = (vector_t*) data;

    db_main_content_t * res = malloc(sizeof(db_main_content_t));
   
    res->index = -1;
    res->image = NULL;
    res->subtitle = NULL;
    res->title = NULL;

    for(int i = 0; i < column; i++)
    {
        if(strcmp(column_name[i],"id") == 0)
        {
            res->index = atoi(column_data[i]);    
        }
        
        else if(strcmp(column_name[i],"title")  == 0)
        {
            res->title = strdup(column_data[i]);    
        }
        
        else if(strcmp(column_name[i],"image")  == 0)
        {
            res->image = strdup(column_data[i]);    
        }
     
        else if(strcmp(column_name[i],"subtitle")  == 0)
        {
            res->subtitle = strdup(column_data[i]);    
        }
    }
    
    vect_push_back(current_vect, res);
    
    return 0;
}

void close_db()
{
    sqlite3_close(database);
}
