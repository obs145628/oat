#include "vm-exec.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "vm-stack.h"
#include "vm-bin.h"
#include "vm-syscall.h"
#include "vm-classes.h"
#include "vm-args.h"
#include "const.h"
#include "dvar.h"
#include "dvar-fun.h"
#include "dvar-class.h"
#include "dvar-obj.h"
#include "date.h"
#include "err.h"

# define PCF_SIZE (10 * 1024)
static char* pcf_frames_[PCF_SIZE];
static char** pcf_ = NULL;

static t_vm_ins ins_code_;

static long time_start_;

static t_vm_saddr pcf_addrs_tab_[PCF_SIZE];
static t_vm_saddr* pcf_addrs_ = NULL;
static t_vm_addr stack_size_;

static void pcf_push_(char* addr)
{
   assert(pcf_);
   assert(pcf_ < pcf_frames_ + PCF_SIZE);
   *(pcf_++) = addr;
}

static char* pcf_pop_()
{
   assert(pcf_);
   assert(pcf_ > pcf_frames_);
   return *(--pcf_);
}

static void push_stack_size_()
{
   assert(pcf_addrs_);
   assert(pcf_addrs_ < pcf_addrs_tab_ + PCF_SIZE);
   *(pcf_addrs_++) = stack_size_;
   stack_size_ = 1;
}

static void pop_stack_size_()
{
   assert(pcf_addrs_);
   assert(pcf_addrs_ > pcf_addrs_tab_);
   stack_size_ =  *(--pcf_addrs_);
}

static void use_addresses_(t_vm_saddr s1, t_vm_saddr s2, t_vm_saddr s3,
                           t_vm_saddr s4, t_vm_saddr s5)
{
   if(s1 > stack_size_)
      stack_size_ = s1;
   if(s2 > stack_size_)
      stack_size_ = s2;
   if(s3 > stack_size_)
      stack_size_ = s3;
   if(s4 > stack_size_)
      stack_size_ = s4;
   if(s5 > stack_size_)
      stack_size_ = s5;
}

static void exec_user_function_(t_vm_addr addr, t_vm_saddr pos)
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_addr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   vm_stack_fup(pos);
   char* next = vm_bin_buffer_pc()
    + VM_INS_INFOS[ins_code_].size;
   pcf_push_(next);
   push_stack_size_();

   vm_bin_buffer_set_pc(vm_bin_buffer_begin() + addr);
}

static void exec_syscall_(t_vm_int syscall, t_vm_saddr pos, t_vm_int count)
{
   vm_syscall_exec(syscall, vm_stack_sp() + pos, count);
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

   use_addresses_(p1, 0, 0, 0, 0);

   t_vm_bool b = dvar_to_bool(vm_stack_at(p1));
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

   exec_user_function_(p1, p2);
}

static void exec_ins_fcall_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar* v = vm_stack_at(p1);
   dvar* args = vm_stack_at(p2);

   assert(p3 >= 0);

   if(v->type == DVAR_TREF)
      v = v->v_ref;

   if(v->type != DVAR_TFUN)
      err("Operator() can only be applied to functions");

   dvar_fun* fn = v->v_fun;
   dvar* args_end = vm_stack_at(p2 + p3 + fn->size);

   if(fn->size)
      for(t_vm_int i = p3 - 1; i >= 0; --i)
         dvar_move(args + fn->size + i, args + i);
   
   for(t_vm_int i = 0; i < fn->size; ++i)
      dvar_copy(args + i, fn->args + i);

   t_vm_int size = args_end - args;

   if(fn->syscall == VM_SYSCALL_NO)
   {
      exec_user_function_(fn->addr, p2);
   }

   else
   {
      dvar* res = vm_syscall_exec(fn->syscall, args, size);
      dvar_move(res, args);
      move_pc_to_next_();
   }
}

static void exec_ins_fret_()
{
   vm_stack_fdown();
   vm_bin_buffer_set_pc(pcf_pop_());
   pop_stack_size_();
}

static void exec_ins_bclear_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   use_addresses_(p1, p1 + p2, 0, 0, 0);

   dvar* begin = vm_stack_at(p1);
   dvar* end = vm_stack_at(p1 + p2);
   dvar_bclear(begin, end);
   move_pc_to_next_();
}

static void exec_ins_putnull_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar_putnull(vm_stack_at(p1), (t_vm_mode) p2);
   move_pc_to_next_();
}

