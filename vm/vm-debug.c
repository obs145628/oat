#include "vm-debug.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "vm-bin.h"
#include "vm-stack.h"
#include "vm-exec.h"
#include "prompt.h"

typedef void (*f_print_ins_)(t_vm_addr addr);

static void print_ins_nop_(t_vm_addr addr)
{
   (void) addr;
   printf("nop\n");
}

static void print_ins_jump_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_addr p1;

   memcpy(&p1, buffer, sizeof(t_vm_addr));

   printf("jump %zu\n", (size_t) p1);
}

static void print_ins_cjump_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));

   printf("cjump %zu, %zu\n", (size_t) p1, (size_t) p2);
}

static void print_ins_fjump_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_addr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   printf("fjump %zu, %zu\n", (size_t) p1, (size_t) p2);
}

static void print_ins_fcall_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   printf("fcall %zu, %zu, %ld\n", (size_t) p1, (size_t) p2, (long) p3);
}

static void print_ins_fret_(t_vm_addr addr)
{
   (void) addr;
   printf("fret\n");
}

static void print_ins_bclear_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   printf("bclear %zu, %zu\n", (size_t) p1, (size_t) p2);
}

static void print_ins_putnull_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));

   printf("putnull %zu, %d\n", (size_t) p1, (int) p2);
}

static void print_ins_putint_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   printf("putint %zu, %d, %ld\n", (size_t) p1, (int) p2, (long) p3);
}

static void print_ins_putdouble_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_double p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_double));

   printf("putdouble %zu, %d, %G\n", (size_t) p1, (int) p2, (double) p3);
}

static void print_ins_putchar_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_char p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_char));

   printf("putchar %zu, %d, %c\n", (size_t) p1, (int) p2, (char) p3);
}

static void print_ins_putbool_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_bool p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_bool));

   printf("putbool %zu, %d, %d\n", (size_t) p1, (int) p2, (int) p3);
}

static void print_ins_putstring_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_addr p3;
   t_vm_int p4;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p4, buffer, sizeof(t_vm_int));

   printf("putstring %zu, %d, %zu, %ld (\"", (size_t) p1, (int) p2,
          (size_t) p3, (long) p4);
   fwrite(vm_bin_buffer_begin() + p3, 1, (size_t) p4, stdout);
   printf("\")\n");
}

static void print_ins_putfunction_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_addr p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_addr));

   printf("putfunction %zu, %d, %zu\n", (size_t) p1, (int) p2, (size_t) p3);
}

static void print_ins_putsyscall_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   printf("putsyscall %zu, %d, %ld\n", (size_t) p1, (int) p2, (long) p3);
}

static void print_ins_putvar_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_saddr p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));

   printf("putvar %zu, %d, %zu\n", (size_t) p1, (int) p2, (size_t) p3);
}

static void print_ins_putref_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   printf("putref %zu, %zu\n", (size_t) p1, (size_t) p2);
}

static void print_ins_copy_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   printf("copy %zu, %zu\n", (size_t) p1, (size_t) p2);
}

static void print_ins_move_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   printf("move %zu, %zu\n", (size_t) p1, (size_t) p2);
}

static void print_ins_spup_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));

   printf("spup %zu\n", (size_t) p1);
}

static void print_ins_spdown_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));

   printf("spdown %zu\n", (size_t) p1);
}

static void print_ins_syscall_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_int p1;

   memcpy(&p1, buffer, sizeof(t_vm_int));

   printf("syscall %ld\n", (long) p1);
}

static void print_ins_bind_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   printf("bind %zu, %zu, %ld\n", (size_t) p1, (size_t) p2, (long) p3);
}

static void print_ins_load_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));

   printf("load %zu, %zu\n", (size_t) p1, (size_t) p2);
}

static void print_ins_store_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));

   printf("store %zu, %zu\n", (size_t) p1, (size_t) p2);
}

static void print_ins_init_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));

   printf("init %zu, %zu\n", (size_t) p1, (size_t) p2);
}

static void print_ins_spe_opu_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr;
   t_vm_ins code;
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&code, buffer, sizeof(t_vm_ins));
   buffer += sizeof(t_vm_ins);
   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   printf("%s %zu, %zu\n", VM_INS_INFOS[code].name, (size_t) p1, (size_t) p2);
}

