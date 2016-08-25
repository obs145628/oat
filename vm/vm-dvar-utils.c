#include "vm-dvar-utils.h"
#include "dvar.h"
#include "dvar-class.h"
#include <assert.h>
#include <string.h>

static t_vm_int id_ = -1;

void vm_define_begin(t_vm_int id, const char* name, t_vm_int parent)
{
   assert(id_ == -1);
   id_ = id;
   dvar_define_class(name, strlen(name), id, parent);
}

void vm_define_end_()
{
   assert(id_ != -1);
   dvar_define_end(id_);
   id_ = -1;
}

void vm_define_field(const char* name, t_vm_int type,
                     t_vm_int visibility, t_vm_bool is_static,
                     const dvar* value)
{
   assert(id_ != -1);
   dvar_define_field(id_, name, strlen(name), type, visibility,
                     is_static, value);
}

void vm_define_method(const char* name,
                      t_vm_int visibility, t_vm_bool is_static,
                      t_vm_int syscall)
{
   assert(id_ != -1);
   dvar v;
   dvar_init_syscall(&v, DVAR_MVAR, syscall);
   dvar_define_field(id_, name, strlen(name), DVAR_CLASS_TSMETHOD,
                     visibility, is_static, &v);
}
