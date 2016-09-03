#ifndef DVAR_SET_H_
# define DVAR_SET_H_

# include "const.h"
# include "phset.h"

struct dvar;

struct dvar_set {
   s_phset* hset;
   t_vm_int refs;
};
typedef struct dvar_set dvar_set;

dvar_set* dvar_set_new(const struct dvar* begin, const struct dvar* end);
void dvar_set_free(dvar_set* s);

dvar_set* dvar_set_copy(dvar_set* s);
dvar_set* dvar_set_move(dvar_set* s);
t_vm_bool dvar_set_to_bool(const dvar_set* s);
char* dvar_set_to_string(const dvar_set* s);
t_vm_bool dvar_set_equals(const dvar_set* a, const dvar_set* b);
uint32_t dvar_set_to_hash(const dvar_set* s);

t_vm_bool dvar_set_contains(const dvar_set* s, const struct dvar* v);
t_vm_bool dvar_set_empty(const dvar_set* s);
t_vm_int dvar_set_size(const dvar_set* s);

void dvar_set_insert(dvar_set* s,
                     const struct dvar* begin, const struct dvar* end);
void dvar_set_remove(dvar_set* s,
                     const struct dvar* begin, const struct dvar* end);
void dvar_set_clear(dvar_set* s);



struct dvar* c__set__empty(struct dvar* l, t_vm_int n);
struct dvar* c__set__size(struct dvar* l, t_vm_int n);
struct dvar* c__set__contains(struct dvar* l, t_vm_int n);
struct dvar* c__set__insert(struct dvar* l, t_vm_int n);
struct dvar* c__set__remove(struct dvar* l, t_vm_int n);
struct dvar* c__set__to_array(struct dvar* l, t_vm_int n);
struct dvar* c__set__it(struct dvar* l, t_vm_int n);


void dvar_set_iterator_init();

struct dvar* c__set_iterator__constructor(struct dvar* l, t_vm_int n);
struct dvar* c__set_iterator__destructor(struct dvar* l, t_vm_int n);
struct dvar* c__set_iterator__is_end(struct dvar* l, t_vm_int n);
struct dvar* c__set_iterator__next(struct dvar* l, t_vm_int n);
struct dvar* c__set_iterator__get_key(struct dvar* l, t_vm_int n);
struct dvar* c__set_iterator__get_value(struct dvar* l, t_vm_int n);

# endif //!DVAR_SET_H_
