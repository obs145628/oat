#ifndef HTTP_SERVER_HH_
# define HTTP_SERVER_HH_

# include <boost/asio.hpp>
# include <thread>
# include <atomic>
# include <memory>
# include <iostream>
# include "http-service.hh"

class HTTPAcceptor
{
public:
   HTTPAcceptor(boost::asio::io_service& ios, unsigned short port)
      : _ios(ios), _acceptor(_ios, boost::asio::ip::tcp::endpoint(
                                boost::asio::ip::address_v4::any(), port)),
        _stopped(false)
   {

   }

   void start()
   {
      _acceptor.listen();
      initAccept();
   }

   void stop()
   {
      _stopped.store(true);
   }

private:

   void initAccept()
   {
      std::shared_ptr<boost::asio::ip::tcp::socket> sock(
         new boost::asio::ip::tcp::socket(_ios));

      _acceptor.async_accept(*sock.get(),
                             [this, sock](const boost::system::error_code& e) {
                                onAccept(e, sock);
                             });
   }

   void onAccept(const boost::system::error_code& ec,
                 std::shared_ptr<boost::asio::ip::tcp::socket> sock)
   {
      if(ec == 0)
      {
         (new HTTPService(sock))->startHandling();
      }
      else
      {
         std::cout << "Network error: " << ec.value()
                   << "; " << ec.message() << std::endl;
      }

      if(_stopped.load())
         initAccept();
      else
         _acceptor.close();


   }

   boost::asio::io_service& _ios;
   boost::asio::ip::tcp::acceptor _acceptor;
   std::atomic<bool> _stopped;

};

class HTTPServer
{

public:
   HTTPServer();

   void start(unsigned short port, unsigned int threadPoolSize);
   void stop();

private:
   boost::asio::io_service _ios;
   std::unique_ptr<boost::asio::io_service::work> _work;
   std::unique_ptr<HTTPAcceptor> _acc;
   std::vector<std::unique_ptr<std::thread>> _threadPool;
};

#endif //!HTTP_SERVER_HH_
