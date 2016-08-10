#include "file-parser.hh"
#include "ast-modules.hh"
#include "ast-functions.hh"
#include "ast-values.hh"
#include "statement-parser.hh"

FileParser::FileParser(Scanner* scanner)
   : Parser(scanner)
{

}

AST* FileParser::exec()
{
   nextToken();
   return program();
}

AST* FileParser::program()
{
   std::vector<AST*> functions;

   while(!(getToken()).isOfType(TokenType::eof))
   {
      functions.push_back(function_definition());
   }

   return new ASTProgram(getToken(), functions);
}

AST* FileParser::function_definition()
{
   std::vector<AST*> children;

   Token nameToken = getToken(TokenType::symbol);
   children.push_back(new ASTVarValue(nameToken));
   nextToken(TokenType::lparenthesis);
   nextToken();

   bool hasComma = false;
   while(true)
   {
      Token t = getToken();
      if(!hasComma & t.isOfType(TokenType::rparenthesis))
         break;

      getToken(TokenType::symbol);
      children.push_back(new ASTVarValue(t));

      Token next = nextToken();
      if(next.isOfType(TokenType::comma))
      {
         hasComma = true;
         nextToken();
      }
      else
         hasComma = false;
   }

   StatementParser parser(_scanner);
   children.push_back(parser.exec());

   return new ASTFunctionDef(nameToken, children);
}
