#include "prompt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dstr.h"
#include "str.h"
#include "numbers.h"

char* promptLine(const char* question, const char* dft)
{
   if(question)
      printf("%s", question);
   if(dft)
      printf("[%s]", dft);

   int c;
   dstr ds;

   dstr_init(&ds);
   while((c = getchar()) != EOF)
   {
      if(c == '\n')
         break;
      char ic = (char) c;
      dstr_append(&ds, &ic, 1);
   }

   if(!ds.len && dft)
   {
      free(ds.s);
      return strClone(dft);
   }

   return ds.s;
}

long promptInt(const char* question, long* dft, int* ok)
{
   char* dftStr = NULL;
   if(dft)
      dftStr = longToStr(*dft);

   char* line = promptLine(question, dftStr);
   if(dftStr)
      free(dftStr);
   long res = strToLong(line, ok);
   free(line);
   return res;
}

double promptDouble(const char* question, double* dft, int* ok)
{
   char* dftStr = NULL;
   if(dft)
      dftStr = doubleToStr(*dft);

   char* line = promptLine(question, dftStr);
   if(dftStr)
      free(dftStr);
   double res = strToDouble(line, ok);
   free(line);
   return res;
}

char promptChar(const char* question, char* dft, int* ok)
{
   static char buffer[2];
   char* dftStr = NULL;
   if(dft)
   {
      buffer[0] = *dft;
      buffer[1] = '\0';
      dftStr = buffer;
   }

   char* line = promptLine(question, dftStr);
   int valid;
   char c = '\0';
   if(strlen(line) == 1)
   {
      c = line[0];
      valid = 1;
   }
   else
      valid = 0;

   free(line);
   if(ok)
      *ok = valid;
   return c;
}

int promptBool(const char* question, int* dft, int* ok)
{
   int temp;
   if(!ok)
      ok = &temp;

   char d;
   char* dp = NULL;
   if(dft)
   {
      d = (*dft ? 'y' : 'n');
      dp = &d;
   }


   char c = promptChar(question, dp, ok);
   int res = 0;

   if(*ok)
   {
      if(c == 'y' || c == 'Y')
         res = 1;
      else if(c == 'n' || c == 'N')
         res = 0;
      else
         *ok = 0;
   }

   return res;
}
