#include "http-client.hh"

HTTPClient::HTTPClient()
{
   _work.reset(new boost::asio::io_service::work(_ios));
   _thread.reset(new std::thread([this]() {
            _ios.run();
         }));
}


std::shared_ptr<HTTPRequest> HTTPClient::createRequest(unsigned int id)
{
   return std::shared_ptr<HTTPRequest>(new HTTPRequest(_ios, id));
}

void HTTPClient::close()
{
   _work.reset(nullptr);
   _thread->join();
}
