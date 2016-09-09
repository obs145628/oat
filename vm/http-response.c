#include "http-response.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tcp-socket.h"
#include "str.h"

s_http_response* http_response_new()
{
   s_http_response* res = malloc(sizeof(s_http_response));
   res->version = HTTP_VERSION_1_1;
   res->status_code = HTTP_STATUS_OK;
   res->status_message = strClone("OK");
   res->headers = http_headers_new();

   res->body_buff = NULL;
   res->body_len = 0;
   res->body_alloc = 0;

   return res;
}

void http_response_free(s_http_response* res)
{
   free(res->status_message);
   http_headers_free(res->headers);
   if(res->body_buff)
      free(res->body_buff);
   free(res);
}

void http_response_set_status_message(s_http_response* res, const char* str)
{
   free(res->status_message);
   res->status_message = strClone(str);
}

const char* http_response_get_header(s_http_response* res, const char* name)
{
   return http_headers_get(res->headers, name);
}

void http_response_set_header(s_http_response* res,
                             const char* name, const char* value)
{
   http_headers_set(res->headers, name, value);
}

void http_response_remove_header(s_http_response* res, const char* name)
{
   http_headers_remove(res->headers, name);
}

void http_response_body_write(s_http_response* res, const void* data, int len)
{
   int new_alloc;
   int total_len;
   char* body_it;

   if(!res->body_buff)
   {
      res->body_buff = malloc(len);
      res->body_len = 0;
      res->body_alloc = len;
   }

   new_alloc = res->body_alloc;
   total_len = res->body_len + len;
   while(total_len > new_alloc)
      new_alloc *= 2;

   if(res->body_alloc != new_alloc)
   {
      res->body_alloc = new_alloc;
      res->body_buff = realloc(res->body_buff, new_alloc);
   }

   body_it = (char*) res->body_buff + res->body_len;
   memcpy(body_it, data, len);
   res->body_len = total_len;
}

int http_response_read(s_http_response* res, struct tcp_socket* s)
{
   char* version;
   char* status_code;
   char* status_message;
   int status_message_len;
   const char* header_content_len;
   void* buff;
   int buff_len;

   if(tcp_socket_read_until(s, " ", &version) != 9)
   {
      if(version)
         free(version);
      return -1;
   }
   version[8] = '\0';

   if(strcmp(version, "HTTP/1.0") == 0)
      res->version = HTTP_VERSION_1_0;
   else if(strcmp(version, "HTTP/1.1") == 0)
      res->version = HTTP_VERSION_1_1;
   else
   {
      free(version);
      return -1;
   }

   free(version);

   if(tcp_socket_read_until(s, " ", &status_code) != 4)
   {
      free(status_code);
      return -1;
   }

   status_code[3] = '\0';
   res->status_code = atoi(status_code);
   free(status_code);
   if(res->status_code <= 0)
      return -1;

   if((status_message_len = tcp_socket_read_until(s, "\r\n", &status_message))
      == -1)
   {
      free(status_message);
      return -1;
   }

   status_message[status_message_len-2] = '\0';

   http_response_set_status_message(res, status_message);
   free(status_message);

   if(http_headers_read(res->headers, s) == -1)
      return -1;

   header_content_len = http_response_get_header(res, "content-length");

   if(header_content_len == NULL)
   {

      if((buff_len = tcp_socket_read_until(s, "\r\n\r\n", (char**) &buff)) == -1)
         return 0;

      res->body_buff = buff;
      res->body_len = buff_len - 4;
      res->body_alloc = buff_len;
   }

   else
   {
      buff_len = atoi(header_content_len);
      if(buff_len <= 0)
         return 0;

      res->body_buff = malloc(buff_len);
      res->body_len = buff_len;
      res->body_alloc = buff_len;
      if(tcp_socket_read(s, res->body_buff, buff_len) == -1)
         return -1;
   }



   return 0;
}

int http_response_write(s_http_response* res, struct tcp_socket* s)
{
   char status_code[4];

   if(res->version == HTTP_VERSION_1_0)
   {
      if(tcp_socket_write(s, "HTTP/1.0 ", 9) == -1)
         return -1;
   }

   else if(res->version == HTTP_VERSION_1_1)
   {
      if(tcp_socket_write(s, "HTTP/1.1 ", 9) == -1)
         return -1;
   }

   sprintf(status_code, "%d", res->status_code);
   status_code[3] = ' ';
   if(tcp_socket_write(s, status_code, 4) == -1)
      return -1;

   if(tcp_socket_write(s, res->status_message, strlen(res->status_message))
      == -1)
      return -1;

   if(tcp_socket_write(s, "\r\n", 2) == -1)
      return -1;

   if(http_headers_write(res->headers, s) == -1)
      return -1;

   if(res->body_buff)
   {
      if(tcp_socket_write(s, res->body_buff, res->body_len) == -1)
         return -1;
      if(tcp_socket_write(s, "\r\n\r\n", 4) == -1)
         return -1;
   }

   return 0;
}
