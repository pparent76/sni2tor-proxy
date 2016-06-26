#include "torsocks.h"
#include <fcntl.h>
#include <sys/ioctl.h>

int connect_tor_socket(struct TorSocket* torsock, char* host, int port)
{
  
    unsigned char message[1000] , server_reply[2000];
     
    //Create socket
    torsock->sock = socket(AF_INET , SOCK_STREAM , 0);
    if (torsock->sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    torsock->server.sin_addr.s_addr = inet_addr("127.0.0.1");
    torsock->server.sin_family = AF_INET;
    torsock->server.sin_port = htons( 9050 );
 
    //Connect to remote server
    if (connect(torsock->sock , (struct sockaddr *)&torsock->server , sizeof(torsock->server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
    
    message[0]=5;message[1]=1;message[2]=0;message[3]='\0';
         
        //Send some data
        if( send(torsock->sock , message , 3 , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
         
        //Receive a reply from the server
        if( recv(torsock->sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            return 0;
        }
         
        puts("Server reply :");
        printf("%d %d\n",(int)server_reply[0],(int)server_reply[1]);
	
	    
    message[0]=5;message[1]=1;message[2]=0;message[3]=3;

    char length[2]; length[0]= strlen(host);length[1]= 0;
    char portC[2];
    portC[1]=port%256; portC[0]=port/256;
         
        //Send some data
        if( send(torsock->sock , message , 4 , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
                        //Send some data
        if( send(torsock->sock , length , 1 , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
                //Send some data
        if( send(torsock->sock , host , strlen(host) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        if( send(torsock->sock , portC , 2 , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }        
        
                 
        //Receive a reply from the server
        if( recv(torsock->sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            return 0;
        }
         
        puts("Server reply :");
        printf("%d %d %d %d %d %d\n",(int)server_reply[0],(int)server_reply[1],(int)server_reply[2],(int)server_reply[3],(int)server_reply[4],(int)server_reply[5]);
	
	//fcntl(torsock->sock, F_SETFL, O_NONBLOCK);
}

int send_data_to_tor_socket(struct TorSocket* torsocket, char* data, int datalen)
{
	if( send(torsocket->sock , data , datalen, 0) < 0)
        {
            puts("Send failed");
            return 0;
        }    
        return 1;
        
}

int availiable_data_in_tor_socket(struct TorSocket* s)
{
int count;
ioctl(s->sock, FIONREAD, &count);
return count;
}


int receive_data_from_tor_socket(struct TorSocket* torsocket, char* data, int maxsize)
{

    int datalen= recv(torsocket->sock , data , maxsize , 0);       
        return datalen;
}



int close_tor_socket(struct TorSocket* torsock)
{
close(torsock->sock);
}

void set_read_timeout_tor_socket(struct TorSocket *sock,int ms)
{
  	
    struct timeval tv;
    
    tv.tv_sec = ms/1000;  /* 30 Secs Timeout */
    tv.tv_usec = (ms%1000)*1000;  // Not init'ing this can cause strange errors

    setsockopt(sock->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
}

int is_tor_socket_closed(struct TorSocket* s)
{
  fd_set rfd;
  FD_ZERO(&rfd);
  FD_SET(s->sock, &rfd);
  struct timeval tv = { 0 };
  select((s->sock)+1, &rfd, 0, 0, &tv);
  if (!FD_ISSET(s->sock, &rfd))
    return 0;
  int n = 0;
  ioctl(s->sock, FIONREAD, &n);
  return n == 0;
}

