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

   else if(t.isOfType({TokenType::kw_let, TokenType::kw_const}))
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

   else if(t.isOfType(TokenType::kw_do))
   {
      return statement_do();
   }

   else if(t.isOfType(TokenType::kw_for))
   {
      return statement_for();
   }

   else if(t.isOfType(TokenType::kw_break))
   {
      return statement_break();
   }

   else if(t.isOfType(TokenType::kw_continue))
   {
      return statement_continue();
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
   //("let" | "const") symbol ("=" expr)? ";"

   Token t = getToken();
   if(!t.isOfType({TokenType::kw_let, TokenType::kw_const}))
      tokenError(t, "let / const expected");
   next();

   bool isConst = t.isOfType(TokenType::kw_const);

   Token symbol = getToken();
   if(!symbol.isOfType(TokenType::symbol))
      tokenError(symbol, "Symbol expected");
   next();

   ASTStatementDefine* res;
   if(!isConst && isTokenOfType(TokenType::semicollon))
      res = new ASTStatementDefine(t, new ASTSymbol(symbol));

   else
   {
      Token op = getToken();
      if(!op.isOfType(TokenType::op_assign))
         tokenError(op, "= expected");
      next();
      res = new ASTStatementDefine(t, new ASTSymbol(symbol), expr(), isConst);
   }

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

ASTStatementDo* Parser::statement_do()
{
   // "do" statement  "while" "(" exp ")" ";"

   Token doToken = getToken();
   if(!doToken.isOfType(TokenType::kw_do))
      tokenError(doToken, "do expected");
   next();

   ASTStatementsBlock* doStatement = statement();

   if(!isTokenOfType(TokenType::kw_while))
      tokenError(getToken(), "while expected");
   next();

   if(!isTokenOfType(TokenType::lparenthesis))
      tokenError(getToken(), "'(' expected");
   next();

   AST* condition = expr();
   if(!isTokenOfType(TokenType::rparenthesis))
      tokenError(getToken(), "')' expected");
   next();

   if(!isTokenOfType(TokenType::semicollon))
      tokenError(getToken(), "';' expected");
   next();

   return new ASTStatementDo(doToken, condition, doStatement);
}

ASTStatementFor* Parser::statement_for()
{
   //"for "(" statement_body expr? ";" expr? ")" statement
   AST* init;
   AST* condition;
   AST* inc;
   ASTStatementsBlock* forStatement;

   Token forToken = getToken();
   if(!forToken.isOfType(TokenType::kw_for))
      tokenError(forToken, "for expected");
   next();

   if(!isTokenOfType(TokenType::lparenthesis))
      tokenError(getToken(), "( expected");
   next();

   init = statement_body();

   if(isTokenOfType(TokenType::semicollon))
      condition = new ASTTrue(Token(getToken().getScanner(), "true",
                                    TokenType::kw_true,
                                    getToken().getPosition()));
   else
      condition = expr();

   if(!isTokenOfType(TokenType::semicollon))
      tokenError(getToken(), "; expected");
   next();

   if(isTokenOfType(TokenType::rparenthesis))
      inc = new ASTNull(Token(getToken().getScanner(),  "null",
                              TokenType::kw_null,
                              getToken().getPosition()));
   else
      inc = expr();

   if(!isTokenOfType(TokenType::rparenthesis))
      tokenError(getToken(), ") expected");
   next();

   forStatement = statement();

   return new ASTStatementFor(forToken, init, condition, inc, forStatement);
}

ASTStatementBreak* Parser::statement_break()
{
   // "break" ";"

   Token t = getToken();
   if(!t.isOfType(TokenType::kw_break))
      tokenError(t, "break keyword expected");
   next();

   if(!getToken().isOfType(TokenType::semicollon))
      tokenError(t, "; expected");
   next();

   return new ASTStatementBreak(t);
}

ASTStatementContinue* Parser::statement_continue()
{
   //  "continue" ";"

   Token t = getToken();
   if(!t.isOfType(TokenType::kw_continue))
      tokenError(t, "continue keyword expected");
   next();

   if(!getToken().isOfType(TokenType::semicollon))
      tokenError(t, "; expected");
   next();

   return new ASTStatementContinue(t);
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
