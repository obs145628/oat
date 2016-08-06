#include "err.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void handleError(const char *message)
{
   fprintf(stderr, "Fatal error occured on oat execution\n"
           "Message: %s\n"
           "Program aborted\n", message);
}

void err(const char* format, ...)
{
   va_list args;
   va_list args2;

   va_start(args, format);
   va_copy(args2, args);

   int len = vsnprintf(0, 0, format, args);
   char* message = malloc(len + 1);
   vsprintf(message, format, args2);

   va_end(args);
   va_end(args2);

   handleError(message);
   free(message);
   exit(1);
}

void errAssert(int b, const char* format, ...)
{
   if(b)
      return;

   va_list args;
   va_start(args, format);
   vfprintf(stderr, format, args);
   va_end(args);
   exit(1);
}
