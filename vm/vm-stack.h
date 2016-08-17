#ifndef VM_STACK_H_
# define VM_STACK_H_

# include "dvar.h"

void vm_stack_init();

dvar* vm_stack_at(t_vm_saddr addr);
dvar* vm_stack_sp();
void vm_stack_up(t_vm_saddr dp);
void vm_stack_down(t_vm_saddr dp);
void vm_stack_fup(t_vm_saddr dp);
t_vm_saddr vm_stack_fdown();


# endif //!VM_STACK_H_
