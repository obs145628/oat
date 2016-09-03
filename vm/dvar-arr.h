#ifndef DVAR_ARR_
# define DVAR_ARR_

# include "const.h"

struct dvar;

struct dvar_arr {
   t_vm_int len;
   t_vm_int alloc;
   t_vm_int refs;
   struct dvar *elems;
};
typedef struct dvar_arr dvar_arr;

dvar_arr* dvar_arr_new(const struct dvar* begin, const struct dvar* end);
void dvar_arr_free(dvar_arr* a);
dvar_arr* dvar_arr_copy(dvar_arr* a);
dvar_arr* dvar_arr_move(dvar_arr* a);
t_vm_bool dvar_arr_to_bool(const dvar_arr* a);
char* dvar_arr_to_string(const dvar_arr* a);
t_vm_bool dvar_arr_equals(const dvar_arr* a, const dvar_arr* b);
uint32_t dvar_arr_to_hash(const dvar_arr* a);

void dvar_arr_at(dvar_arr* a, t_vm_int pos, struct dvar* dst);

void dvar_arr_insert(dvar_arr* a, t_vm_int pos,
                     const struct dvar* begin, const struct dvar* end);
void dvar_arr_remove(dvar_arr* a, t_vm_int pos, t_vm_int size);
void dvar_arr_clear(dvar_arr* a);


struct dvar* c__arr__empty(struct dvar* l, t_vm_int n);
struct dvar* c__arr__size(struct dvar* l, t_vm_int n);
struct dvar* c__arr__push_front(struct dvar* l, t_vm_int n);
struct dvar* c__arr__push_back(struct dvar* l, t_vm_int n);
struct dvar* c__arr__insert(struct dvar* l, t_vm_int n);
struct dvar* c__arr__pop_front(struct dvar* l, t_vm_int n);
struct dvar* c__arr__pop_back(struct dvar* l, t_vm_int n);
struct dvar* c__arr__remove(struct dvar* l, t_vm_int n);
struct dvar* c__arr__to_set(struct dvar* l, t_vm_int n);
struct dvar* c__arr__index_of(struct dvar* l, t_vm_int n);
struct dvar* c__arr__last_index_of(struct dvar* l, t_vm_int n);
struct dvar* c__arr__contains(struct dvar* l, t_vm_int n);
struct dvar* c__arr__it(struct dvar* l, t_vm_int n);


void dvar_array_iterator_init();

struct dvar* c__array_iterator__constructor(struct dvar* l, t_vm_int n);
struct dvar* c__array_iterator__destructor(struct dvar* l, t_vm_int n);
struct dvar* c__array_iterator__is_end(struct dvar* l, t_vm_int n);
struct dvar* c__array_iterator__next(struct dvar* l, t_vm_int n);
struct dvar* c__array_iterator__get_key(struct dvar* l, t_vm_int n);
struct dvar* c__array_iterator__get_value(struct dvar* l, t_vm_int n);

#endif //!DVAR_ARR_
