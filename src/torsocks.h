#ifndef TORSOCKS_H
#define TORSOCKS_H

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

struct TorSocket{
  struct sockaddr_in server;
  int sock;
  int isconnected;
  char * host;
  int port;
};

int connect_tor_socket(struct TorSocket *, char* host, int port);
int send_data_to_tor_socket(struct TorSocket *, char* data, int datalen);
int receive_data_from_tor_socket(struct TorSocket *, char* data, int maxsize);
int availiable_data_in_tor_socket(struct TorSocket *);
int close_tor_socket(struct TorSocket *);
int is_tor_socket_closed(struct TorSocket *);
void set_read_timeout_tor_socket(struct TorSocket *,int ms);
#endif