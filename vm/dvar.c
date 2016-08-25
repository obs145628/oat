#include "dvar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dvar-fun.h"
#include "dvar-str.h"
#include "dvar-arr.h"
#include "dvar-set.h"
#include "dvar-map.h"
#include "dvar-obj.h"
#include "dvar-class.h"
#include "str.h"
#include "numbers.h"
#include "err.h"
#include "vm-syscall.h"
#include "hash.h"
#include "pmap.h"


static void resolve_ref_(const dvar** v)
{
   if((*v)->type == DVAR_TREF)
      *v = (*v)->v_ref;
}



static const char* type_to_cstr_(t_vm_type t)
{
   static const char* TYPES[] = {
      "null",
      "int",
      "double",
      "char",
      "bool",
      "string",
      "function",
      "array",
      "set",
      "map",
      "object",
      "class"
   };
   assert(t >= 0 && t < DVAR_NB_TYPES);
   return TYPES[t];
}

static void reset_content_(dvar *v)
{
   v->type = DVAR_TNULL;
}

static void reset_mode_(dvar* v)
{
   v->mode = DVAR_MVAR;
   if(v->listener)
      dvar_fun_free(v->listener);
   v->listener = NULL;
}

static void free_str_(dvar* v)
{
   dvar_str_free(v->v_str);
}

static void free_fun_(dvar* v)
{
   dvar_fun_free(v->v_fun);
}

static void free_arr_(dvar* v)
{
   dvar_arr_free(v->v_arr);
}

static void free_set_(dvar* v)
{
   dvar_set_free(v->v_set);
}

static void free_map_(dvar* v)
{
   dvar_map_free(v->v_map);
}

static void free_obj_(dvar* v)
{
   dvar_obj_free(v);
}

static void free_class_(dvar* v)
{
   dvar_class_free(v->v_class);
}

static void free_content_(dvar *v)
{
   static const f_dvar_a1 FNS[] = {
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      free_str_,
      free_fun_,
      free_arr_,
      free_set_,
      free_map_,
      free_obj_,
      free_class_
   };

   if(FNS[v->type])
      FNS[v->type](v);

   reset_content_(v);
}

static void reset_(dvar* v)
{
   reset_content_(v);
   reset_mode_(v);
}

typedef t_vm_bool (*f_to_bool_)(const dvar* v);

static t_vm_bool to_bool_null_(const dvar* v)
{
   (void) v;
   return FALSE;
}

static t_vm_bool to_bool_int_(const dvar* v)
{
   return (t_vm_bool) (!!v->v_int);
}

static t_vm_bool to_bool_double_(const dvar* v)
{
   return (t_vm_bool) (!!v->v_double);
}

static t_vm_bool to_bool_char_(const dvar* v)
{
   return (t_vm_bool) (!!v->v_char);
}

static t_vm_bool to_bool_bool_(const dvar* v)
{
   return v->v_bool;
}

static t_vm_bool to_bool_str_(const dvar* v)
{
   return dvar_str_to_bool(v->v_str);
}

static t_vm_bool to_bool_fun_(const dvar* v)
{
   return dvar_fun_to_bool(v->v_fun);
}

static t_vm_bool to_bool_arr_(const dvar* v)
{
   return dvar_arr_to_bool(v->v_arr);
}

static t_vm_bool to_bool_set_(const dvar* v)
{
   return dvar_set_to_bool(v->v_set);
}

static t_vm_bool to_bool_map_(const dvar* v)
{
   return dvar_map_to_bool(v->v_map);
}

static t_vm_bool to_bool_obj_(const dvar* v)
{
   return dvar_obj_to_bool(v->v_obj);
}

static t_vm_bool to_bool_class_(const dvar* v)
{
   return dvar_class_to_bool(v->v_class);
}

static t_vm_bool to_bool_(const dvar* v)
{
   static const f_to_bool_ FNS[] = {
      to_bool_null_,
      to_bool_int_,
      to_bool_double_,
      to_bool_char_,
      to_bool_bool_,
      to_bool_str_,
      to_bool_fun_,
      to_bool_arr_,
      to_bool_set_,
      to_bool_map_,
      to_bool_obj_,
      to_bool_class_
   };
   return FNS[v->type](v);
}

typedef char* (*f_to_str_)(const dvar* v);

static char* to_str_null_(const dvar* v)
{
   (void) v;
   return strClone("null");
}

static char* to_str_int_(const dvar* v)
{
   return intToStr((long) v->v_int);
}

static char* to_str_double_(const dvar* v)
{
   return floatToStr((double) v->v_double);
}

static char* to_str_char_(const dvar* v)
{
   char* str = malloc(2);
   str[0] = (char) v->v_char;
   str[1] = '\0';
   return str;
}

static char* to_str_bool_(const dvar* v)
{
   return strClone(v->v_bool ? "true" : "false");
}

static char* to_str_str_(const dvar* v)
{
   return dvar_str_to_string(v->v_str);
}

static char* to_str_fun_(const dvar* v)
{
   return dvar_fun_to_string(v->v_fun);
}

static char* to_str_arr_(const dvar* v)
{
   return dvar_arr_to_string(v->v_arr);
}

static char* to_str_set_(const dvar* v)
{
   return dvar_set_to_string(v->v_set);
}

static char* to_str_map_(const dvar* v)
{
   return dvar_map_to_string(v->v_map);
}

static char* to_str_obj_(const dvar* v)
{
   return dvar_obj_to_string(v->v_obj);
}

static char* to_str_class_(const dvar* v)
{
   return dvar_class_to_string(v->v_class);
}

static char* to_str_(const dvar* v)
{
   static const f_to_str_ FNS[] = {
      to_str_null_,
      to_str_int_,
      to_str_double_,
      to_str_char_,
      to_str_bool_,
      to_str_str_,
      to_str_fun_,
      to_str_arr_,
      to_str_set_,
      to_str_map_,
      to_str_obj_,
      to_str_class_
   };
   return FNS[v->type](v);
}

typedef t_vm_bool (*f_equals_)(const dvar* a, const dvar* b);

static t_vm_bool equals_null_(const dvar* a, const dvar* b)
{
   (void) a;
   (void) b;
   return TRUE;
}

static t_vm_bool equals_int_(const dvar* a, const dvar* b)
{
   return a->v_int == b->v_int;
}

static t_vm_bool equals_double_(const dvar* a, const dvar* b)
{
   return a->v_double == b->v_double;
}

static t_vm_bool equals_char_(const dvar* a, const dvar* b)
{
   return a->v_char == b->v_char;
}

static t_vm_bool equals_bool_(const dvar* a, const dvar* b)
{
   return a->v_bool == b->v_bool;
}

static t_vm_bool equals_str_(const dvar* a, const dvar* b)
{
   return dvar_str_cmp(a->v_str, b->v_str) == 0;
}

static t_vm_bool equals_fun_(const dvar* a, const dvar* b)
{
   return dvar_fun_equals(a->v_fun, b->v_fun);
}

static t_vm_bool equals_arr_(const dvar* a, const dvar* b)
{
   return dvar_arr_equals(a->v_arr, b->v_arr);
}

static t_vm_bool equals_set_(const dvar* a, const dvar* b)
{
   return dvar_set_equals(a->v_set, b->v_set);
}

static t_vm_bool equals_map_(const dvar* a, const dvar* b)
{
   return dvar_map_equals(a->v_map, b->v_map);
}

static t_vm_bool equals_obj_(const dvar* a, const dvar* b)
{
   return dvar_obj_equals(a->v_obj, b->v_obj);
}

static t_vm_bool equals_class_(const dvar* a, const dvar* b)
{
   return dvar_class_equals(a->v_class, b->v_class);
}

static t_vm_bool equals_(const dvar* a, const dvar* b)
{
   static const f_equals_ FNS[] = {
      equals_null_,
      equals_int_,
      equals_double_,
      equals_char_,
      equals_bool_,
      equals_str_,
      equals_fun_,
      equals_arr_,
      equals_set_,
      equals_map_,
      equals_obj_,
      equals_class_
   };

   return a->type == b->type && FNS[a->type](a, b);
}

typedef uint32_t (*f_to_hash_)(const dvar* v);

static uint32_t to_hash_null_(const dvar* v)
{
   (void) v;
   return hash_int(0);
}

static uint32_t to_hash_int_(const dvar* v)
{
   return hash_int((uint32_t) v->v_int);
}

static uint32_t to_hash_double_(const dvar* v)
{
   return hash_float((double) v->v_double);
}

static uint32_t to_hash_char_(const dvar* v)
{
   return hash_int((uint32_t) v->v_char);
}

static uint32_t to_hash_bool_(const dvar* v)
{
   return hash_int((uint32_t) v->v_bool);
}

static uint32_t to_hash_str_(const dvar* v)
{
   return dvar_str_to_hash(v->v_str);
}

static uint32_t to_hash_fun_(const dvar* v)
{
   return dvar_fun_to_hash(v->v_fun);
}

static uint32_t to_hash_arr_(const dvar* v)
{
   return dvar_arr_to_hash(v->v_arr);
}

static uint32_t to_hash_set_(const dvar* v)
{
   return dvar_set_to_hash(v->v_set);
}

static uint32_t to_hash_map_(const dvar* v)
{
   return dvar_map_to_hash(v->v_map);
}

