#include "dvar.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "numbers.h"
#include "err.h"

static const char* DVAR_STYPES[] = {
   "null",
   "int",
   "double",
   "char",
   "bool",
   "string"
};

static const f_dvar_cast CAST_FNS[] = {
   NULL,
   dvar_cast_null_to_int,
   dvar_cast_null_to_double,
   dvar_cast_null_to_char,
   dvar_cast_null_to_bool,
   dvar_cast_null_to_string,

   dvar_assign_null,
   NULL,
   dvar_cast_int_to_double,
   dvar_cast_int_to_char,
   dvar_cast_int_to_bool,
   dvar_cast_int_to_string,

   dvar_assign_null,
   dvar_cast_double_to_int,
   NULL,
   dvar_cast_double_to_char,
   dvar_cast_double_to_bool,
   dvar_cast_double_to_string,

   dvar_assign_null,
   dvar_cast_char_to_int,
   dvar_cast_char_to_double,
   NULL,
   dvar_cast_char_to_bool,
   dvar_cast_char_to_string,


   dvar_assign_null,
   dvar_cast_bool_to_int,
   dvar_cast_bool_to_double,
   dvar_cast_bool_to_char,
   NULL,
   dvar_cast_bool_to_string,

   dvar_assign_null,
   dvar_cast_string_to_int,
   dvar_cast_string_to_double,
   dvar_cast_string_to_char,
   dvar_cast_string_to_bool,
   NULL
};

static const int CAST_IMPLICIT_TAB[] = {
   0,
   0,
   0,
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
   1,
   0,
   1,
   1,
   0,

   0,
   1,
   1,
   0,
   1,
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
   1,
   0,
};

static const char CAST_HIERARCHY[] = {
   DVAR_TNULL,
   DVAR_TBOOL,
   DVAR_TCHAR,
   DVAR_TINT,
   DVAR_TDOUBLE,
   DVAR_TSTRING
};

static const int CAST_HIERARCHY_LEVEL[] = {
   0,
   3,
   4,
   2,
   1,
   5
};

static void dvar_perform_cast_(dvar* v, char type)
{
   CAST_FNS[(size_t)(v->type) * DVAR_NB_TYPES + (size_t)(type)](v);
}

static char dvar_find_type_(const dvar* v, a_dvar_typed_fns table)
{
   for(int i = DVAR_NB_TYPES - 1; i >= 0; --i)
   {
      char type = CAST_HIERARCHY[i];
      if(table[(size_t) type] && dvar_has_implicit_cast(v->type, type))
         return type;
   }

   return DVAR_TNOT;
}

static char dvar_find_type_2_(const dvar* a, const dvar* b,
                              a_dvar_typed_fns table)
{
   if(table[(size_t) a->type] && !table[(size_t) b->type])
   {
      if(dvar_has_implicit_cast(b->type, a->type))
         return a->type;
   }

   if(!table[(size_t) a->type] && table[(size_t) b->type])
   {
      if(dvar_has_implicit_cast(a->type, b->type))
         return b->type;
   }


   if(table[(size_t) a->type] && table[(size_t) b->type])
   {
      int la = CAST_HIERARCHY_LEVEL[(size_t) a->type];
      int lb = CAST_HIERARCHY_LEVEL[(size_t) b->type];
      if(la > lb && dvar_has_implicit_cast(b->type, a->type))
         return a->type;
      else if(dvar_has_implicit_cast(a->type, b->type))
         return b->type;
   }

   for(int i = DVAR_NB_TYPES - 1; i >= 0; --i)
   {
      char type = CAST_HIERARCHY[i];
      if(table[(size_t) type]
         && (type == a->type || dvar_has_implicit_cast(a->type, type))
         && (type == b->type || dvar_has_implicit_cast(b->type, type)))
         return type;
   }

   return DVAR_TNOT;
}

void dvar_init(dvar* v)
{
   v->type = DVAR_TNULL;
}

void dvar_erase(dvar* v)
{
   if(v->type == DVAR_TSTRING)
      free(v->v_string);
   v->type = DVAR_TNULL;
}

void dvar_assign_null(dvar* v)
{
   dvar_erase(v);
}

void dvar_assign_int(dvar* v, oat_int x)
{
   dvar_erase(v);
   v->type = DVAR_TINT;
   v->v_int = x;
}

