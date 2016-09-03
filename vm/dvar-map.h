#ifndef DVAR_MAP_H_
# define DVAR_MAP_H_

#include "const.h"
#include "phtable.h"

struct dvar;

struct dvar_map {
   s_phtable* hmap;
   t_vm_int refs;
   struct dvar* temp;
};
typedef struct dvar_map dvar_map;

dvar_map* dvar_map_new(const struct dvar* begin, const struct dvar* end);
void dvar_map_free(dvar_map* m);

dvar_map* dvar_map_copy(dvar_map* m);
dvar_map* dvar_map_move(dvar_map* m);
t_vm_bool dvar_map_to_bool(const dvar_map* m);
char* dvar_map_to_string(const dvar_map* m);
t_vm_bool dvar_map_equals(const dvar_map* a, const dvar_map* b);
uint32_t dvar_map_to_hash(const dvar_map* m);

t_vm_bool dvar_map_contains(const dvar_map* m, const struct dvar* key);
t_vm_bool dvar_map_empty(const dvar_map* m);
t_vm_int dvar_map_size(const dvar_map* m);

void dvar_map_at(const struct dvar* self, const struct dvar* key,
                 struct dvar* dst);

void dvar_map_insert(dvar_map* m,
                     const struct dvar* begin, const struct dvar* end);
void dvar_map_remove(dvar_map* m,
                     const struct dvar* begin, const struct dvar* end);
void dvar_map_clear(dvar_map* m);


struct dvar* c__map__empty(struct dvar* l, t_vm_int n);
struct dvar* c__map__size(struct dvar* l, t_vm_int n);
struct dvar* c__map__contains(struct dvar* l, t_vm_int n);
struct dvar* c__map__remove(struct dvar* l, t_vm_int n);
struct dvar* c__map__keys(struct dvar* l, t_vm_int n);
struct dvar* c__map__values(struct dvar* l, t_vm_int n);
struct dvar* c__map__it(struct dvar* l, t_vm_int n);


void dvar_map_iterator_init();

struct dvar* c__map_iterator__constructor(struct dvar* l, t_vm_int n);
struct dvar* c__map_iterator__destructor(struct dvar* l, t_vm_int n);
struct dvar* c__map_iterator__is_end(struct dvar* l, t_vm_int n);
struct dvar* c__map_iterator__next(struct dvar* l, t_vm_int n);
struct dvar* c__map_iterator__get_key(struct dvar* l, t_vm_int n);
struct dvar* c__map_iterator__get_value(struct dvar* l, t_vm_int n);


#endif //!DVAR_MAP_H_
