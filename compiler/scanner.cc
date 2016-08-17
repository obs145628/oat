#include "scanner.hh"
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <cassert>
#include "chars.hh"

Scanner::Scanner(const std::string& label, const std::string& data)
   : _label(label), _data(data),
     _begin(_data.data()), _end(_begin + _data.size()), _it(_begin),
     _pos(0)
{
   readAll();
   assert(_tokens.size() != 0);
}

Scanner Scanner::fromFile(const std::string& path)
{
   std::ifstream fs(path, std::ios::binary);
   if(!fs.is_open())
      throw std::runtime_error{"Unable to open file '" + path + "'"};

   return Scanner(path, std::string(
                     std::istreambuf_iterator<char>(fs),
                     std::istreambuf_iterator<char>()
                     ));
}




bool Scanner::isEof() const
{
   assert(_pos < _tokens.size());
   return _pos + 1 == _tokens.size();
}

Token Scanner::getToken() const
{
   assert(_pos < _tokens.size());
   return _tokens[_pos];
}

void Scanner::next()
{
   move(+1);
}

void Scanner::prev()
{
   move(-1);
}

void Scanner::move(long dp)
{
   assert(_pos < _tokens.size());
   std::size_t dist;

   if(dp < 0)
   {
      dist = static_cast<size_t> (-dp);
      if(dist > _pos)
         tokenError(_tokens[0], "Unexpected begin of file");
      _pos -= dist;
   }

   else
   {
      dist = static_cast<size_t> (dp);
      if(_pos + dist >= _tokens.size())
         tokenError(_tokens[_tokens.size() - 1], "Unexpected end of file");
      _pos += dist;
   }
}

void Scanner::tokenError(Token t, const std::string& message)
{
   std::ostringstream oss;
   char c;
   size_t pos = t.getPosition();

   oss << "Compilation error: " << std::endl;
   oss << "Token: " << t << std::endl;
   oss << "Message: " << message << std::endl << std::endl;

   size_t row = 1;
   size_t col = 1;
   const char* it = _begin;
   for(size_t i = 0; i < pos; ++i)
   {
      c = *(it++);
      if(c == '\n')
      {
         ++row;
         col = 1;
      }
      else
         ++col;
   }

   oss << _label << ":" << row << ":" << col << ":" << std::endl;



   size_t dec = 0;
   size_t len = 0;

   it = _begin + t.getPosition();
   while(it != _end)
   {
      c = *(it++);
      if(c == '\n')
         break;

      oss << c;
      ++len;
   }

   len = std::min(len, t.getRepresentation().size());
   oss << std::endl;
   for(size_t i = 0; i < dec; ++i)
      oss << ' ';
   for(size_t i = 0; i < len; ++i)
      oss << '^';
   oss << std::endl;



   throw std::runtime_error{oss.str()};
}



void Scanner::readAll()
{
   _it = _begin;
   _tokens.clear();

   while(true)
   {
      Token t = readToken();
      if(t.isOfType({TokenType::inline_comment,
                  TokenType::multilines_comment}))
         continue;

      if(t.isOfType(TokenType::unknown))
         tokenError(t, "Unknown token");
      if(t.isOfType(TokenType::reserved))
         tokenError(t, "Reserved token");

      _tokens.push_back(t);
      if(t.isOfType(TokenType::eof))
         break;
   }
}