void dvar_assign_double(dvar* v, oat_double x)
{
   dvar_erase(v);
   v->type = DVAR_TDOUBLE;
   v->v_double = x;
}

void dvar_assign_char(dvar* v, char x)
{
   dvar_erase(v);
   v->type = DVAR_TCHAR;
   v->v_char = x;
}

void dvar_assign_bool(dvar* v, oat_bool x)
{
   dvar_erase(v);
   v->type = DVAR_TBOOL;
   v->v_bool = x;
}

void dvar_assign_string(dvar* v, const char* x, size_t len)
{
   dvar_erase(v);
   v->type = DVAR_TSTRING;
   v->v_string = malloc(len + 1);
   memcpy(v->v_string, x, len);
   v->v_string[len] = '\0';
}

void dvar_assign_string_move(dvar* v, char* x)
{
   dvar_erase(v);
   v->type = DVAR_TSTRING;
   v->v_string = x;
}

void dvar_assign_var(dvar* v, const dvar* x)
{
   dvar_erase(v);
   memcpy(v, x, sizeof(dvar));
   if(x->type == DVAR_TSTRING)
      v->v_string = strClone(x->v_string);
}

oat_bool dvar_to_bool_null(const dvar* v)
{
   (void) (v);
   return FALSE;
}

oat_bool dvar_to_bool_int(const dvar* v)
{
   return (oat_bool) (!!v->v_int);
}

oat_bool dvar_to_bool_double(const dvar* v)
{
   return (oat_bool) (!!v->v_double);
}

oat_bool dvar_to_bool_char(const dvar* v)
{
   return (oat_bool) (!!v->v_char);
}

oat_bool dvar_to_bool_bool(const dvar* v)
{
   return v->v_bool;
}

oat_bool dvar_to_bool_string(const dvar* v)
{
   return (oat_bool) (v->v_string[0] != '\0');
}

oat_bool dvar_to_bool(const dvar* v)
{
   static const f_dvar_to_bool FNS[] = {
      dvar_to_bool_null,
      dvar_to_bool_int,
      dvar_to_bool_double,
      dvar_to_bool_char,
      dvar_to_bool_bool,
      dvar_to_bool_string,
   };

   return FNS[(size_t) v->type](v);
}

char* dvar_to_string_null(const dvar* v)
{
   (void)v;
   return strClone("null");
}

char* dvar_to_string_int(const dvar* v)
{
   return intToStr((long) v->v_int);
}

char* dvar_to_string_double(const dvar* v)
{
   return floatToStr((double) v->v_double);
}

char* dvar_to_string_char(const dvar* v)
{
   char* str = malloc(2);
   str[0] = v->v_char;
   str[1] = '\0';
   return str;
}

char* dvar_to_string_bool(const dvar* v)
{
   return v->v_bool ? strClone("true") : strClone("false");
}

char* dvar_to_string_string(const dvar* v)
{
   return strClone(v->v_string);
}

char* dvar_to_string(const dvar* v)
{
   static const f_dvar_to_string FNS[] = {
      dvar_to_string_null,
      dvar_to_string_int,
      dvar_to_string_double,
      dvar_to_string_char,
      dvar_to_string_bool,
      dvar_to_string_string,
   };

   return FNS[(size_t) v->type](v);
}



const char* dvar_to_type_string(const dvar* v)
{
   return DVAR_STYPES[(size_t) v->type];
}

const char* dvar_type_to_string(char type)
{
   return DVAR_STYPES[(size_t) type];
}




void dvar_cast_to(dvar* v, char type)
{
   if(v->type != type)
      CAST_FNS[v->type * DVAR_NB_TYPES + type](v);
}

void dvar_cast_implicit_to(dvar* v, char type)
{
      if(v->type != type)
      {
         if(dvar_has_implicit_cast(v->type, type))
            dvar_perform_cast_(v, type);
         else
            err("Unable to realize an implicit cast from %s to %s",
                DVAR_STYPES[(size_t) v->type], DVAR_STYPES[(size_t) type]);
      }
}

int dvar_has_implicit_cast(char from, char to)
{
   return CAST_IMPLICIT_TAB[(size_t)(from) * DVAR_NB_TYPES + (size_t)(to)];
}


void dvar_cast_null_to_int(dvar* v)
{
   dvar_assign_int(v, 0);
}

