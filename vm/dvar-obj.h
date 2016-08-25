#ifndef DVAR_OBJ_H_
# define DVAR_OBJ_H_

# include "const.h"

struct dvar;
struct dvar_class;

struct dvar_obj
{
   struct dvar_class* c;
   struct dvar* data;
   void* extra;
   t_vm_int refs;
};

typedef struct dvar_obj dvar_obj;

dvar_obj* dvar_obj_new(t_vm_int id);
void dvar_obj_free(struct dvar* self);
dvar_obj* dvar_obj_copy(dvar_obj* o);
dvar_obj* dvar_obj_move(dvar_obj* o);

t_vm_bool dvar_obj_to_bool(const dvar_obj* o);
char* dvar_obj_to_string(const dvar_obj* o);
t_vm_bool dvar_obj_equals(const dvar_obj* a, const dvar_obj* b);
uint32_t dvar_obj_to_hash(const dvar_obj* o);

void dvar_obj_member(const struct dvar* self, const char* name,
                     struct dvar* dst);

void dvar_obj_get(const dvar_obj* o, const char* name, struct dvar* dst);
void dvar_obj_set(const dvar_obj* o, const char* name, const struct dvar* val);
void dvar_obj_call(const struct dvar* v, const char* name,
                   const struct dvar* begin, const struct dvar* end,
                   struct dvar* res);

t_vm_bool dvar_obj_has(const dvar_obj* o, const char* name);



#endif //!DVAR_OBJ_H_
