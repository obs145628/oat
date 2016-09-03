#ifndef DVAR_RANDOM_GENERATOR_H_
# define DVAR_RANDOM_GENERATOR_H_

# include "const.h"

struct dvar;

void dvar_random_generator_init();

struct dvar* c__random_generator__constructor(struct dvar* l, t_vm_int n);
struct dvar* c__random_generator__destructor(struct dvar* l, t_vm_int n);
struct dvar* c__random_generator__get_bool(struct dvar* l, t_vm_int n);
struct dvar* c__random_generator__get_int(struct dvar* l, t_vm_int n);
struct dvar* c__random_generator__get_double(struct dvar* l, t_vm_int n);
struct dvar* c__random_generator__get_char(struct dvar* l, t_vm_int n);

#endif //!DVAR_RANDOM_GENERATOR_H_
