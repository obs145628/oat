#include "numbers.h"
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "str.h"

#include <stdio.h>

#define NUM_BUFF_LEN 1024
#define FLOAT_PRECISION 1e-10

char* longToStr(long x)
{
   return strPrintf("%ld", x);
}

char* doubleToStr(double x)
{
   return strPrintf("%G", x);
}


long strToLong(const char* str, int* ok)
{
   char* end;
   long res = strtol(str, &end, 10);

   if(ok)
      *ok = end != str && *end == '\0';
   return res;
}

double strToDouble(const char* str, int* ok)
{
   char* end;
   double res = strtod(str, &end);

   if(ok)
      *ok = end != str && *end == '\0';
   return res;
}
