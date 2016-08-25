#include "dvar-arr.h"
#include "dvar.h"
#include <stdlib.h>
#include <string.h>
#include "dstr.h"
#include "hash.h"
#include "err.h"

#define DEFAULT_ALLOC 8

dvar_arr* dvar_arr_new(const dvar* begin, const dvar* end)
{
   dvar_arr* a = malloc(sizeof(dvar_arr));
   a->len = end - begin;
   a->alloc = DEFAULT_ALLOC;
   while(a->len > a->alloc)
      a->alloc *= 2;

   a->elems = malloc(a->alloc * sizeof(dvar));

   dvar* elem = a->elems;
   for(const dvar* it = begin; it != end; ++it, ++elem)
   {
      dvar_init(elem);
      dvar_copy(elem, it);
   }

   a->refs = 1;
   return a;
}

void dvar_arr_free(dvar_arr* a)
{
   if(--a->refs)
      return;

   dvar_bclear(a->elems, a->elems + a->len);
   free(a->elems);
   free(a);
}

dvar_arr* dvar_arr_copy(dvar_arr* a)
{
   ++a->refs;
   return a;
}

dvar_arr* dvar_arr_move(dvar_arr* a)
{
   return a;
}

t_vm_bool dvar_arr_to_bool(const dvar_arr* a)
{
   return a->len != 0;
}

char* dvar_arr_to_string(const dvar_arr* a)
{
   dstr ds;
   dstr_init(&ds);

   dstr_append(&ds, "[", 1);

   for(t_vm_int i = 0; i < a->len; ++i)
   {
      char* elem = dvar_to_str(a->elems + i);
      dstr_append(&ds, elem, strlen(elem));
      free(elem);
      if(i + 1 != a->len)
         dstr_append(&ds, ", ", 2);
   }

   dstr_append(&ds, "]", 1);
   return ds.s;
}

t_vm_bool dvar_arr_equals(const dvar_arr* a, const dvar_arr* b)
{
   return a == b;
}

uint32_t dvar_arr_to_hash(const dvar_arr* a)
{
   return hash_pointer(a);
}

void dvar_arr_at(dvar_arr* a, t_vm_int pos, struct dvar* dst)
{
   if(pos < 0 || pos >= a->len)
      err("Canot access element at %ld in array of size %ld",
          (long) pos, (long) a->len);

   dvar_putref(dst, a->elems + pos);
}

void dvar_arr_insert(dvar_arr* a, t_vm_int pos,
                     const struct dvar* begin, const struct dvar* end)
{
   if(pos < 0 || pos > a->len)
      err("Cannot insert elements at %ld in array of size %ld",
          (long) pos, (long) a->len);

   t_vm_int sub = end - begin;
   t_vm_int newLen = a->len + sub;
   t_vm_int newAlloc = a->alloc;
   while(newLen > newAlloc)
      newAlloc *= 2;

   if(newAlloc != a->alloc)
   {
      a->elems = realloc(a->elems, newAlloc * sizeof(dvar));
      a->alloc = newAlloc;
   }

   memmove(a->elems + pos + sub, a->elems + pos,
           (a->len - pos) * sizeof(dvar));

   dvar* elem = a->elems + pos;
   for(const dvar* it = begin; it != end; ++it, ++elem)
   {
      dvar_init(elem);
      dvar_copy(elem, it);
   }

   a->len = newLen;
}

void dvar_arr_remove(dvar_arr* a, t_vm_int pos, t_vm_int size)
{
   if(size < 1)
      return;

   if(pos < 0 || pos >= a->len || pos + size > a->len)
      err("Cannot remove %lu elements at %lu in array of size %lu",
          size, pos, a->len);

   dvar_bclear(a->elems + pos, a->elems + pos + size);

   memmove(a->elems + pos, a->elems + pos + size,
           (a->len - pos) * sizeof(dvar));

   a->len -= size;
}

void dvar_arr_clear(dvar_arr* a)
{
   dvar_arr_remove(a, 0, a->len);
}


struct dvar* c__arr__empty(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putbool(l, DVAR_MVAR, l->v_arr->len == 0);
   return l;
}

struct dvar* c__arr__size(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putint(l, DVAR_MVAR, l->v_arr->len);
   return l;
}

struct dvar* c__arr__push_front(struct dvar* l, t_vm_int n)
{
   dvar_arr_insert(l->v_arr, 0, l + 1, l + n);
   dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* c__arr__push_back(struct dvar* l, t_vm_int n)
{
   dvar_arr_insert(l->v_arr, l->v_arr->len, l + 1, l + n);
   dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* c__arr__insert(struct dvar* l, t_vm_int n)
{
   if(n == 1)
      err("array.insert: too few arguments");
   t_vm_int pos = dvar_get_int(l + 1, "array.insert: pos must be a number");

   dvar_arr_insert(l->v_arr, pos, l + 2, l + n);
   dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* c__arr__pop_front(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_arr* arr = l->v_arr;
   if(!arr->len)
      err("array.popFront: empty array");

   dvar temp;
   dvar_init(&temp);
   dvar_move(&temp, arr->elems);
   dvar_arr_remove(arr, 0, 1);
   dvar_move(l, &temp);
   return l;
}

struct dvar* c__arr__pop_back(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_arr* arr = l->v_arr;
   if(!arr->len)
      err("array.popBack: empty array");

   dvar temp;
   dvar_init(&temp);
   dvar_move(&temp, arr->elems + arr->len - 1);
   dvar_arr_remove(arr, arr->len - 1, 1);
   dvar_move(l, &temp);
   return l;
}

struct dvar* c__arr__remove(struct dvar* l, t_vm_int n)
{
   if(n < 3)
      err("array.remove: too few arguments");
   t_vm_int pos = dvar_get_int(l + 1, "array.remove: pos must be a number");
   t_vm_int size = dvar_get_int(l + 2, "array.remove: size must be a number");

   dvar_arr_remove(l->v_arr, pos, size);
   dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* c__arr__to_set(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putset(l, DVAR_MVAR, l->v_arr->elems, l->v_arr->elems + l->v_arr->len);
   return l;
}
