#include "server.hh"
#include <iostream>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include "io.hh"
#include "sync-tcp-server.hh"
#include "async-tcp-server.hh"
#include "http-server.hh"

using namespace boost;

int serverMain(int, char**)
{
   const unsigned short PORT = 3333;

   try
   {
      /*
      SyncTCPServer server;
      server.start(PORT);
      */

      HTTPServer server;
      server.start(PORT, 10);

      std::cout << "Server listening on PORT " << PORT << std::endl;

      std::this_thread::sleep_for(std::chrono::seconds(60));

      server.stop();
   }


   catch(system::system_error& e)
   {
      std::cout << "Network error: " << e.code() << "; "
                << e.what() << std::endl;
   }

   return 0;
}