static uint32_t to_hash_obj_(const dvar* v)
{
   return dvar_obj_to_hash(v->v_obj);
}

static uint32_t to_hash_class_(const dvar* v)
{
   return dvar_class_to_hash(v->v_class);
}

static uint32_t to_hash_(const dvar* v)
{
   static const f_to_hash_ FNS[] = {
      to_hash_null_,
      to_hash_int_,
      to_hash_double_,
      to_hash_char_,
      to_hash_bool_,
      to_hash_str_,
      to_hash_fun_,
      to_hash_arr_,
      to_hash_set_,
      to_hash_map_,
      to_hash_obj_,
      to_hash_class_
   };

   return FNS[v->type](v);
}


static void set_null_(dvar* v)
{
   v->type = DVAR_TNULL;
}

static void set_int_(dvar* v, t_vm_int x)
{
   v->type = DVAR_TINT;
   v->v_int = x;
}

static void set_double_(dvar* v, t_vm_double x)
{
   v->type = DVAR_TDOUBLE;
   v->v_double = x;
}

static void set_char_(dvar* v, t_vm_char x)
{
   v->type = DVAR_TCHAR;
   v->v_char = x;
}

static void set_bool_(dvar* v, t_vm_bool x)
{
   assert(x == FALSE || x == TRUE);
   v->type = DVAR_TBOOL;
   v->v_bool = x;
}

static void set_str_(dvar* v, const dvar_str* str)
{
   v->type = DVAR_TSTR;
   v->v_str = dvar_str_copy(str);
}

static void set_str_c_(dvar* v, const char* x, t_vm_int len)
{
   v->type = DVAR_TSTR;
   v->v_str = dvar_str_new(x, len);
}

static void set_fun_(dvar* v, const dvar_fun* fun)
{
   v->type = DVAR_TFUN;
   v->v_fun = dvar_fun_copy(fun);
}

static void set_function_(dvar* v, t_vm_addr addr)
{
   v->type = DVAR_TFUN;
   v->v_fun = dvar_fun_new_function(addr);
}

static void set_syscall_(dvar* v, t_vm_int syscall)
{
   v->type = DVAR_TFUN;
   v->v_fun = dvar_fun_new_syscall(syscall);
}

static void set_arr_(dvar* v, const dvar_arr* arr)
{
   v->type = DVAR_TARR;
   v->v_arr = dvar_arr_copy((dvar_arr*) arr);
}

static void set_arr_new_(dvar* v, const dvar* begin, const dvar* end)
{
   v->type = DVAR_TARR;
   v->v_arr = dvar_arr_new(begin, end);
}

static void set_set_(dvar* v, const dvar_set* set)
{
   v->type = DVAR_TSET;
   v->v_set = dvar_set_copy((dvar_set*) set);
}

static void set_set_new_(dvar* v, const dvar* begin, const dvar* end)
{
   v->type = DVAR_TSET;
   v->v_set = dvar_set_new(begin, end);
}

static void set_map_(dvar* v, const dvar_map* map)
{
   v->type = DVAR_TMAP;
   v->v_map = dvar_map_copy((dvar_map*) map);
}

static void set_map_new_(dvar* v, const dvar* begin, const dvar* end)
{
   v->type = DVAR_TMAP;
   v->v_map = dvar_map_new(begin, end);
}

static void set_obj_(dvar* v, const dvar_obj* obj)
{
   v->type = DVAR_TOBJ;
   v->v_obj = dvar_obj_copy((dvar_obj*) obj);
}

static void set_obj_new_(dvar* v, t_vm_int id)
{
   v->type = DVAR_TOBJ;
   v->v_obj = dvar_obj_new(id);
}

static void set_class_(dvar* v, const dvar_class* c)
{
   v->type = DVAR_TCLASS;
   v->v_class = dvar_class_copy((dvar_class*) c);
}

static void set_class_new_(dvar* v, t_vm_int id)
{
   v->type = DVAR_TCLASS;
   v->v_class = dvar_class_new(id);
}

static void assign_null_(dvar* v)
{
   free_content_(v);
   set_null_(v);
}

static void assign_int_(dvar* v, t_vm_int x)
{
   free_content_(v);
   set_int_(v, x);
}

static void assign_double_(dvar* v, t_vm_double x)
{
   free_content_(v);
   set_double_(v, x);
}

static void assign_char_(dvar* v, t_vm_char x)
{
   free_content_(v);
   set_char_(v, x);
}

static void assign_bool_(dvar* v, t_vm_bool x)
{
   free_content_(v);
   set_bool_(v, x);
}

static void assign_str_(dvar* v, const dvar_str* str)
{
   free_content_(v);
   set_str_(v, str);
}

static void assign_fun_(dvar* v, const dvar_fun* fun)
{
   free_content_(v);
   set_fun_(v, fun);
}

static void assign_arr_(dvar* v, const dvar_arr* arr)
{
   free_content_(v);
   set_arr_(v, arr);
}

static void assign_set_(dvar* v, const dvar_set* set)
{
   free_content_(v);
   set_set_(v, set);
}

static void assign_map_(dvar* v, const dvar_map* map)
{
   free_content_(v);
   set_map_(v, map);
}

static void assign_obj_(dvar* v, const dvar_obj* obj)
{
   free_content_(v);
   set_obj_(v, obj);
}

static void assign_class_(dvar* v, const dvar_class* c)
{
   free_content_(v);
   set_class_(v, c);
}

static void copy_null_(const dvar* v, dvar* dst)
{
   (void) v;
   assign_null_(dst);
}

static void copy_int_(const dvar* v, dvar* dst)
{
   assign_int_(dst, v->v_int);
}

static void copy_double_(const dvar* v, dvar* dst)
{
   assign_double_(dst, v->v_double);
}

static void copy_char_(const dvar* v, dvar* dst)
{
   assign_char_(dst, v->v_char);
}

static void copy_bool_(const dvar* v, dvar* dst)
{
   assign_bool_(dst, v->v_bool);
}

static void copy_str_(const dvar* v, dvar* dst)
{
   assign_str_(dst, v->v_str);
}

static void copy_fun_(const dvar* v, dvar* dst)
{
   assign_fun_(dst, v->v_fun);
}

static void copy_arr_(const dvar* v, dvar* dst)
{
   assign_arr_(dst, v->v_arr);
}

static void copy_set_(const dvar* v, dvar* dst)
{
   assign_set_(dst, v->v_set);
}

static void copy_map_(const dvar* v, dvar* dst)
{
   assign_map_(dst, v->v_map);
}

static void copy_obj_(const dvar* v, dvar* dst)
{
   assign_obj_(dst, v->v_obj);
}

static void copy_class_(const dvar* v, dvar* dst)
{
   assign_class_(dst, v->v_class);
}

static void move_null_(dvar* v, dvar* dst)
{
   assign_null_(dst);
   reset_(v);
}

static void move_int_(dvar* v, dvar* dst)
{
   assign_int_(dst, v->v_int);
   reset_(v);
}

static void move_double_(dvar* v, dvar* dst)
{
   assign_double_(dst, v->v_double);
   reset_(v);
}

static void move_char_(dvar* v, dvar* dst)
{
   assign_char_(dst, v->v_char);
   reset_(v);
}

static void move_bool_(dvar* v, dvar* dst)
{
   assign_bool_(dst, v->v_bool);
   reset_(v);
}

static void move_str_(dvar* v, dvar* dst)
{
   free_content_(dst);
   dst->type = DVAR_TSTR;
   dst->v_str = dvar_str_move(v->v_str);
   reset_(v);
}

static void move_fun_(dvar* v, dvar* dst)
{
   free_content_(dst);
   dst->type = DVAR_TFUN;
   dst->v_fun = dvar_fun_move(v->v_fun);
   reset_(v);
}

static void move_arr_(dvar* v, dvar* dst)
{
   free_content_(dst);
   dst->type = DVAR_TARR;
   dst->v_arr = dvar_arr_move(v->v_arr);
   reset_(v);
}

static void move_set_(dvar* v, dvar* dst)
{
   free_content_(dst);
   dst->type = DVAR_TSET;
   dst->v_set = dvar_set_move(v->v_set);
   reset_(v);
}

static void move_map_(dvar* v, dvar* dst)
{
   free_content_(dst);
   dst->type = DVAR_TMAP;
   dst->v_map = dvar_map_move(v->v_map);
   reset_(v);
}

static void move_obj_(dvar* v, dvar* dst)
{
   free_content_(dst);
   dst->type = DVAR_TOBJ;
   dst->v_obj = dvar_obj_move(v->v_obj);
   reset_(v);
}

static void move_class_(dvar* v, dvar* dst)
{
   free_content_(dst);
   dst->type = DVAR_TCLASS;
   dst->v_class = dvar_class_move(v->v_class);
   reset_(v);
}


static void cast_null_to_int_(dvar* v)
{
   free_content_(v);
   set_int_(v, 0);
}

static void cast_null_to_double_(dvar* v)
{
   free_content_(v);
   set_double_(v, 0);
}

static void cast_null_to_char_(dvar* v)
{
   free_content_(v);
   set_char_(v, 0);
}

static void cast_null_to_bool_(dvar* v)
{
   free_content_(v);
   set_bool_(v, 0);
}

