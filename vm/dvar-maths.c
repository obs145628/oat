#include "dvar-maths.h"
#include <math.h>
#include "dvar.h"
#include "err.h"


struct dvar* syscall_min(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("min: arguments expected");

   dvar* res = l;
   t_vm_double val = dvar_get_double(l, "min: arguments must be numbers");

   for(t_vm_int i = 1; i < n; ++i)
   {
      dvar* x = l + i;
      t_vm_double xval = dvar_get_double(x, "min: arguments must be numbers");
      if(xval < val)
      {
         res = x;
         val = xval;
      }
   }


   dvar_move(l, res);
   return l;
}

struct dvar* syscall_max(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("max: arguments expected");

   dvar* res = l;
   t_vm_double val = dvar_get_double(l, "max: arguments must be numbers");

   for(t_vm_int i = 1; i < n; ++i)
   {
      dvar* x = l + i;
      t_vm_double xval = dvar_get_double(x, "max: arguments must be numbers");
      if(xval > val)
      {
         res = x;
         val = xval;
      }
   }


   dvar_move(l, res);
   return l;
}

struct dvar* syscall_abs(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("abs: argument expected");

   if(l->type == DVAR_TINT)
   {
      if(l->v_int < 0)
         l->v_int = -l->v_int;
   }

   else if(l->type == DVAR_TDOUBLE)
   {
      if(l->v_double < 0)
         l->v_double = - l->v_double;
   }

   else if(l->type == DVAR_TCHAR)
   {
      if(l->v_char < 0)
         l->v_char = - l->v_char;
   }

   else
      err("abs: argument must be a number");

   return l;
}

struct dvar* syscall_round(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("round: argument expected");
   t_vm_double x = dvar_get_double(l, "round: first argument must be a number");

   t_vm_double res = round(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_floor(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("floor: argument expected");
   t_vm_double x = dvar_get_double(l, "floor: first argument must be a number");

   t_vm_double res = floor(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_ceil(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("ceil: argument expected");
   t_vm_double x = dvar_get_double(l, "ceil: first argument must be a number");

   t_vm_double res = ceil(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_fmod(struct dvar* l, t_vm_int n)
{
   if(n < 2)
      err("fmod: 2 arguments expected");
   t_vm_double a = dvar_get_double(l, "fmod: first argument must be a number");
   t_vm_double b = dvar_get_double(l+1,
                                   "fmod: second argument must be a number");

   t_vm_double res = fmod(a, b);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}
struct dvar* syscall_exp(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("exp: argument expected");
   t_vm_double x = dvar_get_double(l, "exp: first argument must be a number");

   t_vm_double res = exp(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_log(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("log: argument expected");
   t_vm_double x = dvar_get_double(l, "log: first argument must be a number");

   t_vm_double res = log(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_pow(struct dvar* l, t_vm_int n)
{
   if(n < 2)
      err("pow: 2 arguments expected");
   t_vm_double a = dvar_get_double(l, "pow first argument must be a number");
   t_vm_double b = dvar_get_double(l+1,
                                   "pow: second argument must be a number");

   t_vm_double res = pow(a, b);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_sqrt(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("sqrt: argument expected");
   t_vm_double x = dvar_get_double(l, "sqrt: first argument must be a number");

   t_vm_double res = sqrt(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_sin(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("sin: argument expected");
   t_vm_double x = dvar_get_double(l, "sin: first argument must be a number");

   t_vm_double res = sin(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_cos(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("cos: argument expected");
   t_vm_double x = dvar_get_double(l, "cos: first argument must be a number");

   t_vm_double res = cos(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_tan(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("tan: argument expected");
   t_vm_double x = dvar_get_double(l, "tan: first argument must be a number");

   t_vm_double res = tan(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_asin(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("asin: argument expected");
   t_vm_double x = dvar_get_double(l, "asin: first argument must be a number");

   t_vm_double res = asin(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_acos(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("acos: argument expected");
   t_vm_double x = dvar_get_double(l, "acos: first argument must be a number");

   t_vm_double res = acos(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_atan(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("atan: argument expected");
   t_vm_double x = dvar_get_double(l, "atan: first argument must be a number");

   t_vm_double res = atan(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_sinh(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("sinh: argument expected");
   t_vm_double x = dvar_get_double(l, "sinh: first argument must be a number");

   t_vm_double res = sinh(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_cosh(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("cosh: argument expected");
   t_vm_double x = dvar_get_double(l, "cosh: first argument must be a number");

   t_vm_double res = cosh(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_tanh(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("tanh: argument expected");
   t_vm_double x = dvar_get_double(l, "tanh: first argument must be a number");

   t_vm_double res = tanh(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_asinh(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("asinh: argument expected");
   t_vm_double x = dvar_get_double(l, "asinh: first argument must be a number");

   t_vm_double res = asinh(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_acosh(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("acosh: argument expected");
   t_vm_double x = dvar_get_double(l, "acosh: first argument must be a number");

   t_vm_double res = acosh(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* syscall_atanh(struct dvar* l, t_vm_int n)
{
   if(n < 1)
      err("atanh: argument expected");
   t_vm_double x = dvar_get_double(l, "atanh: first argument must be a number");

   t_vm_double res = atanh(x);

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}
