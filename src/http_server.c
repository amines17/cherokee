#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h> 
//network related
#include <netdb.h>
#include <netinet/in.h>

#include "headers/http_server.h"
#include "headers/return_code.h"
#include "headers/content_type.h"
#include "db/db_man.h"

#define PORT 8080
#define NB_FORK 4


int create_and_launch_server() 
{
	int server_fd;
	struct sockaddr_in server_address;
    
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
	    perror("cannot create socket"); 
		exit(EXIT_FAILURE);  
	}
 		
	// initialisation de l'address
	memset((char *)&server_address, 0, sizeof(server_address)); 
	server_address.sin_family = AF_INET; 
	server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
	server_address.sin_port = htons(PORT); 

	int reuse = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
    {
		perror("setsockopt(SO_REUSEADDR) failed");
	}
	
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopt(SO_REUSEPORT) failed");
    }

    // bind
	if(bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
	{ 
    	perror("bind failed"); 
		exit(EXIT_FAILURE);  
	}   

	for(int i = 0 ; i < NB_FORK ; i++)
	{
		if(fork() == 0)
		{
			break;
		}	
	}

	if(listen(server_fd, 10) < 0) 
	{ 
		perror("In listen"); 
		exit(EXIT_FAILURE); 
	}

	return server_fd;
}

char* get_recv_data(int client_socket, ssize_t * size_out)
{
	ssize_t total_lenght = 0;

	char * buffer = malloc(1);

    buffer[0] = '\0';

    char recv_buffer[1024] = {0};

	ssize_t valread = 0; 

    while((valread = recv(client_socket, recv_buffer, 1023, MSG_DONTWAIT)) > 0) 
    {   
        total_lenght += valread;
                        
		char * temp = realloc(buffer, total_lenght + 1);

		if(temp == NULL)
		{
			exit(EXIT_FAILURE);
		}

		buffer = temp;
	 
        // pouvoir utiliser strcat
        recv_buffer[valread] = '\0';

        strcat(buffer, recv_buffer);

        buffer[total_lenght] = '\0';
	}
	
	buffer[total_lenght] = '\0';

	*size_out = total_lenght;
 
	return buffer;
}

int str_to_int(const char* str)
{
	char * end = NULL;

    long value = strtol(str, &end, 10); 
	
	if(end == str)
	{ 
		return -1;
	}

	return (int)value;
}

void traiter_demande_client(int client_socket)
{
	ssize_t total_lenght = 0;

	char * buffer = get_recv_data(client_socket, &total_lenght);

    if(total_lenght <= 0)
    { 
    	printf("No bytes are there to read \n");
	    free(buffer);

		return;
	}
                    
	else 
    {	
        printf("recv lenght: %ld \nrecv %s\n", total_lenght, buffer);
    }
  
	// on parse ici
	http_parse_header_t * client_list_data = get_header_with_data(buffer, total_lenght);

	free(buffer);

	if(!client_list_data)
	{
		http_server_send_code_page(client_socket, 500);
		close(client_socket);
		
        return;
	}
   
   	traitement(client_socket, client_list_data);

	destroy_parse_header(&client_list_data);
}

