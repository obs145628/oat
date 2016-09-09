#include "tcp-server.h"
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
#include "err.h"

#define BACKLOG_LEN 5

static void sigchld_handler(int s)
{
   (void) s;
   int saved_errno = errno;
   while(waitpid(-1, NULL, WNOHANG) > 0)
      ;

   errno = saved_errno;
}

s_tcp_server* tcp_server_new(int port, f_tcp_server_cb on_connect)
{
   s_tcp_server* server;
   struct addrinfo hints;

   server = malloc(sizeof(s_tcp_server));
   server->port = port;
   server->on_connect = on_connect;
   server->running = 0;

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   sprintf(server->portBuffer, "%d", port);

   if(getaddrinfo(NULL, server->portBuffer, &hints, &(server->info)) != 0)
      err("Unable to find local address informations");

   if((server->sock = socket(server->info->ai_family, server->info->ai_socktype,
                       server->info->ai_protocol)) == -1)
      err("Unable to open server socket");

   return server;
}

void tcp_server_start(s_tcp_server* server)
{
   int sockFd;
   s_tcp_socket* new_sock;
   struct addrinfo* info;
   struct sigaction sa;
   int status;
   int yes = 1;

   if(server->running)
      return;

   sockFd = server->sock;
   info = server->info;

   if(setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
      err("Unable to fix old bind servers");

   if(bind(sockFd, info->ai_addr, info->ai_addrlen) == -1)
   {
      close(sockFd);
      err("Unable to use port %d\n", server->port);
   }

   freeaddrinfo(info);

   if(listen(sockFd, BACKLOG_LEN) == -1)
      err("Unable to listen to port %d\n", server->port);

   sa.sa_handler = sigchld_handler;
   sa.sa_flags = SA_RESTART;
   if(sigaction(SIGCHLD, &sa, NULL) == -1)
      err("Unable to set-up connection handlers");

   printf("Server listening on port %d\n", server->port);

   server->running = 1;

   while(1)
   {
      if((new_sock = tcp_socket_accept(sockFd)) == NULL)
         err("Unable to connect with client");

      if(!server->running)
         break;

      if(fork())
      {
         tcp_socket_free(new_sock);
      }

      else
      {
         close(sockFd);
         status = server->on_connect(new_sock);
         tcp_socket_free(new_sock);
         exit(status);
      }
   }

   close(sockFd);
}

void tcp_server_stop(s_tcp_server* server)
{
   if(!server->running)
      return;

   server->running = 0;
}

void tcp_server_free(s_tcp_server* server)
{
   tcp_server_stop(server);
   free(server);
}
