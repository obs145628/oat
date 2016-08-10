#ifndef SCANNER_HH_
# define SCANNER_HH_

#include <string>
#include <fstream>
#include "token.hh"

class Scanner
{
public:
   Scanner(const std::string& path);

   Token nextToken();
   Token nextToken(TokenType type);
   Token getToken() const;
   Token getToken(TokenType type) const;
   Token eat(TokenType type);
   Token peekToken();
   bool isEof() const;

private:
   bool _hasFirst;
   Token _token;

   Token readToken();

   Token readInlineComment();
   Token readMultilinesComment();
   Token readLitteral();
   Token readNumber();

   Token buildToken(const std::string& token, TokenType type);

   std::ifstream _fs;

};

#endif //!SCANNER_HH_