static void exec_ins_putint_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar_putint(vm_stack_at(p1), (t_vm_mode) p2, p3);
   move_pc_to_next_();
}

static void exec_ins_putdouble_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_double p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_double));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar_putdouble(vm_stack_at(p1), (t_vm_mode) p2, p3);
   move_pc_to_next_();
}

static void exec_ins_putchar_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_char p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_char));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar_putchar(vm_stack_at(p1), (t_vm_mode) p2, p3);
   move_pc_to_next_();
}

static void exec_ins_putbool_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_bool p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_bool));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar_putbool(vm_stack_at(p1), (t_vm_mode) p2, p3);
   move_pc_to_next_();
}

static void exec_ins_putstring_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
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

   use_addresses_(p1, 0, 0, 0, 0);

   const char* str = vm_bin_buffer_begin() + p3;

   dvar_putstring(vm_stack_at(p1), (t_vm_mode) p2, str, p4);
   move_pc_to_next_();
}

static void exec_ins_putfunction_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_addr p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_addr));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar_putfunction(vm_stack_at(p1), (t_vm_mode) p2, p3);
   move_pc_to_next_();
}

static void exec_ins_putsyscall_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar_putsyscall(vm_stack_at(p1), (t_vm_mode) p2, p3);
   move_pc_to_next_();
}

static void exec_ins_putarr_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_saddr p3;
   t_vm_int p4;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p4, buffer, sizeof(t_vm_int));

   use_addresses_(p1, p3, p3 + p4, 0, 0);

   dvar_putarr(vm_stack_at(p1), (t_vm_mode) p2,
               vm_stack_at(p3), vm_stack_at(p3 + p4));
   move_pc_to_next_();
}

static void exec_ins_putset_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_saddr p3;
   t_vm_int p4;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p4, buffer, sizeof(t_vm_int));

   use_addresses_(p1, p3, p3 + p4, 0, 0);

   dvar_putset(vm_stack_at(p1), (t_vm_mode) p2,
               vm_stack_at(p3), vm_stack_at(p3 + p4));
   move_pc_to_next_();
}
static void exec_ins_putmap_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_saddr p3;
   t_vm_int p4;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p4, buffer, sizeof(t_vm_int));

   use_addresses_(p1, p3, p3 + p4, 0, 0);

   dvar_putmap(vm_stack_at(p1), (t_vm_mode) p2,
               vm_stack_at(p3), vm_stack_at(p3 + p4));
   move_pc_to_next_();
}

static void exec_ins_putobj_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar_putobj(vm_stack_at(p1), (t_vm_mode) p2, p3);
   move_pc_to_next_();
}

static void exec_ins_putclass_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar_putclass(vm_stack_at(p1), (t_vm_mode) p2, p3);
   move_pc_to_next_();
}

static void exec_ins_putvar_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_int p2;
   t_vm_saddr p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));

   use_addresses_(p1, p3, 0, 0, 0);

   dvar_putvar(vm_stack_at(p1), (t_vm_mode) p2, vm_stack_at(p3));
   move_pc_to_next_();
}

static void exec_ins_putref_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   use_addresses_(p1, p2, 0, 0, 0);

   dvar_putref(vm_stack_at(p1), vm_stack_at(p2));
   move_pc_to_next_();
}

static void exec_ins_copy_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   use_addresses_(p1, p2, 0, 0, 0);


   dvar_copy(vm_stack_at(p1), vm_stack_at(p2));
   move_pc_to_next_();
}

static void exec_ins_move_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   use_addresses_(p1, p2, 0, 0, 0);

   dvar_move(vm_stack_at(p1), vm_stack_at(p2));
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

   exec_syscall_(p1, 0, 1);
   move_pc_to_next_();
}

static void exec_ins_bind_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;
   t_vm_int p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p3, buffer, sizeof(t_vm_int));

   assert(p3 >= 0);

   use_addresses_(p1, p2, p2 + p3, 0, 0);


   dvar_bind(vm_stack_at(p1), vm_stack_at(p2), vm_stack_at(p2 + p3));
   move_pc_to_next_();
}

static void exec_ins_load_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar* v1 = vm_stack_at(p1);
   dvar* v2 = (dvar*) (vm_bin_buffer_begin() + p2);

   dvar_putref(v1, v2);
   move_pc_to_next_();
}

