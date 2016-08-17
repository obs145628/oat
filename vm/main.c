#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dvar.h"
#include "str.h"
#include "bin-builder.h"
#include "const.h"
#include "vm-bin.h"
#include "vm-debug.h"
#include "vm-exec.h"

#define BIN_PATH "/home/obs/it/oat/tests/main.oatbin"

int main()
{
   vm_bin_load_file(BIN_PATH);
   vm_exec_init();

   //vm_debug_print_code();
   //vm_debug_step_by_step();
   vm_exec_run();

   /*s_bin_builder* b = bin_builder_new();

   bin_builder_add_int(b, "my_int", 8);
   bin_builder_add_double(b, "my_double", 12.89);
   bin_builder_add_char(b, "my_char", 'c');
   bin_builder_add_bool(b, "my_bool", TRUE);
   bin_builder_add_string(b, "my_string", "Hello, World !\n");

   bin_builder_addi_putint(b, "fact", 1, 1);
   bin_builder_addi_leq(b, NULL, 0, 1, 1);
   bin_builder_addi_cjump(b, NULL, 1, "fact_end");
   bin_builder_addi_putint(b, NULL, 1, 1);
   bin_builder_addi_bminus(b, NULL, 0, 1, 1);
   bin_builder_addi_fjump(b, NULL, "fact", 1);
   bin_builder_addi_mul(b, NULL, 0, 1, 0);
   bin_builder_addi_fret(b, NULL);
   bin_builder_addi_putint(b, "fact_end", 0, 1);
   bin_builder_addi_fret(b, NULL);

   bin_builder_addi_putint(b, "_main", 0, 8);
   bin_builder_addi_assign(b, NULL, 1, 0, 1);
   bin_builder_addi_fjump(b, NULL, "fact", 1);

   bin_builder_addi_assign(b, NULL, 0, 1, 0);
   bin_builder_addi_syscall(b, NULL, 2);

   bin_builder_addi_putint(b, NULL, 0, 0);
   bin_builder_addi_syscall(b, NULL, 0);

   bin_builder_save(b, BIN_PATH);
   bin_builder_free(b);

   */
}
