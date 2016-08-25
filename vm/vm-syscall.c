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
#include "dvar-date.h"

typedef dvar* (*f_syscall_)(dvar* argv, t_vm_int argc);

static dvar* syscall_exit_(dvar* v, t_vm_int argc)
{
   (void) argc;
   int code;
   long time = dateNow() - vm_exec_get_start_time();

   if(v->type == DVAR_TINT)
      code = (int) v->v_int;
   else
      code = 1;

   printf("Program execution finished with code %d\n", code);
   printf("Duration: %ldms\n", time);
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
