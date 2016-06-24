#ifndef BIND_H
#define BIND_H
#include "torsocks.h"
#include "database.h"
/**
 * @brief This function called at the begining of a new connection allows to detect the domain name requested
 * 		in the SNI field, request the database for the corresponding tor hidden service, and prepares
 * 		the binding to this tor hidden service
 * 
 * @return void
 */
int init_binding_tls_to_tor(int socket1,struct TorSocket *socket2, struct database_sin2tor *, int port_tor);

/**
 * @brief Handles bi-directional binding between a regular socket and a tor socket.
 * 	this function ends when the connection is closed by peer on one of the connections
 * 
 * @return void
 */
void bind_socket_to_tor(int socket1,struct TorSocket *socket2);

#endif