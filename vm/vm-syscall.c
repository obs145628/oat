#include "vm-syscall.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vm-stack.h"
#include "vm-exec.h"
#include "dvar.h"
#include "dvar-map.h"
#include "date.h"
#include "vm-classes.h"
#include "err.h"
#include "dvar-str.h"
#include "dvar-fun.h"
#include "dvar-arr.h"
#include "dvar-set.h"
#include "dvar-map.h"
#include "dvar-obj.h"
#include "dvar-class.h"
#include "dvar-date.h"
#include "dvar.h"
#include "dvar-maths.h"
#include "dvar-random-generator.h"
#include "dvar-io.h"

typedef dvar* (*f_syscall_)(dvar* argv, t_vm_int argc);

static dvar* syscall_exit_(dvar* v, t_vm_int argc)
{
   (void) argc;
   int code;

   if(v->type == DVAR_TINT)
      code = (int) v->v_int;
   else
      code = 1;

   exit(code);
   return NULL;;
}

static dvar* syscall_kprint_(dvar* v, t_vm_int argc)
{
   (void) argc;
   dvar_print(v);
   return v;
}

static dvar* syscall_print_(dvar* argv, t_vm_int argc)
{
   for(t_vm_int i = 0; i < argc; ++i)
   {
      char* str = dvar_to_str(argv + i);
      fwrite(str, 1, strlen(str), stdout);
      free(str);
   }
   return argv;
}

static dvar* syscall_to_string_(dvar* v, t_vm_int argc)
{
   (void) argc;
   char* str = dvar_to_str(v);
   dvar_putstring(v, 1, str, strlen(str));
   return v;
}

static dvar* syscall_default_constructor_(dvar* v, t_vm_int argc)
{
   (void) argc;
   return v;
}

static dvar* syscall_map_sub_list_(dvar* v, t_vm_int argc)
{
   (void) argc;

   dvar_map_insert(v->v_map, v + 1, v + 3);
   return v;
}

static dvar* syscall_default_destructor_(dvar* v, t_vm_int argc)
{
   (void) argc;
   return v;
}

static dvar* syscall_typename_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("typename(): argument expected");

   const char* t = dvar_typename(l);
   dvar_putstring(l, DVAR_MVAR, t, strlen(t));
   return l;
}

static dvar* syscall_is_null_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isNull(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TNULL);
   return l;
}

static dvar* syscall_is_int_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isInt(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TINT);
   return l;
}

static dvar* syscall_is_double_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isDouble(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TDOUBLE);
   return l;
}

static dvar* syscall_is_char_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isChar(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TCHAR);
   return l;
}

static dvar* syscall_is_bool_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isBool(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TBOOL);
   return l;
}

static dvar* syscall_is_string_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isString(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TSTR);
   return l;
}

static dvar* syscall_is_function_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isFunction(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TFUN);
   return l;
}

static dvar* syscall_is_array_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isArray(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TARR);
   return l;
}

static dvar* syscall_is_set_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isSet(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TSET);
   return l;
}

static dvar* syscall_is_map_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isMap(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TMAP);
   return l;
}

static dvar* syscall_is_object_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isObject(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TOBJ);
   return l;
}

static dvar* syscall_is_class_(dvar* l, t_vm_int n)
{
   if(n < 1)
      err("isClass(): argument expected");

   dvar_putbool(l, DVAR_MVAR, l->type == DVAR_TCLASS);
   return l;
}

static dvar* syscall_is_instance_of_(dvar* l, t_vm_int n)
{
   if(n < 2)
      err("isInstanceOf(): too few arguments");

   if(l->type != DVAR_TCLASS)
      err("isInstanceof(): first argument must be a class");

   dvar* o = l + 1;

   dvar_putbool(l, DVAR_MVAR, o->type == DVAR_TOBJ
                && o->v_obj->val->c == l->v_class);
   return l;
}


