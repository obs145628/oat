#ifndef VM_DVAR_UTILS_H_
# define VM_DVAR_UTILS_H_

# include "const.h"

struct dvar;

void vm_define_begin(t_vm_int id, const char* name);
void vm_define_end_();
void vm_define_field(const char* name, t_vm_int type,
                     t_vm_int visibility, t_vm_bool is_static,
                     const struct dvar* value);
void vm_define_method(const char* name,
                      t_vm_int visibility, t_vm_bool is_static,
                      t_vm_int syscall);

#endif //!VM_DVAR_UTILS_H_
