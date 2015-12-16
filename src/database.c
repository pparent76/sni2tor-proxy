#include "database.h"
#include <string.h>
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

struct database_sin2tor * create_database_sni2tor(char * server, char* database_name, char* table, char* username, char* password)
{
  
struct database_sin2tor * res;
res=malloc(sizeof (struct database_sin2tor ));
printf("size table %d %s\n", strlen(table),table);
strcpy( res->table,table);
res->conn= mysql_init(NULL);
   /* Connect to database */
   if (!mysql_real_connect(res->conn, server,
         username, password, database_name, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(res->conn));
      return res;
   }

   return res;
   
}


int request_corresponding_service(struct database_sin2tor* database, char* hostname, char requested_tor_hidden_service[256])
{
  
   MYSQL_RES *res;
   MYSQL_ROW row;
   
   char query[2000]="SELECT torservice FROM ";
   strcat( query, database->table);
   strcat(query," WHERE hostname='");
   strcat(query,hostname);   
   strcat(query,"'"); 
   
   printf("SQL query: %s\n",query);
   /* send SQL query */
   if (mysql_query(database->conn, query)) {
      fprintf(stderr, "%s\n", mysql_error(database->conn));
      return 0;
   }
   
   res = mysql_use_result(database->conn);
   
   if ((row = mysql_fetch_row(res)) != NULL)
    strcpy(requested_tor_hidden_service,row[0]);
   else
     return 0;
   
   /* close connection */
   mysql_free_result(res);
   
   return 1;
}

int close_database_sin2tor(struct database_sin2tor* database)
{
mysql_close(database->conn);
}