dvar* vm_syscall_exec(t_vm_int syscall, dvar* argv, t_vm_int argc)
{
   static const f_syscall_ FNS[] = {
      syscall_exit_,
      syscall_kprint_,
      syscall_print_,
      syscall_to_string_,
      syscall_default_constructor_,

      c__rect__constructor,
      c__rect__get_x,
      c__rect__get_y,
      c__rect__get_width,
      c__rect__get_height,

      syscall_map_sub_list_,

      c__rect__set_x,
      c__rect__set_y,
      c__rect__set_width,
      c__rect__set_height,

      syscall_default_destructor_,

      c__date__constructor,
      c__date__destructor,
      c__date__get_time,
      c__date__get_year,
      c__date__get_month,
      c__date__get_day,
      c__date__get_hours,
      c__date__get_minutes,
      c__date__get_seconds,
      c__date__get_milliseconds,

      c__char__is_printable,
      c__char__is_digit,
      c__char__is_lower,
      c__char__is_upper,
      c__str__empty,
      c__str__size,
      c__str__substr,
      c__str__is_int,
      c__str__to_int,
      c__str__is_double,
      c__str__to_double,
      c__fun__bind,
      c__fun__binda,

      c__arr__empty,
      c__arr__size,
      c__arr__push_front,
      c__arr__push_back,
      c__arr__insert,
      c__arr__pop_front,
      c__arr__pop_back,
      c__arr__remove,
      c__arr__to_set,
      c__set__empty,
      c__set__size,
      c__set__contains,
      c__set__insert,
      c__set__remove,
      c__set__to_array,
      c__map__empty,
      c__map__size,
      c__map__contains,
      c__map__remove,
      c__map__keys,
      c__map__values,
      c__date__now,

      syscall_typename_,
      syscall_is_null_,
      syscall_is_int_,
      syscall_is_double_,
      syscall_is_char_,
      syscall_is_bool_,
      syscall_is_string_,
      syscall_is_function_,
      syscall_is_array_,
      syscall_is_set_,
      syscall_is_map_,
      syscall_is_object_,
      syscall_is_class_,
      syscall_is_instance_of_,

      c__str__index_of,
      c__str__last_index_of,
      c__str__contains,
      c__str__replace,
      c__arr__index_of,
      c__arr__last_index_of,
      c__arr__contains,

      c__array_iterator__constructor,
      c__array_iterator__destructor,
      c__array_iterator__is_end,
      c__array_iterator__next,
      c__array_iterator__get_key,
      c__array_iterator__get_value,
      c__arr__it,

      c__set_iterator__constructor,
      c__set_iterator__destructor,
      c__set_iterator__is_end,
      c__set_iterator__next,
      c__set_iterator__get_key,
      c__set_iterator__get_value,
      c__set__it,

      c__map_iterator__constructor,
      c__map_iterator__destructor,
      c__map_iterator__is_end,
      c__map_iterator__next,
      c__map_iterator__get_key,
      c__map_iterator__get_value,
      c__map__it,

      syscall_min,
      syscall_max,
      syscall_abs,
      syscall_round,
      syscall_floor,
      syscall_ceil,
      syscall_fmod,
      syscall_exp,
      syscall_log,
      syscall_pow,
      syscall_sqrt,
      syscall_sin,
      syscall_cos,
      syscall_tan,
      syscall_asin,
      syscall_acos,
      syscall_atan,
      syscall_sinh,
      syscall_cosh,
      syscall_tanh,
      syscall_asinh,
      syscall_acosh,
      syscall_atanh,

      c__random_generator__constructor,
      c__random_generator__destructor,
      c__random_generator__get_bool,
      c__random_generator__get_int,
      c__random_generator__get_double,
      c__random_generator__get_char,

      syscall_prompt_line,
      syscall_prompt_int,
      syscall_prompt_double,
      syscall_prompt_char,
      syscall_prompt_bool,
   };

   assert(syscall >= 0 && syscall < VM_NB_SYSCALL);

   return FNS[syscall](argv, argc);
}

void vm_syscall_exec_fn(dvar_fun* fn, const dvar* begin, const dvar* end,
                        dvar* res)
{
   assert(fn->syscall != VM_SYSCALL_NO);
   dvar_fun* f2 = dvar_fun_copy(fn);
   dvar_fun_bind(f2, begin, end);

   dvar* v = vm_syscall_exec(f2->syscall, f2->args, f2->size);
   dvar_move(res, v);
   dvar_fun_free(f2);
}
