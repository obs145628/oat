#include <iostream>
#include "client.hh"
#include "server.hh"

int main(int argc, char** argv)
{

   if(argc != 2)
   {
      std::cout << "Invalid number of arguments: expected 2, got " << argc
                << std::endl;
      return 1;
   }

   std::string command {argv[1]};

   if(command == "client")
      return clientMain(argc, argv);
   else if(command == "server")
      return serverMain(argc, argv);

   else
   {
      std::cout << "Unknown command: " << command << std::endl;
      return 1;
   }

}