void traitement(int client_socket, http_parse_header_t * header) 
{
	bool verif_1 = false, verif_2 = false;
	
	int id = -1; 
	
	verif_1 = strcmp(header->lien, "/generate") == 0 || strcmp(header->lien, "/generate/") == 0;

	if(!verif_1)	
	{
		verif_2 = strstr(header->lien, "/generate/") == header->lien;
	}	

	if(verif_2)
	{		
		// ajouter verification
		id = str_to_int(header->lien + strlen("/generate/"));

		if(id < 1)
		{
			goto error;
		}
	}

	if(strcmp(header->method, "GET") == 0)
	{
		if(verif_1)
		{	
            http_server_read_all(client_socket, true, 200);
		}

		else if(verif_2) 
		{
			http_server_read(client_socket, id, true);
		}
		
		else
			http_server_get_page(client_socket, header->lien, true);
	}

	else if(strcmp(header->method, "HEAD") == 0)
	{
		if(verif_1)
		{	
			http_server_read_all(client_socket, false, 200);
		}

		else if(verif_2) 
		{
			http_server_read(client_socket, id, false);
		}
		
		else
			http_server_get_page(client_socket, header->lien, false);
	}
	
	else if(strcmp(header->method, "POST") == 0)
	{	
		if(!header->content_length)
		{
			goto error;
		}

		if(verif_1)
		{
			http_server_create(client_socket, header->body, true);
		}
			
		else if(verif_2) 
		{
			http_server_update(client_socket, id, header->body, true);
		}
		
		else
			http_server_post_page(client_socket, header->lien, header->body, atoi(header->content_length) , true);	
	}
	
	else if( strcmp(header->method, "PUT") == 0)
	{
		if(verif_1)
		{
			http_server_create(client_socket, header->body, false);
		}
			
		else if (verif_2) 
		{
			http_server_update(client_socket, id, header->body, false);
		}
		
		else
			http_server_post_page(client_socket, header->lien, header->body, atoi(header->content_length), false);	
	}
	
	else if( strcmp(header->method, "DELETE") == 0)
	{
		if(verif_2)
		{
			http_server_delete(client_socket, id);
		}

		// on n'accepte pas de tout supprimer ?
		else if (verif_1 || strcmp(header->method, "/index.html") == 0)
			goto error;
		
		else
			http_server_delete_page(client_socket, header->lien);	
	}

	else 
	{
		http_server_send_code_page(client_socket, 501);
	}

	goto end_error;

error:
	http_server_send_code_page(client_socket, 400); 
	
end_error :
    printf("end traitement \n");
}

void http_server_get_page(int client_socket, const char* link, bool is_method_get) 
{
    // Linux has a maximum filename length of 255 characters for most filesystems (including EXT4), and a maximum path of 4096 characters.
	char file_to_get[4097]; // on prend le aussi path etc 

	size_t link_size = strlen(link + 1);

    if(link_size)
        strcpy(file_to_get, link + 1);

	else
	    strcpy(file_to_get, "index.html");
    
	struct stat st;
	int stat_return_val = stat(file_to_get, &st);
		
	// si on ne trouve pas le fichier renvoi une error 404
	if(stat_return_val == -1)
	{
		http_server_send_code_page(client_socket, 404);
        return;
	}
    
    // si c'est un dossier
    if(S_ISDIR(st.st_mode))
    {
        http_send_directory_to_client(client_socket, file_to_get);
        return;
    }
    
	size_t file_size = st.st_size;

    ssize_t nb_h_write = http_send_header_to_client(client_socket, 200, file_size, get_content_type_from_filename(file_to_get)); 
	
    if(nb_h_write == -1)
    {
		perror("error ecriture header");
        return;
    }

    // selon la methode
    if(is_method_get)
    {
        http_send_file_to_client(client_socket, file_to_get);
    }
}

void http_server_delete_page(int client_socket, const char* link) 
{    
    // -> si oui on supprime et on envoie un fichier html de message de confirmation
    // -> si non on envoie message erreur avec la page qui va avec

    const char* filename = link + 1;

	struct stat st;
	int stat_return_val = stat(filename, &st);
		
	// si on ne trouve pas le fichier renvoi une error 404
	if(stat_return_val == -1)
	{
        http_server_send_code_page(client_socket, 404);
        return;    
    }
    
    int remove_return_code = remove(filename); 

    // quand c'est reussi: 
    if(remove_return_code == 0) 
    {
        ssize_t nb_h_write = http_send_header_to_client(client_socket, 204, 0, "text/plain"); 
	
        if(nb_h_write == -1)
        {
		    perror("error ecriture header");
            return;
        }

        printf("Reponse send\n\n");
    }

    else
    {
        http_server_send_code_page(client_socket, 403);   
    }
}

