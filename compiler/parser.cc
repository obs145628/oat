#include "parser.hh"
#include "scanner.hh"


#include "ast-modules.hh"


Parser::Parser(Scanner* scanner)
   : _scanner(scanner)
{

}

AST* Parser::buildAST()
{
   return module();
}


bool Parser::isEof() const
{
   return _scanner->isEof();
}

Token Parser::getToken() const
{
   return _scanner->getToken();
}

void Parser::next()
{
   _scanner->next();
}

void Parser::prev()
{
   _scanner->prev();
}

void Parser::move(long dp)
{
   _scanner->move(dp);
}

void Parser::tokenError(Token t, const std::string& message)
{
   _scanner->tokenError(t, message);
}


TokenType Parser::getTokenType() const
{
   return _scanner->getToken().getType();
}

bool Parser::isTokenOfType(TokenType t) const
{
   return _scanner->getToken().isOfType(t);
}

bool Parser::isTokenOfType(const std::vector<TokenType>& types) const
{
   return _scanner->getToken().isOfType(types);
}

Token Parser::peekToken()
{
   next();
   Token t = getToken();
   prev();
   return t;
}
