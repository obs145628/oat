#include "parser.hh"
#include "ast-class.hh"
#include "ast-values.hh"


ASTClass* Parser::class_def()
{
   //"export"? "class" symbol "{" clas_field* "}"

   std::vector<ASTClassField*> fields;
   ASTSymbol* symbol;
   bool exported;

   if(isTokenOfType(TokenType::kw_export))
   {
      exported = true;
      next();
   }
   else
      exported = false;

   Token classToken = getToken();
   if(!classToken.isOfType(TokenType::kw_class))
      tokenError(classToken, "class keywork expected");
   next();

   if(!isTokenOfType(TokenType::symbol))
      tokenError(getToken(), "class name expected");
   symbol = new ASTSymbol(getToken());
   next();

   if(!isTokenOfType(TokenType::lcurlybracket))
      tokenError(getToken(), "{ expected");
   next();

   while(!isTokenOfType(TokenType::rcurlybracket))
      fields.push_back(class_field());
   next();

   return new ASTClass(classToken, symbol, fields, exported);
}

ASTClassField* Parser::class_field()
{
   //class_method | class_var

   Token t = getToken();
   if(t.isOfType(TokenType::kw_static))
      t = peekToken();

   if(t.isOfType({TokenType::kw_let, TokenType::kw_const}))
      return class_var();
   else
      return class_method();
}

ASTClassMethod* Parser::class_method()
{
    //"static"? symbol(name) "(" symbol(arg name),* ")" statement
   bool isStatic;
   if(isTokenOfType(TokenType::kw_static))
   {
      isStatic = true;
      next();
   }
   else
      isStatic = false;

   Token t = getToken();
   if(!t.isOfType(TokenType::symbol))
      tokenError(t, "method name expected");
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
   return new ASTClassMethod(t, name, args, body, isStatic);
}

ASTClassVariable* Parser::class_var()
{
    //"static"? ("let" | "const") symbol ("=" expr)? ";"
   bool isStatic;
   if(isTokenOfType(TokenType::kw_static))
   {
      isStatic = true;
      next();
   }
   else
      isStatic = false;

   Token t = getToken();
   if(!t.isOfType({TokenType::kw_let, TokenType::kw_const}))
      tokenError(t, "let / const expected");
   next();

   bool isConst = t.isOfType(TokenType::kw_const);

   Token symbol = getToken();
   if(!symbol.isOfType(TokenType::symbol))
      tokenError(symbol, "Symbol expected");
   next();

   ASTClassVariable* res;
   if(!isConst && isTokenOfType(TokenType::semicollon))
      res = new ASTClassVariable(t, new ASTSymbol(symbol),
                                 isStatic);

   else
   {
      Token op = getToken();
      if(!op.isOfType(TokenType::op_assign))
         tokenError(op, "= expected");
      next();
      res = new ASTClassVariable(t, new ASTSymbol(symbol), expr(), isConst,
                                 isStatic);
   }

   if(!isTokenOfType(TokenType::semicollon))
      tokenError(getToken(), "; expected");
   next();
   return res;
}
