#ifndef SYNC_TCP_CLIENT_HH_
# define SYNC_TCP_CLIENT_HH_

#include <boost/asio.hpp>


class SyncTCPClient
{

public:
   SyncTCPClient(const std::string address,
                 unsigned short port);
   void connect();
   void close();
   std::string emulateLongComputationOp(unsigned int sec);

   void sendString(const std::string& req);
   std::string readLine();

private:

   void sendRequest(const std::string& req);
   std::string receiveResponse();

   boost::asio::io_service _ios;
   boost::asio::ip::tcp::endpoint _ep;
   boost::asio::ip::tcp::socket _sock;

};

#endif //!SYNC_TCP_CLIENT_HH_
