#include "dvar-obj.h"
#include "dvar.h"
#include "dvar-class.h"
#include "dvar-fun.h"
#include <stdlib.h>
#include <assert.h>
#include "str.h"
#include "hash.h"
#include "err.h"
#include "vm-syscall.h"


dvar_obj* dvar_obj_new(t_vm_int id)
{
   dvar_obj* o = malloc(sizeof(dvar_obj));
   o->c = dvar_class_new(id);
   o->refs = 1;

   t_vm_int len = o->c->model_len;
   dvar* model = o->c->model;

   o->data = malloc(len * sizeof(dvar));
   for(t_vm_int i = 0; i < len; ++i)
   {
      dvar *src = model + i;
      dvar* dst = o->data + i;
      dvar_init(dst);
      dvar_copy(dst, src);
      dst->mode = src->mode;
   }

   return o;
}

void dvar_obj_free(struct dvar* self)
{
   dvar_obj* o = self->v_obj;
   if(--o->refs)
      return;

   dvar fun;
   dvar_init(&fun);
   dvar_obj_get(o, "destructor", &fun);
   assert(fun.type == DVAR_TFUN);
   t_vm_int syscall = fun.v_fun->syscall;
   assert(syscall != VM_SYSCALL_NO);

   vm_syscall_exec(syscall, self, 1);


   dvar_bclear(o->data, o->data + o->c->model_len);
   free(o->data);
   free(o);
}

dvar_obj* dvar_obj_copy(dvar_obj* o)
{
   ++o->refs;
   return o;
}

dvar_obj* dvar_obj_move(dvar_obj* o)
{
   return o;
}

t_vm_bool dvar_obj_to_bool(const dvar_obj* o)
{
   (void) o;
   return TRUE;
}

char* dvar_obj_to_string(const dvar_obj* o)
{
   (void) o;
   return strClone("object");
}

t_vm_bool dvar_obj_equals(const dvar_obj* a, const dvar_obj* b)
{
   return a == b;
}

uint32_t dvar_obj_to_hash(const dvar_obj* o)
{
   return hash_pointer(o);
}

void dvar_obj_member(const dvar* self, const char* name, struct dvar* dst)
{
   dvar_obj* o = self->v_obj;
   dvar_class* c = o->c;

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

   if(field->is_static)
   {
      err("Field %s of Class %s is static", name, c->name);
   }

   t_vm_int id = field->id;

   if(field->type == DVAR_CLASS_TVAR || field->type == DVAR_CLASS_TCONST)
   {
      dvar_putref(dst, o->data + id);
   }

   else
   {
      dvar_copy(dst, o->data + id);
      dvar_fun_bind(dst->v_fun, self, self + 1);
   }
}

void dvar_obj_get(const dvar_obj* o, const char* name, struct dvar* dst)
{
   dvar_class* c = o->c;

   s_pmap_node* it= pmap_find(c->fields, (void*) name);
   assert(it != pmap_end(c->fields));

   dvar_field* f = it->value;
   assert(!f->is_static);

   dvar_copy(dst, o->data + f->id);
}

void dvar_obj_set(const dvar_obj* o, const char* name, const struct dvar* val)
{
   dvar_class* c = o->c;

   s_pmap_node* it= pmap_find(c->fields, (void*) name);
   assert(it != pmap_end(c->fields));

   dvar_field* f = it->value;
   assert(!f->is_static);

   dvar_copy(o->data + f->id, val);
}

void dvar_obj_call(const struct dvar* v, const char* name,
                   const struct dvar* begin, const struct dvar* end,
                   struct dvar* res)
{
   dvar_obj* o = v->v_obj;
   dvar_class* c = o->c;

   s_pmap_node* it= pmap_find(c->fields, (void*) name);
   assert(it != pmap_end(c->fields));

   dvar_field* f = it->value;
   assert(!f->is_static && f->type == DVAR_CLASS_TSMETHOD);

   dvar fun;
   dvar_init(&fun);
   dvar_copy(&fun, o->data + f->id);
   dvar_fun_bind(fun.v_fun, v, v+1);
   vm_syscall_exec_fn(fun.v_fun, begin, end, res);
   dvar_clear(&fun);
}

t_vm_bool dvar_obj_has(const dvar_obj* o, const char* name)
{
   dvar_class* c = o->c;
   s_pmap_node* it = pmap_find(c->fields, (void*) name);
   return it != pmap_end(c->fields);
}
