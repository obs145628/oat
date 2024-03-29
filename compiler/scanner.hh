#ifndef SCANNER_HH_
# define SCANNER_HH_

#include <string>
#include <vector>
#include "token.hh"

class Scanner
{
public:

   Scanner(const std::string& path);

   Scanner(const std::string& dir, const std::string& label,
           const std::string& data);

   std::string getDir() const;
   std::string getLabel() const;
   std::string getData() const;

   bool isEof() const;
   Token getToken() const;

   void next();
   void prev();
   void move(long dp);

   void tokenError(Token t, const std::string& message);

private:
   std::string _dir;
   std::string _label;
   std::string _data;
   const char* _begin;
   const char* _end;
   const char* _it;
   std::size_t _pos;
   std::size_t _line;
   std::vector<Token> _tokens;

   static std::string readFile(const std::string& path);

   void readAll();

   Token readToken();

   Token readInlineComment();
   Token readMultilinesComment();
   Token readLitteral();
   Token readNumber();

   void checkLitteral(const std::string& token);
   bool validChar(const std::string& str);

   Token buildToken(const std::string& token, TokenType type);

   void readTokenError(const std::string& token, const std::string& message);


   bool is_end() const;
   char read_char();
   char peek_char() const;
   std::size_t get_pos() const;
   void set_pos(std::size_t pos);
   void move_pos(std::ptrdiff_t diff);

};

#endif //!SCANNER_HH_
