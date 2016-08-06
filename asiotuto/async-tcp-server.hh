#ifndef ASYNC_TCP_SERVER_HH_
# define ASYNC_TCP_SERVER_HH_

# include <boost/asio.hpp>
# include <thread>
# include <atomic>
# include <memory>
# include <iostream>


class AtsService
{
public:
   AtsService(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
      : _sock(sock)
   {

   }

   void startHandling()
   {
      boost::asio::async_read_until(
         *_sock.get(), _req, '\n',
         [this](const boost::system::error_code& ec, std::size_t len) {
            onRequestReceived(ec, len);
         });
   }

private:

   void onRequestReceived(const boost::system::error_code& ec,
                          std::size_t)
   {
      if(ec != 0)
      {
         std::cout << "Network error: " << ec.value()
                   << "; " << ec.message() << std::endl;
         onFinish();
         return;
      }

      _res = processRequest(_req);

      boost::asio::async_write(
         *_sock.get(), boost::asio::buffer(_res),
         [this](const boost::system::error_code& ec, std::size_t len) {
            onResponseSent(ec, len);
         });

   }

   void onResponseSent(const boost::system::error_code& ec, std::size_t)
   {
      if(ec != 0)
      {
         std::cout << "Network error: " << ec.value()
                   << "; " << ec.message() << std::endl;
      }

      onFinish();
   }

   void onFinish()
   {
      delete this;
   }

   std::string processRequest(boost::asio::streambuf& req)
   {
      std::istream is(&req);
      std::string data;
      std::getline(is, data);

      std::cout << "Request received: '" << data << "'" << std::endl;

      int i = 0;
      while(i != 100000)
         ++i;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      for(size_t i = 0; i < data.size(); ++i)
         data[i] = std::toupper(data[i]);
      std::string res = data + "\n";
      return res;
   }

   std::shared_ptr<boost::asio::ip::tcp::socket> _sock;
   std::string _res;
   boost::asio::streambuf _req;
};

class AtsAcceptor
{
public:
   AtsAcceptor(boost::asio::io_service& ios, unsigned short port)
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
         (new AtsService(sock))->startHandling();
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

class AsyncTCPServer
{

public:
   AsyncTCPServer();

   void start(unsigned short port, unsigned int threadPoolSize);
   void stop();

private:
   boost::asio::io_service _ios;
   std::unique_ptr<boost::asio::io_service::work> _work;
   std::unique_ptr<AtsAcceptor> _acc;
   std::vector<std::unique_ptr<std::thread>> _threadPool;
};

#endif //!ASYNC_TCP_SERVER_HH_
