#include "http-service.hh"
#include <iostream>
#include <vector>

const std::map<unsigned int, std::string> HTTPService::statusTable =
{
   {200, "200 OK"},
   {404, "404 Not Found"},
   {413, "413 Request Entity Too Large"},
   {500, "500 Server Error"},
   {501, "501 Not Implemented"},
   {505, "505 HTTP Version Not Supported"}
};


HTTPService::HTTPService(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
   : _sock(sock),
     _req(4096),
     _resStatusCode(200),
     _ressourceSize(0)
{

}

void HTTPService::startHandling()
{
   boost::asio::async_read_until(
      *_sock.get(), _req, "\r\n",
      [this] (const boost::system::error_code& ec, std::size_t transferred) {
         onRequestLineReceived(ec, transferred);
      });
}

void HTTPService::onRequestLineReceived(const boost::system::error_code& ec,
                                        std::size_t)
{
   if(!handleError(ec))
      return;

   std::string requestLine;
   std::istream is(&_req);
   std::getline(is, requestLine, '\r');
   is.get();

   std::string reqMethod;
   std::istringstream iss(requestLine);
   iss >> reqMethod;
   if(reqMethod != "GET")
   {
      _resStatusCode = 501;
      sendResponse();
      return;
   }

   iss >> _reqRessource;

   std::string httpVersion;
   iss >> httpVersion;

   if(httpVersion != "HTTP/1.1")
   {
      _resStatusCode = 505;
      sendResponse();
      return;
   }

   boost::asio::async_read_until(
      *_sock.get(), _req, "\r\n\r\n",
      [this] (const boost::system::error_code& ec, std::size_t transferred) {
         onHeadersReceived(ec, transferred);
      });
}

void HTTPService::onHeadersReceived(const boost::system::error_code& ec,
                                    std::size_t)
{
   if(!handleError(ec))
      return;

   std::istream is(&_req);
   std::string headerName;
   std::string headerValue;

   while(!is.eof())
   {
      std::getline(is, headerName, ':');
      if(!is.eof())
      {
         std::getline(is, headerValue, '\r');
         is.get();
         _reqHeaders[headerName] = headerValue;
      }
   }

   processRequest();
   sendResponse();
}


void HTTPService::processRequest()
{
   std::string filePath = "/home/obs/Documents/www" + _reqRessource;

   if(!boost::filesystem::exists(filePath))
   {
      _resStatusCode = 404;
      return;
   }

   std::ifstream fs(filePath, std::ifstream::binary);
   if(!fs.is_open())
   {
      _resStatusCode = 500;
      return;
   }

   fs.seekg(0, std::ifstream::end);
   _ressourceSize = static_cast<std::size_t> (fs.tellg());
   fs.seekg(std::ifstream::beg);

   _ressourceBuffer.reset(new char[_ressourceSize]);
   fs.read(_ressourceBuffer.get(), _ressourceSize);

   _resHeaders += "(content-length): " + std::to_string(_ressourceSize)
      + "\r\n";
}

void HTTPService::sendResponse()
{
   _sock->shutdown(boost::asio::ip::tcp::socket::shutdown_receive);

   auto statusLine = statusTable.at(_resStatusCode);
   _resStatusLine += "HTTP/1.1 " + statusLine + "\r\n";
   _resHeaders += "\r\n";

   std::vector<boost::asio::const_buffer> resBuffers;
   resBuffers.push_back(boost::asio::buffer(_resStatusLine));
   if(_resHeaders.length() > 0)
      resBuffers.push_back(boost::asio::buffer(_resHeaders));
   if(_ressourceSize > 0)
      resBuffers.push_back(boost::asio::buffer(_ressourceBuffer.get(),
                                               _ressourceSize));

   boost::asio::async_write(
      *_sock.get(), resBuffers,
      [this] (const boost::system::error_code& ec, std::size_t transferred) {
         onResponseSent(ec, transferred);
      });
}

void HTTPService::onResponseSent(const boost::system::error_code& ec,
                              std::size_t)
{
   if(ec != 0)
   {
      std::cout << "Server error: " << ec.value() << "; "
                   << ec.message() << std::endl;
   }

   _sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
   onFinish();
}

void HTTPService::onFinish()
{
   delete this;
}

bool HTTPService::handleError(const boost::system::error_code& ec)
{
   if(ec == 0)
      return true;

   std::cout << "Server error: " << ec.value() << "; "
             << ec.message() << std::endl;

   if(ec == boost::asio::error::not_found)
   {
      _resStatusCode = 413;
      sendResponse();
   }

   else
   {
      onFinish();
   }

   return false;
}
