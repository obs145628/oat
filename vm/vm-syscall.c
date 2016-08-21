#include "vm-syscall.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vm-stack.h"
#include "vm-exec.h"
#include "dvar.h"
#include "date.h"

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


dvar* vm_syscall_exec(t_vm_int syscall, dvar* argv, t_vm_int argc)
{
   static const f_syscall_ FNS[] = {
      syscall_exit_,
      syscall_kprint_,
      syscall_print_,
      syscall_to_string_
   };

   assert(syscall >= 0 && syscall < VM_NB_SYSCALL);

   return FNS[syscall](argv, argc);
}
