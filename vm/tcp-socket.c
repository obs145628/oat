#include "tcp-socket.h"
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

#define BUFFER_ALLOC_INIT 64

static void* get_in_addr(struct sockaddr* sa)
{
   if(sa->sa_family == AF_INET)
      return &(((struct sockaddr_in*) sa)->sin_addr);
   else
      return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

s_tcp_socket* tcp_socket_accept(int socket)
{
   struct sockaddr_storage addr;
   socklen_t addrSize;
   s_tcp_socket* s = malloc(sizeof(s_tcp_socket));

   addrSize = sizeof(addr);
   if((s->sock = accept(socket, (struct sockaddr*) &addr, &addrSize)) == -1)
   {
      free(s);
      return NULL;
   }

   inet_ntop(addr.ss_family, get_in_addr((struct sockaddr*) &addr),
             s->ip, sizeof(s->ip));
   return s;
}

s_tcp_socket* tcp_socket_connect(int socket, struct addrinfo* info)
{
   s_tcp_socket* s = malloc(sizeof(s_tcp_socket));
   s->sock = socket;



   if(connect(socket, info->ai_addr, info->ai_addrlen) == -1)
   {
      close(socket);
      free(s);
      return NULL;
   }

   inet_ntop(info->ai_family, get_in_addr(info->ai_addr),
             s->ip, sizeof(s->ip));
   return s;
}

void tcp_socket_close(s_tcp_socket* s)
{
   close(s->sock);
}

void tcp_socket_free(s_tcp_socket* s)
{
   tcp_socket_close(s);
   free(s);
}

int tcp_socket_read(s_tcp_socket* s, void* buff, int len)
{
   int read = 0;
   int n;
   char* buffer = (char*) buff;

   while(len > 0)
   {
      if((n = recv(s->sock, buffer, len, 0)) == -1)
         return -1;
      if(n == 0)
         break;

      read += n;
      len -= n;
      buffer += n;
   }

   return read;
}

int tcp_socket_write(s_tcp_socket* s, const void* buff, int len)
{
   int n;
   const char* buffer = (const char*) buff;

   while(len > 0)
   {
      if((n = send(s->sock, buffer, len, 0)) == -1)
         return -1;

      len -= n;
      buffer += n;
   }

   return 0;
}

int tcp_socket_read_string(s_tcp_socket* s, char** out)
{
   int n;
   int alloc = BUFFER_ALLOC_INIT;
   int len = 0;
   char* res = malloc(alloc);
   char* it = res;

   while(1)
   {
      if(len > alloc)
      {
         alloc *= 2;
         res = realloc(res, alloc);
      }

      if((n = recv(s->sock, it, 1, 0)) == -1)
      {
         free(res);
         return -1;
      }

      len += n;
      if(n == 0 || *it == '\0')
         break;

      ++it;
   }

   *out = res;
   return len;
}

int tcp_socket_write_string(s_tcp_socket* s, const char* str)
{
   return tcp_socket_write(s, str, strlen(str) + 1);
}
