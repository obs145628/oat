#include "sync-tcp-server.hh"

SyncTCPServer::SyncTCPServer()
   : _stop(false)
{

}

void SyncTCPServer::start(unsigned short port)
{
   _thread.reset(new std::thread([this, port] {
            run(port);
         }));
}

void SyncTCPServer::stop()
{
   _stop.store(true);
   _thread->join();
}

void SyncTCPServer::run(unsigned short port)
{
   ServerAcceptor acc(_ios, port);
   while(!_stop.load())
      acc.accept();
}
