#include "dvar-map.h"
#include "dvar.h"
#include "dvar-fun.h"
#include "dvar-arr.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash.h"
#include "dstr.h"
#include "err.h"

#define HMAP_SIZE (512)

static int dvar_cmp_(const dvar* a, const dvar* b)
{
   return ! dvar_equals(a, b);
}

static void dvar_free_(dvar* key, dvar* value)
{
   dvar_clear(key);
   dvar_clear(value);
   free(key);
   free(value);
}

dvar_map* dvar_map_new(const struct dvar* begin, const struct dvar* end)
{
   dvar_map* m = malloc(sizeof(dvar_map));
   m->hmap = phtable_new(HMAP_SIZE, (f_phtable_hash) dvar_to_hash,
                         (f_phtable_cmp) dvar_cmp_);
   m->refs = 1;
   m->temp = malloc(sizeof(dvar));
   dvar_init(m->temp);

   dvar_map_insert(m, begin, end);
   return m;
}

void dvar_map_free(dvar_map* m)
{
   if(--m->refs)
      return;

   phtable_foreach(m->hmap, (f_phtable_foreach) dvar_free_);
   phtable_free(m->hmap);
   dvar_clear(m->temp);
   free(m->temp);
}

dvar_map* dvar_map_copy(dvar_map* m)
{
   ++m->refs;
   return m;
}

dvar_map* dvar_map_move(dvar_map* m)
{
   return m;
}

t_vm_bool dvar_map_to_bool(const dvar_map* m)
{
   return !phtable_empty(m->hmap);
}

char* dvar_map_to_string(const dvar_map* m)
{
   dstr ds;
   dstr_init(&ds);

   dstr_append(&ds, "{", 1);

   s_phtable_node* it = phtable_begin(m->hmap);
   while(it != PHTABLE_END)
   {
      char* key = dvar_to_str((dvar*) it->key);
      char* value = dvar_to_str((dvar*) it->value);

      dstr_append(&ds, key, strlen(key));
      dstr_append(&ds, ": ", 2);
      dstr_append(&ds, value, strlen(value));
      free(key);
      free(value);

      s_phtable_node* next = phtable_next(m->hmap, it);
      if(next != PHTABLE_END)
         dstr_append(&ds, ", ", 2);
      it = next;
   }

   dstr_append(&ds, "}", 1);
   return ds.s;
}

t_vm_bool dvar_map_equals(const dvar_map* a, const dvar_map* b)
{
   return a == b;
}

uint32_t dvar_map_to_hash(const dvar_map* m)
{
   return hash_pointer(m);
}

t_vm_bool dvar_map_contains(const dvar_map* m, const struct dvar* key)
{
   return phtable_contains(m->hmap, (void*) key);
}

t_vm_bool dvar_map_empty(const dvar_map* m)
{
   return phtable_empty(m->hmap);
}

t_vm_int dvar_map_size(const dvar_map* m)
{
   return phtable_size(m->hmap);
}

void dvar_map_at(const struct dvar* self, const struct dvar* key,
                 struct dvar* dst)
{
   dvar_map* m = self->v_map;
   s_phtable_node* it = phtable_find(m->hmap, (void*) key);

   if(it != PHTABLE_END)
   {
      dvar* v = (dvar*) it->value;
      dvar_putref(dst, v);
   }

   else
   {
      m->temp = malloc(sizeof(dvar));
      dvar_init_null(m->temp, DVAR_MVAR);
      m->temp->listener = dvar_fun_new_syscall(VM_SYSCALL_MAP_SUB_LIST);
      dvar_fun_bind(m->temp->listener, self, self + 1);
      dvar_fun_bind(m->temp->listener, key, key + 1);
      dvar_putref(dst, m->temp);
   }
}

void dvar_map_insert(dvar_map* m,
                     const struct dvar* begin, const struct dvar* end)
{

   assert((end - begin) % 2 == 0);

   for(const dvar* it = begin; it != end; it += 2)
   {
      dvar* value = malloc(sizeof(dvar));
      dvar_init(value);
      dvar_copy(value, it + 1);

      s_phtable_node* node = phtable_find(m->hmap, (void*) it);

      if(node != PHTABLE_END)
      {
         dvar* old = (dvar*) node->value;
         dvar_clear(old);
         free(old);
         node->value = value;
      }

      else
      {
         dvar* key = malloc(sizeof(dvar));
         dvar_init(key);
         dvar_copy(key, it);

         phtable_insert(m->hmap, key, value);
      }
   }
}

void dvar_map_remove(dvar_map* m,
                     const struct dvar* begin, const struct dvar* end)
{
   for(const dvar* it = begin; it != end; ++it)
   {
      s_phtable_node* node = phtable_find(m->hmap, (void*) it);
      if(node != PHTABLE_END)
      {
         phtable_remove(m->hmap, (void*) it);
         dvar_free_(node->key, node->value);
      }
   }
}

void dvar_map_clear(dvar_map* m)
{
   phtable_foreach(m->hmap, (f_phtable_foreach) dvar_free_);
   phtable_clear(m->hmap);
}



struct dvar* c__map__empty(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putbool(l, DVAR_MVAR, (t_vm_bool) (l->v_map->hmap->count == 0));
   return l;
}

struct dvar* c__map__size(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putint(l, DVAR_MVAR, (t_vm_int) (l->v_map->hmap->count));
   return l;
}

struct dvar* c__map__contains(struct dvar* l, t_vm_int n)
{
   if(n == 1)
      err("map.contains: too feww arguments");

   dvar_putbool(l, DVAR_MVAR, dvar_map_contains(l->v_map, l + 1));
   return l;
}

struct dvar* c__map__remove(struct dvar* l, t_vm_int n)
{
   dvar_map_remove(l->v_map, l + 1, l + n);
   return l;
}

struct dvar* c__map__keys(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar temp;
   dvar_init_arr(&temp, DVAR_MVAR, NULL, NULL);

   s_phtable* map = l->v_map->hmap;
   for(s_phtable_node* it = phtable_begin(map); it; it = phtable_next(map, it))
   {
      dvar* key = it->key;
      dvar_arr_insert(temp.v_arr, temp.v_arr->len, key, key + 1);
   }

   dvar_move(l, &temp);
   return l;
}

struct dvar* c__map__values(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar temp;
   dvar_init_arr(&temp, DVAR_MVAR, NULL, NULL);

   s_phtable* map = l->v_map->hmap;
   for(s_phtable_node* it = phtable_begin(map); it; it = phtable_next(map, it))
   {
      dvar* val = it->value;
      dvar_arr_insert(temp.v_arr, temp.v_arr->len, val, val + 1);
   }

   dvar_move(l, &temp);
   return l;
}
