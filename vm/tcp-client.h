#ifndef TCP_CLIENT_H_
# define TCP_CLIENT_H_

struct addrinfo;
struct tcp_socket;

typedef struct {
   struct tcp_socket* s;
   int sock;
   int port;
   char portBuffer[10];
   struct addrinfo* info;
} s_tcp_client;

s_tcp_client* tcp_client_new(const char* host, int port);
int tcp_client_connect(s_tcp_client* client);
void tcp_client_free(s_tcp_client* client);



#endif //!TCP_CLIENT_H_
