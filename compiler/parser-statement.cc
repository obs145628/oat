#include "parser.hh"
#include "ast-statements.hh"
#include "ast-values.hh"


ASTStatementsBlock* Parser::statement()
{
   if(isTokenOfType(TokenType::lcurlybracket))
   {
      return statements_block();
   }
   else
   {
      return new ASTStatementsBlock(getToken(), {statement_unique()});
   }
}

ASTStatementsBlock* Parser::statements_block()
{
   std::vector<AST*> children;
   Token t = getToken();
   if(!t.isOfType(TokenType::lcurlybracket))
      tokenError(getToken(), "Expected '{' token");
   next();


   while(!isTokenOfType(TokenType::rcurlybracket))
      children.push_back(statement_unique());

   next();
   return new ASTStatementsBlock(t, children);
}

AST* Parser::statement_unique()
{
   if(isTokenOfType(TokenType::lcurlybracket))
   {
      return statements_block();
   }

   else
   {
      return statement_body();
   }
}

AST* Parser::statement_body()
{
   /* statement_empty | statement_define | statement_return | statement_if
    * | statement_while | statement_expr
    */

   Token t = getToken();

   if(t.isOfType(TokenType::semicollon))
   {
      return statement_empty();
   }

   else if(peekToken().isOfType(TokenType::si_define))
   {
      return statement_define();
   }

   else if(t.isOfType(TokenType::si_return))
   {
      return statement_return();
   }

   else if(t.isOfType(TokenType::kw_if))
   {
      return statement_if();
   }

   else if(t.isOfType(TokenType::kw_while))
   {
      return statement_while();
   }

   else
   {
      return statement_expr();
   }
}

ASTStatementEmpty* Parser::statement_empty()
{
   //";"

   Token t = getToken();
   if(!t.isOfType(TokenType::semicollon))
      tokenError(t, "; expected");
   next();
   return new ASTStatementEmpty(t);
}

ASTStatementDefine* Parser::statement_define()
{
   //symbol ":=" expr ";"

   Token symbol = getToken();
   if(!symbol.isOfType(TokenType::symbol))
      tokenError(symbol, "Symbol expected");
   next();

   Token si = getToken();
   if(!si.isOfType(TokenType::si_define))
      tokenError(si, ":= expected");
   next();

   auto res = new ASTStatementDefine(si, new ASTSymbol(symbol), expr());

   if(!isTokenOfType(TokenType::semicollon))
      tokenError(getToken(), "; expected");
   next();
   return res;
}

ASTStatementReturn* Parser::statement_return()
{
   //"=>" expr ";"

   Token t = getToken();
   if(!t.isOfType(TokenType::si_return))
      tokenError(t, "=> expected");
   next();

   auto res = new ASTStatementReturn(t, expr());

   if(!isTokenOfType(TokenType::semicollon))
      tokenError(getToken(), "; expected");
   next();
   return res;
}

ASTStatementIf* Parser::statement_if()
{
   //"if" "(" expr ")" statement ("else" statement)?

   Token t = getToken();
   if(!t.isOfType(TokenType::kw_if))
      tokenError(t, "if expected");
   next();

   if(!isTokenOfType(TokenType::lparenthesis))
      tokenError(getToken(), "'(' expected");
   next();

   AST* condition = expr();
   if(!isTokenOfType(TokenType::rparenthesis))
      tokenError(getToken(), "')' expected");
   next();

   ASTStatementsBlock* ifStatement = statement();
   if(!isTokenOfType(TokenType::kw_else))
      return new ASTStatementIf(t, condition, ifStatement);
   next();

   ASTStatementsBlock* elseStatement = statement();
   return new ASTStatementIf(t, condition, ifStatement, elseStatement);
}

ASTStatementWhile* Parser::statement_while()
{
   //"while" "(" expr ")" statement

   Token t = getToken();
   if(!t.isOfType(TokenType::kw_while))
      tokenError(t, "while expected");
   next();

   if(!isTokenOfType(TokenType::lparenthesis))
      tokenError(getToken(), "'(' expected");
   next();

   AST* condition = expr();
   if(!isTokenOfType(TokenType::rparenthesis))
      tokenError(getToken(), "')' expected");
   next();

   ASTStatementsBlock* whileStatement = statement();
   return new ASTStatementWhile(t, condition, whileStatement);
}

AST* Parser::statement_expr()
{
   //expr ";"

   AST* res = expr();

   if(!isTokenOfType(TokenType::semicollon))
      tokenError(getToken(), "; expected");
   next();
   return res;
}
