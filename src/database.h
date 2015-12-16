#ifndef CONFIG_H
#define CONFIG_H
#include <mysql/mysql.h>


typedef struct database_sin2tor{
 char table[256];
 MYSQL *conn;
}database_sin2tor;


struct database_sin2tor* create_database_sni2tor(char* server, char* database_name, char* table, char* username, char* password);

/**
 * @brief Allows to get the tor hidden service corresponding to a given hostname
 *        as stored in the database.
 * 
 * @param  database the database we should make the request to
 * @param hostname the hostname for wich we would like to know the tor hiddden service
 * @return the tor hiddden service address
 */
int request_corresponding_service(struct database_sin2tor *database, char* hostname, char requested_tor_hidden_service[256]);

int close_database_sin2tor(struct database_sin2tor *database);

#endif