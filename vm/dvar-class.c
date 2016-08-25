#include "dvar-class.h"
#include "dvar.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "str.h"
#include "hash.h"
#include "err.h"
#include "dvar-fun.h"

#define MAX_CLASSES 2048

static dvar_class* classes_list_[MAX_CLASSES];

static int field_is_val_(const dvar_field* f)
{
   return f->type == DVAR_CLASS_TVAR || f->type == DVAR_CLASS_TCONST;
}


dvar_class* dvar_class_new(t_vm_int id)
{
   assert(id >= 0 && id < MAX_CLASSES);
   dvar_class* c = classes_list_[id];
   assert(c && c->defined);
   return c;
}

void dvar_class_free(dvar_class* c)
{
   (void) c;
}

dvar_class* dvar_class_copy(dvar_class* c)
{
   return c;
}

dvar_class* dvar_class_move(dvar_class* c)
{
   return c;
}

t_vm_bool dvar_class_to_bool(const dvar_class* c)
{
   (void) c;
   return TRUE;
}

char* dvar_class_to_string(const dvar_class* c)
{
   return strConcat("Class ", c->name);
}

t_vm_bool dvar_class_equals(const dvar_class* a, const dvar_class* b)
{
   return a->id == b->id;
}

uint32_t dvar_class_to_hash(const dvar_class* c)
{
   return hash_int(c->id);
}

void dvar_class_member(const struct dvar* self, const char* name,
                       struct dvar* dst)
{
   (void) self;
   dvar_class* c = self->v_class;
   s_pmap_node* it = pmap_find(c->fields, (void *) name);

   if(it == pmap_end(c->fields))
   {
      err("Class %s has no field named %s", c->name, name);
   }

   dvar_field* field = it->value;

   if(field->visibility == DVAR_CLASS_VSPECIAL)
   {
      err("Field %s of Class %s isn't accessible", name, c->name);
   }

   if(!field->is_static)
   {
      err("Field %s of Class %s isn't static", name, c->name);
   }

   if(field_is_val_(field))
   {
      dvar_putref(dst, field->value);
   }

   else
   {
      dvar_copy(dst, field->value);
   }
}

void dvar_define_init()
{
   for(size_t i = 0; i < MAX_CLASSES; ++i)
      classes_list_[i] = NULL;
}


void dvar_define_class(const char* name, t_vm_int len, t_vm_int id)
{
   assert(id >= 0 && id < MAX_CLASSES);
   assert(!classes_list_[id]);
   dvar_class* c = malloc(sizeof(dvar_class));
   c->name = strCloneN(name, len);
   c->defined = 0;
   c->id = id;
   c->fields = pmap_new((f_pmap_cmp) strcmp);
   classes_list_[id] = c;
}

void dvar_define_field(t_vm_int class_id, const char* name, t_vm_int len,
                       t_vm_int type, t_vm_int visibility, t_vm_bool is_static,
                       const struct dvar* value)
{
   assert(class_id >= 0 && class_id < MAX_CLASSES);
   dvar_class* c = classes_list_[class_id];
   assert(c && !c->defined);

   dvar_field* f = malloc(sizeof(dvar_field));
   f->name = strCloneN(name, len);
   f->type = type;
   f->visibility = visibility;
   f->is_static = is_static;
   f->value = malloc(sizeof(dvar));
   dvar_init(f->value);
   dvar_copy(f->value, value);
   if(f->type == DVAR_CLASS_TCONST)
      f->value->mode = DVAR_MCONST;

   pmap_insert(c->fields, f->name, f);
}

static void add_default_(t_vm_int id, const char* name, t_vm_int syscall)
{
   dvar_class* c = classes_list_[id];
   if(pmap_find(c->fields, (void*) name) != pmap_end(c->fields))
      return;

   dvar v;
   dvar_init_syscall(&v, DVAR_MVAR, syscall);
   dvar_define_field(id, name, strlen(name), DVAR_CLASS_TSMETHOD,
                     DVAR_CLASS_VPUBLIC, FALSE, &v);
}

void dvar_define_end(t_vm_int class_id)
{
   assert(class_id >= 0 && class_id < MAX_CLASSES);
   dvar_class* c = classes_list_[class_id];
   assert(c && !c->defined);

   add_default_(class_id, "constructor", VM_SYSCALL_DEFAULT_CONSTRUCTOR);
   add_default_(class_id, "destructor", VM_SYSCALL_DEFAULT_DESTRUCTOR);

   c->defined = 1;
   c->model = malloc(sizeof(dvar) * pmap_size(c->fields));

   t_vm_int len = 0;
   s_pmap_node* it = pmap_begin(c->fields);
   while(it != pmap_end(c->fields))
   {
      dvar_field* f = it->value;
      if(!f->is_static)
      {
         f->id = len++;
         dvar* v = c->model + f->id;
         dvar_init(v);
         dvar_copy(v, f->value);
         if(f->type == DVAR_CLASS_TCONST)
         {
            v->mode = DVAR_MCONST;
         }
      }

      it = pmap_next(c->fields, it);
   }

   c->model_len = len;
}
