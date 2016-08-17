#include "parser.hh"
#include "ast-modules.hh"
#include "ast-functions.hh"


ASTModule* Parser::module()
{
   std::vector<AST*> children;
   while(!isTokenOfType(TokenType::eof))
      children.push_back(function_def());
   return new ASTModule(getToken(), children);
}
