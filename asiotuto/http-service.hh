#ifndef HTTP_SERVICE_HH_
# define HTTP_SERVICE_HH_

# include <boost/asio.hpp>
# include <boost/filesystem.hpp>

# include <map>
# include <string>
# include <fstream>
# include <atomic>
# include <thread>
# include <iostream>

class HTTPService
{

   static const std::map<unsigned int, std::string> statusTable;

public:
   HTTPService(std::shared_ptr<boost::asio::ip::tcp::socket> sock);

   void startHandling();

private:
   void onRequestLineReceived(const boost::system::error_code& ec,
                              std::size_t transferred);
   void onHeadersReceived(const boost::system::error_code& ec,
                              std::size_t transferred);
   void processRequest();
   void sendResponse();
   void onResponseSent(const boost::system::error_code& ec,
                              std::size_t transferred);
   void onFinish();

   bool handleError(const boost::system::error_code& ec);

   std::shared_ptr<boost::asio::ip::tcp::socket> _sock;
   boost::asio::streambuf _req;
   std::map<std::string, std::string> _reqHeaders;
   std::string _reqRessource;

   std::unique_ptr<char[]> _ressourceBuffer;
   unsigned int _resStatusCode;
   std::size_t _ressourceSize;
   std::string _resHeaders;
   std::string _resStatusLine;

};

#endif //!HTTP_SERVICE_HH_
