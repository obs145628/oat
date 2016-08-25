#include "dvar-set.h"
#include "dvar.h"
#include <stdlib.h>
#include <string.h>
#include "dvar-arr.h"
#include "hash.h"
#include "dstr.h"
#include "err.h"

#define HSET_SIZE (512)

static int hset_cmp_(const dvar* a, const dvar* b)
{
   return !dvar_equals(a, b);
}

static void dvar_free_(dvar* v)
{
   dvar_clear(v);
   free(v);
}

dvar_set* dvar_set_new(const struct dvar* begin, const struct dvar* end)
{
   dvar_set* s = malloc(sizeof(dvar_set));
   s->hset = phset_new(HSET_SIZE, (f_phset_hash) dvar_to_hash,
                       (f_phset_cmp) hset_cmp_);
   s->refs = 1;
   dvar_set_insert(s, begin, end);
   return s;
}

void dvar_set_free(dvar_set* s)
{
   if(--s->refs)
      return;

   phset_foreach(s->hset, (f_phset_foreach) dvar_free_);
   phset_free(s->hset);
   free(s);
}

dvar_set* dvar_set_copy(dvar_set* s)
{
   ++s->refs;
   return s;
}

dvar_set* dvar_set_move(dvar_set* s)
{
   return s;
}

t_vm_bool dvar_set_to_bool(const dvar_set* s)
{
   return (t_vm_bool) (!phset_empty(s->hset));
}

char* dvar_set_to_string(const dvar_set* s)
{
   dstr ds;
   dstr_init(&ds);

   dstr_append(&ds, "{", 1);

   s_phset_node* it = phset_begin(s->hset);
   while(it != PHSET_END)
   {
      char* elem = dvar_to_str((dvar*) it->key);
      dstr_append(&ds, elem, strlen(elem));
      free(elem);
      s_phset_node* next = phset_next(s->hset, it);
      if(next != PHSET_END)
         dstr_append(&ds, ", ", 2);
      it = next;
   }

   dstr_append(&ds, "}", 1);
   return ds.s;
}

t_vm_bool dvar_set_equals(const dvar_set* a, const dvar_set* b)
{
   return a == b;
}

uint32_t dvar_set_to_hash(const dvar_set* s)
{
   return hash_pointer(s);
}

t_vm_bool dvar_set_contains(const dvar_set* s, const struct dvar* v)
{
   return phset_contains(s->hset, (void*) v);
}

t_vm_bool dvar_set_empty(const dvar_set* s)
{
   return phset_empty(s->hset);
}

t_vm_int dvar_set_size(const dvar_set* s)
{
   return phset_size(s->hset);
}


void dvar_set_insert(dvar_set* s,
                     const struct dvar* begin, const struct dvar* end)
{
   for(const dvar* it = begin; it != end; ++it)
   {
      dvar* v = malloc(sizeof(dvar));
      dvar_init(v);
      dvar_copy(v, it);
      phset_insert(s->hset, v);
   }
}

void dvar_set_remove(dvar_set* s,
                     const struct dvar* begin, const struct dvar* end)
{
      for(const dvar* it = begin; it != end; ++it)
      {
         s_phset_node* node = phset_find(s->hset, (void*) it);
         if(node != PHSET_END)
         {
            phset_remove(s->hset, (void*) it);
            dvar_free_((dvar*) node->key);
         }
      }
}

void dvar_set_clear(dvar_set* s)
{
   phset_foreach(s->hset, (f_phset_foreach) dvar_free_);
   phset_clear(s->hset);
}



struct dvar* c__set__empty(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putbool(l, DVAR_MVAR, l->v_set->hset->count == 0);
   return l;
}

struct dvar* c__set__size(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putint(l, DVAR_MVAR, (t_vm_int) (l->v_set->hset->count));
   return l;
}

struct dvar* c__set__contains(struct dvar* l, t_vm_int n)
{
   if(n == 1)
      err("set.contains: too few arguments");
   dvar_putbool(l, DVAR_MVAR, dvar_set_contains(l->v_set, l + 1));
   return l;
}

struct dvar* c__set__insert(struct dvar* l, t_vm_int n)
{
   dvar_set_insert(l->v_set, l + 1, l + n);
   dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* c__set__remove(struct dvar* l, t_vm_int n)
{
   dvar_set_remove(l->v_set, l + 1, l + n);
   dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* c__set__to_array(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar temp;
   dvar_init_arr(&temp, DVAR_MVAR, NULL, NULL);

   s_phset* set = l->v_set->hset;
   for(s_phset_node* it = phset_begin(set); it; it = phset_next(set, it))
   {
      dvar* key = it->key;
      dvar_arr_insert(temp.v_arr, temp.v_arr->len, key, key + 1);
   }

   dvar_move(l, &temp);
   return l;
}
