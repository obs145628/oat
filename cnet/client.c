#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include "tcp-client.h"
#include "tcp-socket.h"
#include "utils.h"

static char* readLine()
{
   char* buffer;
   size_t len = 512;
   size_t read;

   buffer = malloc(len);

   read = getline(&buffer, &len, stdin);
   buffer[read-1] = '\0';
   return buffer;
}

int runClient(int argc, char** argv)
{
   int port;
   char* host;
   s_tcp_client* client;

   if(argc < 4)
      error("Client: missing argument");
   port = atoi(argv[2]);
   host = argv[3];

   if((client = tcp_client_new(host, port)) == NULL)
      error("Client: unable to create client");

   if(tcp_client_connect(client) != 0)
      error("Client: unable to connect to the server !");

   s_tcp_socket* s = client->s;

   printf("Connected to %s:%d\n", s->ip, port);

   while(1)
   {
      char* message;
      char* data;
      int len;

      message = readLine();

      if(*message == '\0')
      {
         free(message);
         break;
      }
      if(tcp_socket_write_string(s, message) == -1)
         error("Client: unable to write data !");
      free(message);

      if((len = tcp_socket_read_string(s, &data)) == -1)
         error("Client: unable to read data !");

      if(len == 0 || data[len-1] != '\0')
         error("Client: disconnected");

      printf("%s\n", data);
      free(data);
   }

   printf("Done\n");
   tcp_client_free(client);
   return 0;
}