void dvar_cast_null_to_double(dvar* v)
{
   dvar_assign_double(v, 0);
}

void dvar_cast_null_to_char(dvar* v)
{
   dvar_assign_char(v, 0);
}

void dvar_cast_null_to_bool(dvar* v)
{
   dvar_assign_bool(v, FALSE);
}

void dvar_cast_null_to_string(dvar* v)
{
   dvar_assign_string(v, "null", 4);
}

void dvar_cast_int_to_double(dvar* v)
{
   dvar_assign_double(v, (oat_double) v->v_int);
}

void dvar_cast_int_to_char(dvar* v)
{
   dvar_assign_char(v, (char) v->v_int);
}

void dvar_cast_int_to_bool(dvar* v)
{
   if(v->v_int)
      dvar_assign_bool(v, TRUE);
   else
      dvar_assign_bool(v, FALSE);
}

void dvar_cast_int_to_string(dvar* v)
{
   char* str = intToStr((long) v->v_int);
   dvar_assign_string_move(v, str);
}

void dvar_cast_double_to_int(dvar* v)
{
   dvar_assign_int(v, (oat_int) v->v_double);
}

void dvar_cast_double_to_char(dvar* v)
{
   dvar_assign_char(v, (char) v->v_double);
}

void dvar_cast_double_to_bool(dvar* v)
{
   if(v->v_double)
      dvar_assign_bool(v, TRUE);
   else
      dvar_assign_bool(v, FALSE);
}

void dvar_cast_double_to_string(dvar* v)
{
   char* str = floatToStr((double) v->v_double);
   dvar_assign_string_move(v, str);
}

void dvar_cast_char_to_int(dvar* v)
{
   dvar_assign_int(v, (oat_int) v->v_char);
}

void dvar_cast_char_to_double(dvar* v)
{
   dvar_assign_double(v, (oat_double) v->v_char);
}

void dvar_cast_char_to_bool(dvar* v)
{
   if(v->v_char)
      dvar_assign_bool(v, TRUE);
   else
      dvar_assign_bool(v, FALSE);
}

void dvar_cast_char_to_string(dvar* v)
{
   char c = v->v_char;
   dvar_assign_string(v, &c, 1);
}

void dvar_cast_bool_to_int(dvar* v)
{
   dvar_assign_int(v, (oat_int) v->v_bool);
}

void dvar_cast_bool_to_double(dvar* v)
{
   dvar_assign_double(v, (oat_double) v->v_bool);
}

void dvar_cast_bool_to_char(dvar* v)
{
   dvar_assign_char(v, (char) v->v_bool);
}

void dvar_cast_bool_to_string(dvar* v)
{
   if(v->v_bool)
      dvar_assign_string(v, "true", 4);
   else
      dvar_assign_string(v, "false", 5);
}

void dvar_cast_string_to_int(dvar* v)
{
   int ok;
   long x = strToInt(v->v_string, &ok);
   if(!ok)
      err("Failed to cast string to int; str = \"%s\"", v->v_string);
   dvar_assign_int(v, (oat_int) x);
}

void dvar_cast_string_to_double(dvar* v)
{
   int ok;
   double x = strToFloat(v->v_string, &ok);
   if(!ok)
      err("Failed to cast string to double; str = \"%s\"", v->v_string);
   dvar_assign_double(v, (oat_double) x);
}

void dvar_cast_string_to_char(dvar* v)
{
   if(strlen(v->v_string) != 1)
      err("Failed to cast string to char; str = \"%s\"", v->v_string);
   dvar_assign_char(v, v->v_string[0]);
}

void dvar_cast_string_to_bool(dvar* v)
{
   dvar_assign_bool(v, v->v_string[0] != '\0');
}



void dvar_op_postinc_int(dvar* a, dvar* res)
{
   dvar_assign_int(res, a->v_int);
   ++a->v_int;
}

void dvar_op_postinc_double(dvar* a, dvar* res)
{
   dvar_assign_double(res, a->v_double);
   ++a->v_double;
}

void dvar_op_postinc_char(dvar* a, dvar* res)
{
   dvar_assign_char(res, a->v_char);
   ++a->v_char;
}

void dvar_op_postdec_int(dvar* a, dvar* res)
{
   dvar_assign_int(res, a->v_int);
   --a->v_int;
}

void dvar_op_postdec_double(dvar* a, dvar* res)
{
   dvar_assign_double(res, a->v_double);
   --a->v_double;
}

