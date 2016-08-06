#ifndef HTTP_CLIENT_HH_
# define HTTP_CLIENT_HH_

# include <memory>
# include <thread>
# include <boost/asio.hpp>
# include "http-request.hh"

class HTTPClient
{
public:

   HTTPClient();
   std::shared_ptr<HTTPRequest> createRequest(unsigned int id);
   void close();

private:
   boost::asio::io_service _ios;
   std::unique_ptr<boost::asio::io_service::work> _work;
   std::unique_ptr<std::thread> _thread;
};

#endif //HTTP_CLIENT_HH_
