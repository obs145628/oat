#include "http-server.hh"
#include <cassert>

HTTPServer::HTTPServer()
{
   _work.reset(new boost::asio::io_service::work(_ios));
}

void HTTPServer::start(unsigned short port, unsigned int threadPoolSize)
{
   assert(threadPoolSize > 0);

   _acc.reset(new HTTPAcceptor(_ios, port));
   _acc->start();

   for(unsigned int i = 0; i < threadPoolSize; ++i)
   {
      std::unique_ptr<std::thread> th(
         new std::thread([this]() {
               _ios.run();
            }));
      _threadPool.push_back(std::move(th));
   }
}

void HTTPServer::stop()
{
   _acc->stop();
   _ios.stop();
   for(auto& th: _threadPool)
      th->join();
}
