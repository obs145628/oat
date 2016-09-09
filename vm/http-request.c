#include "http-request.h"
#include <stdlib.h>
#include <string.h>
#include "tcp-socket.h"
#include "str.h"
#include "err.h"

s_http_request* http_request_new()
{
   s_http_request* req = malloc(sizeof(s_http_request));
   req->method = HTTP_METHOD_GET;
   req->path = strClone("/");
   req->version = HTTP_VERSION_1_1;
   req->headers = http_headers_new();

   req->body_buff = NULL;
   req->body_len = 0;
   req->body_alloc = 0;

   return req;
}

void http_request_free(s_http_request* req)
{
   free(req->path);
   http_headers_free(req->headers);
   if(req->body_buff)
      free(req->body_buff);
   free(req);
}

void http_request_set_path(s_http_request* req, const char* path)
{
   free(req->path);
   req->path = strClone(path);
}

const char* http_request_get_header(s_http_request* req, const char* name)
{
   return http_headers_get(req->headers, name);
}

void http_request_set_header(s_http_request* req,
                             const char* name, const char* value)
{
   http_headers_set(req->headers, name, value);
}

void http_remove_header(s_http_request* req, const char* name)
{
   http_headers_remove(req->headers, name);
}

void http_request_body_write(s_http_request* req, const void* data, int len)
{
   int new_alloc;
   int total_len;
   char* body_it;

   if(!req->body_buff)
   {
      req->body_buff = malloc(len);
      req->body_len = 0;
      req->body_alloc = len;
   }

   new_alloc = req->body_alloc;
   total_len = req->body_len + len;
   while(total_len > new_alloc)
      new_alloc *= 2;

   if(req->body_alloc != new_alloc)
   {
      req->body_alloc = new_alloc;
      req->body_buff = realloc(req->body_buff, new_alloc);
   }

   body_it = (char*) req->body_buff + req->body_len;
   memcpy(body_it, data, len);
   req->body_len = total_len;
}

int http_request_read(s_http_request* req, struct tcp_socket* s)
{
   char* method;
   int method_len;
   char* path;
   int path_len;
   char* version;
   const char* header_content_len;
   void* buff;
   int buff_len;

   if((method_len = tcp_socket_read_until(s, " ", &method)) == -1)
      return -1;

   method[method_len-1] = '\0';

   if(strcmp(method, "GET") == 0)
      req->method = HTTP_METHOD_GET;
   else if(strcmp(method, "HEAD") == 0)
      req->method = HTTP_METHOD_HEAD;
   else if(strcmp(method, "POST") == 0)
      req->method = HTTP_METHOD_POST;
   else
   {
      free(method);
      return -1;
   }

   free(method);

   if((path_len = tcp_socket_read_until(s, " ", &path)) == -1)
      return -1;

   path[path_len-1] = '\0';
   http_request_set_path(req, path);
   free(path);


   if(tcp_socket_read_until(s, "\r\n", &version) != 10)
   {
      if(version)
         free(version);
      return -1;
   }
   version[8] = '\0';

   if(strcmp(version, "HTTP/1.0") == 0)
      req->version = HTTP_VERSION_1_0;
   else if(strcmp(version, "HTTP/1.1") == 0)
      req->version = HTTP_VERSION_1_1;
   else
   {
      free(version);
      return -1;
   }

   free(version);

   if(http_headers_read(req->headers, s) == -1)
      return -1;

   header_content_len = http_request_get_header(req, "content-length");

   if(header_content_len == NULL)
   {
      /*
      if((buff_len = tcp_socket_read_until(s, "\r\n", (char**) &buff)) == -1)
         return 0;

      req->body_buff = buff;
      req->body_len = buff_len - 2;
      req->body_alloc = buff_len;
      */
   }

   else
   {
      buff_len = atoi(header_content_len);
      if(buff_len <= 0)
         return 0;

      req->body_buff = malloc(buff_len);
      req->body_len = buff_len;
      req->body_alloc = buff_len;
      if(tcp_socket_read(s, req->body_buff, buff_len) == -1)
         return -1;
   }

   return 0;
}


int http_request_write(s_http_request* req, struct tcp_socket* s)
{
   if(req->method == HTTP_METHOD_GET)
   {
      if(tcp_socket_write(s, "GET ", 4) == -1)
         return -1;
   }

   else if(req->method == HTTP_METHOD_HEAD)
   {
      if(tcp_socket_write(s, "HEAD ", 5) == -1)
         return -1;
   }

   else if(req->method == HTTP_METHOD_POST)
   {
      if(tcp_socket_write(s, "POST ", 5) == -1)
         return -1;
   }

   if(tcp_socket_write(s, req->path, strlen(req->path)) == -1)
      return -1;



   if(req->version == HTTP_VERSION_1_0)
   {
      if(tcp_socket_write(s, " HTTP/1.0\r\n", 11) == -1)
         return -1;
   }

   else if(req->version == HTTP_VERSION_1_1)
   {
      if(tcp_socket_write(s, " HTTP/1.1\r\n", 11) == -1)
         return -1;
   }

   if(http_headers_write(req->headers, s) == -1)
      return -1;

   if(tcp_socket_write(s, "\r\n", 2) == -1)
      return -1;

   if(req->body_buff)
   {
      if(tcp_socket_write(s, req->body_buff, req->body_len) == -1)
         return -1;
      if(tcp_socket_write(s, "\r\n\r\n", 4) == -1)
         return -1;
   }

   return 0;
}
