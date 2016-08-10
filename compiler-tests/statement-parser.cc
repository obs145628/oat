#include "statement-parser.hh"
#include "ast-statements.hh"
#include "ast-values.hh"
#include "expr-parser.hh"
#include <stdexcept>

StatementParser::StatementParser(Scanner* scanner)
   : Parser(scanner)
{

}

AST* StatementParser::exec()
{
   nextToken();
   return statement();
}


AST* StatementParser::statement()
{
   std::vector<AST*> list;
   Token t = getToken();

   if(t.isOfType(TokenType::lcurlybracket))
   {
      nextToken();
      while(!(getToken().isOfType(TokenType::rcurlybracket)))
      {
         list.push_back(statement());
      }
      nextToken();
   }

   else
   {
      list.push_back(body());
      eat(TokenType::semicollon);
   }

   return new ASTStatementsList(t, list);
}

AST* StatementParser::body()
{
   Token t = getToken();

   if(t.isOfType(TokenType::semicollon))
   {
      return new ASTStatementEmpty(t);
   }

   else if(t.isOfType(TokenType::kw_let))
   {
      return let();
   }

   else if(t.isOfType(TokenType::kw_print))
   {
      
      return print();
   }

   else if(t.isOfType(TokenType::kw_return))
   {
      return statement_return();
   }

   else
   {
      ExprParser parser(_scanner);
      return parser.exec();
   }
}

AST* StatementParser::let()
{
   Token symbol = nextToken(TokenType::symbol);
   nextToken(TokenType::op_assign);

   ExprParser parser(_scanner);
   return new ASTStatementLet(getToken(),
                              new ASTVarValue(symbol),  parser.exec());
}

AST* StatementParser::print()
{
   ExprParser parser(_scanner);
   return new ASTStatementPrint(getToken(), parser.exec());
}

AST* StatementParser::statement_return()
{
   ExprParser parser(_scanner);
   return new ASTStatementReturn(getToken(), parser.exec());
}