static void exec_ins_store_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar* v1 = vm_stack_at(p1);
   dvar* v2 = (dvar*) (vm_bin_buffer_begin() + p2);

   if(v1->type == DVAR_TREF)
      v1 = v1->v_ref;

   if(v2->mode == DVAR_MCONST)
      err("Unable to set a const global variable");

   if(v2->mode == DVAR_MTCONST && v2->type != v1->type)
      err("Unable to change type of a tconst global vaiable");

   t_vm_mode mode = v2->mode;
   dvar_copy(v2, v1);
   v2->mode = mode;

   move_pc_to_next_();
}

static void exec_ins_init_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_addr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_addr));

   use_addresses_(p1, 0, 0, 0, 0);

   dvar* v1 = vm_stack_at(p1);
   dvar* v2 = (dvar*) (vm_bin_buffer_begin() + p2);

   dvar_init(v2);
   t_vm_mode mode = v1->mode;
   dvar_copy(v2, v1);
   v2->mode = mode;

   move_pc_to_next_();
}

typedef void (*f_ins_opu_)(dvar* a, dvar* res);

static void exec_ins_spe_opu_()
{
   static const f_ins_opu_ FNS[] = {
      (f_ins_opu_) dvar_postinc,
      (f_ins_opu_) dvar_postdec,
      (f_ins_opu_) dvar_preinc,
      (f_ins_opu_) dvar_predec,
      (f_ins_opu_) dvar_uplus,
      (f_ins_opu_) dvar_uminus,
      (f_ins_opu_) dvar_lnot,
      (f_ins_opu_) dvar_bnot
   };

   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));

   use_addresses_(p1, p2, 0, 0, 0);

   FNS[ins_code_ - VM_INS_POSTINC](vm_stack_at(p1), vm_stack_at(p2));
   move_pc_to_next_();
}

typedef void (*f_ins_opb_)(dvar* a, dvar* b, dvar* res);

static void exec_ins_spe_opb_()
{

   static const f_ins_opb_ FNS[] = {
      (f_ins_opb_) dvar_mul,
      (f_ins_opb_) dvar_div,
      (f_ins_opb_) dvar_mod,
      (f_ins_opb_) dvar_bplus,
      (f_ins_opb_) dvar_bminus,
      (f_ins_opb_) dvar_gt,
      (f_ins_opb_) dvar_lt,
      (f_ins_opb_) dvar_geq,
      (f_ins_opb_) dvar_leq,
      (f_ins_opb_) dvar_eq,
      (f_ins_opb_) dvar_neq,
      (f_ins_opb_) dvar_land,
      (f_ins_opb_) dvar_lor,
      (f_ins_opb_) dvar_lshift,
      (f_ins_opb_) dvar_rshift,
      (f_ins_opb_) dvar_band,
      (f_ins_opb_) dvar_bxor,
      (f_ins_opb_) dvar_bor,
      (f_ins_opb_) dvar_assign,
      (f_ins_opb_) dvar_pluseq,
      (f_ins_opb_) dvar_minuseq,
      (f_ins_opb_) dvar_muleq,
      (f_ins_opb_) dvar_diveq,
      (f_ins_opb_) dvar_modeq,
      (f_ins_opb_) dvar_lshifteq,
      (f_ins_opb_) dvar_rshifteq,
      (f_ins_opb_) dvar_bandeq,
      (f_ins_opb_) dvar_bxoreq,
      (f_ins_opb_) dvar_boreq,
      (f_ins_opb_) dvar_subscript
   };

   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;
   t_vm_saddr p3;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));

   use_addresses_(p1, p2, p3, 0, 0);

   FNS[ins_code_ - VM_INS_MUL](vm_stack_at(p1), vm_stack_at(p2),
                               vm_stack_at(p3));
   move_pc_to_next_();
}

static void exec_ins_ternary_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;
   t_vm_saddr p2;
   t_vm_saddr p3;
   t_vm_saddr p4;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p2, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p3, buffer, sizeof(t_vm_saddr));
   buffer += sizeof(t_vm_saddr);
   memcpy(&p4, buffer, sizeof(t_vm_saddr));

   use_addresses_(p1, p2, p3, p4, 0);

   dvar_ternary(vm_stack_at(p1),vm_stack_at(p2), vm_stack_at(p3),
                vm_stack_at(p4));
   move_pc_to_next_();
}

static void exec_ins_member_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
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

   use_addresses_(p1, p4, 0, 0, 0);

   dvar_member(vm_stack_at(p1), vm_bin_buffer_begin() + p2, p3,
               vm_stack_at(p4));
   move_pc_to_next_();
}

