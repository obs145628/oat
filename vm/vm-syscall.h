#ifndef VM_SYSCALL_H_
# define VM_SYSCALL_H_

# include "const.h"
# include "dvar.h"
# include "dvar-fun.h"

dvar* vm_syscall_exec(t_vm_int syscall, dvar* argv, t_vm_int argc);

void vm_syscall_exec_fn(dvar_fun* fn, const dvar* begin, const dvar* end,
                        dvar* res);

#endif //!VM_SYSCALL_H_
