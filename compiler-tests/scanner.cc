#include "scanner.hh"
#include <stdexcept>
#include <sstream>
#include "chars.hh"

Scanner::Scanner(const std::string& path)
   : _hasFirst(false), _fs(path, std::ios::binary)
{
   if(!_fs.is_open())
      throw std::runtime_error("Unable to open file '" + path + "'.");
}

Token Scanner::nextToken()
{
   if(_hasFirst && _token.isOfType(TokenType::eof))
      throw std::runtime_error{"End Of File reached"};
   _token = readToken();
   _hasFirst = true;
   return _token;
}

Token Scanner::nextToken(TokenType type)
{
   if(!(nextToken().isOfType(type)))
      throw std::runtime_error{"Expected token of type "
            + Token::typeName(type) + ", but got token of type "
            + _token.getTypeString()};

   return _token;
}

Token Scanner::getToken() const
{
   if(!_hasFirst)
      throw std::runtime_error{"No token read yet !"};
   return _token;
}

Token Scanner::getToken(TokenType type) const
{
   if(!(getToken().isOfType(type)))
      throw std::runtime_error{"Expected token of type "
            + Token::typeName(type) + ", but got token of type "
            + _token.getTypeString()};

   return _token;
}

Token Scanner::eat(TokenType type)
{
   getToken(type);
   return nextToken();
}

Token Scanner::peekToken()
{
   if(_hasFirst && _token.isOfType(TokenType::eof))
      throw std::runtime_error{"End Of File reached"};
   auto pos = _fs.tellg();
   Token token = readToken();
   _fs.seekg(pos);
   return token;
}

bool Scanner::isEof() const
{
   return _hasFirst && _token.isOfType(TokenType::eof);
}



Token Scanner::readToken()
{
   char c;
   bool empty = true;

   while(!_fs.eof())
   {
      _fs.get(c);
      if(!chars::isBlanck(c))
      {
         empty = false;
         break;
      }
   }

   if(empty)
      return buildToken("", TokenType::eof);

   if(c == '\'' || c == '"' || c == '`')
   {
      _fs.seekg(-1, std::ios::cur);
      return readLitteral();
   }

   if(chars::isDigit(c))
   {
      _fs.seekg(-1, std::ios::cur);
      return readNumber();
   }

   if(c == '/' && !_fs.eof())
   {
      char next;
      _fs.get(next);

      if(next == '/')
      {
         _fs.seekg(-2, std::ios::cur);
         return readInlineComment();
      }

      if(next == '*')
      {
         _fs.seekg(-2, std::ios::cur);
         return readMultilinesComment();
      }

      _fs.seekg(-1, std::ios::cur);
   }

   bool isSymbol = chars::isSymbol(c);
   std::ostringstream oss;
   oss << c;

   while(!_fs.eof())
   {
      _fs.get(c);
      if(chars::isBlanck(c))
      {
         _fs.seekg(-1, std::ios::cur);
         break;
      }


      if(isSymbol)
      {
         if(!chars::isSymbol(c))
         {
            _fs.seekg(-1, std::ios::cur);
            break;
         }
      }
      else
      {
         std::string sub = oss.str() + std::string(&c, 1);
         std::vector<TokenType> matches = Token::lookupTypes(sub);
         if(matches.empty())
         {
            _fs.seekg(-1, std::ios::cur);
            break;
         }
      }

      oss << c;
   }

   std::string token = oss.str();
   TokenType type = Token::findType(token);

   if(type != TokenType::unknown)
      return buildToken(token, type);

   if(isSymbol)
      return buildToken(token, TokenType::symbol);

   return buildToken(token, TokenType::unknown);
}



Token Scanner::readInlineComment()
{
   char c;
   std::ostringstream oss;

   while(!_fs.eof())
   {
      _fs.get(c);
      if(c == '\n')
      {
         _fs.seekg(-1, std::ios::cur);
         break;
      }

      oss << c;
   }

   return buildToken(oss.str(), TokenType::inline_comment);
}

Token Scanner::readMultilinesComment()
{
   char c;
   std::ostringstream oss;
   bool hasFirst = false;
   bool ok = false;

   oss << "/*";
   _fs.seekg(2, std::ios::cur);

   /*lol **/

   while(!_fs.eof())
   {
      _fs.get(c);
      oss << c;

      if(hasFirst && c == '/')
      {
         ok = true;
         break;
      }

      hasFirst = (c == '*');
   }

   if(!ok)
      throw std::runtime_error{"Unfinished multi-lines comment"};

   return buildToken(oss.str(), TokenType::multilines_comment);
}

Token Scanner::readLitteral()
{
   char c;
   char end;
   std::ostringstream oss;
   bool escaped = false;
   bool ok = false;

   _fs.get(end);
   oss << end;

   while(!_fs.eof())
   {
      _fs.get(c);
      oss << c;

      if(escaped)
         escaped = false;
      else if(c == '\\')
            escaped = true;
      else if(c == end)
      {
         ok = true;
         break;
      }
   }

   if(!ok)
      throw std::runtime_error{"Unfinished litteral"};

   if(end == '\'')
      return buildToken(oss.str(), TokenType::litteral1);
   else if(end == '"')
      return buildToken(oss.str(), TokenType::litteral2);
   else
      return buildToken(oss.str(), TokenType::litteral3);
}

Token Scanner::readNumber()
{
   char c;
   std::ostringstream oss;
   bool hasDot = false;

   while(!_fs.eof())
   {
      _fs.get(c);

      if(c == '.')
      {
         if(hasDot)
            throw std::runtime_error{"Second dot in number"};
         hasDot = true;
      }
      else if(!chars::isDigit(c))
      {
         _fs.seekg(-1, std::ios::cur);
         break;
      }

      oss << c;
   }

   std::string token = oss.str();
   if(token.empty() || token[0] == '.' || token[token.size() - 1] == '.')
      throw std::runtime_error{"Invalid number"};

   return buildToken(token, hasDot ? TokenType::real : TokenType::integer);
}

Token Scanner::buildToken(const std::string& token, TokenType type)
{
   return Token(token, type,
                static_cast<std::size_t>(_fs.tellg()) - token.size());
}
