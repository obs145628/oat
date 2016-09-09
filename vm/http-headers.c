#include "http-headers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "tcp-socket.h"
#include "err.h"

static void free_pair_(void* a, void* b)
{
   free(a);
   free(b);
}

s_http_headers* http_headers_new()
{
   return pmap_new((f_pmap_cmp) strCmpIgnoreCase);
}

void http_headers_free(s_http_headers* headers)
{
   pmap_foreach(headers, free_pair_);
   pmap_free(headers);
}

const char* http_headers_get(s_http_headers* headers, const char* name)
{
   char* key = strTrim(name);
   s_pmap_node* it = pmap_find(headers, (void*) key);
   free(key);
   return it == pmap_end(headers) ? NULL : (const char*) it->value;
}

void http_headers_set(s_http_headers* headers,
                      const char* name, const char* value)
{
   char* trim_key = strTrim(name);
   char* trim_value = strTrim(value);
   s_pmap_node* it = pmap_find(headers, (void*) trim_key);

   if(it == pmap_end(headers))
   {
      pmap_insert(headers, (void*) trim_key, (void*) trim_value);
   }

   else
   {
      free(it->value);
      it->value = (void*) trim_value;
      free(trim_key);
   }

}
void http_headers_remove(s_http_headers* headers,
                         const char* name)
{
   char* key = strTrim(name);
   s_pmap_node* it = pmap_find(headers, (void*) key);
   free(key);

   if(it != pmap_end(headers))
   {
      free(it->key);
      free(it->value);
      pmap_remove(headers, (void*) name);
   }
}


int http_headers_read(s_http_headers* headers, struct tcp_socket* s)
{
   char* header_line;
   int header_len;
   size_t header_sep;

   while(1)
   {
      if((header_len = tcp_socket_read_until(s, "\r\n", &header_line)) == -1)
         return -1;
      if(header_len == 2 && header_line[0] == '\r' && header_line[1] == '\n')
         break;

      header_line[header_len-2] = '\0';

      header_sep = strIndexOf(header_line, ":");
      if(header_sep == STR_NFOUND)
         return -1;

      header_line[header_sep] = '\0';
      http_headers_set(headers, header_line, header_line + header_sep + 1);

      //free(header_line);
   }

   return 0;
}

int http_headers_write(s_http_headers* headers, struct tcp_socket* s)
{
   s_pmap_node* it;
   const char* name;
   const char* value;

   it = pmap_begin(headers);
   while(it != pmap_end(headers))
   {
      name = it->key;
      value = it->value;

      if(tcp_socket_write(s, name, strlen(name)) == -1)
         return -1;
      if(tcp_socket_write(s, ": ", 2) == -1)
         return -1;
      if(tcp_socket_write(s, value, strlen(value)) == -1)
         return -1;
      if(tcp_socket_write(s, "\r\n", 2) == -1)
         return -1;

      it = pmap_next(headers, it);
   }

   return 0;
}
