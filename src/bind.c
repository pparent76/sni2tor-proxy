#include "bind.h"
#include <unistd.h>
#include "tls.h"
#include <sys/ioctl.h>
#include <unistd.h>

#define IDLE_TIME_US 50000
#define BIND_TIMEOUT_S 300

int init_binding_tls_to_tor(int socket1, struct TorSocket* socket2, struct database_sin2tor * database, int port_tor)
{
    char client_message[20000];
    char res_request[256];
    int read_size;      
    
    //Read first message in order to determine host.
    if( (read_size = recv(socket1 , client_message , 20000 , 0)) > 0 )
    {
        char* hostname=NULL;
        if (tls_protocol->parse_packet(client_message,read_size,&hostname)!=-1 && hostname!=NULL)
        {
            printf("hostname: %s\n",hostname);
            request_corresponding_service(database,hostname,res_request);
            printf("Association: %s\n",res_request);
	    if (strlen(res_request)==0)
	      strcpy(res_request,"472v2u6j7znf5c3y.onion");
            connect_tor_socket(socket2,res_request, port_tor);
        }
        else
        {
            printf("Error while fetching sni name\n");
	    return -1;
        }
        //Send the message back to client
        send_data_to_tor_socket(socket2,client_message,read_size);
        //write(client_sock , client_message , strlen(client_message));
    }
}

int isclosed(int sock) {
  fd_set rfd;
  FD_ZERO(&rfd);
  FD_SET(sock, &rfd);
  struct timeval tv = { 0 };
  select(sock+1, &rfd, 0, 0, &tv);
  if (!FD_ISSET(sock, &rfd))
    return 0;
  int n = 0;
  ioctl(sock, FIONREAD, &n);
  return n == 0;
}

void bind_socket_to_tor(int socket1, struct TorSocket* socket2)
{
    char client_message[20000];
    char tor_response[20000];
    struct timeval tv;
    int read_size;
    int client_nodata_counter=0;
    int tor_nodata_counter=0;
    int count_tor;
    int count_sni;
    int global_counter=0;

    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    


    setsockopt(socket1, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
    set_read_timeout_tor_socket(socket2,10);
//Receive a message from client
    while(1)
    {


	//See how much data we have from sni client
        ioctl(socket1, FIONREAD, &count_sni);
        if (count_sni > 0)
        {
            read_size = recv(socket1 , client_message , 20000 , 0);
            //Send the message back to client
             send_data_to_tor_socket(socket2,client_message,read_size);


        }

	//See how much data we have from tor client  
        count_tor=availiable_data_in_tor_socket(socket2);
        if (count_tor > 0)
        {
            int len=receive_data_from_tor_socket(socket2,tor_response,20000);
       
            int res;
            res=write(socket1 , tor_response , len);
        }
      
	 if (isclosed(socket1))
	 {
	   printf("Connection closed by sni client\n");
	   shutdown(socket1,SHUT_RDWR);
	   usleep(IDLE_TIME_US);
	   close(socket1);
	   close_tor_socket(socket2);	   
           return;
	 }
	 
	 if (is_tor_socket_closed(socket2))
	 {
	   printf("Connection closed by tor client\n");
	   shutdown(socket1,SHUT_RDWR);
	   usleep(IDLE_TIME_US);
	   close(socket1);
	   close_tor_socket(socket2);
           return; 
	 }
	 
        //If we have received no data from any socket
        if (count_tor==0 && count_sni==0)
        {
            if (global_counter >1)
            {
                usleep(IDLE_TIME_US);
            }
            global_counter++;
        }
        else
            global_counter=0;
	

        if (global_counter> (BIND_TIMEOUT_S*1000000)/IDLE_TIME_US)
        {
            printf("Inactive for too long closing socket\n");
	    shutdown(socket1,SHUT_RDWR);
	    usleep(IDLE_TIME_US);
	    close(socket1);
	    close_tor_socket(socket2);
            return;
        }
    }
}

