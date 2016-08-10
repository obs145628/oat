#include "parser.hh"

Parser::Parser(Scanner* scanner)
   : _scanner(scanner)
{

}

Token Parser::getToken() const
{
   return _scanner->getToken();
}

Token Parser::getToken(TokenType type) const
{
   return _scanner->getToken(type);
}

Token Parser::nextToken()
{
   return _scanner->nextToken();
}

Token Parser::nextToken(TokenType type)
{
   return _scanner->nextToken(type);
}

Token Parser::eat(TokenType type)
{
   return _scanner->eat(type);
}

Token Parser::peekToken()
{
   return _scanner->peekToken();
}