bool file_content_is_same(FILE *file_to_test, const char *content, int content_size)
{
    int current_content_pos = 0;
    
    char file_char = fgetc(file_to_test);

    // si content_size == 0
    if((file_char != EOF && content_size == 0) || file_char != content[current_content_pos])
    {
        return false;
    }

    current_content_pos++;

    do
    {
        file_char = fgetc(file_to_test);        

        if(file_char != content[current_content_pos])
        {
            return false;
        }

        current_content_pos++;

    }while(file_char != EOF && current_content_pos != content_size);
    
    // return flag status
    return true;
}

void http_server_post_page(int client_socket, const char* lien, const char* content, int content_lenght, bool is_method_post) 
{
    // on fait les cas de base 

    // creer le fichier puis l'afficher i guess

    /*
        -> si oui on supprime et on cree un fichier /link avec les donnees du body (prend en compte le mime) et envoie message qui dit que le fichier a ete mis a jour 
        -> si non on le cree et on envoie message qui dit que le fichier a ete cree avec succes 
    */

    if(!is_method_post)
    {
        FILE * file_to_test = fopen(lien + 1, "r");

        if(file_to_test)
        {
            if( file_content_is_same(file_to_test, content, content_lenght))
            {
                printf("content is same\n");
                fclose(file_to_test);   
                goto is_correct;
            }

            fclose(file_to_test);
        }
    }

    FILE * post_file = fopen(lien + 1, "w+");

    if(!post_file)
    {
        // si on veut creer un nouveau document dans un dossier qui n'existe pas
        http_server_send_code_page(client_socket, 403);         

        return;
    }

    size_t write_result = fwrite(content, sizeof(char), content_lenght, post_file);
    
    fclose(post_file);

    if(write_result != content_lenght)
    {
        // erreur ecriture
        perror("error dans l'ecriture du fichier sur le disque probablement \n");
        http_server_send_code_page(client_socket, 500);
    }

    else // reussite 
    {

is_correct :        

        http_server_send_code_page(client_socket, 201);
        printf("Reponse envoyee \n");
    }
}

void http_server_send_code_page(int cliend_fd, int error_code)
{
    const char* code_page_directory = "http_status_code";

    char code_page[50];

    snprintf(code_page, 50, "%s/%d.html", code_page_directory, error_code);

    // on recupere la taille du fichier error_page
    struct stat st;
	int stat_return_val = stat(code_page_directory, &st);
	
    if(stat_return_val == -1)
    {
        perror("Erreur fichier code");
        printf("Code to send : %d\n", error_code);
        printf("page to send : %s\n", code_page);
        
        return;
    }

    size_t file_size = st.st_size; 

    http_send_header_to_client(cliend_fd, error_code, file_size, "text/html");
    http_send_file_to_client(cliend_fd, code_page);
}

void http_send_file_to_client(int client_socket, const char* filename) 
{
	FILE * file_var = fopen(filename, "r");

	//envoie du body
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

	while((read = getline(&line, &len, file_var)) != -1) 
	{   		
		ssize_t nb_write = write(client_socket, line, read);

		if(nb_write == -1)
        {
            perror("error ecriture");
            break;
        }		
	}

	free(line);
    
	fclose(file_var);
}

