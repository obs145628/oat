#include "dvar-fun.h"
#include <stdlib.h>
#include <assert.h>
#include "dvar.h"
#include "dvar-arr.h"
#include "str.h"
#include "hash.h"
#include "err.h"

# define ALLOC_SIZE_ 8

static dvar_fun* fun_new_()
{
   dvar_fun* f = malloc(sizeof(dvar_fun));
   f->alloc = ALLOC_SIZE_;
   f->size = 0;
   f->args = malloc(ALLOC_SIZE_ * sizeof(dvar));
   return f;
}

dvar_fun* dvar_fun_new_syscall(t_vm_int syscall)
{
   assert(syscall > 0 && syscall < VM_NB_SYSCALL);
   dvar_fun* f = fun_new_();
   f->syscall = syscall;
   return f;
}

dvar_fun* dvar_fun_new_function(t_vm_addr addr)
{
   dvar_fun* f = fun_new_();
   f->syscall = VM_SYSCALL_NO;
   f->addr = addr;
   return f;
}

void dvar_fun_free(dvar_fun* f)
{
   dvar_bclear(f->args, f->args + f->size);
   free(f->args);
   free(f);
}

dvar_fun* dvar_fun_copy(const dvar_fun* f)
{
   dvar_fun* copy = fun_new_();
   copy->syscall = f->syscall;
   copy->addr = f->addr;
   dvar_fun_bind(copy, f->args, f->args + f->size);
   return copy;
}

dvar_fun* dvar_fun_move(dvar_fun* f)
{
   return f;
}

void dvar_fun_bind(dvar_fun* f, const dvar* begin, const dvar* end)
{
   t_vm_int newSize = f->size + (end - begin);
   t_vm_int newAlloc = f->alloc;
   while(newSize > newAlloc)
      newAlloc *= 2;

   if(newAlloc != f->alloc)
   {
      f->alloc = newAlloc;
      f->args = realloc(f->args, newSize * sizeof(dvar));
   }

   dvar* arg = f->args + f->size;
   for(const dvar* it = begin; it != end; ++it, ++arg)
   {
      dvar_init(arg);
      dvar_copy(arg, it);
   }

   f->size = newSize;
}

t_vm_bool dvar_fun_equals(const dvar_fun* a, const dvar_fun* b)
{
   if(a->syscall != b->syscall)
      return FALSE;

   if(a->syscall == VM_SYSCALL_NO && a->addr != b->addr)
      return FALSE;

   return TRUE;
}

t_vm_bool dvar_fun_to_bool(const dvar_fun* f)
{
   (void) f;
   return TRUE;
}

char* dvar_fun_to_string(const dvar_fun* f)
{
   if(f->syscall == VM_SYSCALL_NO)
      return strClone("user function");
   else
      return strClone("native function");
}

uint32_t dvar_fun_to_hash(const dvar_fun* f)
{
   if(f->syscall == VM_SYSCALL_NO)
      return hash_int((uint32_t) f->addr);
   else
      return hash_int((uint32_t) f->syscall);
}


struct dvar* c__fun__bind(struct dvar* l, t_vm_int n)
{
   dvar_fun_bind(l->v_fun, l + 1, l + n);
   return l;
}

struct dvar* c__fun__binda(struct dvar* l, t_vm_int n)
{
   if(n == 1)
      err("function.binda: too few arguments");

   struct dvar* arr = l+1;
   if(arr->type != DVAR_TARR)
      err("function.binda: first argument must be an array");

   dvar_fun_bind(l->v_fun, arr->v_arr->elems,
                 arr->v_arr->elems + arr->v_arr->len);
   return l;
}
