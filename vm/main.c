#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dvar.h"
#include "bin-builder.h"
#include "vm-bin.h"
#include "vm-debug.h"
#include "vm-exec.h"
#include "vm-classes.h"

#include "dvar-arr.h"
#include "dvar-str.h"
#include "dvar-set.h"
#include "dvar-map.h"
#include "vm-syscall.h"
#include "dvar-class.h"

#include "numbers.h"

# define BIN "/home/obs/it/oat/tests/main.oatbin"

int fact(int n)
{
   return n <= 1 ? 1 : n * fact(n - 1);
}

void init_int(dvar* v, t_vm_int x)
{
   dvar_init_int(v, DVAR_MVAR, x);
}

void init_str(dvar* v, const char* x)
{
   dvar_init_str(v, DVAR_MVAR, x, strlen(x));
}

void member(const dvar* v, const char* str, dvar* res)
{
   dvar_member(v, str, strlen(str), res);
}

int main()
{

   /*
   s_bin_builder* b = bin_builder_new();
   bin_builder_add_string(b, "str1", "toString");
   bin_builder_add_string(b, "str2", "obs");
   bin_builder_add_string(b, "str3", "oat");
   bin_builder_add_var(b, "x");

   bin_builder_addi_putint(b, "abs", 1, 1, 0);
   bin_builder_addi_geq(b, NULL, 0, 1, 1);
   bin_builder_addi_cjump(b, NULL, 1, "abs_end");
   bin_builder_addi_uminus(b, NULL, 0, 0);
   bin_builder_addi_fret(b, "abs_end");

   bin_builder_addi_putint(b, "fact", 1, 1, 1);
   bin_builder_addi_leq(b, NULL, 0, 1, 1);
   bin_builder_addi_cjump(b, NULL, 1, "fact_end");
   bin_builder_addi_putint(b, NULL, 1, 1, 1);
   bin_builder_addi_bminus(b, NULL, 0, 1, 1);
   bin_builder_addi_fjump(b, NULL, "fact", 1);
   bin_builder_addi_mul(b, NULL, 0, 1, 0);
   bin_builder_addi_fret(b, NULL);
   bin_builder_addi_putint(b, "fact_end", 0, 1, 1);
   bin_builder_addi_fret(b, NULL);

   bin_builder_addi_nop(b, "_main");



   bin_builder_save(b, BIN);
   bin_builder_free(b);
   */



   vm_classes_init();
   vm_bin_load_file(BIN);
   vm_exec_init();
   //vm_debug_print_code();
   //vm_debug_step_by_step();
   vm_exec_run();

}
