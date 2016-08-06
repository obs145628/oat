#ifndef HTTP_CALLBACK_HH_
# define HTTP_CALLBACK_HH_

# include <boost/asio.hpp>

class HTTPClient;
class HTTPRequest;
class HTTPResponse;

typedef void(*HTTPCallback)(const HTTPRequest& req,
                            const HTTPResponse& res,
                            const boost::system::error_code& ec);

#endif //!HTTP_CALLBACK_HH_
