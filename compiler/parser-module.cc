#include "parser.hh"
#include "ast-modules.hh"
#include "ast-functions.hh"
#include "ast-values.hh"
#include "str.hh"
#include "scanner.hh"


ASTModule* Parser::module()
{
   std::vector<AST*> children;
   while(!isTokenOfType(TokenType::eof))
      component(children);
   return new ASTModule(getToken(), children);
}

void Parser::component(std::vector<AST*>& children)
{

   Token t = getToken();
   if(t.isOfType(TokenType::kw_export))
      t = peekToken();

   if(t.isOfType({TokenType::kw_let, TokenType::kw_const}))
      children.push_back(global_def());
   else if(t.isOfType(TokenType::kw_import))
      import_def(children);
   else
      children.push_back(function_def());
}

ASTGlobalDef* Parser::global_def()
{
   //"export"? ("let" | "const") symbol ("=" expr)? ";"
   bool exported;
   if(isTokenOfType(TokenType::kw_export))
   {
      exported = true;
      next();
   }
   else
      exported = false;

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
                             exported);

   else
   {
      Token op = getToken();
      if(!op.isOfType(TokenType::op_assign))
         tokenError(op, "= expected");
      next();
      res = new ASTGlobalDef(t, new ASTSymbol(symbol), expr(), isConst,
                             exported);
   }

   if(!isTokenOfType(TokenType::semicollon))
      tokenError(getToken(), "; expected");
   next();
   return res;
}


void Parser::import_def(std::vector<AST*>& child)
{
   //import_def : "import" litteral2 ";"

   if(!isTokenOfType(TokenType::kw_import))
      tokenError(getToken(), "import expected");
   next();

   Token t = getToken();
   if(!t.isOfType(TokenType::litteral2))
      tokenError(t, "litteral path expected");
   next();

   if(!isTokenOfType(TokenType::semicollon))
      tokenError(t, "; expected");
   next();

   std::string dir = _scanner->getDir();
   std::string path = str::pathJoin(dir, t.getLitteral2());

   if(_files.find(path) != _files.end())
      return;
   _files.insert(path);

   Scanner* copy = _scanner;
   Scanner* scanner = new Scanner(path);
   _pool.push_back(scanner);

   _scanner = scanner;

   AST* imported = buildAST();
   _scanner = copy;

   for(AST* importedChild: imported->getChildren())
      child.push_back(importedChild);
}
