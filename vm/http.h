#ifndef HTTP_H_
# define HTTP_H_

# define HTTP_METHOD_GET (1)
# define HTTP_METHOD_HEAD (2)
# define HTTP_METHOD_POST (3)

# define HTTP_VERSION_1_0 (10)
# define HTTP_VERSION_1_1 (11)

# define HTTP_STATUS_OK (200)
# define HTTP_STATUS_NOT_FOUND (404)

typedef int t_http_method;
typedef int t_http_version;
typedef int t_http_status;

#endif //!HTTP_H_
