#ifndef DVAR_IO_H_
# define DVAR_IO_H_

# include "const.h"

struct dvar;

struct dvar* syscall_prompt_line(struct dvar* l, t_vm_int n);
struct dvar* syscall_prompt_int(struct dvar* l, t_vm_int n);
struct dvar* syscall_prompt_double(struct dvar* l, t_vm_int n);
struct dvar* syscall_prompt_char(struct dvar* l, t_vm_int n);
struct dvar* syscall_prompt_bool(struct dvar* l, t_vm_int n);

#endif //!DVAR_IO_H_
