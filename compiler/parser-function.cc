#include "parser.hh"
#include "ast-functions.hh"
#include "ast-values.hh"


ASTFunctionDef* Parser::function_def()
{
   //symbol(name) "(" symbol(arg name),* ")" statement

   Token t = getToken();
   if(!t.isOfType(TokenType::symbol))
      tokenError(t, "function name expected");
   next();
   ASTSymbol* name = new ASTSymbol(t);

   if(!isTokenOfType(TokenType::lparenthesis))
      tokenError(getToken(), "( expected");
   next();

   std::vector<ASTSymbol*> args;
   bool hasComma = false;

   while(true)
   {
      Token t = getToken();
      if(!hasComma && t.isOfType(TokenType::rparenthesis))
         break;

      if(!t.isOfType(TokenType::symbol))
         tokenError(t, "argument name expected");
      args.push_back(new ASTSymbol(t));
      next();

      if(isTokenOfType(TokenType::comma))
      {
         hasComma = true;
         next();
      }
      else
      {
         hasComma = false;
      }
   }

   next();
   ASTStatementsBlock* body = statement();
   return new ASTFunctionDef(t, name, args, body);
}
