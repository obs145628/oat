#include <iostream>
#include <stdexcept>
#include "ast.hh"
#include "expr-parser.hh"
#include "dot-builder-visitor.hh"
#include "statement-parser.hh"
#include "file-parser.hh"
#include "runner.hh"
#include "unit.hh"

#define BUILD_TREE 1
#define CHECK_TOKENS 1
#define RUN 0

int main()
{

   if(BUILD_TREE)
   {
      Scanner scanner("/home/obs/Documents/data.txt");
      FileParser parser(&scanner);
      AST* result = parser.exec();
      DotTree* dt = DotBuilderVisitor(result).getTree();
      dt->saveImage("./ast.png");

      delete dt;
      delete result;
   }

   if(CHECK_TOKENS)
   {
      Scanner scanner("/home/obs/Documents/data.txt");
      while(!scanner.isEof())
      {
         std::cout << scanner.nextToken() << std::endl;
      }
   }

   if(RUN)
   {
      Scanner scanner("/home/obs/Documents/data.txt");
      Runner runner(&scanner);
      runner.run();
   }


   return 0;
}
