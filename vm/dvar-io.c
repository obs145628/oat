#include "dvar-io.h"
#include "dvar.h"
#include <stdlib.h>
#include <string.h>
#include "prompt.h"

struct dvar* syscall_prompt_line(struct dvar* l, t_vm_int n)
{
   char* question = NULL;
   char* dft = NULL;

   if(n > 0)
      question = dvar_get_str(l, "promptLine: argument 1 must be a string");
   if(n > 1)
      dft = dvar_get_str(l+1, "promptLine: argument 2 must be a string");

   char* res = promptLine(question, dft);

   if(question)
      free(question);
   if(dft)
      free(dft);

   dvar_putstring(l, DVAR_MVAR, res, strlen(res));
   free(res);
   return l;
}

struct dvar* syscall_prompt_int(struct dvar* l, t_vm_int n)
{
   int ok;
   long temp;
   char* question = NULL;
   long* dft = NULL;

   if(n > 0)
      question = dvar_get_str(l, "promptInt: argument 1 must be a string");
   if(n > 1)
   {
      temp = dvar_get_int(l+1, "promptInt: argument 2 must be an int");
      dft = &temp;
   }

   long res = promptInt(question, dft, &ok);
   if(question)
      free(question);

   if(ok)
      dvar_putint(l, DVAR_MVAR, (t_vm_int) res);
   else
      dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* syscall_prompt_double(struct dvar* l, t_vm_int n)
{
   int ok;
   double temp;
   char* question = NULL;
   double* dft = NULL;

   if(n > 0)
      question = dvar_get_str(l, "promptDouble: argument 1 must be a string");
   if(n > 1)
   {
      temp = dvar_get_double(l+1, "promptDouble: argument 2 must be a double");
      dft = &temp;
   }

   double res = promptDouble(question, dft, &ok);
   if(question)
      free(question);

   if(ok)
      dvar_putdouble(l, DVAR_MVAR, (t_vm_double) res);
   else
      dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* syscall_prompt_char(struct dvar* l, t_vm_int n)
{
   int ok;
   char temp;
   char* question = NULL;
   char* dft = NULL;

   if(n > 0)
      question = dvar_get_str(l, "promptChar: argument 1 must be a string");
   if(n > 1)
   {
      temp = dvar_get_char(l+1, "promptChar: argument 2 must be a double");
      dft = &temp;
   }

   char res = promptChar(question, dft, &ok);
   if(question)
      free(question);

   if(ok)
      dvar_putchar(l, DVAR_MVAR, (t_vm_char) res);
   else
      dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* syscall_prompt_bool(struct dvar* l, t_vm_int n)
{
   int ok;
   int temp;
   char* question = NULL;
   int* dft = NULL;

   if(n > 0)
      question = dvar_get_str(l, "promptBool: argument 1 must be a string");
   if(n > 1)
   {
      temp = dvar_get_double(l+1, "promptBool: argument 2 must be a boolean");
      dft = &temp;
   }

   int res = promptBool(question, dft, &ok);
   if(question)
      free(question);

   if(ok)
      dvar_putbool(l, DVAR_MVAR, (t_vm_bool) res);
   else
      dvar_putnull(l, DVAR_MVAR);
   return l;
}