static void cast_null_to_str_(dvar* v)
{
   free_content_(v);
   set_str_c_(v, "null", 4);
}

static void cast_int_to_double_(dvar* v)
{
   t_vm_double x = (t_vm_double) (v->v_int);
   free_content_(v);
   set_double_(v, x);
}

static void cast_int_to_char_(dvar* v)
{
   t_vm_char x = (t_vm_char) (v->v_int);
   free_content_(v);
   set_char_(v, x);
}

static void cast_int_to_bool_(dvar* v)
{
   t_vm_bool x = (t_vm_bool) (!!v->v_int);
   free_content_(v);
   set_bool_(v, x);
}

static void cast_int_to_str_(dvar* v)
{
   char* x = intToStr((long) (v->v_int));
   free_content_(v);
   set_str_c_(v, x, (t_vm_int) strlen(x));
   free(x);
}

static void cast_double_to_int_(dvar* v)
{
   t_vm_int x = (t_vm_int) (v->v_double);
   free_content_(v);
   set_int_(v, x);
}

static void cast_double_to_char_(dvar* v)
{
   t_vm_char x = (t_vm_char) (v->v_double);
   free_content_(v);
   set_char_(v, x);
}

static void cast_double_to_bool_(dvar* v)
{
   t_vm_bool x = (t_vm_bool) (!!v->v_double);
   free_content_(v);
   set_bool_(v, x);
}

static void cast_double_to_str_(dvar* v)
{
   char* x = floatToStr((double) (v->v_double));
   free_content_(v);
   set_str_c_(v, x, (t_vm_int) strlen(x));
   free(x);
}

static void cast_char_to_int_(dvar* v)
{
   t_vm_int x = (t_vm_int) (v->v_char);
   free_content_(v);
   set_int_(v, x);
}

static void cast_char_to_double_(dvar* v)
{
   t_vm_double x = (t_vm_double) (v->v_char);
   free_content_(v);
   set_double_(v, x);
}

static void cast_char_to_bool_(dvar* v)
{
   t_vm_bool x = (t_vm_bool) (!!v->v_char);
   free_content_(v);
   set_bool_(v, x);
}

static void cast_char_to_str_(dvar* v)
{
   char x = (char) (v->v_char);
   free_content_(v);
   set_str_c_(v, &x, 1);
}

static void cast_bool_to_int_(dvar* v)
{
   t_vm_int x = (t_vm_int) (v->v_bool);
   free_content_(v);
   set_int_(v, x);
}

static void cast_bool_to_double_(dvar* v)
{
   t_vm_double x = (t_vm_double) (v->v_bool);
   free_content_(v);
   set_double_(v, x);
}

static void cast_bool_to_char_(dvar* v)
{
   t_vm_char x = (t_vm_char) (v->v_char);
   free_content_(v);
   set_char_(v, x);
}

static void cast_bool_to_str_(dvar* v)
{
   t_vm_bool x = v->v_bool;
   free_content_(v);
   if(x)
      set_str_c_(v, "true", 4);
   else
      set_str_c_(v, "false", 5);
}

static void cast_str_to_int_(dvar* v)
{
   char* str = to_str_(v);
   int ok;
   t_vm_int x = (t_vm_int) (strToInt(str, &ok));
   if(!ok)
      err("Unable to cast str to int, str = '%s'", str);
   free(str);

   free_content_(v);
   set_int_(v, x);
}

static void cast_str_to_double_(dvar* v)
{
   char* str = to_str_(v);
   int ok;
   t_vm_double x = (t_vm_double) (strToFloat(str, &ok));
   if(!ok)
      err("Unable to cast str to double, str = '%s'", str);
   free(str);

   free_content_(v);
   set_double_(v, x);
}

static void cast_str_to_char_(dvar* v)
{
   dvar_str* str = v->v_str;
   if(str->len != 1)
      err("Unable to cast str to char, str = '%s'", to_str_(v));
   t_vm_char x = (str->chars[0].v_char);
   free_content_(v);
   set_char_(v, x);
}

static void cast_str_to_bool_(dvar* v)
{
   t_vm_bool x = (t_vm_bool) (!!(v->v_str->len));
   free_content_(v);
   set_bool_(v, x);
}

static void cast_fun_to_bool_(dvar* v)
{
   assign_bool_(v, dvar_fun_to_bool(v->v_fun));
}

static void cast_fun_to_str_(dvar* v)
{
   char* x = dvar_fun_to_string(v->v_fun);
   free_content_(v);
   set_str_c_(v, x, (t_vm_int) strlen(x));
   free(x);
}

static void cast_arr_to_bool_(dvar* v)
{
   assign_bool_(v, dvar_arr_to_bool(v->v_arr));
}

static void cast_arr_to_str_(dvar* v)
{
   char* x = dvar_arr_to_string(v->v_arr);
   free_content_(v);
   set_str_c_(v, x, (t_vm_int) strlen(x));
   free(x);
}

static void cast_set_to_bool_(dvar* v)
{
   assign_bool_(v, dvar_set_to_bool(v->v_set));
}

static void cast_set_to_str_(dvar* v)
{
   char* x = dvar_set_to_string(v->v_set);
   free_content_(v);
   set_str_c_(v, x, (t_vm_int) strlen(x));
   free(x);
}

static void cast_map_to_bool_(dvar* v)
{
   assign_bool_(v, dvar_map_to_bool(v->v_map));
}

static void cast_map_to_str_(dvar* v)
{
   char* x = dvar_map_to_string(v->v_map);
   free_content_(v);
   set_str_c_(v, x, (t_vm_int) strlen(x));
   free(x);
}

static void cast_obj_to_bool_(dvar* v)
{
   assign_bool_(v, dvar_obj_to_bool(v->v_obj));
}

static void cast_obj_to_str_(dvar* v)
{
   char* x = dvar_obj_to_string(v->v_obj);
   free_content_(v);
   set_str_c_(v, x, (t_vm_int) strlen(x));
   free(x);
}

static void cast_class_to_bool_(dvar* v)
{
   assign_bool_(v, dvar_class_to_bool(v->v_class));
}

static void cast_class_to_str_(dvar* v)
{
   char* x = dvar_class_to_string(v->v_class);
   free_content_(v);
   set_str_c_(v, x, (t_vm_int) strlen(x));
   free(x);
}


