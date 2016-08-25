#include "args.h"
#include <string.h>

int argsHasOption(const SList* l, char c, const char* s)
{
   for(char** it = l->begin; it != l->end; ++it)
   {
      char* arg = *it;
      if(arg[0] != '-')
         continue;
      if(arg[1] == '-')
      {
         if(s && strcmp(arg + 2, s) == 0)
            return 1;
         continue;
      }

      ++arg;
      while(*arg != '\0')
      {
         if(*arg == c)
            return 1;
         ++arg;
      }
   }

   return 0;
}


SList* argsGetParameters(const SList* l)
{
   SList* p = SListNew();

   for(char** it = l->begin + 1; it < l->end; ++it)
   {
      char* arg = *it;
      if(*arg != '\0' && *arg != '-')
         SListInsertBack(p, arg);
   }

   return p;
}