void http_send_directory_to_client(int client_socket, const char* filename) 
{
    printf("filename %s\n",filename);

    DIR * directory = opendir(filename);
    
    if (directory) 
    {
        http_send_header_to_client(client_socket, 200, -1, "text/html");

        struct dirent * file_in_dir;

        struct stat st;
        char data_size[10];
        
        stat("template/folder_list_data_header", &st);
        
        int size_write = snprintf(data_size, 10, "%ld\r\n", st.st_size);
        write(client_socket, data_size, size_write);

        http_send_file_to_client(client_socket, "template/folder_list_data_header");
        write(client_socket, "\r\n", 2);  

        char template[] = "<tr> <td> <a href=\"%s/%s\"> %s </a> </td> </tr> \r\n";
        
        size_t template_size = strlen(template);

        while((file_in_dir = readdir(directory)) != NULL) 
        {
            size_t total_size = template_size + strlen(filename) + (strlen(file_in_dir->d_name) * 2) + 1;
            
            char * res = malloc(total_size);

            int snprintf_res = snprintf(res, total_size, template, filename, file_in_dir->d_name, file_in_dir->d_name);
            write(client_socket , res , snprintf_res);  
            
            free(res);
        }

        stat("template/folder_list_data_header", &st);
        size_write = snprintf(data_size, 10, "%ld\r\n", st.st_size);
        http_send_file_to_client(client_socket, "template/page_list_data_footer");
        write(client_socket, "\r\n", 2); 

        closedir(directory);
    }

    else 
    {
        http_server_send_code_page(client_socket, 500);
    }
}

ssize_t http_send_header_to_client(int client_socket, int return_code, long content_length_, const char* content_type_) 
{
    char first_line[50];
	char content_type[50];
	char content_lenght[50];

    size_t header_size = 0;

 	header_size += snprintf(first_line, 50, "HTTP/1.1 %d %s\r\n", return_code, get_return_code_string(return_code));
 	header_size += snprintf(content_type, 50, "Content-Type: %s\r\n", content_type_); 
    
    if(content_length_ > -1)
 	{
        header_size += snprintf(content_lenght, 50, "Content-Length: %ld\r\n\r\n", content_length_);
    }

    else 
    {
        header_size += snprintf(content_lenght, 50, "Transfer-Encoding: chunked\r\n\r\n");
    }

	char * header_to_send  = malloc(sizeof(char) * (header_size + 1));
		
	sprintf(header_to_send, "%s%s%s", first_line, content_type, content_lenght); 

	// envoie de l'header
	ssize_t nb_h_write = write(client_socket , header_to_send , header_size );

    free(header_to_send);

    return nb_h_write;
}

// remplace les caracteres speciaux
void replace_value(char *str) 
{
    int d = 0; 

    char eStr[] = "00"; 

    while(!d) 
    {    
        d = 1;
     
        for(int i = 0; i < strlen(str); ++i) 
        {
            if(str[i] == '+')
            {
                str[i] = ' ';
            }
            
            if(str[i] == '%') 
            {
                if(str[i+1] == 0)
                {   
                    return;
                }

                if(isxdigit(str[i+1]) && isxdigit(str[i+2])) 
                {
                    d = 0;

                    /* combine the next to numbers into one */
                    eStr[0] = str[i+1];
                    eStr[1] = str[i+2];

                    /* convert it to decimal */
                    long int x = strtol(eStr, NULL, 16);

                    /* remove the hex */
                    memmove(&str[i+1], &str[i+3], strlen(&str[i+3])+1);

                    str[i] = x;
                }
            }
        }
    }
}

db_main_content_t * http_server_string_to_db_object(const char * data)
{
    db_main_content_t * res = malloc(sizeof(db_main_content_t));

    res->index = -1;
    
    //title=123&image=456%40aaa.com&subtitle=chien  
    char * title_begin = strstr(data, "=") + 1;
    char * title_end = strstr(data, "&");
  
    char * image_begin = strstr(title_end + 1, "=") + 1;
    char * image_end = strstr(title_end + 1, "&");
      
    res->image = strndup(image_begin, image_end - image_begin);
    res->title = strndup(title_begin, title_end - title_begin);
    res->subtitle = strdup( strstr(image_end + 1, "=") + 1);    

    return res;
}

void http_delete_content(db_main_content_t * content)
{
    free(content->image);
    free(content->title);
    free(content->subtitle);
    free(content);
}

