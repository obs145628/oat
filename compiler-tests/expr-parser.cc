#include "expr-parser.hh"
#include "scanner.hh"
#include "ast-operators.hh"
#include "ast-values.hh"
#include "ast-functions.hh"


ExprParser::ExprParser(Scanner* scanner)
   : Parser(scanner)
{

}


AST* ExprParser::exec()
{
   nextToken();
   return expr();
}

AST* ExprParser::factor()
{
   Token t = getToken();
   nextToken();

   if(t.isOfType(TokenType::op_plus))
   {
      return new ASTOp1Plus(t, factor());
   }

   else if(t.isOfType(TokenType::op_minus))
   {
      return new ASTOp1Minus(t, factor());
   }

   else if(t.getType() == TokenType::lparenthesis)
   {
      AST* ast = expr();
      eat(TokenType::rparenthesis);
      return ast;
   }

   else if(t.isOfType(TokenType::integer))
   {
      return new ASTIntValue(t);
   }

   else if(t.isOfType(TokenType::symbol))
   {
      if(!getToken().isOfType(TokenType::lparenthesis))
         return new ASTVarValue(t);

      std::vector<AST*> children;
      children.push_back(new ASTVarValue(t));
      nextToken();


      bool hasComma = false;
      while(true)
      {
         Token t = getToken();
         if(!hasComma & t.isOfType(TokenType::rparenthesis))
            break;

         children.push_back(expr());

         Token next = getToken();
         if(next.isOfType(TokenType::comma))
         {
            hasComma = true;
            nextToken();
         }
         else
            hasComma = false;
      }

      nextToken();
      return new ASTFunctionCall(t, children);
   }

   else
   {
      throw std::runtime_error{"Unexpected token of type " + t.getTypeString()};
   }

}

AST* ExprParser::term()
{
   AST* result = factor();

   while(true)
   {
      Token t = getToken();

      if(t.isOfType(TokenType::op_mul))
      {
         nextToken();
         result = new ASTOp2Mul(t, result, factor());
      }
      else if(t.isOfType(TokenType::op_div))
      {
         nextToken();
         result = new ASTOp2Div(t, result, factor());
      }
      else if(t.isOfType(TokenType::op_mod))
      {
         nextToken();
         result = new ASTOp2Mod(t, result, factor());
      }
      else
         break;
   }

   return result;
}

AST* ExprParser::expr()
{
   AST* result = term();

   while(true)
   {
      Token t = getToken();

      if(t.isOfType(TokenType::op_plus))
      {
         nextToken();
         result = new ASTOp2Plus(t, result, term());
      }
      else if(t.isOfType(TokenType::op_minus))
      {
         nextToken();
         result = new ASTOp2Minus(t, result, term());
      }
      else
         break;
   }

   return result;
}
