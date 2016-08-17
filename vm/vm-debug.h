#ifndef VM_DEBUG_H_
# define VM_DEBUG_H_

# include "const.h"

void vm_debug_print_code();
void vm_debug_print_next_ins();
void vm_debug_print_stack(t_vm_saddr pos);

void vm_debug_step_by_step();

#endif //!VM_DEBUG_H_