static void cast_to_(dvar* v, t_vm_type type)
{
   static const f_dvar_a1 FNS[] = {
      NULL,
      cast_null_to_int_,
      cast_null_to_double_,
      cast_null_to_char_,
      cast_null_to_bool_,
      cast_null_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      NULL,
      cast_int_to_double_,
      cast_int_to_char_,
      cast_int_to_bool_,
      cast_int_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      cast_double_to_int_,
      NULL,
      cast_double_to_char_,
      cast_double_to_bool_,
      cast_double_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      cast_char_to_int_,
      cast_char_to_double_,
      NULL,
      cast_char_to_bool_,
      cast_char_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      cast_bool_to_int_,
      cast_bool_to_double_,
      cast_bool_to_char_,
      NULL,
      cast_bool_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      cast_str_to_int_,
      cast_str_to_double_,
      cast_str_to_char_,
      cast_str_to_bool_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      NULL,
      NULL,
      NULL,
      cast_fun_to_bool_,
      cast_fun_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      NULL,
      NULL,
      NULL,
      cast_arr_to_bool_,
      cast_arr_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      NULL,
      NULL,
      NULL,
      cast_set_to_bool_,
      cast_set_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      NULL,
      NULL,
      NULL,
      cast_map_to_bool_,
      cast_map_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      NULL,
      NULL,
      NULL,
      cast_obj_to_bool_,
      cast_obj_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,

      NULL,
      NULL,
      NULL,
      NULL,
      cast_class_to_bool_,
      cast_class_to_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   assert(type >= 0 && type < DVAR_NB_TYPES);
   if(v->type == type)
      return;

   size_t pos = (size_t) (v->type) * DVAR_NB_TYPES + (size_t) type;
   assert(FNS[pos]);
   FNS[pos](v);
}

static t_vm_bool has_implicit_cast_(t_vm_type from, t_vm_type to)
{
   static const t_vm_bool  TAB[] = {
      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      0,
      1,
      1,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      1,
      0,
      1,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      1,
      1,
      0,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      1,
      1,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
   };


   assert(from >= 0 && from < DVAR_NB_TYPES);
   assert(to >= 0 && to < DVAR_NB_TYPES);

   return TAB[(size_t) from * DVAR_NB_TYPES + (size_t) to];
}

static const char CAST_HIERARCHY_[] = {
   DVAR_TNULL,
   DVAR_TBOOL,
   DVAR_TCHAR,
   DVAR_TINT,
   DVAR_TDOUBLE,
   DVAR_TSTR,
   DVAR_TFUN,
   DVAR_TARR,
   DVAR_TSET,
   DVAR_TMAP,
   DVAR_TOBJ,
   DVAR_TCLASS
};

static const int CAST_HIERARCHY_LEVEL_[] = {
   0,
   3,
   4,
   2,
   1,
   5,
   6,
   7,
   8,
   9,
   10,
   11
};

typedef const void** a_typed_fns;

static t_vm_type find_type_(const dvar* v, a_typed_fns table)
{
   for(int i = DVAR_NB_TYPES - 1; i >= 0; --i)
   {
      t_vm_type type = CAST_HIERARCHY_[i];
      if(table[(size_t) type] && has_implicit_cast_(v->type, type))
         return type;
   }

   return DVAR_TNOT;
}

static t_vm_type find_type_2_(const dvar* a, const dvar* b,
                              a_typed_fns table)
{
   if(table[(size_t) a->type] && !table[(size_t) b->type])
   {
      if(has_implicit_cast_(b->type, a->type))
         return a->type;
   }

   if(!table[(size_t) a->type] && table[(size_t) b->type])
   {
      if(has_implicit_cast_(a->type, b->type))
         return b->type;
   }


   if(table[(size_t) a->type] && table[(size_t) b->type])
   {
      int la = CAST_HIERARCHY_LEVEL_[(size_t) a->type];
      int lb = CAST_HIERARCHY_LEVEL_[(size_t) b->type];
      if(la > lb && has_implicit_cast_(b->type, a->type))
         return a->type;
      else if(has_implicit_cast_(a->type, b->type))
         return b->type;
   }

   for(int i = DVAR_NB_TYPES - 1; i >= 0; --i)
   {
      t_vm_type type = CAST_HIERARCHY_[i];
      if(table[(size_t) type]
         && (type == a->type || has_implicit_cast_(a->type, type))
         && (type == b->type || has_implicit_cast_(b->type, type)))
         return type;
   }

   return DVAR_TNOT;
}

typedef void (*f_copy_)(const dvar* v, dvar* dst);
static void copy_(const dvar* v, dvar* dst)
{
   static const f_copy_ FNS[] = {
      copy_null_,
      copy_int_,
      copy_double_,
      copy_char_,
      copy_bool_,
      copy_str_,
      copy_fun_,
      copy_arr_,
      copy_set_,
      copy_map_,
      copy_obj_,
      copy_class_,
   };

   FNS[v->type](v, dst);
}

typedef void (*f_move_)(dvar* v, dvar* dst);
static void move_(dvar* v, dvar* dst)
{
   static const f_move_ FNS[] = {
      move_null_,
      move_int_,
      move_double_,
      move_char_,
      move_bool_,
      move_str_,
      move_fun_,
      move_arr_,
      move_set_,
      move_map_,
      move_obj_,
      move_class_
   };

   FNS[v->type](v, dst);
}

void dvar_init(dvar* v)
{
   v->listener = NULL;
   reset_mode_(v);
   reset_content_(v);
}

void dvar_init_null(dvar* v, t_vm_mode mode)
{
   dvar_init(v);
   v->mode = mode;
   set_null_(v);
}

void dvar_init_int(dvar* v, t_vm_mode mode, t_vm_int x)
{
   dvar_init(v);
   v->mode = mode;
   set_int_(v, x);
}

void dvar_init_double(dvar* v, t_vm_mode mode, t_vm_double x)
{
   dvar_init(v);
   v->mode = mode;
   set_double_(v, x);
}

void dvar_init_char(dvar* v, t_vm_mode mode, t_vm_char x)
{
   dvar_init(v);
   v->mode = mode;
   set_char_(v, x);
}

void dvar_init_bool(dvar* v, t_vm_mode mode, t_vm_bool x)
{
   dvar_init(v);
   v->mode = mode;
   set_bool_(v, x);
}

void dvar_init_str(dvar* v, t_vm_mode mode, const char* x, t_vm_int len)
{
   dvar_init(v);
   v->mode = mode;
   set_str_c_(v, x, len);
}

void dvar_init_function(dvar* v, t_vm_mode mode, t_vm_addr addr)
{
   dvar_init(v);
   v->mode = mode;
   set_function_(v, addr);
}

void dvar_init_syscall(dvar* v, t_vm_mode mode, t_vm_int syscall)
{
   dvar_init(v);
   v->mode = mode;
   set_syscall_(v, syscall);
}

void dvar_init_arr(dvar* v, t_vm_mode mode,
                   const dvar* begin, const dvar* end)
{
   dvar_init(v);
   v->mode = mode;
   set_arr_new_(v, begin, end);
}

void dvar_init_set(dvar* v, t_vm_mode mode,
                   const dvar* begin, const dvar* end)
{
   dvar_init(v);
   v->mode = mode;
   set_set_new_(v, begin, end);
}

void dvar_init_map(dvar* v, t_vm_mode mode,
                   const dvar* begin, const dvar* end)
{
   dvar_init(v);
   v->mode = mode;
   set_map_new_(v, begin, end);
}

void dvar_init_obj(dvar* v, t_vm_mode mode, t_vm_int id)
{
   dvar_init(v);
   v->mode = mode;
   set_obj_new_(v, id);
}

void dvar_init_class(dvar* v, t_vm_mode mode, t_vm_int id)
{
   dvar_init(v);
   v->mode = mode;
   set_class_new_(v, id);
}


t_vm_bool dvar_cast_to(dvar* v, t_vm_type type)
{
   resolve_ref_((const dvar**)&v);

   if(v->type == type)
      return FALSE;

   if(!has_implicit_cast_(v->type, type))
      return FALSE;

   cast_to_(v, type);
   return TRUE;
}


void dvar_clear(dvar* v)
{
   if(v->type != DVAR_TREF)
      free_content_(v);
   else
      reset_content_(v);
   reset_mode_(v);
}

void dvar_bclear(dvar* begin, dvar* end)
{
   while(begin != end)
      dvar_clear(begin++);
}

t_vm_bool dvar_to_bool(const dvar* v)
{
   resolve_ref_(&v);
   return to_bool_(v);
}

char* dvar_to_str(const dvar* v)
{
   resolve_ref_(&v);
   return to_str_(v);
}

t_vm_bool dvar_equals(const dvar* a, const dvar* b)
{
   resolve_ref_(&a);
   resolve_ref_(&b);
   return equals_(a, b);
}

uint32_t dvar_to_hash(const dvar* v)
{
   resolve_ref_(&v);
   return to_hash_(v);
}


void dvar_putnull(dvar* v, t_vm_mode mode)
{
   dvar_clear(v);
   dvar_init_null(v, mode);
}

void dvar_putint(dvar* v, t_vm_mode mode, t_vm_int x)
{
   dvar_clear(v);
   dvar_init_int(v, mode, x);
}

void dvar_putdouble(dvar* v, t_vm_mode mode, t_vm_double x)
{
   dvar_clear(v);
   dvar_init_double(v, mode, x);
}

void dvar_putchar(dvar* v, t_vm_mode mode, t_vm_char x)
{
   dvar_clear(v);
   dvar_init_char(v, mode, x);
}

void dvar_putbool(dvar* v, t_vm_mode mode, t_vm_bool x)
{
   dvar_clear(v);
   dvar_init_bool(v, mode, x);
}

void dvar_putstring(dvar* v, t_vm_mode mode, const char* x, t_vm_int len)
{
   dvar_clear(v);
   dvar_init_str(v, mode, x, len);
}

void dvar_putfunction(dvar* v, t_vm_mode mode, t_vm_addr addr)
{
   dvar_clear(v);
   dvar_init_function(v, mode, addr);
}

void dvar_putsyscall(dvar* v, t_vm_mode mode, t_vm_int syscall)
{
   dvar_clear(v);
   dvar_init_syscall(v, mode, syscall);
}

void dvar_putarr(dvar* v, t_vm_mode mode, const dvar* begin, const dvar* end)
{
   dvar_clear(v);
   dvar_init_arr(v, mode, begin, end);
}

void dvar_putset(dvar* v, t_vm_mode mode, const dvar* begin, const dvar* end)
{
   dvar_clear(v);
   dvar_init_set(v, mode, begin, end);
}

void dvar_putmap(dvar* v, t_vm_mode mode, const dvar* begin, const dvar* end)
{
   dvar_clear(v);
   dvar_init_map(v, mode, begin, end);
}

void dvar_putobj(dvar* v, t_vm_mode mode, t_vm_int id)
{
   dvar_clear(v);
   dvar_init_obj(v, mode, id);
}

void dvar_putclass(dvar* v, t_vm_mode mode, t_vm_int id)
{
   dvar_clear(v);
   dvar_init_class(v, mode, id);
}

void dvar_putvar(dvar* v, t_vm_mode mode, dvar* src)
{
   dvar_move(v, src);
   v->mode = mode;
}

void dvar_putref(dvar* dst, dvar* src)
{
   dvar_clear(dst);
   resolve_ref_((const dvar**) &src);
   dst->type = DVAR_TREF;
   dst->v_ref = src;
}


void dvar_copy(dvar* dst, const dvar* src)
{
   if(dst == src)
      return;

   if(dst->type == DVAR_TREF)
      reset_content_(dst);
   reset_mode_(dst);

   resolve_ref_(&src);
   copy_(src, dst);
}

void dvar_move(dvar* dst, dvar* src)
{
   if(dst == src)
      return;

   if(dst->type == DVAR_TREF)
      reset_content_(dst);
   reset_mode_(dst);

   if(src->type == DVAR_TREF)
   {
      copy_(src->v_ref, dst);
      reset_(src);
   }
   else
   {
      move_(src, dst);
   }
}

void dvar_bind(dvar* dst, const dvar* begin, const dvar* end)
{
   resolve_ref_((const dvar**)&dst);
   assert(dst->type == DVAR_TFUN);

   dvar_fun_bind(dst->v_fun, begin, end);
}


typedef void (*f_ops_inc_dec_)(dvar* op, dvar* res);

static void ops_inc_dec_(const f_ops_inc_dec_ fns[], const char* operator,
                         dvar* op, dvar* res)
{
   if(op->type != DVAR_TREF)
      err("operator %s must be applied to a reference", operator);
   op = op->v_ref;

   if(!fns[op->type])
      err("operator %s cannot be applied to operand of type %s",
          operator, type_to_cstr_(op->type));

   if(op->mode == DVAR_MCONST)
      err("operator %s cannot be applied to const operand", operator);

   if(res->type == DVAR_TREF)
      reset_content_(res);
   reset_mode_(res);

   fns[op->type](op, res);
}

static void op_postinc_int_(dvar* op, dvar* res)
{
   assign_int_(res, op->v_int++);
}

static void op_postinc_double_(dvar* op, dvar* res)
{
   assign_double_(res, op->v_double++);
}

static void op_postinc_char_(dvar* op, dvar* res)
{
   assign_char_(res, op->v_char++);
}


void dvar_postinc(dvar* op, dvar* res)
{
   static const f_ops_inc_dec_ FNS[] = {
      NULL,
      op_postinc_int_,
      op_postinc_double_,
      op_postinc_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_inc_dec_(FNS, "++(post)", op, res);
}

static void op_postdec_int_(dvar* op, dvar* res)
{
   assign_int_(res, op->v_int--);
}

static void op_postdec_double_(dvar* op, dvar* res)
{
   assign_double_(res, op->v_double--);
}

static void op_postdec_char_(dvar* op, dvar* res)
{
   assign_char_(res, op->v_char--);
}


void dvar_postdec(dvar* op, dvar* res)
{
   static const f_ops_inc_dec_ FNS[] = {
      NULL,
      op_postdec_int_,
      op_postdec_double_,
      op_postdec_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_inc_dec_(FNS, "--(post)", op, res);
}

static void op_preinc_int_(dvar* op, dvar* res)
{
   assign_int_(res, ++op->v_int);
}

static void op_preinc_double_(dvar* op, dvar* res)
{
   assign_double_(res, ++op->v_double);
}

static void op_preinc_char_(dvar* op, dvar* res)
{
   assign_char_(res, ++op->v_char);
}


void dvar_preinc(dvar* op, dvar* res)
{
   static const f_ops_inc_dec_ FNS[] = {
      NULL,
      op_preinc_int_,
      op_preinc_double_,
      op_preinc_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_inc_dec_(FNS, "++(pre)", op, res);
}

static void op_predec_int_(dvar* op, dvar* res)
{
   assign_int_(res, --op->v_int);
}

static void op_predec_double_(dvar* op, dvar* res)
{
   assign_double_(res, --op->v_double);
}

static void op_predec_char_(dvar* op, dvar* res)
{
   assign_char_(res, --op->v_char);
}


void dvar_predec(dvar* op, dvar* res)
{
   static const f_ops_inc_dec_ FNS[] = {
      NULL,
      op_predec_int_,
      op_predec_double_,
      op_predec_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_inc_dec_(FNS, "--(post)", op, res);
}





typedef void (*f_ops_unary_)(const dvar* op, dvar* res);

static void ops_unary_(const f_ops_unary_ fns[], const char* operator,
                       const dvar* op, dvar* res)
{
   resolve_ref_(&op);
   if(res->type == DVAR_TREF)
      reset_content_(res);
   reset_mode_(res);

   if(fns[op->type])
   {
      fns[op->type](op, res);
      return;
   }

   t_vm_type type = find_type_(op, (a_typed_fns) fns);
   if(type == DVAR_TNOT)
      err("Operator %s cannot be applied to operand of type %s",
          operator, type_to_cstr_(op->type));

   dvar ac;
   dvar_init(&ac);
   copy_(op, &ac);
   cast_to_(&ac, type);
   fns[type](&ac, res);
   free_content_(&ac);
}

static void op_uplus_int_(const dvar* op, dvar* res)
{
   assign_int_(res, op->v_int);
}

static void op_uplus_double_(const dvar* op, dvar* res)
{
   assign_double_(res, op->v_double);
}

static void op_uplus_char_(const dvar* op, dvar* res)
{
   assign_char_(res, op->v_char);
}

void dvar_uplus(const dvar* op, dvar* res)
{
   static const f_ops_unary_ FNS[] = {
      NULL,
      op_uplus_int_,
      op_uplus_double_,
      op_uplus_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_unary_(FNS, "+", op, res);
}

static void op_uminus_int_(const dvar* op, dvar* res)
{
   assign_int_(res, - op->v_int);
}

static void op_uminus_double_(const dvar* op, dvar* res)
{
   assign_double_(res, - op->v_double);
}

static void op_uminus_char_(const dvar* op, dvar* res)
{
   assign_char_(res, - op->v_char);
}

void dvar_uminus(const dvar* op, dvar* res)
{
   static const f_ops_unary_ FNS[] = {
      NULL,
      op_uminus_int_,
      op_uminus_double_,
      op_uminus_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_unary_(FNS, "-", op, res);
}

static void op_lnot_gen_(const dvar* op, dvar* res)
{
   assign_bool_(res, !to_bool_(op));
}

void dvar_lnot(const dvar* op, dvar* res)
{
   static const f_ops_unary_ FNS[] = {
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_,
      op_lnot_gen_
   };

   ops_unary_(FNS, "!", op, res);
}

static void op_bnot_int_(const dvar* op, dvar* res)
{
   assign_int_(res, ~ op->v_int);
}

static void op_bnot_char_(const dvar* op, dvar* res)
{
   assign_char_(res, ~ op->v_char);
}

void dvar_bnot(const dvar* op, dvar* res)
{
   static const f_ops_unary_ FNS[] = {
      NULL,
      op_bnot_int_,
      NULL,
      op_bnot_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_unary_(FNS, "~", op, res);
}



typedef void (*f_ops_binary_)(const dvar* a, const dvar* b, dvar* res);

static void ops_binary_(const f_ops_binary_ fns[], const char* operator,
                        const dvar* a, const dvar* b, dvar* res)
{
   resolve_ref_(&a);
   resolve_ref_(&b);

   if(res->type == DVAR_TREF)
      reset_content_(res);
   reset_mode_(res);

   if(a->type == b->type && fns[a->type])
   {
      fns[a->type](a, b, res);
      return;
   }

   t_vm_type type = find_type_2_(a, b, (a_typed_fns) fns);

   if(type == DVAR_TNOT)
      err("Operator %s cannot be applied to operands of type %s and %s",
          operator, type_to_cstr_(a->type), type_to_cstr_(b->type));

   const dvar* ac;
   const dvar* bc;
   dvar v1;
   dvar v2;

   if(a->type == type)
   {
      ac = a;
   }
   else
   {
      dvar_init(&v1);
      copy_(a, &v1);
      cast_to_(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      copy_(b, &v2);
      cast_to_(&v2, type);
      bc = &v2;
   }

   fns[type](ac, bc, res);

   if(a->type != type)
      free_content_(&v1);
   if(b->type != type)
      free_content_(&v2);
}


static void op_mul_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int * b->v_int);
}

static void op_mul_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_double_(res, a->v_double * b->v_double);
}

static void op_mul_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char * b->v_char);
}

void dvar_mul(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_mul_int_,
      op_mul_double_,
      op_mul_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "*", a, b, res);
}

static void op_div_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int / b->v_int);
}

static void op_div_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_double_(res, a->v_double / b->v_double);
}

