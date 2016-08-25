#ifndef VM_CLASSES_H_
# define VM_CLASSES_H_

# endif //!VM_CLASSES_H_

# include "const.h"

struct dvar;

void vm_classes_init();

struct dvar* c__rect__constructor(struct dvar* l, t_vm_int n);
struct dvar* c__rect__get_x(struct dvar* l, t_vm_int n);
struct dvar* c__rect__get_y(struct dvar* l, t_vm_int n);
struct dvar* c__rect__get_width(struct dvar* l, t_vm_int n);
struct dvar* c__rect__get_height(struct dvar* l, t_vm_int n);
struct dvar* c__rect__set_x(struct dvar* l, t_vm_int n);
struct dvar* c__rect__set_y(struct dvar* l, t_vm_int n);
struct dvar* c__rect__set_width(struct dvar* l, t_vm_int n);
struct dvar* c__rect__set_height(struct dvar* l, t_vm_int n);
