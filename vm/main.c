#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dvar.h"
#include "bin-builder.h"
#include "vm-bin.h"
#include "vm-debug.h"
#include "vm-exec.h"

# define BIN "/home/obs/it/oat/tests/main.oatbin"

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

   bin_builder_addi_putstring(b, NULL, 0, 1, "str1", 8);
   bin_builder_addi_putstring(b, NULL, 1, 1, "str2", 3);
   bin_builder_addi_putref(b, NULL, 3, 0);
   bin_builder_addi_pluseq(b, NULL, 3, 1, 4);
   bin_builder_addi_bplus(b, NULL, 0, 1, 2);
   bin_builder_addi_copy(b, NULL, 1, 0);
   bin_builder_addi_move(b, NULL, 2, 0);

   bin_builder_addi_member(b, NULL, 0, "str1", 8, 1);
   bin_builder_addi_fcall(b, NULL, 1, 5, 0);


   bin_builder_addi_fcall(b, NULL, 1, 5, 1);
   bin_builder_addi_syscall(b, NULL, 1);

   bin_builder_addi_syscall(b, NULL, 0);



   bin_builder_save(b, BIN);
   bin_builder_free(b);

   */

   vm_bin_load_file(BIN);
   vm_exec_init();
   //vm_debug_print_code();
   //vm_debug_step_by_step();
   vm_exec_run();

}
