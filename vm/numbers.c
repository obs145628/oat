#include "numbers.h"
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include <stdio.h>

#define NUM_BUFF_LEN 1024
#define FLOAT_PRECISION 1e-10

long strToUInt(const char* str, int* ok)
{
   int valid = 0;
   long value = 0;
   char c;

   if(*str != '\0')
   {
      valid = 1;
      while((c = *(str++)) != 0)
      {
         if(!isdigit(c))
         {
            valid = 0;
            break;
         }
         value = 10 * value+ (c - '0');
      }
   }

   if(ok)
      *ok = valid;

   return valid ? value : 0;
}

long strToInt(const char* str, int* ok)
{
   int valid;
   long value;

   if(*str == '-')
      value = - strToUInt(str + 1, &valid);
   else
      value = strToUInt(str, &valid);

   if(ok)
      *ok = valid;
   return value;
}

char* intToStr(long x)
{
   static char buffer[NUM_BUFF_LEN];
   int bufferSize = 0;

   int neg = x < 0;
   x = neg ? -x : x;

   do
   {
      char digit = (char) (x % 10);
      buffer[bufferSize++] = digit + '0';
      x /= 10;
   }   while(x);

   char* str = malloc(bufferSize + neg + 1);
   char* res = str;
   if(neg)
      *(str++) = '-';

   for(int i = bufferSize - 1; i >= 0; --i)
      *(str++) = buffer[i];

   *str = '\0';
   return res;
}

double strToUFloat(const char* str, int* ok)
{
   int valid = 0;
   double intPart = 0;
   double decPart = 0;
   double divisor = 1;
   int inDec = 0;
   char c;

   if(*str != '\0')
   {
      valid = 1;
      size_t i = 0;
      while((c = str[i++]) != '\0')
      {
         if(!isdigit(c) && (c != '.' || str[i] == '\0' ||  i == 1 || inDec))
         {
            valid = 0;
            break;
         }
         else if(c == '.')
         {
            inDec = 1;
         }
         else if(inDec)
         {
            decPart = 10 * decPart + (c - '0');
            divisor *= 10;
         }
         else
         {
            intPart = 10 * intPart + (c - '0');
         }
      }
   }

   if(ok)
      *ok = valid;

   return valid ? intPart + (decPart / divisor) : 0;
}

double strToFloat(const char* str, int* ok)
{
   int valid;
   double value;

   if(*str == '-')
      value = - strToUFloat(str + 1, &valid);
   else
      value = strToUFloat(str, &valid);

   if(ok)
      *ok = valid;
   return value;
}

char* floatToStr(double x)
{
   static char buffer[NUM_BUFF_LEN];
   int bufferSize = 0;

   int neg = x < 0;
   x = neg ? -x : x;

   double intPart = floor(x);
   double decPart = x - intPart;

   do
   {
      char digit = (char) fmod(intPart, 10.0);
      buffer[bufferSize++] = digit + '0';
      intPart = floor(intPart / 10.0);
   }   while(intPart > FLOAT_PRECISION);
   buffer[bufferSize++] = '.';
   int dotPos = bufferSize - 1;

   while(decPart > FLOAT_PRECISION)
   {
      decPart *= 10.0;
      double fDigit = floor(decPart);
      char digit = (char) fDigit;
      decPart -= fDigit;
      buffer[bufferSize++] = digit + '0';
   }


   char* str = malloc(bufferSize + neg + 1);
   char* res = str;
   if(neg)
      *(str++) = '-';

   for(int i = dotPos - 1; i >= 0; --i)
      *(str++) = buffer[i];

   if(dotPos + 1 != bufferSize)
   {
      *(str++) = '.';
      for(int i = dotPos + 1; i < bufferSize; ++i)
         *(str++) = buffer[i];
   }

   *str = '\0';
   return res;
}
