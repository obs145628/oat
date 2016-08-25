#include "dvar-str.h"
#include "dvar.h"
#include <stdlib.h>
#include "err.h"
#include "numbers.h"


dvar_str* dvar_str_new(const char* str, t_vm_int len)
{
   dvar_str* s = malloc(sizeof(dvar_str));
   s->len = len;

   s->chars = malloc(len * sizeof(dvar));
   for(t_vm_int i = 0; i < len; ++i)
      dvar_init_char(s->chars + i, DVAR_MTCONST, str[i]);

   return s;
}

void dvar_str_free(dvar_str* s)
{
   dvar_bclear(s->chars, s->chars + s->len);
   free(s->chars);
   free(s);
}

dvar_str* dvar_str_copy(const dvar_str* s)
{
   dvar_str* copy = malloc(sizeof(dvar_str));
   copy->len = s->len;

   copy->chars = malloc(s->len * sizeof(dvar));
   for(t_vm_int i = 0; i < s->len; ++i)
      dvar_init_char(copy->chars + i, DVAR_MTCONST, s->chars[i].v_char);

   return copy;
}

dvar_str* dvar_str_move(dvar_str* s)
{
   return s;
}

t_vm_int dvar_str_cmp(const dvar_str* s1, const dvar_str* s2)
{
   t_vm_int c1 = s1->len;
   t_vm_int c2 = s2->len;

   for(t_vm_int i = 0; i < c1 && i < c2; ++i)
   {
      t_vm_int diff = (t_vm_int) (s1->chars[i].v_char - s2->chars[i].v_char);
      if(diff)
         return diff;
   }

   return c1 - c2;
}

t_vm_bool dvar_str_to_bool(const dvar_str* s)
{
   return (t_vm_bool) (s->len != 0);
}

char* dvar_str_to_string(const dvar_str* s)
{
   char* str = malloc(s->len + 1);
   for(t_vm_int i = 0; i < s->len; ++i)
      str[i] = s->chars[i].v_char;

   str[s->len] = '\0';
   return str;
}

uint32_t dvar_str_to_hash(const dvar_str* s)
{
   uint32_t hash = 5381;
   uint32_t c;

   for(t_vm_int i = 0; i < s->len; ++i)
   {
      c = (uint32_t) (s->chars[i].v_char);
      hash = ((hash << 5) + hash) + c;
   }

   return hash;
}

void dvar_str_concat(dvar_str* a, const dvar_str* b)
{
   t_vm_int len = a->len + b->len;
   a->chars = realloc(a->chars, len * sizeof(dvar));
   for(t_vm_int i = 0; i < b->len; ++i)
      dvar_init_char(a->chars + a->len + i, DVAR_MTCONST, b->chars[i].v_char);

   a->len = len;
}

dvar_str* dvar_str_sub(const dvar_str* s, t_vm_int begin, t_vm_int size)
{
   if(size == 0)
      return dvar_str_new("", 0);

   if(begin < 0 || begin >= s->len || size < 0 || begin > s->len)
      err("Unable to extract %ld charachters of string of size %ld"
          " starting at position %ld",
          size, s->len, begin);

   dvar_str* res = malloc(sizeof(dvar_str));
   res->len = size;

   res->chars = malloc(size * sizeof(dvar));
   for(t_vm_int i = 0; i < size; ++i)
      dvar_init_char(res->chars + i, DVAR_MTCONST, s->chars[begin+i].v_char);

   return res;
}



struct dvar* c__str__empty(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putbool(l, DVAR_MVAR, l->v_str->len == 0);
   return l;
}

struct dvar* c__str__size(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putint(l, DVAR_MVAR, l->v_str->len);
   return l;
}

struct dvar* c__str__substr(struct dvar* l, t_vm_int n)
{
   if(n < 2)
      err("string.substr: too few arguments");

   t_vm_int begin = dvar_get_int(l+1, "string.substr: begin must be a number");
   t_vm_int size;

   if(n > 2)
      size = dvar_get_int(l+2, "string.substr: size must be a number");
   else
      size = l->v_str->len - begin;

   dvar_str* s = dvar_str_sub(l->v_str, begin, size);
   dvar_str_free(l->v_str);
   l->v_str = s;
   return l;
}

struct dvar* c__str__is_int(struct dvar* l, t_vm_int n)
{
   (void) n;
   char* str = dvar_str_to_string(l->v_str);
   int ok;
   strToLong(str, &ok);

   dvar_putbool(l, DVAR_MVAR, (t_vm_bool) ok);
   free(str);
   return l;
}

struct dvar* c__str__to_int(struct dvar* l, t_vm_int n)
{
   (void) n;
   char* str = dvar_str_to_string(l->v_str);
   int ok;
   long res = strToLong(str, &ok);
   if(!ok)
      err("string.toInt: invalid string '%s'", str);

   dvar_putint(l, DVAR_MVAR, (t_vm_int) res);
   free(str);
   return l;
}

struct dvar* c__str__is_double(struct dvar* l, t_vm_int n)
{
   (void) n;
   char* str = dvar_str_to_string(l->v_str);
   int ok;
   strToDouble(str, &ok);

   dvar_putbool(l, DVAR_MVAR, (t_vm_bool) ok);
   free(str);
   return l;
}

struct dvar* c__str__to_double(struct dvar* l, t_vm_int n)
{
   (void) n;
   char* str = dvar_str_to_string(l->v_str);
   int ok;
   double res = strToDouble(str, &ok);
   if(!ok)
      err("string.toDouble: invalid string '%s'", str);

   dvar_putdouble(l, DVAR_MVAR, (t_vm_double) res);
   free(str);
   return l;
}