void dvar_op_postdec_char(dvar* a, dvar* res)
{
   dvar_assign_char(res, a->v_char);
   --a->v_char;
}

void dvar_op_preinc_int(dvar* a, dvar* res)
{
   dvar_assign_int(res, a->v_int + 1);
   ++a->v_int;
}

void dvar_op_preinc_double(dvar* a, dvar* res)
{
   dvar_assign_double(res, a->v_double + 1);
   ++a->v_double;
}

void dvar_op_preinc_char(dvar* a, dvar* res)
{
   dvar_assign_char(res, a->v_char + 1);
   ++a->v_char;
}

void dvar_op_predec_int(dvar* a, dvar* res)
{
   dvar_assign_int(res, a->v_int - 1);
   --a->v_int;
}

void dvar_op_predec_double(dvar* a, dvar* res)
{
   dvar_assign_double(res, a->v_double - 1);
   --a->v_double;
}

void dvar_op_predec_char(dvar* a, dvar* res)
{
   dvar_assign_char(res, a->v_char - 1);
   --a->v_char;
}

void dvar_op_uplus_int(const dvar* a, dvar* res)
{
   dvar_assign_int(res, a->v_int);
}

void dvar_op_uplus_double(const dvar* a, dvar* res)
{
   dvar_assign_double(res, a->v_double);
}

void dvar_op_uplus_char(const dvar* a, dvar* res)
{
   dvar_assign_char(res, a->v_char);
}

void dvar_op_uminus_int(const dvar* a, dvar* res)
{
   dvar_assign_int(res, -a->v_int);
}

void dvar_op_uminus_double(const dvar* a, dvar* res)
{
   dvar_assign_double(res, -a->v_double);
}

void dvar_op_uminus_char(const dvar* a, dvar* res)
{
   dvar_assign_char(res, -a->v_char);
}


void dvar_op_mul_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_int(res, a->v_int * b->v_int);
}

void dvar_op_mul_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_double(res, a->v_double * b->v_double);
}

void dvar_op_mul_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_char(res, a->v_char * b->v_char);
}

void dvar_op_div_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_int(res, a->v_int / b->v_int);
}

void dvar_op_div_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_double(res, a->v_double / b->v_double);
}

void dvar_op_div_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_char(res, a->v_char / b->v_char);
}

void dvar_op_mod_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_int(res, a->v_int % b->v_int);
}

void dvar_op_mod_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_char(res, a->v_char % b->v_char);
}

void dvar_op_bplus_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_int(res, a->v_int + b->v_int);
}

void dvar_op_bplus_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_double(res, a->v_double + b->v_double);
}

void dvar_op_bplus_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_char(res, a->v_char + b->v_char);
}

void dvar_op_bplus_string(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_string_move(res, strConcat(a->v_string, b->v_string));
}

void dvar_op_bminus_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_int(res, a->v_int - b->v_int);
}

void dvar_op_bminus_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_double(res, a->v_double - b->v_double);
}

void dvar_op_bminus_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_char(res, a->v_char - b->v_char);
}


void dvar_op_gt_null(const dvar* a, const dvar* b, dvar* res)
{
   (void) a;
   (void) b;
   dvar_assign_bool(res, FALSE);
}

void dvar_op_gt_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_int > b->v_int);
}

void dvar_op_gt_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_double > b->v_double);
}

void dvar_op_gt_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_char > b->v_char);
}

void dvar_op_gt_bool(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_bool > b->v_bool);
}

void dvar_op_gt_string(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, strcmp(a->v_string, b->v_string) > 0);
}


void dvar_op_lt_null(const dvar* a, const dvar* b, dvar* res)
{
   (void) a;
   (void) b;
   dvar_assign_bool(res, FALSE);
}

void dvar_op_lt_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_int < b->v_int);
}

void dvar_op_lt_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_double < b->v_double);
}

void dvar_op_lt_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_char < b->v_char);
}

void dvar_op_lt_bool(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_bool < b->v_bool);
}

void dvar_op_lt_string(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, strcmp(a->v_string, b->v_string) < 0);
}

void dvar_op_geq_null(const dvar* a, const dvar* b, dvar* res)
{
   (void) a;
   (void) b;
   dvar_assign_bool(res, TRUE);
}

