#include "dvar-str.h"
#include "dvar.h"
#include <stdlib.h>


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

t_vm_bool dvar_str_to_bool_(const dvar_str* s)
{
   return (t_vm_bool) (s->len != 0);
}

char* dvar_str_to_string_(const dvar_str* s)
{
   char* str = malloc(s->len + 1);
   for(t_vm_int i = 0; i < s->len; ++i)
      str[i] = s->chars[i].v_char;

   str[s->len] = '\0';
   return str;
}

void dvar_str_concat(dvar_str* a, const dvar_str* b)
{
   t_vm_int len = a->len + b->len;
   a->chars = realloc(a->chars, len * sizeof(dvar));
   for(t_vm_int i = 0; i < b->len; ++i)
      dvar_init_char(a->chars + a->len + i, DVAR_MTCONST, b->chars[i].v_char);

   a->len = len;
}
