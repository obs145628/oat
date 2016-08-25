#include "vm-classes.h"
#include <string.h>
#include "dvar.h"
#include "dvar-class.h"
#include "dvar-obj.h"
#include "err.h"
#include "dvar-date.h"



static void def_begin_(const char* name, t_vm_int id, t_vm_int parent)
{
   dvar_define_class(name, strlen(name), id, parent);
}

static void def_end_(t_vm_int id)
{
   dvar_define_end(id);
}

static void def_field_(t_vm_int class_id, const char* name, t_vm_int type,
                       t_vm_int visibility, t_vm_bool is_static,
                       const dvar* value)
{
   dvar_define_field(class_id, name, strlen(name), type, visibility,
                     is_static, value);
}

static void def_method_(t_vm_int class_id, const char* name,
                        t_vm_int visibility, t_vm_bool is_static,
                        t_vm_int syscall)
{
   dvar v;
   dvar_init_syscall(&v, DVAR_MVAR, syscall);
   dvar_define_field(class_id, name, strlen(name), DVAR_CLASS_TSMETHOD,
                     visibility, is_static, &v);
}



static void define_rect_()
{
   dvar zero;
   dvar_init_int(&zero, DVAR_CLASS_TVAR, 0);
   def_begin_("Rect", VM_CLASS_RECT, DVAR_CLASS_NOPARENT);

   def_field_(VM_CLASS_RECT, "_x",
              DVAR_CLASS_TVAR, DVAR_CLASS_VSPECIAL, FALSE, &zero);
   def_field_(VM_CLASS_RECT, "_y",
              DVAR_CLASS_TVAR, DVAR_CLASS_VSPECIAL, FALSE, &zero);
   def_field_(VM_CLASS_RECT, "_w",
              DVAR_CLASS_TVAR, DVAR_CLASS_VSPECIAL, FALSE, &zero);
   def_field_(VM_CLASS_RECT, "_h",
              DVAR_CLASS_TVAR, DVAR_CLASS_VSPECIAL, FALSE, &zero);


   def_method_(VM_CLASS_RECT, "constructor", DVAR_CLASS_VPUBLIC, FALSE,
               VM_SYSCALL_RECT_CONSTRUCTOR);
   def_method_(VM_CLASS_RECT, "getX", DVAR_CLASS_VPUBLIC, FALSE,
               VM_SYSCALL_RECT_GET_X);
   def_method_(VM_CLASS_RECT, "getY", DVAR_CLASS_VPUBLIC, FALSE,
               VM_SYSCALL_RECT_GET_Y);
   def_method_(VM_CLASS_RECT, "getWidth", DVAR_CLASS_VPUBLIC, FALSE,
               VM_SYSCALL_RECT_GET_WIDTH);
   def_method_(VM_CLASS_RECT, "getHeight", DVAR_CLASS_VPUBLIC, FALSE,
               VM_SYSCALL_RECT_GET_HEIGHT);
   def_method_(VM_CLASS_RECT, "setX", DVAR_CLASS_VPUBLIC, FALSE,
               VM_SYSCALL_RECT_SET_X);
   def_method_(VM_CLASS_RECT, "setY", DVAR_CLASS_VPUBLIC, FALSE,
               VM_SYSCALL_RECT_SET_Y);
   def_method_(VM_CLASS_RECT, "setWidth", DVAR_CLASS_VPUBLIC, FALSE,
               VM_SYSCALL_RECT_SET_WIDTH);
   def_method_(VM_CLASS_RECT, "setHeight", DVAR_CLASS_VPUBLIC, FALSE,
               VM_SYSCALL_RECT_SET_HEIGHT);

   def_end_(VM_CLASS_RECT);
}

struct dvar* c__rect__constructor(struct dvar* l, t_vm_int n)
{
   if(n != 5)
      err("Rect constructor expected 4 arguments, got %d", (int) (n-1));

   if(!dvar_cast_to(l + 1, DVAR_TDOUBLE))
      err("Rect constructor: Invalid argument 1 type");
   if(!dvar_cast_to(l + 2, DVAR_TDOUBLE))
      err("Rect constructor: Invalid argument 2 type");
   if(!dvar_cast_to(l + 3, DVAR_TDOUBLE))
      err("Rect constructor: Invalid argument 3 type");
   if(!dvar_cast_to(l + 4, DVAR_TDOUBLE))
      err("Rect constructor: Invalid argument 4 type");

   dvar_obj_val* o = l->v_obj->val;

   dvar_obj_set(o, "_x", l + 1);
   dvar_obj_set(o, "_y", l + 2);
   dvar_obj_set(o, "_w", l + 3);
   dvar_obj_set(o, "_h", l + 4);

   return l;
}

struct dvar* c__rect__get_x(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_obj_get(l->v_obj->val, "_x", l);
   return l;
}

struct dvar* c__rect__get_y(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_obj_get(l->v_obj->val, "_y", l);
   return l;
}

struct dvar* c__rect__get_width(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_obj_get(l->v_obj->val, "_w", l);
   return l;
}

struct dvar* c__rect__get_height(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_obj_get(l->v_obj->val, "_h", l);
   return l;
}

struct dvar* c__rect__set_x(struct dvar* l, t_vm_int n)
{
   if(n == 1)
      err("Rect.setX: missing argument");

   if(!dvar_cast_to(l + 1, DVAR_TDOUBLE))
      err("Rect.setX: invalid argument type");

   dvar_obj_set(l->v_obj->val, "_x", l + 1);

   dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* c__rect__set_y(struct dvar* l, t_vm_int n)
{
   if(n == 1)
      err("Rect.setY: missing argument");

   if(!dvar_cast_to(l + 1, DVAR_TDOUBLE))
      err("Rect.setY: invalid argument type");

   dvar_obj_set(l->v_obj->val, "_y", l + 1);

   dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* c__rect__set_width(struct dvar* l, t_vm_int n)
{
   if(n == 1)
      err("Rect.setWidth: missing argument");

   if(!dvar_cast_to(l + 1, DVAR_TDOUBLE))
      err("Rect.setWidth: invalid argument type");

   dvar_obj_set(l->v_obj->val, "_w", l + 1);

   dvar_putnull(l, DVAR_MVAR);
   return l;
}

struct dvar* c__rect__set_height(struct dvar* l, t_vm_int n)
{
   if(n == 1)
      err("Rect.setHeight: missing argument");

   if(!dvar_cast_to(l + 1, DVAR_TDOUBLE))
      err("Rect.setHeight: invalid argument type");

   dvar_obj_set(l->v_obj->val, "_h", l + 1);

   dvar_putnull(l, DVAR_MVAR);
   return l;
}







void vm_classes_init()
{
   dvar_type_init_null();
   dvar_type_init_int();
   dvar_type_init_double();
   dvar_type_init_char();
   dvar_type_init_bool();
   dvar_type_init_str();
   dvar_type_init_fun();
   dvar_type_init_arr();
   dvar_type_init_set();
   dvar_type_init_map();

   dvar_define_init();
   define_rect_();

   dvar_date_init();
}
