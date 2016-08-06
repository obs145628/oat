#ifndef HTTP_REQUEST_HH_
# define HTTP_REQUEST_HH_

# include <boost/asio.hpp>
# include <mutex>
# include "http-callback.hh"
# include "http-response.hh"

class HTTPRequest
{

   friend class HTTPClient;

   static const unsigned int DEFAULT_PORT;
   HTTPRequest(boost::asio::io_service& ios, unsigned int id);

public:

   std::string getHost() const;
   unsigned int getPort() const;
   const std::string& getUri() const;
   unsigned int getId() const;

   void setHost(const std::string& host);
   void setPort(unsigned int port);
   void setUri(const std::string& uri);
   void setCallback(HTTPCallback callback);

   void execute();
   void cancel();

private:
   void onHostNameResolved(const boost::system::error_code& ec,
                           boost::asio::ip::tcp::resolver::iterator it);
   void onConnectionEstablished(const boost::system::error_code& ec,
                                boost::asio::ip::tcp::resolver::iterator it);
   void onRequestSent(const boost::system::error_code& ec,
                      std::size_t transferred);
   void onStatusLineReceived(const boost::system::error_code& ec,
                             std::size_t transferred);
   void onHeadersReceived(const boost::system::error_code& ec,
                          std::size_t transferred);
   void onResponseBodyReceived(const boost::system::error_code& ec,
                               std::size_t transferred);
   void onFinish(const boost::system::error_code& ec);

   std::string _host;
   unsigned int _port;
   std::string _uri;
   unsigned int _id;
   HTTPCallback _callback;
   std::string _requestBuf;

   boost::asio::ip::tcp::socket _sock;
   boost::asio::ip::tcp::resolver _resolver;
   HTTPResponse _response;

   bool _cancelled;
   std::mutex _cancelMux;

   boost::asio::io_service& _ios;

};

#endif //!HTTP_REQUEST_HH_
