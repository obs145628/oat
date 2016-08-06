#include "sync-udp-client.hh"
#include <iostream>

using namespace boost;

SyncUDPClient::SyncUDPClient()
   : _sock(_ios)
{
   _sock.open(asio::ip::udp::v4());
}

std::string SyncUDPClient::emulateLongComputationOp(const std::string& address,
                                                    unsigned short port,
                                                    unsigned int sec)
{
   std::string req = "EMULATE_LONG_COMP_OP " + std::to_string(sec)
      + "\n";
   asio::ip::udp::endpoint ep(asio::ip::address::from_string(address), port);

   sendRequest(ep, req);
   return receiveResponse(ep);
}

void SyncUDPClient::sendRequest(const boost::asio::ip::udp::endpoint& ep,
                                const std::string& req)
{
   _sock.send_to(asio::buffer(req), ep);
}

std::string SyncUDPClient::receiveResponse(boost::asio::ip::udp::endpoint& ep)
{
   char response[1024];

   std::size_t len = _sock.receive_from(asio::buffer(response), ep);
   //_sock.shutdown(asio::ip::udp::socket::shutdown_both);
   return std::string(response, len);
}
