#include "vm-args.h"
#include <string.h>
#include <assert.h>
#include "dvar.h"
#include "dvar-arr.h"

int argc_ = 0;
char** argv_;
dvar var_args_;


void vm_args_init(int argc, char** argv)
{
   argc_ = argc;
   argv_ = argv;
   dvar_init_arr(&var_args_, DVAR_MVAR, NULL, NULL);

   for(int i = 0; i < argc; ++i)
   {
      dvar s;
      dvar_init_str(&s, DVAR_MVAR, argv[i], (t_vm_int) strlen(argv[i]));
      dvar_arr_insert(var_args_.v_arr, var_args_.v_arr->len, &s, &s + 1);
   }
}

dvar* vm_args_get_var()
{
   assert(argc_);
   return &var_args_;
}
