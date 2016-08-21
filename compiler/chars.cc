#include "chars.hh"

namespace chars
{


   bool isBlanck(char c)
   {
      return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v';
   }

   bool isVisible(char c)
   {
      return c >= 33 && c <= 126;
   }

   bool isValid(char c)
   {
      return c == ' ' || c == '\t' || c == '\n' || isVisible(c);
   }

   bool isDigit(char c)
   {
      return c >= '0' && c <= '9';
   }

   bool isLower(char c)
   {
      return c >= 'a' && c <= 'z';
   }

   bool isUpper(char c)
   {
      return c >= 'A' && c <= 'Z';
   }

   bool isLetter(char c)
   {
      return isLower(c) || isUpper(c);
   }

   bool isSymbol(char c)
   {
      return isLetter(c) || isDigit(c) || c == '_';
   }

   bool isToken(char c)
   {
      return c == '(' || c == ')' || c == '[' || c == ']'
         || c == '{' || c == '}' || c == '!' || c == '~' || c == ',' || c == ';'
         || c == '?';
   }

}
