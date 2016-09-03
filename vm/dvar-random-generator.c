#include "dvar-random-generator.h"
#include "vm-dvar-utils.h"
#include "dvar.h"
#include "dvar-class.h"
#include "dvar-obj.h"
#include "rand.h"
#include "date.h"


void dvar_random_generator_init()
{
   vm_define_begin(VM_CLASS_RANDOM_GENERATOR, "RandomGenerator",
                   DVAR_CLASS_NOPARENT);
   vm_define_method("constructor", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_RANDOM_GENERATOR_CONSTRUCTOR);
   vm_define_method("destructor", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_RANDOM_GENERATOR_DESTRUCTOR);
   vm_define_method("getBool", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_RANDOM_GENERATOR_GET_BOOL);
   vm_define_method("getInt", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_RANDOM_GENERATOR_GET_INT);
   vm_define_method("getDouble", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_RANDOM_GENERATOR_GET_DOUBLE);
   vm_define_method("getChar", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_RANDOM_GENERATOR_GET_CHAR);
   vm_define_end_();
}

struct dvar* c__random_generator__constructor(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_obj_val* o = l->v_obj->val;

   long seed;
   if(n > 1)
      seed = (long) dvar_get_int(l+1, "RandomGenerator.constructor():"
                                 " argument bust me an integer");
   else
      seed = dateNow();

   o->extra = rand_new(seed);
   return l;
}

struct dvar* c__random_generator__destructor(struct dvar* l, t_vm_int n)
{
   (void) n;
   rand_free(l->v_obj->val->extra);
   return l;
}

struct dvar* c__random_generator__get_bool(struct dvar* l, t_vm_int n)
{
   (void) n;
   s_rand* r = l->v_obj->val->extra;
   dvar_putbool(l, DVAR_MVAR, (t_vm_bool) rand_get_bool(r));
   return l;
}

struct dvar* c__random_generator__get_int(struct dvar* l, t_vm_int n)
{
   s_rand* r = l->v_obj->val->extra;

   t_vm_int res = (t_vm_int) rand_get_long(r);

   if(n > 1)
   {
      t_vm_int min;
      t_vm_int max;

      if(n > 2)
      {
         min = dvar_get_int(l+1, "RandomGenerator.getInt: min not a number");
         max = dvar_get_int(l+2, "RandomGenerator.getInt: max not a number");
      }
      else
      {
         min = 0;
         max = dvar_get_int(l+1, "RandomGenerator.getInt: max not a number");
      }

      res = res % (max - min) + min;
   }

   dvar_putint(l, DVAR_MVAR, res);
   return l;
}

struct dvar* c__random_generator__get_double(struct dvar* l, t_vm_int n)
{
   s_rand* r = l->v_obj->val->extra;

   t_vm_double res = (t_vm_double) rand_get_double(r);

   if(n > 1)
   {
      t_vm_double min;
      t_vm_double max;

      if(n > 2)
      {
         min = dvar_get_double(l+1,
                               "RandomGenerator.getDouble: min not a number");
         max = dvar_get_double(l+2,
                               "RandomGenerator.getDouble: max not a number");
      }
      else
      {
         min = 0;
         max = dvar_get_double(l+1,
                               "RandomGenerator.getDouble: max not a number");
      }

      res = res * (max - min) + min;
   }

   dvar_putdouble(l, DVAR_MVAR, res);
   return l;
}

struct dvar* c__random_generator__get_char(struct dvar* l, t_vm_int n)
{
   s_rand* r = l->v_obj->val->extra;

   t_vm_char res = (t_vm_char) rand_get_char(r);

   if(n > 1)
   {
      t_vm_char min;
      t_vm_char max;

      if(n > 2)
      {
         min = dvar_get_char(l+1, "RandomGenerator.getChar: min not a number");
         max = dvar_get_char(l+2, "RandomGenerator.getChar: max not a number");
      }
      else
      {
         min = 0;
         max = dvar_get_char(l+1, "RandomGenerator.getChar: max not a number");
      }

      res = res % (max - min) + min;
   }

   dvar_putchar(l, DVAR_MVAR, res);
   return l;
}
