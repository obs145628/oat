#include "prompt.h"
#include <stdio.h>
#include <stdlib.h>
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

long promptInt(const char* question, int* ok)
{
   char* line = promptLine(question, NULL);
   long res = strToInt(line, ok);
   free(line);
   return res;
}
