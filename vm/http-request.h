#ifndef HTTP_REQUEST_H_
# define HTTP_REQUEST_H_

# include "http.h"
# include "http-headers.h"

struct tcp_socket;

struct http_request
{
   t_http_method method;
   char* path;
   t_http_version version;
   s_http_headers* headers;

   void* body_buff;
   int body_len;
   int body_alloc;
};

typedef struct http_request s_http_request;

s_http_request* http_request_new();
void http_request_free(s_http_request* req);

void http_request_set_path(s_http_request* req, const char* path);

const char* http_request_get_header(s_http_request* req, const char* name);
void http_request_set_header(s_http_request* req,
                             const char* name, const char* value);
void http_remove_header(s_http_request* req, const char* name);

int http_request_read(s_http_request* req, struct tcp_socket* s);
int http_request_write(s_http_request* req, struct tcp_socket* s);

void http_request_body_write(s_http_request* req, const void* data, int len);


#endif //!HTTP_REQUEST_H_
