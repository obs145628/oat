#include "tcp-client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "tcp-socket.h"

s_tcp_client* tcp_client_new(const char* host, int port)
{
   s_tcp_client* client;
   struct addrinfo hints;

   client = malloc(sizeof(s_tcp_client));
   client->port = port;

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   sprintf(client->portBuffer, "%d", port);

   if(getaddrinfo(host, client->portBuffer, &hints, &(client->info)) != 0)
   {
      free(client);
      return NULL;
   }

   if((client->sock = socket(client->info->ai_family, client->info->ai_socktype,
                             client->info->ai_protocol)) == -1)
   {
      freeaddrinfo(client->info);
      free(client);
      return NULL;
   }

   return client;
}

int tcp_client_connect(s_tcp_client* client)
{
   if((client->s = tcp_socket_connect(client->sock, client->info)) == NULL)
      return 1;

   freeaddrinfo(client->info);
   return 0;
}

void tcp_client_free(s_tcp_client* client)
{
   tcp_socket_free(client->s);
   free(client);
}
