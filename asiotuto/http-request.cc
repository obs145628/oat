#include "http-request.hh"
#include <cassert>
#include "http.hh"

const unsigned int HTTPRequest::DEFAULT_PORT = 80;

HTTPRequest::HTTPRequest(boost::asio::io_service& ios, unsigned int id)
   : _port(DEFAULT_PORT), _id(id), _callback(nullptr), _sock(ios),
     _resolver(ios), _cancelled(false), _ios(ios)
{

}

std::string HTTPRequest::getHost() const
{
   return _host;
}

unsigned int HTTPRequest::getPort() const
{
   return _port;
}

const std::string& HTTPRequest::getUri() const
{
   return _uri;
}

unsigned int HTTPRequest::getId() const
{
   return _id;
}

void HTTPRequest::setHost(const std::string& host)
{
   _host = host;
}

void HTTPRequest::setPort(unsigned int port)
{
   _port = port;
}

void HTTPRequest::setUri(const std::string& uri)
{
   _uri = uri;
}

void HTTPRequest::setCallback(HTTPCallback callback)
{
   _callback = callback;
}

void HTTPRequest::execute()
{
   assert(_port > 0);
   assert(_host.length() > 0);
   assert(_uri.length() > 0);
   assert(_callback != nullptr);

   boost::asio::ip::tcp::resolver::query resolverQuery(
      _host, std::to_string(_port),
      boost::asio::ip::tcp::resolver::query::numeric_service);

   std::unique_lock<std::mutex> cancelLock(_cancelMux);
   if(_cancelled)
   {
      cancelLock.unlock();
      onFinish(boost::system::error_code(
                  boost::asio::error::operation_aborted));
      return;
   }

   _resolver.async_resolve(
      resolverQuery, [this](const boost::system::error_code& ec,
                            boost::asio::ip::tcp::resolver::iterator it) {
         onHostNameResolved(ec, it);
      });
}

void HTTPRequest::cancel()
{
   std::unique_lock<std::mutex> cancelLock(_cancelMux);
   _cancelled = true;

   _resolver.cancel();
   if(_sock.is_open())
      _sock.cancel();
}



void HTTPRequest::onHostNameResolved(
   const boost::system::error_code& ec,
   boost::asio::ip::tcp::resolver::iterator it)
{
   if(ec != 0)
   {
      onFinish(ec);
      return;
   }

   std::unique_lock<std::mutex> cancelLock(_cancelMux);
   if(_cancelled)
   {
      cancelLock.unlock();
      onFinish(boost::system::error_code(
                  boost::asio::error::operation_aborted));
      return;
   }

   boost::asio::async_connect(
      _sock, it, [this](const boost::system::error_code& ec,
                        boost::asio::ip::tcp::resolver::iterator it) {
         onConnectionEstablished(ec, it);
      });
}


void HTTPRequest::onConnectionEstablished(
   const boost::system::error_code& ec,
   boost::asio::ip::tcp::resolver::iterator)
{
   if(ec != 0)
   {
      onFinish(ec);
      return;
   }

   _requestBuf += "GET " + _uri + " HTTP/1.1\r\n";
   _requestBuf += "Host: " + _host + "\r\n";
   _requestBuf += "\r\n";

   std::unique_lock<std::mutex> cancelLock(_cancelMux);
   if(_cancelled)
   {
      cancelLock.unlock();
      onFinish(boost::system::error_code(
                  boost::asio::error::operation_aborted));
      return;
   }

   boost::asio::async_write(
      _sock, boost::asio::buffer(_requestBuf),
      [this](const boost::system::error_code& ec,
             std::size_t transferred) {
         onRequestSent(ec, transferred);
      });
}

void HTTPRequest::onRequestSent(const boost::system::error_code& ec,
                                std::size_t)
{
   if(ec != 0)
   {
      onFinish(ec);
      return;
   }

   _sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
   std::unique_lock<std::mutex> cancelLock(_cancelMux);
   if(_cancelled)
   {
      cancelLock.unlock();
      onFinish(boost::system::error_code(
                  boost::asio::error::operation_aborted));
      return;
   }

   boost::asio::async_read_until(
      _sock, _response.getResponseBuf(),
      "\r\n",
      [this](const boost::system::error_code& ec, std::size_t transferred) {
         onStatusLineReceived(ec, transferred);
      });

}

void HTTPRequest::onStatusLineReceived(const boost::system::error_code& ec,
                             std::size_t)
{
   if(ec != 0)
   {
      onFinish(ec);
      return;
   }

   std::string httpVersion;
   std::string strStatusCode;
   std::string statusMessage;

   std::istream is(&_response.getResponseBuf());
   is >> httpVersion;
   if(httpVersion != "HTTP/1.1")
   {
      onFinish(http_errors::invalid_response);
      return;
   }

   is >> strStatusCode;
   unsigned int statusCode = 200;
   try {
      statusCode = std::stoul(strStatusCode);
   }
   catch(std::logic_error&) {
      onFinish(http_errors::invalid_response);
      return;
   }

   std::getline(is, statusMessage, '\r');
   is.get();

   _response.setStatusCode(statusCode);
   _response.setStatusMessage(statusMessage);

   std::unique_lock<std::mutex> cancelLock(_cancelMux);
   if(_cancelled)
   {
      cancelLock.unlock();
      onFinish(boost::system::error_code(
                  boost::asio::error::operation_aborted));
      return;
   }

   boost::asio::async_read_until(
      _sock, _response.getResponseBuf(),  "\r\n\r\n",
      [this](const boost::system::error_code& ec, std::size_t transferred) {
         onHeadersReceived(ec, transferred);
      });
}

void HTTPRequest::onHeadersReceived(const boost::system::error_code& ec,
                                    std::size_t)
{
   if(ec != 0)
   {
      onFinish(ec);
      return;
   }

   std::string header;
   std::string headerName;
   std::string headerValue;
   std::istream is(&_response.getResponseBuf());

   while(true)
   {
      std::getline(is, header, '\r');
      is.get();
      if(header == "")
         break;

      size_t sepPos = header.find(':');
      if(sepPos != std::string::npos)
      {
         headerName = header.substr(0, sepPos);
         if(sepPos < header.length() - 1)
            headerValue = header.substr(sepPos + 1);
         else
            headerValue = "";
         _response.addHeader(headerName, headerValue);
      }
   }

   std::unique_lock<std::mutex> cancelLock(_cancelMux);
   if(_cancelled)
   {
      cancelLock.unlock();
      onFinish(boost::system::error_code(
                  boost::asio::error::operation_aborted));
      return;
   }

   boost::asio::async_read(
      _sock, _response.getResponseBuf(),
      [this](const boost::system::error_code& ec, std::size_t transferred) {
         onResponseBodyReceived(ec, transferred);
      });
}



void HTTPRequest::onResponseBodyReceived(const boost::system::error_code& ec,
                               std::size_t)
{
   if(ec == boost::asio::error::eof)
      onFinish(boost::system::error_code());
   else
      onFinish(ec);
}

void HTTPRequest::onFinish(const boost::system::error_code& ec)
{
   if(ec != 0)
   {
      std::cout << "HTTP Request error: " << ec.value()
                << "; " << ec.message() << std::endl;
   }
   _callback(*this, _response, ec);
}