void dvar_op_geq_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_int >= b->v_int);
}

void dvar_op_geq_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_double >= b->v_double);
}

void dvar_op_geq_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_char >= b->v_char);
}

void dvar_op_geq_bool(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_bool >= b->v_bool);
}

void dvar_op_geq_string(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, strcmp(a->v_string, b->v_string) >= 0);
}

void dvar_op_leq_null(const dvar* a, const dvar* b, dvar* res)
{
   (void) a;
   (void) b;
   dvar_assign_bool(res, TRUE);
}

void dvar_op_leq_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_int <= b->v_int);
}

void dvar_op_leq_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_double <= b->v_double);
}

void dvar_op_leq_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_char <= b->v_char);
}

void dvar_op_leq_bool(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_bool <= b->v_bool);
}


void dvar_op_leq_string(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, strcmp(a->v_string, b->v_string) <= 0);
}

void dvar_op_eq_null(const dvar* a, const dvar* b, dvar* res)
{
   (void) a;
   (void) b;
   dvar_assign_bool(res, TRUE);
}

void dvar_op_eq_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_int == b->v_int);
}

void dvar_op_eq_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_double == b->v_double);
}

void dvar_op_eq_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_char == b->v_char);
}

void dvar_op_eq_bool(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_bool == b->v_bool);
}

void dvar_op_eq_string(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, strcmp(a->v_string, b->v_string) == 0);
}

void dvar_op_neq_null(const dvar* a, const dvar* b, dvar* res)
{
   (void) a;
   (void) b;
   dvar_assign_bool(res, FALSE);
}

void dvar_op_neq_int(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_int != b->v_int);
}

void dvar_op_neq_double(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_double != b->v_double);
}

void dvar_op_neq_char(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_char != b->v_char);
}

void dvar_op_neq_bool(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, a->v_bool != b->v_bool);
}

void dvar_op_neq_string(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, strcmp(a->v_string, b->v_string) != 0);
}


void dvar_op_assign_null(dvar* a, const dvar* b, dvar* res)
{
   (void) b;
   dvar_assign_null(a);
   dvar_assign_null(res);
}

void dvar_op_assign_int(dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_int(a, b->v_int);
   dvar_assign_int(res, b->v_int);
}

void dvar_op_assign_double(dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_double(a, b->v_double);
   dvar_assign_double(res, b->v_double);
}

void dvar_op_assign_char(dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_char(a, b->v_char);
   dvar_assign_char(res, b->v_char);
}

void dvar_op_assign_bool(dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(a, b->v_bool);
   dvar_assign_bool(res, b->v_bool);
}

void dvar_op_assign_string(dvar* a, const dvar* b, dvar* res)
{
   size_t len = strlen(b->v_string);
   dvar_assign_string(a, b->v_string, len);
   dvar_assign_string(res, b->v_string, len);
}


void dvar_op_pluseq_int(dvar* a, const dvar* b, dvar* res)
{
   a->v_int += b->v_int;
   dvar_assign_int(res, a->v_int);
}

void dvar_op_pluseq_double(dvar* a, const dvar* b, dvar* res)
{
   a->v_double += b->v_double;
   dvar_assign_double(res, a->v_double);
}

void dvar_op_pluseq_char(dvar* a, const dvar* b, dvar* res)
{
   a->v_char += b->v_char;
   dvar_assign_char(res, a->v_char);
}

void dvar_op_pluseq_string(dvar* a, const dvar* b, dvar* res)
{
   char* str = strConcat(a->v_string, b->v_string);
   size_t len = strlen(str);
   dvar_assign_string(a, str, len);
   dvar_assign_string(res, str, len);
}


void dvar_op_minuseq_int(dvar* a, const dvar* b, dvar* res)
{
   a->v_int -= b->v_int;
   dvar_assign_int(res, a->v_int);
}

void dvar_op_minuseq_double(dvar* a, const dvar* b, dvar* res)
{
   a->v_double -= b->v_double;
   dvar_assign_double(res, a->v_double);
}

void dvar_op_minuseq_char(dvar* a, const dvar* b, dvar* res)
{
   a->v_char -= b->v_char;
   dvar_assign_char(res, a->v_char);
}

void dvar_op_muleq_int(dvar* a, const dvar* b, dvar* res)
{
   a->v_int *= b->v_int;
   dvar_assign_int(res, a->v_int);
}

