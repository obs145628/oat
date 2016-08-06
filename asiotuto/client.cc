#include "client.hh"
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include "io.hh"
#include "sync-tcp-client.hh"
#include "sync-udp-client.hh"
#include "async-tcp-client.hh"
#include "http-client.hh"

using namespace boost;

#define ADDRESS "127.0.0.1"
#define PORT 3333

void testServer(SyncTCPClient& client, std::string message)
{
   std::cout << "Request: '" << message << "'" << std::endl;
   std::cout << "Sending request to server..." << std::endl;
   client.sendString(message);

   std::cout << "Waiting for response..." << std::endl;
   std::string res = client.readLine();
   std::cout << "Response: '" << res << "'" << std::endl;
}

void handler(const HTTPRequest& req, const HTTPResponse& res,
             const boost::system::error_code& ec)
{
   if(ec == 0)
   {
      std::cout << "Request #" << req.getId() << " has completed." << std::endl
                << "Response: " << res.getResponse().rdbuf() << std::endl;
   }
   else if(ec == boost::asio::error::operation_aborted)
   {
      std::cout << "Request #" << req.getId()
                << " has been cancelled by the user" << std::endl;
   }
   else
   {
      std::cout << "Request #" << req.getId() << " failed!" << std::endl
                << "Error: " << ec.value() << "; " << ec.message()
                << std::endl;
   }
}

int clientMain(int, char**)
{

   try
   {
      HTTPClient client;

      std::shared_ptr<HTTPRequest> req1 = client.createRequest(1);

      req1->setHost("localhost");
      req1->setUri("/index.html");
      req1->setPort(3333);
      req1->setCallback(handler);
      req1->execute();

      std::this_thread::sleep_for(std::chrono::seconds(15));
      client.close();

      /*

      SyncTCPClient client(ADDRESS, PORT);

      std::cout << "Connection..." << std::endl;
      client.connect();

      testServer(client, "Lol Ipop \n Bonjour moi\n");
      testServer(client, "2800 lol\n");

      std::cout << "Closing connection..." << std::endl;
      client.close();

      */

      /*

      SyncUDPClient client;

      std::cout << "Sending request to server..." << std::endl;
      std::string res = client.emulateLongComputationOp(ADDRESS, PORT, 10);

      std::cout << "Response: '" << res << "'" << std::endl;

      std::cout << "Done !" << std::endl;

      */

      /*

      AsyncTCPClient client;

      client.emulateLongComputationOp(10, ADDRESS, PORT, asyncHandler, 1);

      std::this_thread::sleep_for(std::chrono::seconds(5));

      client.emulateLongComputationOp(11, ADDRESS, PORT, asyncHandler, 2);

      //client.cancelRequest(1);

      std::this_thread::sleep_for(std::chrono::seconds(6));

      client.emulateLongComputationOp(12, ADDRESS, PORT, asyncHandler, 3);

      std::this_thread::sleep_for(std::chrono::seconds(15));

      client.close();

      */
   }

   catch(system::system_error& e)
   {
      std::cout << "System error: " << e.code() << ": "
                << e.what() << std::endl;
      return e.code().value();
   }

   return 0;
}
