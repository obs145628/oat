#ifndef PARSER_HH_
# define PARSER_HH_

#include "scanner.hh"
#include "ast.hh"
#include "token.hh"

class Parser
{

public:
   Parser(Scanner* scanner);
   virtual AST* exec() = 0;

protected:
   Token getToken() const;
   Token getToken(TokenType type) const;
   Token nextToken();
   Token nextToken(TokenType type);
   Token eat(TokenType type);
   Token peekToken();

   Scanner* _scanner;

};

#endif //!PARSER_HH_
