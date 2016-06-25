#include "bind.h"
#include <unistd.h>
#include "tls.h"

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
	      return -1;
            connect_tor_socket(socket2,res_request, port_tor);
        }
        else
        {
            printf("Error while fetching sni name\n");
        }
        //Send the message back to client
        send_data_to_tor_socket(socket2,client_message,read_size);
        //write(client_sock , client_message , strlen(client_message));
    }
}

void bind_socket_to_tor(int socket1, struct TorSocket* socket2)
{
    char client_message[20000];
    char tor_response[20000];    
    struct timeval tv;
    int read_size;
    int client_nodata_counter=0;
    int tor_nodata_counter=0;    
    
    tv.tv_sec = 0;  
    tv.tv_usec = 10000;  

    setsockopt(socket1, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
    set_read_timeout_tor_socket(socket2,10);
    //Receive a message from client
    while(1)
    {

        read_size = recv(socket1 , client_message , 20000 , 0);


        //Send the message back to client
        if (read_size>0)
        {
            send_data_to_tor_socket(socket2,client_message,read_size);
            // read_size =0;
        }

        if (read_size==0)
        {
	    
            printf("No data from client\n");
	    close(socket1);
            return;
        }
        
//        if (read_size<0)
//        {
//            printf("Disconected from client\n");
//	    close(socket1);
//        }        

        int len=receive_data_from_tor_socket(socket2,tor_response,20000);
        if (len>0)
        {
            int res;
            res=write(socket1 , tor_response , len);
        }

        if (len==0)
        {
            printf("Disconected from tor\n");
       //    sleep(1);
	    tor_nodata_counter++;
	    if (tor_nodata_counter>60)
	    {	    
            close(socket1);
            return;
	    }
        }

    }
}

