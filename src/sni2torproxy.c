/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

#include "torsocks.h"
#include "tls.h"
#include "protocol.h"
#include "database.h"
#include <stdlib.h>
#include <linux/ioctl.h>
#include <unistd.h>
#include "bind.h"
#include <unistd.h>
#include <getopt.h>

void print_usage();
int getDataAvailliable (int socket);

    
static struct option long_options[] =
        {
          {"dbuser",  required_argument, 0, 'u'},
          {"dbpasswd",    required_argument, 0, 'p'},
          {"dbname",    required_argument, 0, 'n'},	  
          {"listening-port",  required_argument, 0, 'l'},
          {"forwarding-port",    required_argument, 0, 'f'},	  
          {0, 0, 0, 0}
        };
// 	    static struct option long_options[] = {
//         {"area",      no_argument,       0,  'a' },
//         {"perimeter", no_argument,       0,  'p' },
//         {"length",    required_argument, 0,  'l' },
//         {"breadth",   required_argument, 0,  'b' },
//         {0,           0,                 0,  0   }
//     };
// 	
int main(int argc , char *argv[])
{

    struct database_sni2tor *database;
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    struct TorSocket s;
    pid_t pid;
    int opt = 0;
    int listening_port=8888;
    int tor_hidden_services_port=443;
    char dbUser[256]="proxyuser";
    char dbPasswd[256]="proxypasswd";
    char dbname[256]="sni2tor-proxy";

    
   int option_index = 0;
    while ((opt = getopt_long (argc, argv, "u:p:l:f:",long_options, &option_index)) != -1) {
      printf("%d\n", opt);
        switch (opt) {
             case 'l' :  listening_port=atoi(optarg);
                 break;
             case 'f' :  tor_hidden_services_port=atoi(optarg);
                 break;
             case 'u' :  strcpy(dbUser,optarg);
                 break;
             case 'p' :  strcpy(dbPasswd,optarg);
                 break;	
             case 'n' :  strcpy(dbname,optarg);
                 break;			 
             default: print_usage(); 
                 //exit(EXIT_FAILURE);
        }
    }
    
printf("BDD config: %s %s %s\n",dbUser,dbPasswd,dbname);
     
     /************************************************************
     * 
     * 	Binds to socket port we should be listening to
     * 
     * **********************************************************/
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");


    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( listening_port );
    
    printf("Listening to port: %d\n",listening_port);


    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }


    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    
    /************************************************************
     * 
     * 			Forking loop		
     * 
     * **********************************************************/
    do
    {
        //Accept and incoming connection
        puts("Waiting for incoming connections...");
        c = sizeof(struct sockaddr_in);

        //accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            return 1;
        }

        pid=fork();
    }
    while(pid!=0);

    /************************************************************
     * 
     * 		Child Job: handle connection with peer		
     * 
     * **********************************************************/
    puts("Connection accepted");

    //Create database
    database=create_database_sni2tor("localhost", dbname,"Association",dbUser,dbPasswd);


    //init tls binding
    init_binding_tls_to_tor(client_sock,&s,database,tor_hidden_services_port);

    //Bind until done
    bind_socket_to_tor(client_sock,&s);
  
    puts("Bye!\n");
    exit 0;
}

void print_usage()
{
printf("Usage:??\n");
}