void dvar_op_muleq_double(dvar* a, const dvar* b, dvar* res)
{
   a->v_double -= b->v_double;
   dvar_assign_double(res, a->v_double);
}

void dvar_op_muleq_char(dvar* a, const dvar* b, dvar* res)
{
   a->v_char *= b->v_char;
   dvar_assign_char(res, a->v_char);
}

void dvar_op_diveq_int(dvar* a, const dvar* b, dvar* res)
{
   a->v_int /= b->v_int;
   dvar_assign_int(res, a->v_int);
}

void dvar_op_diveq_double(dvar* a, const dvar* b, dvar* res)
{
   a->v_double /= b->v_double;
   dvar_assign_double(res, a->v_double);
}

void dvar_op_diveq_char(dvar* a, const dvar* b, dvar* res)
{
   a->v_char /= b->v_char;
   dvar_assign_char(res, a->v_char);
}
void dvar_op_modeq_int(dvar* a, const dvar* b, dvar* res)
{
   a->v_int %= b->v_int;
   dvar_assign_int(res, a->v_int);
}

void dvar_op_modeq_char(dvar* a, const dvar* b, dvar* res)
{
   a->v_char %= b->v_char;
   dvar_assign_char(res, a->v_char);
}


void dvar_op_postinc(dvar* a, dvar* res)
{
   static const f_dvar_op_postinc TABLE[] = {
      NULL,
      dvar_op_postinc_int,
      dvar_op_postinc_double,
      dvar_op_postinc_char,
      NULL,
      NULL
   };

   errAssert(!!TABLE[(size_t)a->type],
             "Operator ++(post) cannot be applied to operand of type %s",
             dvar_to_type_string(a));

   TABLE[(size_t)a->type](a, res);
}

void dvar_op_postdec(dvar* a, dvar* res)
{
   static const f_dvar_op_postdec TABLE[] = {
      NULL,
      dvar_op_postdec_int,
      dvar_op_postdec_double,
      dvar_op_postdec_char,
      NULL,
      NULL
   };

   errAssert(!!TABLE[(size_t)a->type],
             "Operator --(post) cannot be applied to operand of type %s",
             dvar_to_type_string(a));

   TABLE[(size_t)a->type](a, res);
}

void dvar_op_preinc(dvar* a, dvar* res)
{
   static const f_dvar_op_preinc TABLE[] = {
      NULL,
      dvar_op_preinc_int,
      dvar_op_preinc_double,
      dvar_op_preinc_char,
      NULL,
      NULL
   };

   errAssert(!!TABLE[(size_t)a->type],
             "Operator ++(pre) cannot be applied to operand of type %s",
             dvar_to_type_string(a));

   TABLE[(size_t)a->type](a, res);
}

void dvar_op_predec(dvar* a, dvar* res)
{
   static const f_dvar_op_predec TABLE[] = {
      NULL,
      dvar_op_predec_int,
      dvar_op_predec_double,
      dvar_op_predec_char,
      NULL,
      NULL
   };

   errAssert(!!TABLE[(size_t)a->type],
             "Operator --(pre) cannot be applied to operand of type %s",
             dvar_to_type_string(a));

   TABLE[(size_t)a->type](a, res);
}

void dvar_op_uplus(const dvar* a, dvar* res)
{
   static const f_dvar_op_uplus TABLE[] = {
      NULL,
      dvar_op_uplus_int,
      dvar_op_uplus_double,
      dvar_op_uplus_char,
      NULL,
      NULL
   };

   if(TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, res);
      return;
   }

   char type = dvar_find_type_(a, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator + cannot be applied to operand of type %s",
             dvar_to_type_string(a));

   dvar ac;
   dvar_init(&ac);
   dvar_assign_var(&ac, a);
   dvar_cast_to(&ac, type);
   TABLE[(size_t) type](&ac, res);
   dvar_erase(&ac);
}

void dvar_op_uminus(const dvar* a, dvar* res)
{
   static const f_dvar_op_uminus TABLE[] = {
      NULL,
      dvar_op_uminus_int,
      dvar_op_uminus_double,
      dvar_op_uminus_char,
      NULL,
      NULL
   };

   if(TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, res);
      return;
   }

   char type = dvar_find_type_(a, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator - cannot be applied to operand of type %s",
             dvar_to_type_string(a));

   dvar ac;
   dvar_init(&ac);
   dvar_assign_var(&ac, a);
   dvar_cast_to(&ac, type);
   TABLE[(size_t) type](&ac, res);
   dvar_erase(&ac);
}

