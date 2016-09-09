#include "str.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>
#include "dstr.h"
#include "io.h"
#include "err.h"

char* strClone(const char* s)
{
   size_t n = strlen(s);
   char* clone = malloc(n + 1);
   memcpy(clone, s, n);
   clone[n] = '\0';
   return clone;
}

char* strCloneN(const char* s, size_t len)
{
   char* res = malloc(len + 1);
   memcpy(res, s, len);
   res[len] = '\0';
   return res;
}

char* strFormat(const char* format, ...)
{
   va_list args;
   va_start(args, format);

   dstr ds;
   dstr_init(&ds);
   ioVwritef(dstrWriter(&ds), format, args);

   va_end(args);
   return ds.s;
}

char* strPrintf(const char* format, ...)
{
   va_list args;
   va_list args2;
   va_start(args, format);
   va_copy(args2, args);

   int len = vsnprintf(0, 0, format, args);
   char* str = malloc(len + 1);
   vsprintf(str, format, args2);

   va_end(args);
   va_end(args2);
   return str;
}

char* strConcat(const char* s1, const char* s2)
{
   size_t len1 = strlen(s1);
   size_t len2 = strlen(s2);

   char* res = malloc(len1 + len2 + 1);
   memcpy(res, s1, len1);
   memcpy(res + len1, s2, len2);
   res[len1 + len2] = '\0';
   return res;
}

size_t strIndexOfChar(const char* s, char c)
{
   for(size_t i = 0; s[i] != '\0'; ++i)
      if(s[i] == c)
         return i;
   return STR_NFOUND;
}

size_t strLastIndexOfChar(const char* s, char c)
{
   size_t pos = STR_NFOUND;
   for(size_t i = 0; s[i] != '\0'; ++i)
      if(s[i] == c)
         pos = i;
   return pos;
}

char* strSubstring(const char* s, size_t pos, size_t size)
{
   size_t len = strlen(s);
   if(pos + size > len)
      err("substring: pos: %zu, size: %zu, on string of length %zu\n",
              pos, size, len);

   char* res = malloc(size + 1);
   memcpy(res, s + pos, size);
   res[size] = '\0';
   return res;
}

char* strPrefix(const char* s, size_t n)
{
   size_t len = strlen(s);
   if(n > len)
      err("Unable to extract the %zu first charachters of a string"
              " of size %zu\n", n, len);

   char* res = malloc(n + 1);
   memcpy(res, s, n);
   res[n] = '\0';
   return res;
}

char* strSuffix(const char* s, size_t n)
{
   size_t len = strlen(s);
   if(n > len)
      err("Unable to extract the %zu last charachters of a string"
              " of size %zu\n", n, len);

   char* res = malloc(n + 1);
   memcpy(res, s + len - n, n);
   res[n] = '\0';
   return res;
}

char* strSuffixFrom(const char* s, size_t pos)
{
   size_t len = strlen(s);
   if(pos > len)
      err("Unable to extract the last charachters of a string"
          " of size %zu starting at index %zu\n", len, pos);

   size_t n = len - pos;
   char* res = malloc(n + 1);
   memcpy(res, s + pos, n);
   res[n] = '\0';
   return res;
}

char* strToLower(const char* s)
{
   size_t n = strlen(s);
   char* res = malloc(n+1);

   for(size_t i = 0; i < n; ++i)
      res[i] = tolower(s[i]);

   res[n] = '\0';
   return res;
}

char* strToUpper(const char* s)
{
   size_t n = strlen(s);
   char* res = malloc(n+1);

   for(size_t i = 0; i < n; ++i)
      res[i] = toupper(s[i]);

   res[n] = '\0';
   return res;
}

size_t strCountChar(const char* s, char c)
{
   size_t n = 0;
   char it;
   while((it = *(s++)) != '\0')
      n += (it == c);
   return n;
}

SList* strSplitChar(const char* s, char sep)
{
   SList* l = SListNew();
   const char* begin = s;
   const char* it = s;

   do
   {
      if(*it == '\0' || *it == sep)
      {
         char* sub = strSubstring(s, begin - s, it - begin);
         SListInsertBack(l, sub);
         begin = it + 1;
      }
   }
   while(*(it++) != '\0');

   return l;
}

char* strTrim(const char* s)
{
   const char* begin = s;
   const char* end = s + strlen(s) - 1;

   while(*begin == ' ' || *begin == '\t' || *begin == '\n')
      ++begin;

   while(end > begin && (*end == ' ' || *end == '\t' || *end == '\n'))
      --end;

   return strSubstring(begin, 0, end + 1 - begin);
}

char* strFromInt(long x)
{
   dstr ds;
   dstr_init(&ds);
   ioWriteInt(dstrWriter(&ds), x);
   return ds.s;
}

char* strFromFloat(double x)
{
   dstr ds;
   dstr_init(&ds);
   ioWriteFloat(dstrWriter(&ds), x);
   return ds.s;
}

size_t strIndexOf(const char* str, const char* sub)
{
   size_t n = strlen(str);
   size_t m = strlen(sub);

   if(m > n)
      return STR_NFOUND;

   for(size_t i = 0; i <= n - m; ++i)
   {
      int equals = 1;
      for(size_t j = 0; j < m; ++j)
      {
         if(str[j] != sub[j])
         {
            equals = 0;
            break;
         }
      }
      if(equals)
         return i;
      ++str;
   }

   return STR_NFOUND;
}

size_t strLastIndexOf(const char* str, const char* sub)
{
   size_t n = strlen(str);
   size_t m = strlen(sub);

   if(m > n)
      return STR_NFOUND;

   str += n - m;

   for(size_t i = n - m; i < n; --i)
   {
      int equals = 1;
      for(size_t j = 0; j < m; ++j)
      {
         if(str[j] != sub[j])
         {
            equals = 0;
            break;
         }
      }
      if(equals)
         return i;
      --str;
   }

   return STR_NFOUND;
}

int strContains(const char* str, const char* sub)
{
   return strIndexOf(str, sub) != STR_NFOUND;
}

char* strReplace(const char* str, const char* sub, const char* rep)
{
   dstr ds;
   dstr_init(&ds);
   size_t n = strlen(str);
   size_t m = strlen(sub);
   size_t p = strlen(rep);

   if(m > n)
      return strClone(str);

   size_t i;
   for(i = 0; i <= n - m; ++i)
   {
      int equals = 1;
      for(size_t j = 0; j < m; ++j)
      {
         if(str[j] != sub[j])
         {
            equals = 0;
            break;
         }
      }

      if(equals)
      {
         dstr_append(&ds, rep, p);
      }
      else
      {
         dstr_append(&ds, str, 1);
      }
      ++str;
   }

   dstr_append(&ds, str, n - i);
   return ds.s;
}

int strCmpIgnoreCase(const char* s1, const char* s2)
{
   while(*s1 && (toupper(*s1) == toupper(*s2)))
      ++s1, ++s2;
   return toupper(*(const unsigned char*) s1)
      - toupper(*(const unsigned char*) s2);
}
