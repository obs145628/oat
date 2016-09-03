#ifndef TCP_SERVER_H_
# define TCP_SERVER_H_

struct addrinfo;
struct tcp_socket;

typedef int (*f_tcp_server_cb)(struct tcp_socket* socket);

typedef struct {
   int sock;
   int port;
   char portBuffer[10];
   f_tcp_server_cb on_connect;
   int running;
   struct addrinfo* info;
} s_tcp_server;

s_tcp_server* tcp_server_new(int port, f_tcp_server_cb on_connect);

int tcp_server_start(s_tcp_server* server);
int tcp_server_stop(s_tcp_server* server);
void tcp_server_free(s_tcp_server* server);



#endif //!TCP_SERVER_H_
