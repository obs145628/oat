#include <iostream>
#include <string.h>
#include <stdexcept>
#include "compiler.hh"

int main(int argv, char** argc)
{

   std::string srcPath;
   std::string dstPath;
   bool outOption = false;

   for(int i = 1; i < argv; ++i)
   {
      std::string arg(argc[i]);

      if(outOption)
         dstPath = arg;

      else if(arg == "-o")
         outOption = true;

      else
         srcPath = arg;
   }

   if(srcPath.empty())
   {
      std::cerr << "No input file" << std::endl;
      return 1;
   }

   if(dstPath.empty())
   {
      std::cerr << "No output file" << std::endl;
      return 1;
   }

   try
   {
      Compiler compiler(srcPath);
      compiler.compileToPath(dstPath);
   }

   catch(const std::exception &e)
   {
      std::cerr << e.what() << std::endl;
      std::cerr << "Compilation aborted !" << std::endl;
      return 1;
   }

   return 0;
}
