#ifndef HTTP_RESPONSE_H_
# define HTTP_RESPONSE_H_

# include "http.h"
# include "http-headers.h"

struct tcp_socket;

struct http_response
{
   t_http_version version;
   t_http_status status_code;
   char* status_message;
   s_http_headers* headers;

   void* body_buff;
   int body_len;
   int body_alloc;
};

typedef struct http_response s_http_response;

s_http_response* http_response_new();
void http_response_free(s_http_response* res);

void http_response_set_status_message(s_http_response* res, const char* str);

const char* http_response_get_header(s_http_response* res, const char* name);
void http_response_set_header(s_http_response* res,
                             const char* name, const char* value);
void http_response_remove_header(s_http_response* res, const char* name);

void http_response_body_write(s_http_response* res, const void* data, int len);

int http_response_read(s_http_response* res, struct tcp_socket* s);
int http_response_write(s_http_response* res, struct tcp_socket* s);

#endif //!HTTP_RESPONSE_H_
