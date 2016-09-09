#ifndef TCP_SOCKET_H_
# define TCP_SOCKET_H_

#include <netinet/in.h>

struct sockaddr;
struct addrinfo;

struct tcp_socket {
   int sock;
   char ip[INET6_ADDRSTRLEN];
};

typedef struct tcp_socket s_tcp_socket;

s_tcp_socket* tcp_socket_accept(int socket);
s_tcp_socket* tcp_socket_connect(int socket, struct addrinfo* info);
void tcp_socket_close(s_tcp_socket* s);
void tcp_socket_free(s_tcp_socket* s);

/**
 * Try to reads len bytes from a socket
 * Returns -1 in case of error
 * Returns a value < len if end of file is reached
 */
int tcp_socket_read(s_tcp_socket* s, void* buff, int len);

/**
 * Try to writes len bytes to a socket
 * Returns -1 in case of faillure
 * returns 0 if success
 */
int tcp_socket_write(s_tcp_socket* s, const void* buff, int len);

/**
 * Try to reads a null-terminated string from the socket
 * Returns -1 in case of error
 * Returns the number of bytes in case of success
 * No '\0' charachter is added in case of EOF reached
 */
int tcp_socket_read_string(s_tcp_socket* s, char** out);

/**
 * Try to writes a string to a socket, including the null-terminated character
 * Returns -1 in case of faillure
 * returns 0 if success
 */
int tcp_socket_write_string(s_tcp_socket* s, const char* str);

/**
 * Try to read a string from the socket, until found limit string
 * Returns -1 in case of error or eof
 * Returns the numbers of bytes in case of success
 */
int tcp_socket_read_until(s_tcp_socket* s, const char* limit, char** out);

/**
 * Read all data until the socket is closed
 * Returns -1 in case of error
 * Returns the number of bytes read in cases of success
 */
int tcp_socket_read_all(s_tcp_socket* s, char** out);

#endif //!TCP_SOCKET_H_
