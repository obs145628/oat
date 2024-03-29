#ifndef DVAR_FUN_H_
# define DVAR_FUN_H_

# include "const.h"

struct dvar;

struct dvar_fun {
   t_vm_int syscall;
   t_vm_addr addr;
   t_vm_int size;
   t_vm_int alloc;
   struct dvar *args;
};
typedef struct dvar_fun dvar_fun;

dvar_fun* dvar_fun_new_syscall(t_vm_int syscall);
dvar_fun* dvar_fun_new_function(t_vm_addr addr);
void dvar_fun_free(dvar_fun* f);
dvar_fun* dvar_fun_copy(const dvar_fun* f);
dvar_fun* dvar_fun_move(dvar_fun* f);
void dvar_fun_bind(dvar_fun* f, const struct dvar* begin,
                   const struct dvar* end);

t_vm_bool dvar_fun_equals(const dvar_fun* a, const dvar_fun* b);
t_vm_bool dvar_fun_to_bool(const dvar_fun* f);
char* dvar_fun_to_string(const dvar_fun* f);
uint32_t dvar_fun_to_hash(const dvar_fun* f);


struct dvar* c__fun__bind(struct dvar* l, t_vm_int n);
struct dvar* c__fun__binda(struct dvar* l, t_vm_int n);

#endif //!DVAR_FUN_H_
