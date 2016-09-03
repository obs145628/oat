#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client.h"
#include "server.h"
#include "utils.h"

int main(int argc, char** argv)
{
   if(argc < 2)
      error("missing argument");

   char* mode = argv[1];

   if(!strcmp(mode, "client"))
      return runClient(argc, argv);
   else if(!strcmp(mode, "server"))
      return runServer(argc, argv);
   else
      error("Unknown mode");

   return 0;
}
