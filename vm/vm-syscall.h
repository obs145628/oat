#ifndef VM_SYSCALL_H_
# define VM_SYSCALL_H_

# include "const.h"
# include "dvar.h"

dvar* vm_syscall_exec(t_vm_int syscall, dvar* argv, t_vm_int argc);

#endif //!VM_SYSCALL_H_
