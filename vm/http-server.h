#ifndef HTTP_SERVER_H_
# define HTTP_SERVER_H_

struct tcp_server;
struct http_request;
struct http_response;

typedef void (*f_http_server_on_request)(struct http_request* req,
                                         struct http_response* res);

struct http_server
{
   struct tcp_server* server;
   f_http_server_on_request on_request;
   const char* name;
   int port;
};

typedef struct http_server s_http_server;

s_http_server* http_server_new(f_http_server_on_request on_request, int port);
void http_server_free(s_http_server* server);

void http_server_listen(s_http_server* server);

#endif //!HTTP_SERVER_H_