static void op_div_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char / b->v_char);
}

void dvar_div(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_div_int_,
      op_div_double_,
      op_div_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "/", a, b, res);
}

static void op_mod_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int % b->v_int);
}

static void op_mod_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char % b->v_char);
}

void dvar_mod(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_mod_int_,
      NULL,
      op_mod_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "%", a, b, res);
}

static void op_bplus_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int + b->v_int);
}

static void op_bplus_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_double_(res, a->v_double + b->v_double);
}

static void op_bplus_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char + b->v_char);
}

static void op_bplus_str_(const dvar* a, const dvar* b, dvar* res)
{
   dvar_str* as = a->v_str;
   dvar_str* bs = b->v_str;
   assign_str_(res, as);
   dvar_str_concat(res->v_str, bs);
}

void dvar_bplus(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_bplus_int_,
      op_bplus_double_,
      op_bplus_char_,
      NULL,
      op_bplus_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "+", a, b, res);
}

static void op_bminus_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int - b->v_int);
}

static void op_bminus_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_double_(res, a->v_double - b->v_double);
}

static void op_bminus_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char - b->v_char);
}

void dvar_bminus(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_bminus_int_,
      op_bminus_double_,
      op_bminus_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "-", a, b, res);
}

static void op_gt_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_int > b->v_int));
}

static void op_gt_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_double > b->v_double));
}

static void op_gt_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_char > b->v_char));
}

static void op_gt_str_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (dvar_str_cmp(a->v_str, b->v_str) > 0));
}

void dvar_gt(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_gt_int_,
      op_gt_double_,
      op_gt_char_,
      NULL,
      op_gt_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, ">", a, b, res);
}

static void op_lt_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_int < b->v_int));
}