// generation 
void http_server_create(int client_fd, const char * data, bool is_method_post) 
{
    db_main_content_t * content = http_server_string_to_db_object(data);

    bool res = db_ajouter_content(content->title, content->image, content->subtitle);

    http_delete_content(content);

    if(res)
    {
        http_server_send_code_page(client_fd, 201);
    }

    else
    {
        http_server_send_code_page(client_fd, 400);
    }
}

void http_server_read(int client_fd, int index, bool is_method_get) 
{
    db_main_content_t * content =  db_get_content_by_id(index);

    if(!content)
    {
        http_server_send_code_page(client_fd, 404);
        return;
    }

    size_t size_total = 0;

    replace_value(content->title);
    replace_value(content->image);
    replace_value(content->subtitle);

    size_t t1 = strlen(content->title);
    size_t t2 = strlen( content->image);
    size_t t3 = strlen(content->subtitle);

    const char * page_to_send[] = {"template/page_data_header", "template/page_data_mid1", "template/page_data_mid2", "template/page_data_footer"};

    for(int i = 0 ; i < 4 ; i++)
    {
        struct stat st;    
        stat(page_to_send[i], &st);
        size_total += st.st_size;    
    }

    size_total += t1 + t2 + t3;

    http_send_header_to_client(client_fd, 200, size_total, "text/html"); 
    
    http_send_file_to_client(client_fd, page_to_send[0]);
    write(client_fd, content->title, t1); 

    http_send_file_to_client(client_fd, page_to_send[1]);
    write(client_fd, content->image, t2); 

    http_send_file_to_client(client_fd, page_to_send[2]);
    write(client_fd, content->subtitle, t3); 

    http_send_file_to_client(client_fd, page_to_send[3]);

    http_delete_content(content);
}

void http_server_read_all(int client_fd, bool is_method_get, int status) 
{
    vector_t * content_list = db_get_list_content();

    struct stat st;
    char data_size[10];

    http_send_header_to_client(client_fd, status, -1, "text/html");

    
	stat("template/page_list_data_header", &st);
    
    int size_write = snprintf(data_size, 10, "%ld\r\n",st.st_size);

    write(client_fd ,data_size, size_write);  

    http_send_file_to_client(client_fd, "template/page_list_data_header");
    
    write(client_fd , "\r\n", 2);  

    char template[] = "<tr> <td> %d </td> <td> %s </td> <td> <a href=\"/generate/%d\"> Lien </a>  </td> </tr> \r\n";

    size_t template_size = strlen(template);

    for(int i = 0 ; i < content_list->size ; i++)
    {
        db_main_content_t * current_content = vect_get_element_index(content_list, i);
        replace_value(current_content->title);
        size_t title_size = strlen(current_content->title); 
        size_t total_size = template_size + title_size + 20;
        char * res = malloc(total_size);
        int snprintf_res = snprintf(res, total_size, template, current_content->index, current_content->title, current_content->index);
        write(client_fd , res , snprintf_res );  
        
        free(res); 
    }

    stat("template/page_list_data_header", &st);
    
    size_write = snprintf(data_size, 10, "%ld\r\n",st.st_size);

    http_send_file_to_client(client_fd, "template/page_list_data_footer");
     
    write(client_fd ,"\r\n", 2);  
     
    vect_delete_list(&content_list);
}

void http_server_update(int client_fd, int index, const char* data, bool is_method_post) 
{
    db_main_content_t * value = http_server_string_to_db_object(data);

    if(update_content(index, value->title, value->image, value->subtitle))
    {
        http_server_send_code_page(client_fd, 201);
    } 

    else
    {
        http_server_send_code_page(client_fd, 403);
    }

    http_delete_content(value);
}

void http_server_delete(int client_fd, int index) 
{
    bool res = db_delete_content(index);

    if(res)
    {        
        http_server_read_all(client_fd, true, 204);
    }

    else
    {
        http_server_send_code_page(client_fd, 403); 
    }
}
