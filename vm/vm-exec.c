#include "vm-exec.h"
#include <string.h>
#include <assert.h>
#include "vm-stack.h"
#include "vm-bin.h"
#include "vm-syscall.h"
#include "const.h"
#include "dvar.h"
#include "date.h"

# define PCF_SIZE (10 * 1024)
static const char* pcf_frames_[PCF_SIZE];
static const char** pcf_ = NULL;

static t_vm_ins ins_code_;

static long time_start_;

static void pcf_push_(const char* addr)
{
   assert(pcf_);
   assert(pcf_ < pcf_frames_ + PCF_SIZE);
   *(pcf_++) = addr;
}

static const char* pcf_pop_()
{
   assert(pcf_);
   assert(pcf_ > pcf_frames_);
   return *(--pcf_);
}

typedef void (*f_exec_ins_)();

static void move_pc_to_next_()
{
   vm_bin_buffer_move_pc(VM_INS_INFOS[ins_code_].size);
}

static void exec_ins_nop_()
{
   move_pc_to_next_();
}

static void exec_ins_jump_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_addr p1;

   memcpy(&p1, buffer, sizeof(t_vm_addr));

   vm_bin_buffer_set_pc(vm_bin_buffer_begin() + p1);
}

static void exec_ins_cjump_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));

   oat_bool b = dvar_to_bool(vm_stack_at(p1));
   if(b)
      vm_bin_buffer_set_pc(vm_bin_buffer_begin() + p2);
   else
      move_pc_to_next_();
}

static void exec_ins_fjump_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_addr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   vm_stack_fup(p2);
   const char* next = vm_bin_buffer_pc()
    + VM_INS_INFOS[VM_INS_FJUMP].size;
   pcf_push_(next);

   vm_bin_buffer_set_pc(vm_bin_buffer_begin() + p1);
}

static void exec_ins_fret_()
{
   vm_stack_fdown();
   vm_bin_buffer_set_pc(pcf_pop_());
}

static void exec_ins_bclear_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   dvar* begin = vm_stack_sp() + p1;
   dvar* end = begin + p2;
   for(dvar* it = begin; it != end; ++it)
      dvar_erase(it);

   move_pc_to_next_();
}

static void exec_ins_putnull_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));

   dvar_assign_null(vm_stack_at(p1));
   move_pc_to_next_();
}

static void exec_ins_putint_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));

   dvar_assign_int(vm_stack_at(p1), p2);
   move_pc_to_next_();
}

static void exec_ins_putdouble_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_double p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_double));

   dvar_assign_double(vm_stack_at(p1), p2);
   move_pc_to_next_();
}

static void exec_ins_putchar_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_char p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_char));

   dvar_assign_char(vm_stack_at(p1), p2);
   move_pc_to_next_();
}

static void exec_ins_putbool_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_bool p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_bool));

   dvar_assign_bool(vm_stack_at(p1), p2);
   move_pc_to_next_();
}

static void exec_ins_putstring_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   const char* str = vm_bin_buffer_begin() + p2;

   dvar_assign_string(vm_stack_at(p1), str, (size_t) p3);
   move_pc_to_next_();
}

static void exec_ins_spup_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));

   vm_stack_up(p1);
   move_pc_to_next_();
}

static void exec_ins_spdown_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));

   vm_stack_down(p1);
   move_pc_to_next_();
}

static void exec_ins_syscall_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_int p1;

   memcpy(&p1, buffer, sizeof(t_vm_int));

   vm_syscall_exec(p1);
   move_pc_to_next_();
}

static void exec_ins_spe_opu_()
{
   static const f_dvar_opu FNS[] = {
      (f_dvar_opu) dvar_op_postinc,
      (f_dvar_opu) dvar_op_postdec,
      (f_dvar_opu) dvar_op_preinc,
      (f_dvar_opu) dvar_op_predec,
      (f_dvar_opu) dvar_op_uplus,
      (f_dvar_opu) dvar_op_uminus,
      (f_dvar_opu) dvar_op_lnot
   };

   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;
   dvar* sp = vm_stack_sp();

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   FNS[ins_code_ - VM_INS_POSTINC](sp + p1, sp + p2);
   move_pc_to_next_();
}

static void exec_ins_spe_opb_()
{

   static const f_dvar_opb FNS[] = {
      (f_dvar_opb) dvar_op_mul,
      (f_dvar_opb) dvar_op_div,
      (f_dvar_opb) dvar_op_mod,
      (f_dvar_opb) dvar_op_bplus,
      (f_dvar_opb) dvar_op_bminus,
      (f_dvar_opb) dvar_op_gt,
      (f_dvar_opb) dvar_op_lt,
      (f_dvar_opb) dvar_op_geq,
      (f_dvar_opb) dvar_op_leq,
      (f_dvar_opb) dvar_op_eq,
      (f_dvar_opb) dvar_op_neq,
      (f_dvar_opb) dvar_op_land,
      (f_dvar_opb) dvar_op_lor,
      (f_dvar_opb) dvar_op_assign,
      (f_dvar_opb) dvar_op_pluseq,
      (f_dvar_opb) dvar_op_minuseq,
      (f_dvar_opb) dvar_op_muleq,
      (f_dvar_opb) dvar_op_diveq,
      (f_dvar_opb) dvar_op_modeq,
   };

   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;
   t_vm_saddr p3;
   dvar* sp = vm_stack_sp();

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));

   FNS[ins_code_ - VM_INS_MUL](sp + p1, sp + p2, sp + p3);
   move_pc_to_next_();
}

void vm_exec_init()
{
   vm_stack_init();
   pcf_ = pcf_frames_;

   t_vm_addr addr;
   const char* buffer = vm_bin_buffer_begin();
   memcpy(&addr, buffer, sizeof(t_vm_addr));

   vm_bin_buffer_set_pc(buffer + addr);
   time_start_ = dateNow();
}

void vm_exec_ins()
{
   static const f_exec_ins_ FNS[] = {
      exec_ins_nop_,
      exec_ins_jump_,
      exec_ins_cjump_,
      exec_ins_fjump_,
      exec_ins_fret_,
      exec_ins_bclear_,
      exec_ins_putnull_,
      exec_ins_putint_,
      exec_ins_putdouble_,
      exec_ins_putchar_,
      exec_ins_putbool_,
      exec_ins_putstring_,
      exec_ins_spup_,
      exec_ins_spdown_,
      exec_ins_syscall_,
      exec_ins_spe_opu_,
      exec_ins_spe_opu_,
      exec_ins_spe_opu_,
      exec_ins_spe_opu_,
      exec_ins_spe_opu_,
      exec_ins_spe_opu_,
      exec_ins_spe_opu_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
      exec_ins_spe_opb_,
   };

   memcpy(&ins_code_, vm_bin_buffer_pc(), sizeof(t_vm_ins));

   assert(ins_code_ < VM_NB_INS);

   FNS[ins_code_]();
}

void vm_exec_run()
{
   while(1)
      vm_exec_ins();
}

long vm_exec_get_start_time()
{
   return time_start_;
}