static void print_ins_spe_opb_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr;
   t_vm_ins code;
   t_vm_saddr p1;
   t_vm_saddr p2;
   t_vm_saddr p3;

   memcpy(&code, buffer, sizeof(t_vm_ins));
   buffer += sizeof(t_vm_ins);
   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));

   printf("%s %zu, %zu, %zu\n", VM_INS_INFOS[code].name,
          (size_t) p1, (size_t) p2, (size_t) p3);
}

static void print_ins_ternary_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr;
   t_vm_ins code;
   t_vm_saddr p1;
   t_vm_saddr p2;
   t_vm_saddr p3;
   t_vm_saddr p4;

   memcpy(&code, buffer, sizeof(t_vm_ins));
   buffer += sizeof(t_vm_ins);
   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p4, buffer, sizeof(t_vm_saddr));

   printf("ternary %zu, %zu, %zu, %zu\n", (size_t) p1, (size_t) p2,
          (size_t) p3, (size_t) p4);
}

static void print_ins_member_(t_vm_addr addr)
{
   const char* buffer = vm_bin_buffer_begin() + addr + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;
   t_vm_int p3;
   t_vm_saddr p4;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p3, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p4, buffer, sizeof(t_vm_saddr));

   printf("member %zu, %zu, %ld, %zu (\"", (size_t) p1, (size_t) p2,
          (long) p3, (size_t) p4);
   fwrite(vm_bin_buffer_begin() + p2, 1, (size_t) p3, stdout);
   printf("\")\n");
}



static void print_ins_(t_vm_addr addr)
{
   static const f_print_ins_ FNS[] = {
      print_ins_nop_,
      print_ins_jump_,
      print_ins_cjump_,
      print_ins_fjump_,
      print_ins_fcall_,
      print_ins_fret_,
      print_ins_bclear_,
      print_ins_putnull_,
      print_ins_putint_,
      print_ins_putdouble_,
      print_ins_putchar_,
      print_ins_putbool_,
      print_ins_putstring_,
      print_ins_putfunction_,
      print_ins_putsyscall_,
      print_ins_putvar_,
      print_ins_putref_,
      print_ins_copy_,
      print_ins_move_,
      print_ins_spup_,
      print_ins_spdown_,
      print_ins_syscall_,
      print_ins_bind_,
      print_ins_load_,
      print_ins_store_,
      print_ins_init_,
      print_ins_spe_opu_,
      print_ins_spe_opu_,
      print_ins_spe_opu_,
      print_ins_spe_opu_,
      print_ins_spe_opu_,
      print_ins_spe_opu_,
      print_ins_spe_opu_,
      print_ins_spe_opu_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_spe_opb_,
      print_ins_ternary_,
      print_ins_member_
   };
   t_vm_ins code;
   memcpy(&code, vm_bin_buffer_begin() + addr, sizeof(t_vm_ins));

   assert(code < VM_NB_INS);

   FNS[code](addr);
}

void vm_debug_print_code()
{
   t_vm_addr addr;
   t_vm_addr end = (t_vm_addr) vm_bin_buffer_size();
   const char* buffer = vm_bin_buffer_begin();

   memcpy(&addr, buffer, sizeof(t_vm_addr));

   printf("Start address: %zu\n", (size_t) addr);

   while(addr != end)
   {
      t_vm_ins code;
      memcpy(&code, buffer + addr, sizeof(t_vm_ins));

      printf("%zu: ", (size_t) addr);
      print_ins_(addr);

      addr += VM_INS_INFOS[code].size;
   }
}

void vm_debug_print_next_ins()
{
   t_vm_addr addr = (t_vm_addr) (vm_bin_buffer_pc() - vm_bin_buffer_begin());
   printf("%zu: ", (size_t) addr);
   print_ins_(addr);
}

void vm_debug_print_stack(t_vm_saddr pos)
{
   printf("----- Stack (0 to %zu)\n", (size_t) pos);
   for(t_vm_saddr i = 0; i < pos; ++i)
   {
      printf("%zu: ", (size_t) i);
      dvar_print(vm_stack_at(i));
   }
   printf("-----\n");
}

void vm_debug_step_by_step()
{
   printf("Debug session started (step by step)\n");

   while(vm_bin_buffer_pc() != vm_bin_buffer_end())
   {
      vm_debug_print_stack(10);
      printf("Next instruction: ");
      vm_debug_print_next_ins();
      char* data = promptLine("Execute it: (y/n)", NULL);
      int ok = strcmp(data, "y") == 0;
      if(!ok)
         break;
      vm_exec_ins();
      printf("Instruction executed\n\n\n");
   }

   printf("Debug session finished\n");

}
