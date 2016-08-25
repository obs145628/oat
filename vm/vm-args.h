#ifndef VM_ARGS_H_
# define VM_ARGS_H_

struct dvar;

void vm_args_init(int argc, char** argv);
struct dvar* vm_args_get_var();

# endif //!VM_ARGS_H_
