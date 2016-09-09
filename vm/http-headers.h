#ifndef HTTP_HEADERS_H_
# define HTTP_HEADERS_H_

# include "pmap.h"

struct tcp_socket;

typedef s_pmap s_http_headers;

s_http_headers* http_headers_new();
void http_headers_free(s_http_headers* headers);

const char* http_headers_get(s_http_headers* headers, const char* name);
void http_headers_set(s_http_headers* headers,
                      const char* name, const char* value);
void http_headers_remove(s_http_headers* headers,
                         const char* name);

int http_headers_read(s_http_headers* headers, struct tcp_socket* s);
int http_headers_write(s_http_headers* headers, struct tcp_socket* s);

#endif //!HTTP_HEAFERS_H_
