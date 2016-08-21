#include "parser.hh"
#include "ast-modules.hh"
#include "ast-functions.hh"
#include "ast-values.hh"


ASTModule* Parser::module()
{
   std::vector<AST*> children;
   while(!isTokenOfType(TokenType::eof))
      component(children);
   return new ASTModule(getToken(), children);
}

void Parser::component(std::vector<AST*>& children)
{
   if(isTokenOfType({TokenType::kw_let, TokenType::kw_const}))
      children.push_back(global_def());
   else
      children.push_back(function_def());
}

ASTGlobalDef* Parser::global_def()
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

   ASTGlobalDef* res;
   if(!isConst && isTokenOfType(TokenType::semicollon))
      res = new ASTGlobalDef(t, new ASTSymbol(symbol),
                             false, 0);

   else
   {
      Token op = getToken();
      if(!op.isOfType(TokenType::op_assign))
         tokenError(op, "= expected");
      next();
      res = new ASTGlobalDef(t, new ASTSymbol(symbol), expr(), isConst,
                             false, 0);
   }

   if(!isTokenOfType(TokenType::semicollon))
      tokenError(getToken(), "; expected");
   next();
   return res;
}