static void op_lt_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_double < b->v_double));
}

static void op_lt_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_char < b->v_char));
}

static void op_lt_str_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (dvar_str_cmp(a->v_str, b->v_str) < 0));
}

void dvar_lt(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_lt_int_,
      op_lt_double_,
      op_lt_char_,
      NULL,
      op_lt_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "<", a, b, res);
}

static void op_geq_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_int >= b->v_int));
}

static void op_geq_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_double >= b->v_double));
}

static void op_geq_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_char >= b->v_char));
}

static void op_geq_str_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (dvar_str_cmp(a->v_str, b->v_str) >= 0));
}

void dvar_geq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_geq_int_,
      op_geq_double_,
      op_geq_char_,
      NULL,
      op_geq_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, ">=", a, b, res);
}

static void op_leq_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_int <= b->v_int));
}

static void op_leq_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_double <= b->v_double));
}

static void op_leq_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_char <= b->v_char));
}

static void op_leq_str_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (dvar_str_cmp(a->v_str, b->v_str) <= 0));
}

void dvar_leq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_leq_int_,
      op_leq_double_,
      op_leq_char_,
      NULL,
      op_leq_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "<=", a, b, res);
}


static void op_eq_null_(const dvar* a, const dvar* b, dvar* res)
{
   (void) a;
   (void) b;
   assign_bool_(res, TRUE);
}

static void op_eq_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_int == b->v_int));
}

static void op_eq_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_double == b->v_double));
}

static void op_eq_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_char == b->v_char));
}

static void op_eq_bool_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_bool == b->v_bool));
}

static void op_eq_str_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (dvar_str_cmp(a->v_str, b->v_str) == 0));
}

static void op_eq_fun_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, dvar_fun_equals(a->v_fun, b->v_fun));
}

static void op_eq_arr_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, dvar_arr_equals(a->v_arr, b->v_arr));
}

static void op_eq_set_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, dvar_set_equals(a->v_set, b->v_set));
}

static void op_eq_map_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, dvar_map_equals(a->v_map, b->v_map));
}

static void op_eq_obj_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, dvar_obj_equals(a->v_obj, b->v_obj));
}

static void op_eq_class_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, dvar_class_equals(a->v_class, b->v_class));
}

void dvar_eq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      op_eq_null_,
      op_eq_int_,
      op_eq_double_,
      op_eq_char_,
      op_eq_bool_,
      op_eq_str_,
      op_eq_fun_,
      op_eq_arr_,
      op_eq_set_,
      op_eq_map_,
      op_eq_obj_,
      op_eq_class_
   };

   ops_binary_(FNS, "==", a, b, res);
}

static void op_neq_null_(const dvar* a, const dvar* b, dvar* res)
{
   (void) a;
   (void) b;
   assign_bool_(res, FALSE);
}

static void op_neq_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_int != b->v_int));
}

static void op_neq_double_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_double != b->v_double));
}

static void op_neq_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_char != b->v_char));
}

static void op_neq_bool_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (a->v_bool != b->v_bool));
}

static void op_neq_str_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (dvar_str_cmp(a->v_str, b->v_str) != 0));
}

static void op_neq_fun_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (!dvar_fun_equals(a->v_fun, b->v_fun)));
}

static void op_neq_arr_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (!dvar_arr_equals(a->v_arr, b->v_arr)));
}

static void op_neq_set_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (!dvar_set_equals(a->v_set, b->v_set)));
}

static void op_neq_map_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (!dvar_map_equals(a->v_map, b->v_map)));
}

static void op_neq_obj_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (!dvar_obj_equals(a->v_obj, b->v_obj)));
}

static void op_neq_class_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (!dvar_class_equals(a->v_class, b->v_class)));
}

void dvar_neq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      op_neq_null_,
      op_neq_int_,
      op_neq_double_,
      op_neq_char_,
      op_neq_bool_,
      op_neq_str_,
      op_neq_fun_,
      op_neq_arr_,
      op_neq_set_,
      op_neq_map_,
      op_neq_obj_,
      op_neq_class_
   };

   ops_binary_(FNS, "!=", a, b, res);
}

static void op_land_gen_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (to_bool_(a) && to_bool_(b)));
}

void dvar_land(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      op_land_gen_,
      op_land_gen_,
      op_land_gen_,
      op_land_gen_,
      op_land_gen_,
      op_land_gen_,
      op_land_gen_,
      op_land_gen_,
      op_land_gen_,
      op_land_gen_,
      op_land_gen_,
      op_land_gen_
   };

   ops_binary_(FNS, "&&", a, b, res);
}

static void op_lor_gen_(const dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(res, (t_vm_bool) (to_bool_(a) || to_bool_(b)));
}

void dvar_lor(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_,
      op_lor_gen_
   };

   ops_binary_(FNS, "||", a, b, res);
}

static void op_lshift_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int << b->v_int);
}

static void op_lshift_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char << b->v_char);
}

void dvar_lshift(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_lshift_int_,
      NULL,
      op_lshift_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "<<", a, b, res);
}

static void op_rshift_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int >> b->v_int);
}

static void op_rshift_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char >> b->v_char);
}

void dvar_rshift(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_rshift_int_,
      NULL,
      op_rshift_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, ">>", a, b, res);
}

static void op_band_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int & b->v_int);
}

static void op_band_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char & b->v_char);
}

void dvar_band(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_band_int_,
      NULL,
      op_band_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "&", a, b, res);
}

static void op_bxor_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int ^ b->v_int);
}

static void op_bxor_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char ^ b->v_char);
}

void dvar_bxor(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_bxor_int_,
      NULL,
      op_bxor_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "^", a, b, res);
}

static void op_bor_int_(const dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int | b->v_int);
}

static void op_bor_char_(const dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char | b->v_char);
}

void dvar_bor(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      NULL,
      op_bor_int_,
      NULL,
      op_bor_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_binary_(FNS, "|", a, b, res);
}


static void call_listener_(dvar* v, const dvar* begin, const dvar* end)
{

   if(!v->listener || v->listener->syscall == VM_SYSCALL_NO)
      return;

   dvar res;
   dvar_init(&res);
   vm_syscall_exec_fn(v->listener, begin, end, &res);
   dvar_clear(&res);
}


typedef void (*f_ops_assign_)(dvar* a, const dvar* b, dvar* res);

static void op_assign_null_(dvar* a, const dvar* b, dvar* res)
{
   (void) b;
   assign_null_(a);
   assign_null_(res);
}

static void op_assign_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(a, b->v_int);
   assign_int_(res, a->v_int);
}

static void op_assign_double_(dvar* a, const dvar* b, dvar* res)
{
   assign_double_(a, b->v_double);
   assign_double_(res, a->v_double);
}

static void op_assign_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(a, b->v_char);
   assign_char_(res, a->v_char);
}

static void op_assign_bool_(dvar* a, const dvar* b, dvar* res)
{
   assign_bool_(a, b->v_bool);
   assign_bool_(res, a->v_bool);
}

static void op_assign_str_(dvar* a, const dvar* b, dvar* res)
{
   assign_str_(a, b->v_str);
   assign_str_(res, a->v_str);
}

static void op_assign_fun_(dvar* a, const dvar* b, dvar* res)
{
   assign_fun_(a, b->v_fun);
   assign_fun_(res, a->v_fun);
}

static void op_assign_arr_(dvar* a, const dvar* b, dvar* res)
{
   assign_arr_(a, b->v_arr);
   assign_arr_(res, a->v_arr);
}

static void op_assign_set_(dvar* a, const dvar* b, dvar* res)
{
   assign_set_(a, b->v_set);
   assign_set_(res, a->v_set);
}

static void op_assign_map_(dvar* a, const dvar* b, dvar* res)
{
   assign_map_(a, b->v_map);
   assign_map_(res, a->v_map);
}

static void op_assign_obj_(dvar* a, const dvar* b, dvar* res)
{
   assign_obj_(a, b->v_obj);
   assign_obj_(res, a->v_obj);
}

static void op_assign_class_(dvar* a, const dvar* b, dvar* res)
{
   assign_class_(a, b->v_class);
   assign_class_(res, a->v_class);
}

void dvar_assign(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      op_assign_null_,
      op_assign_int_,
      op_assign_double_,
      op_assign_char_,
      op_assign_bool_,
      op_assign_str_,
      op_assign_fun_,
      op_assign_arr_,
      op_assign_set_,
      op_assign_map_,
      op_assign_obj_,
      op_assign_class_
   };

   if(a->type != DVAR_TREF)
      err("left operand of = must be a reference");
   a = a->v_ref;

   resolve_ref_(&b);
   if(res->type == DVAR_TREF)
      reset_content_(res);
   reset_mode_(res);

   if(a->mode == DVAR_MCONST)
      err("left operand of = cannot be a const");

   if(a->mode == DVAR_MTCONST && a->type != b->type)
   {
      if(!has_implicit_cast_(b->type, a->type))
         err("left operand of = is type const %s and right operand is of type %s",
             type_to_cstr_(a->type), type_to_cstr_(b->type));

      dvar bc;
      dvar_init(&bc);
      copy_(b, &bc);
      cast_to_(&bc, a->type);
      FNS[bc.type](a, &bc, res);
      free_content_(&bc);
   }

   else
   {
      FNS[b->type](a, b, res);
   }

   call_listener_(a, a, a + 1);
}

