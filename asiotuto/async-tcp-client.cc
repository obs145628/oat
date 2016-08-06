#include "async-tcp-client.hh"
#include <iostream>

using namespace boost;

AsyncTCPClient::AsyncTCPClient()
{
   _work.reset(new boost::asio::io_service::work(_ios));
   _thread.reset(new std::thread([this]() {
            _ios.run();
         }));
}

void AsyncTCPClient::emulateLongComputationOp(unsigned int sec,
                                              const std::string& address,
                                              unsigned short port,
                                              Callback cb, unsigned int id)
{
   std::string req = "EMULATE_LONG_COMP_OP " + std::to_string(sec)
      + "\n";

   std::shared_ptr<TCSession> session(new TCSession(_ios,
                                                    address,
                                                    port,
                                                    req,
                                                    id,
                                                    cb));

   session->_sock.open(session->_ep.protocol());

   std::unique_lock<std::mutex> lock(_activeSessionGuard);
   _activeSessions[id] = session;
   lock.unlock();

   session->_sock.async_connect(
      session->_ep,
      [this, session](const system::error_code& ec) {
         if(ec != 0)
         {
            session->_ec = ec;
            onRequestComplete(session);
            return;
         }

         asio::async_write(
            session->_sock,
            asio::buffer(session->_req),
            [this, session] (const boost::system::error_code& ec,
                             std::size_t)
            {
               if(ec != 0)
               {
                  session->_ec = ec;
                  onRequestComplete(session);
                  return;
               }

               asio::async_read_until(
                  session->_sock,
                  session->_resBuf,
                  '\n',
                  [this, session](const boost::system::error_code& ec,
                                  std::size_t)
                  {
                     if(ec != 0)
                        session->_ec = ec;

                     else
                     {
                        std::istream is(&session->_resBuf);
                        std::getline(is, session->_res);
                     }

                     onRequestComplete(session);
                  });

            });

      });
}


void AsyncTCPClient::cancelRequest(unsigned int id)
{
   std::unique_lock<std::mutex> lock(_activeSessionGuard);

   auto it = _activeSessions.find(id);
   if(it != _activeSessions.end())
   {
      std::unique_lock<std::mutex> cancelLock(it->second->_cancelGuard);
      it->second->_cancelled = true;
      it->second->_sock.cancel();
   }
}

void AsyncTCPClient::close()
{
   _work.reset(NULL);
   _thread->join();
}

void AsyncTCPClient::onRequestComplete(std::shared_ptr<TCSession> session)
{
   boost::system::error_code ignoredEc;
   session->_sock.shutdown(asio::ip::tcp::socket::shutdown_both, ignoredEc);

   std::unique_lock<std::mutex> lock(_activeSessionGuard);

   auto it = _activeSessions.find(session->_id);
   if(it != _activeSessions.end())
      _activeSessions.erase(it);
   lock.unlock();

   boost::system::error_code ec = (session->_ec == 0 && session->_cancelled) ?
      asio::error::operation_aborted : session->_ec;

   session->_cb(session->_id, session->_res, ec);
}

void asyncHandler(unsigned int id, const std::string& res,
                  const boost::system::error_code& ec)
{
   if(ec == 0)
   {
      std::cout << "Request #" << id << " has completed. Response: "
                << res << std::endl;
   }

   else if(ec == asio::error::operation_aborted)
   {
      std::cout << "Request #" << id << " has been canceled vy the user."
                << std::endl;
   }

   else
   {
      std::cout << "Request #" << id << " failed. Error: "
                << ec.value() << "; " << ec.message()
                << std::endl;
   }
}
