#include "unit.hh"

#include <iostream>
#include "str.hh"
#include "token.hh"
#include <stdexcept>
#include "ast.hh"
#include "expr-parser.hh"
#include "dot-builder-visitor.hh"
#include "statement-parser.hh"
#include "file-parser.hh"
#include "runner.hh"


/*

long exec(const AST* ast)
{
   if(ast->isOfType(TokenType::integer))
      return ast->getValue().getInt();

   else if(ast->isOfType(TokenType::op_plus) && ast->size() == 1)
      return exec(ast->getChild(0));
   else if(ast->isOfType(TokenType::op_minus) && ast->size() == 1)
      return - exec(ast->getChild(0));

   else if(ast->isOfType(TokenType::op_plus))
      return exec(ast->getChild(0)) + exec(ast->getChild(1));
   else if(ast->isOfType(TokenType::op_minus))
      return exec(ast->getChild(0)) - exec(ast->getChild(1));
   else if(ast->isOfType(TokenType::op_mul))
      return exec(ast->getChild(0)) * exec(ast->getChild(1));
   else if(ast->isOfType(TokenType::op_div))
      return exec(ast->getChild(0)) / exec(ast->getChild(1));
   else if(ast->isOfType(TokenType::op_mod))
      return exec(ast->getChild(0)) % exec(ast->getChild(1));

   else
      throw std::runtime_error{"Unknown token type "
            + ast->getValue().getTypeString() };
}

*/

#define BUILD_TREE 1
#define CHECK_TOKENS 0
#define RUN 1

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
      Unit unit("/home/obs/Documents/data.txt");
      while(!unit.scanner().isEof())
      {
         std::cout << unit.scanner().nextToken() << std::endl;
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
