#include "http-client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "http-request.h"
#include "http-response.h"
#include "str.h"
#include "err.h"
#include "tcp-client.h"

s_http_client* http_client_new()
{
   s_http_client* client = malloc(sizeof(s_http_client));
   client->version = HTTP_VERSION_1_1;
   client->user_agent = "oat http-client b1";
   return client;
}

void http_client_free(s_http_client* client)
{
   free(client);
}

struct http_response* http_client_send(s_http_client* client,
                                       struct http_request* req,
                                       const char* url)
{
   size_t pos;
   size_t pos2;
   char* protocol;
   char* host;
   char* hostHeader;
   char* path;
   char* port;


   pos = strIndexOf(url, "://");

   if(pos == STR_NFOUND)
   {
      protocol = strClone("http");
   }
   else
   {
      protocol = strPrefix(url, pos);
      url += pos + 3;
   }

   pos = strIndexOf(url, "/");

   if(pos == STR_NFOUND)
   {
      pos2 = strIndexOf(url, ":");

      if(pos2 == STR_NFOUND)
      {
         host = strClone(url);
         hostHeader = strClone(url);
         path = strClone("/");
         port = strClone("80");
      }
      else
      {
         host = strPrefix(url, pos2);
         hostHeader = strClone(url);
         path = strClone("/");
         port = strSuffixFrom(url, pos2 + 1);
      }
   }


   else
   {
      pos2 = strIndexOf(url, ":");

      if(pos2 == STR_NFOUND)
      {
         host = strPrefix(url, pos);
         hostHeader = strClone(host);
         path = strClone(url + pos);
         port = strClone("80");
      }

      else
      {
         host = strPrefix(url, pos2);
         hostHeader = strPrefix(url, pos);
         path = strSuffixFrom(url, pos);
         port = strSubstring(url, pos2+1, pos - pos2 - 1);
      }
   }

   req->version = client->version;

   if(strcmp(protocol, "http") != 0)
      err("invalid protocol: %s\n", protocol);
   free(protocol);

   if(*host == '\0')
      err("Empty host");

   int port_num = atoi(port);
   if(port_num <= 0 || port_num >= 65535)
      err("Invalid port number: %s\n", port);
   free(port);

   http_request_set_path(req, path);
   http_request_set_header(req, "host", hostHeader);
   http_request_set_header(req, "user-agent", client->user_agent);

   s_tcp_client* tcp_client = tcp_client_new(host, port_num);
   tcp_client_connect(tcp_client);

   if(http_request_write(req, tcp_client->s) == -1)
      err("Unable to send http request to %s", host);

   s_http_response* res = http_response_new();
   if(http_response_read(res, tcp_client->s) == -1)
      err("Unable to get http response from %s", host);


   free(host);
   free(hostHeader);
   free(path);
   http_request_free(req);
   tcp_client_free(tcp_client);

   return res;
}
