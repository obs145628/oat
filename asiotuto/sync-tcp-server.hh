#ifndef SYNC_TCP_SERVER_HH_
# define SYNC_TCP_SERVER_HH_

# include <boost/asio.hpp>
# include <thread>
# include <atomic>
# include <memory>
# include <iostream>
# include <cctype>

class ServerService
{
public:
   ServerService(){}

   void startHandlingClient(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
   {
      std::thread th([this, sock]() {
            handleClient(sock);
         });

      th.detach();
   }

private:
   void handleClient(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
   {
      try
      {
         boost::asio::streambuf req;
         boost::asio::read_until(*sock.get(), req, '\n');
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

         std::cout << "Sending response: '" << res << "'..." << std::endl;

         boost::asio::write(*sock.get(), boost::asio::buffer(res));
      }
      catch(boost::system::system_error& e)
      {
         std::cout << "Network error: " << e.code() << "; "
                   << e.what() << std::endl;
      }

      delete this;
   }
};

class ServerAcceptor
{
public:
   ServerAcceptor(boost::asio::io_service& ios, unsigned short port)
      : _ios(ios),
        _acceptor(_ios, boost::asio::ip::tcp::endpoint(
                     boost::asio::ip::address_v4::any(), port))
   {
      _acceptor.listen();
   }

   void accept()
   {
      std::shared_ptr<boost::asio::ip::tcp::socket> sock(
         new boost::asio::ip::tcp::socket(_ios));
      _acceptor.accept(*sock.get());

      (new ServerService)->startHandlingClient(sock);
   }

private:
   boost::asio::io_service& _ios;
   boost::asio::ip::tcp::acceptor _acceptor;
};

class SyncTCPServer
{

public:
   SyncTCPServer();

   void start(unsigned short port);
   void stop();

private:
   void run(unsigned short port);

   std::unique_ptr<std::thread> _thread;
   std::atomic<bool> _stop;
   boost::asio::io_service _ios;

};

# endif// !SYNC_TCP_SERVER_HH_
