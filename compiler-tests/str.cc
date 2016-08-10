#include "str.hh"
#include "chars.hh"

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

}