static void ops_assign_(const f_ops_assign_ fns[], const char* operator,
                        dvar* a, const dvar* b, dvar* res)
{
   if(a->type != DVAR_TREF)
      err("left operand of %s must be a reference", operator);
   a = a->v_ref;

   if(a->mode == DVAR_MCONST)
      err("left operand of %s cannot be a const", operator);

   if(res->type == DVAR_TREF)
      reset_content_(res);
   resolve_ref_(&b);
   reset_mode_(res);

   if(a->type == b->type && fns[a->type])
   {
      fns[a->type](a, b, res);
      call_listener_(a, a, a + 1);
      return;
   }

   if(!fns[a->type] || !has_implicit_cast_(b->type, a->type))
      err("Operator %s cannot be applied to operands of type %s and %s",
          operator, type_to_cstr_(a->type), type_to_cstr_(b->type));

   dvar bc;
   dvar_init(&bc);
   copy_(b, &bc);
   cast_to_(&bc, a->type);
   fns[a->type](a, &bc, res);
   free_content_(&bc);

   call_listener_(a, a, a + 1);
}

static void op_pluseq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int += b->v_int);
}

static void op_pluseq_double_(dvar* a, const dvar* b, dvar* res)
{
   assign_double_(res, a->v_double += b->v_double);
}

static void op_pluseq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char += b->v_char);
}

static void op_pluseq_str_(dvar* a, const dvar* b, dvar* res)
{
   dvar_str_concat(a->v_str, b->v_str);
   assign_str_(res, a->v_str);
}

void dvar_pluseq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_pluseq_int_,
      op_pluseq_double_,
      op_pluseq_char_,
      NULL,
      op_pluseq_str_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };


   ops_assign_(FNS, "+=", a, b, res);
}

static void op_minuseq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int -= b->v_int);
}

static void op_minuseq_double_(dvar* a, const dvar* b, dvar* res)
{
   assign_double_(res, a->v_double -= b->v_double);
}

static void op_minuseq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char -= b->v_char);
}

void dvar_minuseq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_minuseq_int_,
      op_minuseq_double_,
      op_minuseq_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_assign_(FNS, "-=", a, b, res);
}

static void op_muleq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int *= b->v_int);
}

static void op_muleq_double_(dvar* a, const dvar* b, dvar* res)
{
   assign_double_(res, a->v_double *= b->v_double);
}

static void op_muleq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char *= b->v_char);
}

void dvar_muleq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_muleq_int_,
      op_muleq_double_,
      op_muleq_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_assign_(FNS, "*=", a, b, res);
}

static void op_diveq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int /= b->v_int);
}

static void op_diveq_double_(dvar* a, const dvar* b, dvar* res)
{
   assign_double_(res, a->v_double /= b->v_double);
}

static void op_diveq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char /= b->v_char);
}

void dvar_diveq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_diveq_int_,
      op_diveq_double_,
      op_diveq_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_assign_(FNS, "/=", a, b, res);
}

static void op_modeq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int %= b->v_int);
}

static void op_modeq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char %= b->v_char);
}


void dvar_modeq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_modeq_int_,
      NULL,
      op_modeq_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_assign_(FNS, "%=", a, b, res);
}

static void op_lshifteq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int <<= b->v_int);
}

static void op_lshifteq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char <<= b->v_char);
}


void dvar_lshifteq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_lshifteq_int_,
      NULL,
      op_lshifteq_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_assign_(FNS, "<<=", a, b, res);
}

static void op_rshifteq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int >>= b->v_int);
}

static void op_rshifteq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char >>= b->v_char);
}


void dvar_rshifteq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_rshifteq_int_,
      NULL,
      op_rshifteq_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_assign_(FNS, ">>=", a, b, res);
}

static void op_bandeq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int &= b->v_int);
}

static void op_bandeq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char &= b->v_char);
}


void dvar_bandeq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_bandeq_int_,
      NULL,
      op_bandeq_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_assign_(FNS, "&=", a, b, res);
}

static void op_bxoreq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int ^= b->v_int);
}

static void op_bxoreq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char ^= b->v_char);
}


void dvar_bxoreq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_bxoreq_int_,
      NULL,
      op_bxoreq_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_assign_(FNS, "^=", a, b, res);
}

static void op_boreq_int_(dvar* a, const dvar* b, dvar* res)
{
   assign_int_(res, a->v_int |= b->v_int);
}

static void op_boreq_char_(dvar* a, const dvar* b, dvar* res)
{
   assign_char_(res, a->v_char |= b->v_char);
}


void dvar_boreq(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      NULL,
      op_boreq_int_,
      NULL,
      op_boreq_char_,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   };

   ops_assign_(FNS, "|=", a, b, res);
}

void dvar_ternary(const dvar* a, const dvar* b, const dvar* c, dvar* d)
{
   resolve_ref_(&a);
   resolve_ref_(&b);
   resolve_ref_(&c);

   if(d->type == DVAR_TREF)
      reset_content_(d);
   reset_mode_(d);

   if(to_bool_(a))
      copy_(b, d);
   else
      copy_(c, d);
}

static t_vm_int subscript_to_int_(const dvar* a, const char* message)
{
   if(a->type == DVAR_TINT)
      return a->v_int;

   if(has_implicit_cast_(a->type, DVAR_TINT))
   {
      dvar b;
      dvar_init(&b);
      copy_(a, &b);
      cast_to_(&b, DVAR_TINT);
      t_vm_int x = b.v_int;
      free_content_(&b);
      return x;
   }

   err(message, type_to_cstr_(a->type));
   return 0;
}

typedef void (*f_op_subscript_)(const dvar* a, const dvar* b, dvar* c);

static void op_subscript_str_(const dvar* a, const dvar* b, dvar* c)
{
   t_vm_int pos = subscript_to_int_(
      b, "string: cannot cast right operand of [] (%s) to int");

   const dvar_str* str = a->v_str;

   if(pos < 0 || pos >= str->len)
   {
      err("operator[] : cannot access index %ld of string of size %ld",
          pos, str->len);
   }


   dvar* cvar = str->chars + pos;
   free_content_(c);
   c->type = DVAR_TREF;
   c->v_ref = cvar;
}

static void op_subscript_arr_(const dvar* a, const dvar* b, dvar* c)
{
   t_vm_int pos = subscript_to_int_(
      b, "array: cannot cast right operand of [] (%s) to int");

   dvar_arr_at(a->v_arr, pos, c);
}

static void op_subscript_map_(const dvar* a, const dvar* b, dvar* c)
{
   dvar_map_at(a, b, c);
}

void dvar_subscript(const dvar* a, const dvar* b, dvar* c)
{
   static const f_op_subscript_ FNS[] = {
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      op_subscript_str_,
      NULL,
      op_subscript_arr_,
      NULL,
      op_subscript_map_,
      NULL,
      NULL
   };

   resolve_ref_(&a);
   resolve_ref_(&b);

   if(c->type == DVAR_TREF)
      reset_content_(c);
   reset_mode_(c);

   if(FNS[a->type])
   {
      FNS[a->type](a, b, c);
      return;
   }

   t_vm_type type = find_type_(a, (a_typed_fns) FNS);
   if(type == DVAR_TNOT)
      err("Operator[] cannot be applied to left operand of type %s",
          type_to_cstr_(a->type));

   dvar ac;
   dvar_init(&ac);
   copy_(a, &ac);
   cast_to_(&ac, type);
   FNS[type](&ac, b, c);
   free_content_(&ac);
}



typedef void (*f_op_member_)(const dvar* a, const char* str, dvar* c);

static s_pmap* members_null_;
static s_pmap* members_int_;
static s_pmap* members_double_;
static s_pmap* members_char_;
static s_pmap* members_bool_;
static s_pmap* members_str_;
static s_pmap* members_fun_;
static s_pmap* members_arr_;
static s_pmap* members_set_;
static s_pmap* members_map_;

static void member_find_map_(const dvar* v, const char* name, dvar* res,
                             s_pmap* map)
{
   s_pmap_node* it = pmap_find(map, (void*) name);
   if(it == pmap_end(map))
   {
      err("Variable of type %s doesn't have a member %s",
          type_to_cstr_(v->type), name);
   }

   t_vm_int* n = it->value;
   dvar_putsyscall(res, DVAR_MVAR, *n);
   dvar_fun_bind(res->v_fun, v, v + 1);
}

static void member_null_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_null_);
}

static void member_int_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_int_);
}

static void member_double_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_double_);
}

static void member_char_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_char_);
}

static void member_bool_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_bool_);
}

static void member_str_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_str_);
}

static void member_fun_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_fun_);
}

static void member_arr_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_arr_);
}

static void member_set_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_set_);
}

static void member_map_(const dvar* v, const char* str, dvar* res)
{
   member_find_map_(v, str, res, members_map_);
}


void dvar_member(const dvar* v, const char* str, t_vm_int len, dvar* res)
{
   static const f_op_member_ FNS[] = {
      member_null_,
      member_int_,
      member_double_,
      member_char_,
      member_bool_,
      member_str_,
      member_fun_,
      member_arr_,
      member_set_,
      member_map_,
      dvar_obj_member,
      dvar_class_member
   };

   char* name = strCloneN(str, len);

   resolve_ref_(&v);
   FNS[v->type](v, name, res);
   free(name);
}

