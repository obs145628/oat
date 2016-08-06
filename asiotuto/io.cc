#include "io.hh"
#include <iostream>
#include <sstream>

using namespace boost;

void writeToSocket(asio::ip::tcp::socket& sock, const std::string& message)
{
   std::size_t len = message.length();
   std::size_t written = 0;

   while(written != len)
   {
      written += sock.write_some(asio::buffer(message.c_str() + written,
                                              len - written));
   }
}

std::string readFromSocket(asio::ip::tcp::socket& sock, size_t len)
{
   char* buffer = new char[len];
   std::size_t read = 0;

   while(read != len)
   {
      read += sock.read_some(asio::buffer(buffer + read,
                                          len - read));
   }

   std::string str(buffer);
   delete buffer;
   return str;
}

std::string readLineFromSocket(asio::ip::tcp::socket& sock)
{
   asio::streambuf buf;

   asio::read_until(sock, buf, '\n');
   std::string message;
   std::istream is(&buf);
   std::getline(is, message);
   return message;
}

struct SessionWrite
{
   std::shared_ptr<asio::ip::tcp::socket> sock;
   std::string buf;
   std::size_t written;
};

static void callbackWrite(const boost::system::error_code& ec,
                          std::size_t written,
                          std::shared_ptr<SessionWrite> s)
{
   if(ec != 0)
   {
      std::cout << "Network error: " << ec.value() << ", " << ec.message()
                << std::endl;
      return;
   }

   s->written += written;
   if(s->written == s->buf.length())
      return;

   s->sock->async_write_some(
      asio::buffer(s->buf.c_str() + s->written, s->buf.length() - s->written),
      std::bind(callbackWrite,
                std::placeholders::_1, std::placeholders::_2, s));
}

void asyncWriteToSocket(std::shared_ptr<asio::ip::tcp::socket> sock,
                        const std::string& message)
{
   std::shared_ptr<SessionWrite> s(new SessionWrite);
   s->buf = message;
   s->written = 0;
   s->sock = sock;

   s->sock->async_write_some(
      asio::buffer(s->buf),
      std::bind(callbackWrite,
                std::placeholders::_1, std::placeholders::_2, s));
}

struct SessionRead
{
   std::shared_ptr<asio::ip::tcp::socket> sock;
   std::unique_ptr<char[]> buf;
   std::size_t read;
   unsigned int size;
};


static void callbackRead(const boost::system::error_code& ec,
                         std::size_t read,
                         std::shared_ptr<SessionRead> s)
{
   if(ec != 0)
   {
      std::cout << "Network error: " << ec.value() << ", " << ec.message()
                << std::endl;
      return;
   }

   s->read += read;
   if(s->read == s->size)
      return;

   s->sock->async_read_some(
      asio::buffer(s->buf.get() + s->read, s->size - s->read),
      std::bind(callbackRead,
                std::placeholders::_1, std::placeholders::_2, s));
}

void asyncReadFromSocket(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
{
   std::shared_ptr<SessionRead> s(new SessionRead);
   s->buf.reset(new char[1024]);
   s->read = 0;
   s->sock = sock;
   s->size = 1024;

   s->sock->async_read_some(
      asio::buffer(s->buf.get(), s->size),
      std::bind(callbackRead,
                std::placeholders::_1, std::placeholders::_2, s));
}


void writeUniqueToSocket(boost::asio::ip::tcp::socket& sock,
                   const std::string& message)
{
   asio::write(sock, asio::buffer(message.c_str(), message.size()));
   sock.shutdown(asio::socket_base::shutdown_send);
}

std::string readUniqueFromSocket(boost::asio::ip::tcp::socket& sock)
{
   asio::streambuf buf;

   system::error_code ec;
   asio::read(sock, buf, ec);

   if(ec != asio::error::eof)
      throw system::system_error(ec);

   std::string message;
   std::istream is(&buf);
   is >> message;
   return message;
}
