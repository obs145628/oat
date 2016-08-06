#ifndef ASYNC_TCP_CLIENT_HH_
# define ASYNC_TCP_CLIENT_HH_

#include <boost/asio.hpp>
#include <mutex>
#include <thread>
#include <memory>
#include <iostream>
#include <map>

typedef void(*Callback) (unsigned int id, const std::string& res,
                         const boost::system::error_code& ec);

struct TCSession
{
   TCSession(boost::asio::io_service& ios,
             const std::string address,
             unsigned short port,
             const std::string req,
             unsigned int id,
             Callback cb)
      : _sock(ios),
        _ep(boost::asio::ip::address::from_string(address), port),
        _req(req),
        _id(id),
        _cb(cb),
        _cancelled(false)
   {

   }

   boost::asio::ip::tcp::socket _sock;
   boost::asio::ip::tcp::endpoint _ep;
   std::string _req;
   boost::asio::streambuf _resBuf;
   std::string _res;
   boost::system::error_code _ec;
   unsigned int _id;
   Callback _cb;
   bool _cancelled;
   std::mutex _cancelGuard;
};

class AsyncTCPClient : public boost::noncopyable
{

public:
   AsyncTCPClient();
   void emulateLongComputationOp(unsigned int sec,
                                 const std::string& address,
                                 unsigned short port,
                                 Callback cb, unsigned int id);

   void cancelRequest(unsigned int id);
   void close();

private:

   void onRequestComplete(std::shared_ptr<TCSession> session);

   boost::asio::io_service _ios;
   std::map<int, std::shared_ptr<TCSession>> _activeSessions;
   std::mutex _activeSessionGuard;
   std::unique_ptr<boost::asio::io_service::work> _work;
   std::unique_ptr<std::thread> _thread;
};

void asyncHandler(unsigned int id, const std::string& res,
                  const boost::system::error_code& ec);

#endif //!ASYNC_TCP_CLIENT_HH_
