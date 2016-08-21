#include "dvar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dvar-fun.h"
#include "dvar-str.h"
#include "str.h"
#include "numbers.h"
#include "err.h"


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
}

static void free_str_(dvar* v)
{
   dvar_str_free(v->v_str);
}

static void free_fun_(dvar* v)
{
   dvar_fun_free(v->v_fun);
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
      free_fun_
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
   return dvar_str_to_bool_(v->v_str);
}

static t_vm_bool to_bool_fun_(const dvar* v)
{
   return dvar_fun_to_bool_(v->v_fun);
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
      to_bool_fun_
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
   return dvar_str_to_string_(v->v_str);
}

static char* to_str_fun_(const dvar* v)
{
   return dvar_fun_to_string_(v->v_fun);
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
      to_str_fun_
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
   assign_bool_(v, dvar_fun_to_bool_(v->v_fun));
}

static void cast_fun_to_str_(dvar* v)
{
   char* x = dvar_fun_to_string_(v->v_fun);
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
      cast_int_to_double_,
      cast_int_to_char_,
      cast_int_to_bool_,
      cast_int_to_str_,
      NULL,

      NULL,
      cast_double_to_int_,
      NULL,
      cast_double_to_char_,
      cast_double_to_bool_,
      cast_double_to_str_,
      NULL,

      NULL,
      cast_char_to_int_,
      cast_char_to_double_,
      NULL,
      cast_char_to_bool_,
      cast_char_to_str_,
      NULL,

      NULL,
      cast_bool_to_int_,
      cast_bool_to_double_,
      cast_bool_to_char_,
      NULL,
      cast_bool_to_str_,
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
      cast_fun_to_bool_,
      cast_fun_to_str_,
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
      1,
      1,
      1,
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
      1,
      1,
      0,
      1,
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
      1,
      0,
      0,

      0,
      0,
      0,
      0,
      1,
      0,
      0
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
   DVAR_TFUN
};

static const int CAST_HIERARCHY_LEVEL_[] = {
   0,
   3,
   4,
   2,
   1,
   5,
   6
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
      copy_fun_
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
      move_fun_
   };

   FNS[v->type](v, dst);
}

void dvar_init(dvar* v)
{
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

void dvar_eq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      op_eq_null_,
      op_eq_int_,
      op_eq_double_,
      op_eq_char_,
      op_eq_bool_,
      op_eq_str_,
      op_eq_fun_
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

void dvar_neq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_binary_ FNS[] = {
      op_neq_null_,
      op_neq_int_,
      op_neq_double_,
      op_neq_char_,
      op_neq_bool_,
      op_neq_str_,
      op_neq_fun_
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
      NULL
   };

   ops_binary_(FNS, "|", a, b, res);
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

void dvar_assign(dvar* a, const dvar* b, dvar* res)
{
   static const f_ops_assign_ FNS[] = {
      op_assign_null_,
      op_assign_int_,
      op_assign_double_,
      op_assign_char_,
      op_assign_bool_,
      op_assign_str_,
      op_assign_fun_
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

void dvar_subscript(const dvar* a, const dvar* b, dvar* c)
{
   static const f_op_subscript_ FNS[] = {
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      op_subscript_str_,
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

void dvar_member(const dvar* v, const char* str, t_vm_int len, dvar* res)
{
   char* name = malloc(len + 1);
   memcpy(name, str, len);
   name[len] = '\0';

   resolve_ref_(&v);

   if(strcmp(name, "toString") != 0)
   {
      err("Variable of type %s doesn't have a member %s",
          type_to_cstr_(v->type), name);
   }

   dvar_putsyscall(res, 1, VM_SYSCALL_TOSTRING);
   dvar_fun_bind(res->v_fun, v, v + 1);
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