static void exec_ins_defclass_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_addr p1;
   t_vm_int p2;
   t_vm_int p3;
   t_vm_int p4;

   memcpy(&p1, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p2, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p3, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p4, buffer, sizeof(t_vm_int));

   dvar_define_class(vm_bin_buffer_begin() + p1, p2, p3, p4);
   move_pc_to_next_();
}

static void exec_ins_deffield_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_int p1;
   t_vm_addr p2;
   t_vm_int p3;
   t_vm_int p4;
   t_vm_saddr p5;

   memcpy(&p1, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p2, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p3, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p4, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p5, buffer, sizeof(t_vm_saddr));

   dvar_define_field(p1, vm_bin_buffer_begin() + p2, p3, p4, DVAR_CLASS_VPUBLIC,
                     FALSE, vm_stack_at(p5));
   move_pc_to_next_();
}

static void exec_ins_defsfield_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_int p1;
   t_vm_addr p2;
   t_vm_int p3;
   t_vm_int p4;
   t_vm_saddr p5;

   memcpy(&p1, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p2, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p3, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p4, buffer, sizeof(t_vm_int));
   buffer += sizeof(t_vm_int);
   memcpy(&p5, buffer, sizeof(t_vm_saddr));

   dvar_define_field(p1, vm_bin_buffer_begin() + p2, p3, p4, DVAR_CLASS_VPUBLIC,
                     TRUE, vm_stack_at(p5));
   move_pc_to_next_();
}

static void exec_ins_defend_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_int p1;

   memcpy(&p1, buffer, sizeof(t_vm_int));

   dvar_define_end(p1);
   move_pc_to_next_();
}

static void exec_ins_ssuper_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_saddr p1;

   memcpy(&p1, buffer, sizeof(t_vm_saddr));

   dvar* v = vm_stack_at(p1);
   assert(v->type == DVAR_TOBJ);
   v->v_obj->is_super = TRUE;

   move_pc_to_next_();
}

static void exec_ins_setfscope_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_addr p1;
   t_vm_int p2;

   memcpy(&p1, buffer, sizeof(t_vm_addr));
   buffer += sizeof(t_vm_addr);
   memcpy(&p2, buffer, sizeof(t_vm_int));

   err_set_scope_name(vm_bin_buffer_begin() + p1, p2);
   move_pc_to_next_();
}

static void exec_ins_setfline_()
{
   const char* buffer = vm_bin_buffer_pc() + sizeof(t_vm_ins);
   t_vm_int p1;

   memcpy(&p1, buffer, sizeof(t_vm_int));

   assert(p1 >= 0);
   err_set_line_(p1);
   move_pc_to_next_();
}


void vm_exec_init()
{
   vm_classes_init();
   vm_stack_init();
   dvar_copy(vm_stack_sp(), vm_args_get_var());
   pcf_ = pcf_frames_;
   pcf_addrs_ = pcf_addrs_tab_;
   stack_size_ = 1;

   t_vm_addr addr;
   char* buffer = vm_bin_buffer_begin();
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
      exec_ins_fcall_,
      exec_ins_fret_,
      exec_ins_bclear_,
      exec_ins_putnull_,
      exec_ins_putint_,
      exec_ins_putdouble_,
      exec_ins_putchar_,
      exec_ins_putbool_,
      exec_ins_putstring_,
      exec_ins_putfunction_,
      exec_ins_putsyscall_,
      exec_ins_putarr_,
      exec_ins_putset_,
      exec_ins_putmap_,
      exec_ins_putobj_,
      exec_ins_putclass_,
      exec_ins_putvar_,
      exec_ins_putref_,
      exec_ins_copy_,
      exec_ins_move_,
      exec_ins_spup_,
      exec_ins_spdown_,
      exec_ins_syscall_,
      exec_ins_bind_,
      exec_ins_load_,
      exec_ins_store_,
      exec_ins_init_,
      exec_ins_spe_opu_,
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
      exec_ins_ternary_,
      exec_ins_member_,
      exec_ins_defclass_,
      exec_ins_deffield_,
      exec_ins_defsfield_,
      exec_ins_defend_,
      exec_ins_ssuper_,
      exec_ins_setfscope_,
      exec_ins_setfline_
   };

   memcpy(&ins_code_, vm_bin_buffer_pc(), sizeof(t_vm_ins));

   if(ins_code_ >= VM_NB_INS)
      printf("code: %d\n", (int) ins_code_);

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
