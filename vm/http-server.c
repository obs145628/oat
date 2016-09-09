#include "http-server.h"
#include <stdlib.h>
#include <stdio.h>
#include "tcp-socket.h"
#include "tcp-server.h"
#include "http-request.h"
#include "http-response.h"
#include "err.h"

static s_http_server* server_;

static int on_conect_(s_tcp_socket* s)
{
   static char len_buffer[256];
   s_http_request* req;
   s_http_response* res;

   req = http_request_new();
   if(http_request_read(req, s) == -1)
      err("Unable to read http request from %s\n", s->ip);

   res = http_response_new();
   http_response_set_header(res, "server", server_->name);

   server_->on_request(req, res);



   sprintf(len_buffer, "%d", res->body_len);
   http_response_set_header(res, "content-length", len_buffer);

   if(http_response_write(res, s) == -1)
      err("Unable to send http response to %s\n", s->ip);

   tcp_socket_close(s);

   return 0;
}

s_http_server* http_server_new(f_http_server_on_request on_request, int port)
{
   s_http_server* server = malloc(sizeof(s_http_server));
   server->server = tcp_server_new(port, on_conect_);
   server->on_request = on_request;
   server->name = "oat http-server b1";
   server->port = port;
   return server;
}

void http_server_free(s_http_server* server)
{
   tcp_server_free(server->server);
   free(server);
}

void http_server_listen(s_http_server* server)
{
   server_ = server;
   tcp_server_start(server->server);
}
