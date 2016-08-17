#include <iostream>
#include "scanner.hh"
#include "compiler.hh"
#include "dot-tree.hh"


#define DISPLAY_TOKENS 0
#define COMPILE 1
#define BUILD_TREE 1

#define SOURCE "/home/obs/it/oat/tests/main.oat"
#define OUT "/home/obs/it/oat/tests/main.oatbin"
#define OUT_TREE "/home/obs/it/oat/tests/ast.png"

int main()
{

   if(DISPLAY_TOKENS)
   {
      Scanner scanner = Scanner::fromFile(SOURCE);
      while(!scanner.isEof())
      {
         Token t = scanner.getToken();
         scanner.next();
         std::cout << t << std::endl;
      }
   }

   if(COMPILE)
   {
      Compiler compiler(SOURCE);
      compiler.compileToPath(OUT);

      if(BUILD_TREE)
      {
         DotTree* tree = compiler.buildDotTree();
         tree->saveImage(OUT_TREE);
         delete tree;
      }
   }
}
