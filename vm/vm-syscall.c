#include "vm-syscall.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vm-stack.h"
#include "vm-exec.h"
#include "dvar.h"
#include "date.h"

typedef void (*f_syscall_)();

static void syscall_exit_()
{
   int code;
   dvar* v = vm_stack_sp();
   long time = dateNow() - vm_exec_get_start_time();

   if(v->type == DVAR_TINT)
      code = (int) v->v_int;
   else
      code = 1;

   printf("Program execution finished with code %d\n", code);
   printf("Duration: %ldms\n", time);
   exit(code);
}

static void syscall_kprint_()
{
   dvar* v = vm_stack_sp();
   dvar_print(v);
}

static void syscall_print_()
{
   char* str = dvar_to_string(vm_stack_sp());
   fwrite(str, 1, strlen(str), stdout);
   free(str);
}


void vm_syscall_exec(t_vm_int syscall)
{
   static const f_syscall_ FNS[] = {
      syscall_exit_,
      syscall_kprint_,
      syscall_print_
   };

   assert(syscall >= 0 && syscall < VM_NB_SYSCALL);

   FNS[syscall]();
}
