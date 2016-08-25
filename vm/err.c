#include "err.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* scope_name_ = NULL;
static size_t scope_name_len_ = 0;
static size_t line_number_ = 0;

void err_set_scope_name(const char* scope_name, size_t len)
{
   scope_name_ = scope_name;
   scope_name_len_ = len;
}

void err_set_line_(size_t line)
{
   line_number_ = line;
}

static void handleError(const char *message)
{
   fprintf(stderr, "A fatal error occured during execution\n");

   if(scope_name_)
   {
      char* scope_name = malloc(scope_name_len_ + 1);
      memcpy(scope_name, scope_name_, scope_name_len_);
      scope_name[scope_name_len_] = '\0';

      fprintf(stderr, "At %s", scope_name);
      if(line_number_)
         fprintf(stderr, "::%zu", line_number_);
      fprintf(stderr, "\n");
   }

   fprintf(stderr, "Error: %s\n", message);
   fprintf(stderr, "Program aborted\n");
}

static void errv(const char* format, va_list args)
{
   va_list args2;
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

void err(const char* format, ...)
{
   va_list args;
   va_start(args, format);
   errv(format, args);
   va_end(args);

   /*
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
   */
}

void errAssert(int b, const char* format, ...)
{
   if(b)
      return;

   va_list args;
   va_start(args, format);
   errv(format, args);
   va_end(args);
}
