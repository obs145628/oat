#ifndef SYNC_UDP_CLIENT_HH_
# define SYNC_UDP_CLIENT_HH_

#include <boost/asio.hpp>


class SyncUDPClient
{

public:
   SyncUDPClient();
   std::string emulateLongComputationOp(const std::string& address,
                                        unsigned short port,
                                        unsigned int sec);

private:

   void sendRequest(const boost::asio::ip::udp::endpoint& ep,
                    const std::string& req);
   std::string receiveResponse(boost::asio::ip::udp::endpoint& ep);

   boost::asio::io_service _ios;
   boost::asio::ip::udp::socket _sock;

};

#endif //!SYNC_UDP_CLIENT_HH_
