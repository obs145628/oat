#ifndef HTTP_CLIENT_H_
# define HTTP_CLIENT_H_

# include "http.h"

struct http_request;
struct http_response;

struct http_client
{
   t_http_version version;
   const char* user_agent;
};

typedef struct http_client s_http_client;

s_http_client* http_client_new();
void http_client_free(s_http_client* client);

struct http_response* http_client_send(s_http_client* client,
                                       struct http_request* req,
                                       const char* url);

#endif //!HTTP_CLIENT_H_
