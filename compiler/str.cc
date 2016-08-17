#include "str.hh"
#include "chars.hh"
#include <stdexcept>
#include <sstream>

#include <iostream>

namespace str
{

   bool startsWith(const std::string& s, const std::string& sub)
   {
      return s.find(sub) == 0;
   }

   bool isSymbol(const std::string& str)
   {
      if(str.empty() || !chars::isSymbol(str[0]) || chars::isDigit(str[0]))
         return false;

      for(size_t i = 1; i < str.size(); ++i)
         if(!chars::isSymbol(str[i]))
            return false;
      return true;
   }

   long toInt(const std::string& str, bool* ok)
   {
      bool valid = false;
      long n = 0;

      if(!str.empty())
      {
         valid = true;
         for(size_t i = 0; i < str.size(); ++i)
         {
            char c = str[i];
            if(!chars::isDigit(c))
            {
               valid = false;
               break;
            }
            n = 10 * n + static_cast<long>(c - '0');
         }
      }

      if(ok)
         *ok = valid;
      return valid ? n : 0;
   }

   double toFloat(const std::string& str, bool* ok)
   {
      bool valid = false;
      double intPart = 0;
      double pow = 1;
      bool dotFound = false;

      if(!str.empty())
      {
         valid = true;
         for(size_t i = 0; i < str.size(); ++i)
         {
            char c = str[i];
            if(c == '.')
            {
               if(!i || i + 1 == str.size() || dotFound)
               {
                  valid = false;
                  break;
               }
               dotFound = true;
            }

            else if(!chars::isDigit(c))
            {
               valid = false;
               break;
            }

            else
            {
               intPart = 10 * intPart + static_cast<long>(c - '0');
               if(dotFound)
                  pow *= 10;
            }
         }
      }

      if(ok)
         *ok = valid;
      return valid ? intPart / pow : 0;
   }

   char litteralToChar(const std::string& str)
   {
      if(str.empty())
         throw std::runtime_error{"empty litteral"};

      if(str.length() == 1)
      {
         char c = str[0];
         if(c < 32 || c > 126)
            throw std::runtime_error{"not printable litteral char"};
         return c;
      }

      if(str[0] != '\\')
         throw std::runtime_error{"Multicharachters litteral"};

      if(str.length() == 2)
      {
         char c = str[1];
         if(c >= '0' && c <= '7')
            return c - '0';
         if(c == 'a')
            return '\a';
         else if(c == 'b')
            return '\b';
         else if(c == 'f')
            return '\f';
         else if(c== 'n')
            return '\n';
         else if(c == 'r')
            return '\r';
         else if(c == 't')
            return '\t';
         else if(c == 'v')
            return '\v';
         else if(c == '\\')
            return '\\';
         else if(c == '\'')
            return '\'';
         else if(c == '"')
            return '"';
         else if(c == '?')
            return '?';
         else
            throw std::runtime_error{"Unknwn escape sequence"};
      }

      if(str.length() > 4)
         throw std::runtime_error{"Multicharacthers litteral"};

      int c = 0;
      for(size_t i = 1; i < str.size(); ++i)
      {
         if(str[i] < '0' || str[i] > '7')
            throw std::runtime_error{"Multicharachters litteral"};
         c = 8 * c + (str[i] - '0');
      }

      return static_cast<char> (c);
   }

   std::string litteralToStr(const std::string& str)
   {
      std::ostringstream oss;
      size_t i = 0;

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

         oss << litteralToChar(sub);
         i = j;
      }

      return oss.str();
   }
}