static t_vm_int* get_sys_member_(t_vm_int syscall)
{
   t_vm_int* n = malloc(sizeof(t_vm_int));
   *n = syscall;
   return n;
}


t_vm_int dvar_get_int(const dvar* v, const char* message)
{
   resolve_ref_(&v);
   if(v->type == DVAR_TINT)
      return v->v_int;

   if(!has_implicit_cast_(v->type, DVAR_TINT))
      err(message);

   dvar c;
   dvar_init(&c);
   dvar_copy(&c, v);
   cast_to_(&c, DVAR_TINT);

   t_vm_int res = c.v_int;
   dvar_clear(&c);
   return res;
}

t_vm_double dvar_get_double(const dvar* v, const char* message)
{
   resolve_ref_(&v);
   if(v->type == DVAR_TDOUBLE)
      return v->v_double;

   if(!has_implicit_cast_(v->type, DVAR_TDOUBLE))
      err(message);

   dvar c;
   dvar_init(&c);
   dvar_copy(&c, v);
   cast_to_(&c, DVAR_TDOUBLE);

   t_vm_double res = c.v_double;
   dvar_clear(&c);
   return res;
}

t_vm_char dvar_get_char(const dvar* v, const char* message)
{
   resolve_ref_(&v);
   if(v->type == DVAR_TCHAR)
      return v->v_char;

   if(!has_implicit_cast_(v->type, DVAR_TCHAR))
      err(message);

   dvar c;
   dvar_init(&c);
   dvar_copy(&c, v);
   cast_to_(&c, DVAR_TCHAR);

   t_vm_char res = c.v_char;
   dvar_clear(&c);
   return res;
}

t_vm_bool dvar_get_bool(const dvar* v, const char* message)
{
   resolve_ref_(&v);
   if(v->type == DVAR_TBOOL)
      return v->v_bool;

   if(!has_implicit_cast_(v->type, DVAR_TBOOL))
      err(message);

   dvar c;
   dvar_init(&c);
   dvar_copy(&c, v);
   cast_to_(&c, DVAR_TBOOL);

   t_vm_bool res = c.v_bool;
   dvar_clear(&c);
   return res;
}

char* dvar_get_str(const dvar* v, const char* message)
{
   resolve_ref_(&v);
   if(v->type == DVAR_TSTR)
      return to_str_str_(v);

   if(!has_implicit_cast_(v->type, DVAR_TSTR))
      err(message);

   dvar c;
   dvar_init(&c);
   dvar_copy(&c, v);
   cast_to_(&c, DVAR_TSTR);

   char* res = to_str_str_(&c);
   dvar_clear(&c);
   return res;
}

void dvar_type_init_null()
{
   members_null_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_null_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
}

void dvar_type_init_int()
{
   members_int_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_int_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
}

void dvar_type_init_double()
{
   members_double_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_double_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
}

void dvar_type_init_char()
{
   members_char_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_char_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
   pmap_insert(members_char_, "isPrintable",
               get_sys_member_(VM_SYSCALL_CHAR_IS_PRINTABLE));
   pmap_insert(members_char_, "isDigit",
               get_sys_member_(VM_SYSCALL_CHAR_IS_DIGIT));
   pmap_insert(members_char_, "isLower",
               get_sys_member_(VM_SYSCALL_CHAR_IS_LOWER));
   pmap_insert(members_char_, "isUpper",
               get_sys_member_(VM_SYSCALL_CHAR_IS_UPPER));
}

void dvar_type_init_bool()
{
   members_bool_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_bool_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
}

void dvar_type_init_str()
{
   members_str_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_str_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
   pmap_insert(members_str_, "empty",
               get_sys_member_(VM_SYSCALL_STR_EMPTY));
   pmap_insert(members_str_, "size",
               get_sys_member_(VM_SYSCALL_STR_SIZE));
   pmap_insert(members_str_, "substr",
               get_sys_member_(VM_SYSCALL_STR_SUBSTR));
   pmap_insert(members_str_, "isInt",
               get_sys_member_(VM_SYSCALL_STR_IS_INT));
   pmap_insert(members_str_, "toInt",
               get_sys_member_(VM_SYSCALL_STR_TO_INT));
   pmap_insert(members_str_, "isDouble",
               get_sys_member_(VM_SYSCALL_STR_IS_DOUBLE));
   pmap_insert(members_str_, "toDouble",
               get_sys_member_(VM_SYSCALL_STR_TO_DOUBLE));
}

void dvar_type_init_fun()
{
   members_fun_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_fun_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
   pmap_insert(members_fun_, "bind",
               get_sys_member_(VM_SYSCALL_FUN_BIND));
   pmap_insert(members_fun_, "binda",
               get_sys_member_(VM_SYSCALL_FUN_BINDA));
}

void dvar_type_init_arr()
{
   members_arr_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_arr_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
   pmap_insert(members_arr_, "empty",
               get_sys_member_(VM_SYSCALL_ARR_EMPTY));
   pmap_insert(members_arr_, "size",
               get_sys_member_(VM_SYSCALL_ARR_SIZE));
   pmap_insert(members_arr_, "pushFront",
               get_sys_member_(VM_SYSCALL_ARR_PUSH_FRONT));
   pmap_insert(members_arr_, "pushBack",
               get_sys_member_(VM_SYSCALL_ARR_PUSH_BACK));
   pmap_insert(members_arr_, "insert",
               get_sys_member_(VM_SYSCALL_ARR_INSERT));
   pmap_insert(members_arr_, "popFront",
               get_sys_member_(VM_SYSCALL_ARR_POP_FRONT));
   pmap_insert(members_arr_, "popBack",
               get_sys_member_(VM_SYSCALL_ARR_POP_BACK));
   pmap_insert(members_arr_, "remove",
               get_sys_member_(VM_SYSCALL_ARR_REMOVE));
   pmap_insert(members_arr_, "toSet",
               get_sys_member_(VM_SYSCALL_ARR_TO_SET));
}

void dvar_type_init_set()
{
   members_set_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_set_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
   pmap_insert(members_set_, "empty",
               get_sys_member_(VM_SYSCALL_SET_EMPTY));
   pmap_insert(members_set_, "size",
               get_sys_member_(VM_SYSCALL_SET_SIZE));
   pmap_insert(members_set_, "contains",
               get_sys_member_(VM_SYSCALL_SET_CONTAINS));
   pmap_insert(members_set_, "insert",
               get_sys_member_(VM_SYSCALL_SET_INSERT));
   pmap_insert(members_set_, "remove",
               get_sys_member_(VM_SYSCALL_SET_REMOVE));
   pmap_insert(members_set_, "toArray",
               get_sys_member_(VM_SYSCALL_SET_TO_ARRAY));
}

void dvar_type_init_map()
{
   members_map_ = pmap_new((f_pmap_cmp) strcmp);
   pmap_insert(members_map_, "toString",
               get_sys_member_(VM_SYSCALL_TOSTRING));
   pmap_insert(members_map_, "empty",
               get_sys_member_(VM_SYSCALL_MAP_EMPTY));
   pmap_insert(members_map_, "size",
               get_sys_member_(VM_SYSCALL_MAP_SIZE));
   pmap_insert(members_map_, "contains",
               get_sys_member_(VM_SYSCALL_MAP_CONTAINS));
   pmap_insert(members_map_, "remove",
               get_sys_member_(VM_SYSCALL_MAP_REMOVE));
   pmap_insert(members_map_, "keys",
               get_sys_member_(VM_SYSCALL_MAP_KEYS));
   pmap_insert(members_map_, "values",
               get_sys_member_(VM_SYSCALL_MAP_VALUES));
}


void dvar_print(const dvar* v)
{
   const char* ref;
   const char* qualifier;

   if(v->type == DVAR_TREF)
   {
      v = v->v_ref;
      ref = "&";
   }
   else
   {
      ref = "";
   }

   if(v->mode == DVAR_MCONST)
      qualifier = "const ";
   else if(v->mode == DVAR_MTCONST)
      qualifier = "tconst ";
   else
      qualifier = "";

   char* str = to_str_(v);
   printf("<%s%s%s> : {%s}\n", qualifier, type_to_cstr_(v->type), ref, str);
   free(str);
}


struct dvar* c__char__is_printable(struct dvar* l, t_vm_int n)
{
   (void) n;
   t_vm_char c = l->v_char;
   dvar_putbool(l, DVAR_MVAR, c >= 33 && c <= 126);
   return l;
}

struct dvar* c__char__is_digit(struct dvar* l, t_vm_int n)
{
   (void) n;
   t_vm_char c = l->v_char;
   dvar_putbool(l, DVAR_MVAR, c >= '0' && c <= '9');
   return l;
}

struct dvar* c__char__is_lower(struct dvar* l, t_vm_int n)
{
   (void) n;
   t_vm_char c = l->v_char;
   dvar_putbool(l, DVAR_MVAR, c >= 'a' && c <= 'z');
   return l;
}

struct dvar* c__char__is_upper(struct dvar* l, t_vm_int n)
{
   (void) n;
   t_vm_char c = l->v_char;
   dvar_putbool(l, DVAR_MVAR, c >= 'A' && c <= 'Z');
   return l;
}
