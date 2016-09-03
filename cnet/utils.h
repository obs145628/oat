#ifndef UTILS_H_
# define UTILS_H_

# define BACKLOG_LEN 5

#include <netinet/in.h>

struct addrinfo;

void error(const char* str);

char* findIp(const char* host);
int resolveHost(const char* host, int port, struct sockaddr* out);
in_addr_t findAddr(const char* host);

int resolveHostInfos(const char* host, int port, struct addrinfo** info);

int writeString(int socket, const char* str);
char* readString(int socket);

#endif //!UTILS_H_
