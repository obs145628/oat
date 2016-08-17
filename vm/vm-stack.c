#include "vm-stack.h"
#include <assert.h>

#define STACK_SIZE (500 * 1024)
#define STACK_FRAMES_SIZE (5 * 1024)

static dvar stack_[STACK_SIZE];
static t_vm_saddr stack_frames[STACK_FRAMES_SIZE];
static dvar* stack_sp_ = NULL;
static t_vm_saddr* stack_fsp_ = NULL;

void vm_stack_init()
{
   assert(!stack_sp_);
   for(size_t i = 0; i < STACK_SIZE; ++i)
      dvar_init(stack_ + STACK_SIZE);
   stack_sp_ = stack_;
   stack_fsp_ = stack_frames;
}

dvar* vm_stack_at(t_vm_saddr addr)
{
   assert(stack_sp_);
   assert(stack_sp_ + addr < stack_ + STACK_SIZE);
   return stack_sp_ + addr;
}

dvar* vm_stack_sp()
{
   assert(stack_sp_);
   return stack_sp_;
}

void vm_stack_up(t_vm_saddr dp)
{
   assert(stack_sp_);
   assert(stack_sp_ + dp < stack_ + STACK_SIZE);
   stack_sp_ += dp;
}

void vm_stack_down(t_vm_saddr dp)
{
   assert(stack_sp_);
   assert(stack_sp_ - dp >= stack_);
   stack_sp_ -= dp;
}

void vm_stack_fup(t_vm_saddr dp)
{
   assert(stack_fsp_ < stack_frames + STACK_FRAMES_SIZE);
   *(stack_fsp_++) = dp;
   vm_stack_up(dp);
}

t_vm_saddr vm_stack_fdown()
{
   assert(stack_fsp_ > stack_frames);
   t_vm_saddr dp = *(--stack_fsp_);
   vm_stack_down(dp);
   return dp;
}