void dvar_op_lnot(const dvar* a, dvar* res)
{
   dvar_assign_bool(res, (oat_bool) (!dvar_to_bool(a)));
}

void dvar_op_mul(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_mul TABLE[] = {
      NULL,
      dvar_op_mul_int,
      dvar_op_mul_double,
      dvar_op_mul_char,
      NULL,
      NULL
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator * cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_div(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_div TABLE[] = {
      NULL,
      dvar_op_div_int,
      dvar_op_div_double,
      dvar_op_div_char,
      NULL,
      NULL
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator / cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_mod(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_mod TABLE[] = {
      NULL,
      dvar_op_mod_int,
      NULL,
      dvar_op_mod_char,
      NULL,
      NULL
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator % cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_bplus(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_bplus TABLE[] = {
      NULL,
      dvar_op_bplus_int,
      dvar_op_bplus_double,
      dvar_op_bplus_char,
      NULL,
      dvar_op_bplus_string
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator + cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_bminus(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_bminus TABLE[] = {
      NULL,
      dvar_op_bminus_int,
      dvar_op_bminus_double,
      dvar_op_bminus_char,
      NULL,
      NULL
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator - cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_gt(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_gt TABLE[] = {
      dvar_op_gt_null,
      dvar_op_gt_int,
      dvar_op_gt_double,
      dvar_op_gt_char,
      dvar_op_gt_bool,
      dvar_op_gt_string
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator > cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_lt(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_lt TABLE[] = {
      dvar_op_lt_null,
      dvar_op_lt_int,
      dvar_op_lt_double,
      dvar_op_lt_char,
      dvar_op_lt_bool,
      dvar_op_lt_string
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator < cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_geq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_geq TABLE[] = {
      dvar_op_geq_null,
      dvar_op_geq_int,
      dvar_op_geq_double,
      dvar_op_geq_char,
      dvar_op_geq_bool,
      dvar_op_geq_string
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator >= cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_leq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_leq TABLE[] = {
      dvar_op_leq_null,
      dvar_op_leq_int,
      dvar_op_leq_double,
      dvar_op_leq_char,
      dvar_op_leq_bool,
      dvar_op_leq_string
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator <= cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_eq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_eq TABLE[] = {
      dvar_op_eq_null,
      dvar_op_eq_int,
      dvar_op_eq_double,
      dvar_op_eq_char,
      dvar_op_eq_bool,
      dvar_op_eq_string
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator == cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_neq(const dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_neq TABLE[] = {
      dvar_op_neq_null,
      dvar_op_neq_int,
      dvar_op_neq_double,
      dvar_op_neq_char,
      dvar_op_neq_bool,
      dvar_op_neq_string
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   char type = dvar_find_type_2_(a, b, (a_dvar_typed_fns) TABLE);

   errAssert(type != DVAR_TNOT,
             "Operator != cannot be applied to operands of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

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
      dvar_assign_var(&v1, a);
      dvar_cast_to(&v1, type);
      ac = &v1;
   }

   if(b->type == type)
   {
      bc = b;
   }
   else
   {
      dvar_init(&v2);
      dvar_assign_var(&v2, b);
      dvar_cast_to(&v2, type);
      bc = &v2;
   }

   TABLE[(size_t) type](ac, bc, res);

   if(a->type != type)
      dvar_erase(&v1);
   if(b->type != type)
      dvar_erase(&v2);
}

void dvar_op_land(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, (oat_bool)
                    (dvar_to_bool(a) && dvar_to_bool(b)));
}

void dvar_op_lor(const dvar* a, const dvar* b, dvar* res)
{
   dvar_assign_bool(res, (oat_bool)
                    (dvar_to_bool(a) || dvar_to_bool(b)));
}

void dvar_op_assign(dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_assign TABLE[] = {
      dvar_op_assign_null,
      dvar_op_assign_int,
      dvar_op_assign_double,
      dvar_op_assign_char,
      dvar_op_assign_bool,
      dvar_op_assign_string
   };

   TABLE[(size_t)b->type](a, b, res);
}

void dvar_op_pluseq(dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_pluseq TABLE[] = {
      NULL,
      dvar_op_pluseq_int,
      dvar_op_pluseq_double,
      dvar_op_pluseq_char,
      NULL,
      dvar_op_pluseq_string
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   errAssert(!!TABLE[(size_t)a->type]
             && dvar_has_implicit_cast(b->type, a->type),
             "Operator += cannot be applied to operand of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

   dvar bc;
   dvar_init(&bc);
   dvar_assign_var(&bc, b);
   dvar_cast_to(&bc, a->type);
   TABLE[(size_t) a->type](a, &bc, res);
   dvar_erase(&bc);
}

void dvar_op_minuseq(dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_minuseq TABLE[] = {
      NULL,
      dvar_op_minuseq_int,
      dvar_op_minuseq_double,
      dvar_op_minuseq_char,
      NULL,
      NULL
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   errAssert(!!TABLE[(size_t)a->type]
             && dvar_has_implicit_cast(b->type, a->type),
             "Operator -= cannot be applied to operand of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

   dvar bc;
   dvar_init(&bc);
   dvar_assign_var(&bc, b);
   dvar_cast_to(&bc, a->type);
   TABLE[(size_t) a->type](a, &bc, res);
   dvar_erase(&bc);
}

void dvar_op_muleq(dvar* a, const dvar* b, dvar* res)
{
      static const f_dvar_op_muleq TABLE[] = {
      NULL,
      dvar_op_muleq_int,
      dvar_op_muleq_double,
      dvar_op_muleq_char,
      NULL,
      NULL
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   errAssert(!!TABLE[(size_t)a->type]
             && dvar_has_implicit_cast(b->type, a->type),
             "Operator *= cannot be applied to operand of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

   dvar bc;
   dvar_init(&bc);
   dvar_assign_var(&bc, b);
   dvar_cast_to(&bc, a->type);
   TABLE[(size_t) a->type](a, &bc, res);
   dvar_erase(&bc);
}

void dvar_op_diveq(dvar* a, const dvar* b, dvar* res)
{
      static const f_dvar_op_diveq TABLE[] = {
      NULL,
      dvar_op_diveq_int,
      dvar_op_diveq_double,
      dvar_op_diveq_char,
      NULL,
      NULL
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   errAssert(!!TABLE[(size_t)a->type]
             && dvar_has_implicit_cast(b->type, a->type),
             "Operator /= cannot be applied to operand of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

   dvar bc;
   dvar_init(&bc);
   dvar_assign_var(&bc, b);
   dvar_cast_to(&bc, a->type);
   TABLE[(size_t) a->type](a, &bc, res);
   dvar_erase(&bc);
}

void dvar_op_modeq(dvar* a, const dvar* b, dvar* res)
{
   static const f_dvar_op_modeq TABLE[] = {
      NULL,
      dvar_op_modeq_int,
      NULL,
      dvar_op_modeq_char,
      NULL,
      NULL
   };

   if(a->type == b->type && TABLE[(size_t) a->type])
   {
      TABLE[(size_t) a->type](a, b, res);
      return;
   }

   errAssert(!!TABLE[(size_t)a->type]
             && dvar_has_implicit_cast(b->type, a->type),
             "Operator %= cannot be applied to operand of type %s and %s",
             dvar_to_type_string(a), dvar_to_type_string(b));

   dvar bc;
   dvar_init(&bc);
   dvar_assign_var(&bc, b);
   dvar_cast_to(&bc, a->type);
   TABLE[(size_t) a->type](a, &bc, res);
   dvar_erase(&bc);
}

void dvar_print(dvar* v)
{
   printf("{ <%s> (", DVAR_STYPES[(size_t)v->type]);

   if(v->type == DVAR_TNULL)
      printf("null");
   else if(v->type == DVAR_TINT)
      printf("%ld", (long) v->v_int);
   else if(v->type == DVAR_TDOUBLE)
      printf("%G", (double) v->v_double);
   else if(v->type == DVAR_TCHAR)
      printf("'%c'", v->v_char);
   else if(v->type == DVAR_TBOOL && v->v_bool == TRUE)
      printf("true");
   else if(v->type == DVAR_TBOOL && v->v_bool == FALSE)
      printf("false");
   else if(v->type == DVAR_TSTRING)
      printf("\"%s\"", v->v_string);
   else
      printf("???");

   printf(") }\n");
}
