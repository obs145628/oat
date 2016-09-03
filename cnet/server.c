#include "server.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "tcp-server.h"
#include "tcp-socket.h"
#include "utils.h"

static s_tcp_server* server;

static int onConnect(s_tcp_socket* s)
{
   char* data;
   int len;

   printf("Got connection from %s\n", s->ip);

   while(1)
   {
      if((len = tcp_socket_read_string(s, &data)) == -1)
         error("Server: unable to read data");

      if(len == 0 || data[len-1] != '\0')
         break;

      for(int i = 0; i < len - 1; ++i)
         data[i] = toupper(data[i]);

      if(tcp_socket_write_string(s, data))
         error("Server: unable to write data");
      free(data);
   }

   printf("Connection from %s closed\n", s->ip);

   return 0;
}



int runServer(int argc, char** argv)
{
   int port;

   if(argc < 3)
      error("Server: missing argument");
   port = atoi(argv[2]);

   if((server = tcp_server_new(port, onConnect)) == NULL)
      error("Server: creation error");

   if(tcp_server_start(server) != 0)
      error("Server: unable to start server");

   printf("Done\n");
   tcp_server_free(server);
   return 0;
}
