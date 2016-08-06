#include "path.h"
#include <string.h>
#include <stdlib.h>
#include "str.h"

static char* pathClone(const char* s, size_t* olen)
{
   size_t n = strlen(s);
   if(n && s[n-1] == '/')
      --n;

   char* res = malloc(n+1);
   memcpy(res, s, n);
   res[n] = '\0';

   if(olen)
      *olen = n;
   return res;
}

char* pathBasename(const char* s)
{
   size_t len;
   char* res = pathClone(s, &len);

   size_t pos = len;
   for(size_t i = len - 1; i < len; --i)
   {
      if(res[i] == '/')
      {
         pos = i;
         break;
      }
   }

   if(pos != len)
   {
      memmove(res, res + pos + 1, len - pos);
      res[len - pos] = '\0';
   }
   return res;
}

char* pathDirname(const char* s)
{
   if(*s == '\0')
      return strClone(".");
   if(!strcmp(s, "/"))
      return strClone("/");

   size_t len;
   char* res = pathClone(s, &len);

   size_t pos = len;
   for(size_t i = len - 1; i < len; --i)
   {
      if(res[i] == '/')
      {
         pos = i;
         break;
      }
   }

   if(pos == len)
   {
      res[0] = '.';
      res[1] = '\0';
   }

   else if(!pos)
   {
      res[1] = '\0';
   }

   else
   {
      res[pos] = '\0';
   }
   return res;
}

char* pathExtname(const char* s)
{
   size_t len;
   char* res = pathClone(s, &len);

   size_t begin = 0;
   for(size_t i = len - 1; i < len; --i)
   {
      if(res[i] == '/')
      {
         begin = i + 1;
         break;
      }
   }

   size_t pos = len;
   for(size_t i = len - 1; i >= begin && i < len; --i)
   {
      if(res[i] == '.')
      {
         pos = i;
         break;
      }
   }

   if(pos == len || pos == begin)
   {
      res[0] = '\0';
   }
   else
   {
      memmove(res, res + pos, len - pos);
      res[len - pos] = '\0';
   }

   return res;
}

int pathIsAbsolute(const char* s)
{
   return s[0] == '/';
}

static void pathAdd(char* dst, size_t *len, const char* begin, const char* end)
{
   size_t subLen = end - begin;

   if(subLen == 2 && begin[0] == '.' && begin[1] == '.') //..
   {
      if(*len == 1 && *dst == '/')
      {
         //nothing to do
      }
      else if(*len == 1 && *dst == '.')
      {
         dst[1] = '.';
         *len = 2;
      }
      else if(*len >= 2 && dst[*len - 1] == '.' && dst[*len - 2] == '.')
      {
         dst[*len] = '/';
         dst[*len + 1] = '.';
         dst[*len + 2] = '.';
         *len += 3;
      }
      else
      {
         while(*len && dst[*len - 1] != '/')
            --(*len);

         if(!*len)
         {
            *len = 1;
            *dst = '.';
         }
         else if(*len != 1 || *dst != '/')
         {
            --(*len);
         }
      }

   }

   else if(subLen == 1 && begin[0] == '.') //.
   {
      //nothing to do ?
   }

   else
   {
      if(*len == 1 && *dst == '.')
      {
         *len = 0;
      }
      else if(*len >= 2 && dst[*len - 1] == '.' && dst[*len - 2] == '.')
      {
         *len -= 2;
      }
      else if(dst[*len - 1] != '/')
      {
         dst[*len] = '/';
         ++(*len);
      }

      memcpy(dst + *len, begin, subLen);
      *len += subLen;
   }
}

char* pathNormalize(const char* s)
{
   if(*s == '\0')
      return strClone(".");

   size_t len = strlen(s);
   size_t rlen = 1;
   char* res = malloc(len + 3);
   if(*s == '/')
      *res = '/';
   else
      *res = '.';

   const char* i1 = s;
   const char* i2 = s;

   do
   {
      if(*i2 == '\0' || *i2 == '/')
      {
         if(i1 != i2)
         {
            pathAdd(res, &rlen, i1, i2);
         }
         i1 = i2 + 1;
      }
   }
   while(*(i2++) != '\0');

   if(len > 1 && s[len-1] == '/')
   {
      res[rlen++] = '/';
   }

   res[rlen] = '\0';
   return res;
}

char* pathJoin(const char* s1, const char* s2)
{
   size_t len1 = strlen(s1);
   size_t len2 = strlen(s2);
   char* concated = malloc(len1 + len2 + 2);
   memcpy(concated, s1, len1);
   concated[len1] = '/';
   memcpy(concated + len1 + 1, s2, len2);
   concated[len1 + len2 + 1] = '\0';
   char* res = pathNormalize(concated);
   free(concated);
   return res;
}
