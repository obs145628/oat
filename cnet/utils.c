#include "utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_ALLOC_INIT 64

void error(const char* str)
{
   fprintf(stderr, "%s\n", str);
   exit(1);
}

char* findIp(const char* host)
{
   struct addrinfo hints;
   struct addrinfo *res;
   void* addr;
   char* ipstr = malloc(INET6_ADDRSTRLEN + 1);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if(getaddrinfo(host, NULL, &hints, &res) != 0)
      error("Unable to get addr info");

   if(res == NULL)
      error("Host not found");

   if(res->ai_family == AF_INET)
   {
      struct sockaddr_in* ip = (struct sockaddr_in*) res->ai_addr;
      addr = &(ip->sin_addr);
   }
   else
   {
      struct sockaddr_in6* ip = (struct sockaddr_in6*) res->ai_addr;
      addr = &(ip->sin6_addr);
   }

   inet_ntop(res->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
   freeaddrinfo(res);
   return ipstr;
}

int resolveHost(const char* host, int port, struct sockaddr* out)
{
   struct addrinfo hints;
   struct addrinfo *res;
   memset(out, 0, sizeof(*out));
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if(getaddrinfo(host, NULL, &hints, &res) != 0)
      return 1;
   if(res == NULL)
      return 1;

   if(res->ai_family == AF_INET)
   {
      struct sockaddr_in* ip4 = (struct sockaddr_in*) res->ai_addr;
      struct sockaddr_in* out4 = (struct sockaddr_in*) out;
      memcpy(&(out4->sin_addr.s_addr), &(ip4->sin_addr.s_addr),
             sizeof(ip4->sin_addr.s_addr));
      out4->sin_family = AF_INET;
      out4->sin_port = htons(port);
   }
   else
   {
      struct sockaddr_in6* ip6 = (struct sockaddr_in6*) res->ai_addr;
      struct sockaddr_in6* out6 = (struct sockaddr_in6*) out;
      memcpy(&(out6->sin6_addr.s6_addr), &(ip6->sin6_addr.s6_addr),
             sizeof(ip6->sin6_addr.s6_addr));
      out6->sin6_family = AF_INET6;
      out6->sin6_port = htons(port);
   }

   return 0;
}

in_addr_t findAddr(const char* host)
{
   char* ip = findIp(host);
   in_addr_t addr = inet_addr(ip);
   free(ip);
   return addr;
}

int resolveHostInfos(const char* host, int port, struct addrinfo** info)
{
   struct addrinfo hints;
   char portBuffer[10];

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   if(host == NULL)
      hints.ai_flags = AI_PASSIVE;

   sprintf(portBuffer, "%d", port);

   return getaddrinfo(host, portBuffer, &hints, info);
}

int writeString(int socket, const char* str)
{
   int len = strlen(str) + 1;
   return send(socket, str, len, 0) != len;
}

char* readString(int socket)
{
   size_t len = BUFFER_ALLOC_INIT;
   char* res = malloc(len);
   char* it = res;

   while(1)
   {
      if((size_t) (it - res) > len)
      {
         len *= 2;
         res = realloc(res, len);
      }

      if(recv(socket, it, 1, 0) != 1)
      {
         printf("fail\n");
         free(res);
         res = NULL;
         break;
      }

      printf("[%c] - %d\n", *it, (int) *it);

      if(*it == '\0')
         break;
      ++it;
   }

   printf("success\n");

   return res;
}
