#ifndef DVAR_CLASS_H_
# define DVAR_CLASS_H_

# include "const.h"
# include "pmap.h"

# define DVAR_CLASS_TVAR (1)
# define DVAR_CLASS_TCONST (2)
# define DVAR_CLASS_TCMETHOD (3)
# define DVAR_CLASS_TSMETHOD (4)

# define DVAR_CLASS_VPUBLIC (1)
# define DVAR_CLASS_VSPECIAL (8)

# define DVAR_CLASS_NOPARENT (-1)

struct dvar;

struct dvar_field
{
   char* name;
   t_vm_int type;
   t_vm_int visibility;
   t_vm_bool is_static;
   t_vm_int id;
   struct dvar* value;
};

typedef struct dvar_field dvar_field;

struct dvar_class
{
   struct dvar_class* parent;
   char* name;
   t_vm_int id;
   s_pmap* fields;
   struct dvar* model;
   t_vm_int model_len;
   int defined;
};

typedef struct dvar_class dvar_class;

dvar_class* dvar_class_new(t_vm_int id);
void dvar_class_free(dvar_class* c);
dvar_class* dvar_class_copy(dvar_class* c);
dvar_class* dvar_class_move(dvar_class* c);

t_vm_bool dvar_class_to_bool(const dvar_class* c);
char* dvar_class_to_string(const dvar_class* c);
t_vm_bool dvar_class_equals(const dvar_class* a, const dvar_class* b);
uint32_t dvar_class_to_hash(const dvar_class* c);

void dvar_class_member(const struct dvar* self, const char* name,
                       struct dvar* dst);

void dvar_define_init();

void dvar_define_class(const char* name, t_vm_int len, t_vm_int id,
   t_vm_int parent_id);

void dvar_define_field(t_vm_int class_id, const char* name, t_vm_int len,
                       t_vm_int type, t_vm_int visibility, t_vm_bool is_static,
                       const struct dvar* value);

void dvar_define_end(t_vm_int class_id);



# endif //!DVAR_CLASS_H_
