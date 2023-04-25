#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <unistd.h> 
//network related
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <stdbool.h>
#include <signal.h>
 
#include "headers/content_type.h"
#include "headers/return_code.h"
#include "headers/http_header_parser.h"
#include "headers/http_server.h"
#include "db/db_man.h"

#define MAX_CLIENTS 100 


bool should_continue = true;

// ctrl + c safe
void signal_handler(int signo) 
{
    printf("signal number %d \n", signo);
	should_continue = false;
    
	signal(signo, SIG_DFL);
}

int main(int argc, char**argv)
{
	// gestion ctrl + c
	signal(SIGINT, signal_handler); 
	signal(SIGPIPE, SIG_IGN); 

	// initialisation db
	init_db();

	//creation du socket	
	int server_fd = create_and_launch_server();  
	
	// initialisation des struct 
	init_global_content_struct();
	init_return_code_global_struct();

    int nb_client = 0;

    struct pollfd pollfds[ MAX_CLIENTS];
    
    for(int i = 0; i < MAX_CLIENTS; i++)
    {   
		pollfds[i].fd = 0;
		pollfds[i].events = 0;
		pollfds[i].revents = 0;
	}
    
    pollfds[0].fd = server_fd;
    pollfds[0].events = POLLIN; 

	printf("Wait for connection\n");

	nb_client = 1;

	// notre serveur attend de nouvelles connexion
	while(should_continue)
	{
		int poll_res = poll(pollfds, nb_client , -1);

        // verification
        if(poll_res > 0)
        {
            if (pollfds[0].revents & POLLIN)
            {
                struct sockaddr_in address;
                socklen_t addrlen = sizeof(address);

				printf("Connection found\n");
                
				int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                
				printf("accept value %d \n", client_socket);

				if(client_socket != -1)
				{
					for (int i = 1; i < MAX_CLIENTS; i++)
					{
						if(pollfds[i].fd == 0)
						{
							pollfds[i].fd = client_socket;
							pollfds[i].events = POLLIN;
							nb_client++;

							break;
						}
					}
				}

				else
				{
					perror("error accept");
				}
            }

            for (int i = 1; i < MAX_CLIENTS; i++)
            {
                 
                if(pollfds[i].fd > 0 && pollfds[i].revents & POLLIN)
                { 
					traiter_demande_client(pollfds[i].fd);

                    close(pollfds[i].fd);

                    pollfds[i].fd = 0;
                    pollfds[i].events = 0;
                    pollfds[i].revents = 0;
                    nb_client--;                  
                    
                }
            }
        }				
	}
 
	close(server_fd);

	delete_global_content_struct();
	delete_return_code_global_struct(); 

	close_db();
 
	return EXIT_SUCCESS;
}