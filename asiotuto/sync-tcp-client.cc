#include "sync-tcp-client.hh"
#include <iostream>

using namespace boost;

SyncTCPClient::SyncTCPClient(const std::string address,
                             unsigned short port)
   : _ep(asio::ip::address::from_string(address), port),
     _sock(_ios)
{
   _sock.open(_ep.protocol());
}

void SyncTCPClient::connect()
{
   _sock.connect(_ep);
}

void SyncTCPClient::close()
{
   _sock.shutdown(asio::ip::tcp::socket::shutdown_both);
   _sock.close();
}

std::string SyncTCPClient::emulateLongComputationOp(unsigned int sec)
{
   std::string req = "EMULATE_LONG_COMP_OP " + std::to_string(sec)
      + "\n";
   sendRequest(req);
   return receiveResponse();
}

void SyncTCPClient::sendString(const std::string& req)
{
   sendRequest(req);
}

std::string SyncTCPClient::readLine()
{
   return receiveResponse();
}

void SyncTCPClient::sendRequest(const std::string& req)
{
   asio::write(_sock, asio::buffer(req));
}

std::string SyncTCPClient::receiveResponse()
{
   asio::streambuf buf;
   asio::read_until(_sock, buf, '\n');

   std::istream is(&buf);
   std::string res;
   std::getline(is, res);
   return res;
}