Token Scanner::readToken()
{
   char c;
   bool empty = true;

   while(!is_end())
   {
      c = read_char();
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
      move_pos(-1);
      return readLitteral();
   }

   if(chars::isDigit(c))
   {
      move_pos(-1);
      return readNumber();
   }

   if(c == '/' && !is_end())
   {
      char next = read_char();

      if(next == '/')
      {
         move_pos(-2);
         return readInlineComment();
      }

      if(next == '*')
      {
         move_pos(-2);
         return readMultilinesComment();
      }

      move_pos(-1);
   }

   bool isSymbol = chars::isSymbol(c);
   std::ostringstream oss;
   oss << c;

   while(!is_end())
   {
      c = read_char();
      if(chars::isBlanck(c))
      {
         move_pos(-1);
         break;
      }


      if(isSymbol)
      {
         if(!chars::isSymbol(c))
         {
            move_pos(-1);
            break;
         }
      }
      else
      {
         std::string sub = oss.str() + std::string(&c, 1);
         std::vector<TokenType> matches = Token::lookupTypes(sub);
         if(matches.empty())
         {
            move_pos(-1);
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

   while(!is_end())
   {
      c = read_char();
      if(c == '\n')
      {
         move_pos(-1);
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
   move_pos(2);

   while(!is_end())
   {
      c = read_char();
      oss << c;

      if(hasFirst && c == '/')
      {
         ok = true;
         break;
      }

      hasFirst = (c == '*');
   }

      if(!ok)
         readTokenError(oss.str(), "Unfinished multi-lines comment");

   return buildToken(oss.str(), TokenType::multilines_comment);
}

Token Scanner::readLitteral()
{
   char c;
   char end;
   std::ostringstream oss;
   bool escaped = false;
   bool ok = false;

   end = read_char();
   oss << end;

   while(!is_end())
   {
      c = read_char();
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
      readTokenError(oss.str(), "Unfinished litteral");

   checkLitteral(oss.str());
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

   while(!is_end())
   {
      c = read_char();

      if(c == '.')
      {
         if(hasDot)
         {
            oss << '.';
            readTokenError(oss.str(), "Invalid number");
         }
         hasDot = true;
      }
      else if(!chars::isDigit(c))
      {
         move_pos(-1);
         break;
      }

      oss << c;
   }

   std::string token = oss.str();
   if(token.empty() || token[0] == '.' || token[token.size() - 1] == '.')
      readTokenError(token, "Invalid number");

   return buildToken(token, hasDot ? TokenType::real : TokenType::integer);
}

void Scanner::checkLitteral(const std::string& token)
{
   if(token[0] == '`')
      readTokenError(token, "formatted litterals are not supported yet");

   if(token[0] == '\'')
   {
      if(!validChar(token.substr(1, token.size() - 2)))
         readTokenError(token, "Invalid character");
   }

   size_t i = 0;
   std::string str = token.substr(1, token.size() - 2);

   while(i < str.size())
   {

      size_t j = i + 1;
      if(str[i] == '\\' && j < str.size())
      {
         char c = str[j++];
         if(c >= '0' && c <= '7')
            while(j < str.size() && j - i < 4
                  && str[j] >= '0' && str[j] <= '7')
               ++j;
      }

      std::string sub = str.substr(i, j - i);
      if(!validChar(sub))
         readTokenError(token, "Invalid char '" + sub + '"');

      i = j;
   }

}

bool Scanner::validChar(const std::string& str)
{
   if(str.empty())
      return false;

   if(str.length() == 1)
   {
      char c = str[0];
      return c >= 32 && c <= 126;
   }

   if(str[0] != '\\')
      return false;

   if(str.length() == 2)
   {
      char c = str[1];
      return c == 'a' || c == 'b' || c == 'f' || c == 'n' || c == 'r'
         || c == 't' || c == 'v' || c == '\\' || c == '\'' || c == '"'
         || c == '?' || (c >= '0' && c <= '7');
   }

   if(str.length() > 4)
      return false;

   for(size_t i = 1; i < str.size(); ++i)
   {
      if(str[i] < '0' || str[i] > '7')
         return false;
   }

   return true;
}

Token Scanner::buildToken(const std::string& token, TokenType type)
{
   return Token(token, type,
                static_cast<std::size_t>(get_pos()) - token.size());
}

void Scanner::readTokenError(const std::string& token,
                             const std::string& message)
{
   tokenError(buildToken(token, TokenType::unknown), message);
}

bool Scanner::is_end() const
{
   return _it == _end;
}
char Scanner::read_char()
{
   assert(_it != _end);
   return *(_it++);
}

char Scanner::peek_char() const
{
   assert(_it != _end);
   return *_it;
}

std::size_t Scanner::get_pos() const
{
   return _it - _begin;
}

void Scanner::set_pos(std::size_t pos)
{
   const char* it = _begin + pos;
   assert(it >= _begin && it <= _end);
   _it = it;
}

void Scanner::move_pos(std::ptrdiff_t diff)
{
   const char* it = _it + diff;
   assert(it >= _begin && it <= _end);
   _it = it;
}
