#ifndef DVAR_H_
# define DVAR_H_

# include "const.h"

# define DVAR_TNOT (-1)
# define DVAR_TNULL (0)
# define DVAR_TINT (1)
# define DVAR_TDOUBLE (2)
# define DVAR_TCHAR (3)
# define DVAR_TBOOL (4)
# define DVAR_TSTRING (5)

# define DVAR_NB_TYPES (6)

struct dvar_elem {
   char type;
   union {
      oat_int v_int;
      oat_double v_double;
      char v_char;
      oat_bool v_bool;
      char* v_string;
   };
};

typedef struct dvar_elem dvar;

typedef void (*f_dvar_cast)(dvar* v);
typedef oat_bool (*f_dvar_to_bool)(const dvar* v);
typedef char* (*f_dvar_to_string)(const dvar* v);
typedef void (*f_dvar_opu)(dvar* a, dvar* res);
typedef void (*f_dvar_opb)(dvar* a, dvar* b, dvar* res);


typedef const void** a_dvar_typed_fns;

typedef void (*f_dvar_op_postinc)(dvar* a, dvar* res);
typedef void (*f_dvar_op_postdec)(dvar* a, dvar* res);
typedef void (*f_dvar_op_preinc)(dvar* a, dvar* res);
typedef void (*f_dvar_op_predec)(dvar* a, dvar* res);
typedef void (*f_dvar_op_uplus)(const dvar* a, dvar* res);
typedef void (*f_dvar_op_uminus)(const dvar* a, dvar* res);
typedef void (*f_dvar_op_lnot)(const dvar* a, dvar* res);
typedef void (*f_dvar_op_mul)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_div)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_mod)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_bplus)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_bminus)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_gt)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_lt)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_geq)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_leq)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_eq)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_neq)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_land)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_lor)(const dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_assign)(dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_pluseq)(dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_minuseq)(dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_muleq)(dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_diveq)(dvar* a, const dvar* b, dvar* res);
typedef void (*f_dvar_op_modeq)(dvar* a, const dvar* b, dvar* res);



void dvar_init(dvar* v);
void dvar_erase(dvar* v);
void dvar_assign_null(dvar* v);
void dvar_assign_int(dvar* v, oat_int x);
void dvar_assign_double(dvar* v, oat_double x);
void dvar_assign_char(dvar* v, char x);
void dvar_assign_bool(dvar* v, oat_bool x);
void dvar_assign_string(dvar* v, const char* x, size_t len);
void dvar_assign_string_move(dvar* v, char* x);
void dvar_assign_var(dvar* v, const dvar* x);

oat_bool dvar_to_bool_null(const dvar* v);
oat_bool dvar_to_bool_int(const dvar* v);
oat_bool dvar_to_bool_double(const dvar* v);
oat_bool dvar_to_bool_char(const dvar* v);
oat_bool dvar_to_bool_bool(const dvar* v);
oat_bool dvar_to_bool_string(const dvar* v);
oat_bool dvar_to_bool(const dvar* v);

char* dvar_to_string_null(const dvar* v);
char* dvar_to_string_int(const dvar* v);
char* dvar_to_string_double(const dvar* v);
char* dvar_to_string_char(const dvar* v);
char* dvar_to_string_bool(const dvar* v);
char* dvar_to_string_string(const dvar* v);
char* dvar_to_string(const dvar* v);

const char* dvar_to_type_string(const dvar* v);
const char* dvar_type_to_string(char type);

void dvar_cast_to(dvar* v, char type);
void dvar_cast_implicit_to(dvar* v, char type);
int dvar_has_implicit_cast(char from, char to);

void dvar_cast_null_to_int(dvar* v);
void dvar_cast_null_to_double(dvar* v);
void dvar_cast_null_to_char(dvar* v);
void dvar_cast_null_to_bool(dvar* v);
void dvar_cast_null_to_string(dvar* v);
void dvar_cast_int_to_double(dvar* v);
void dvar_cast_int_to_char(dvar* v);
void dvar_cast_int_to_bool(dvar* v);
void dvar_cast_int_to_string(dvar* v);
void dvar_cast_double_to_int(dvar* v);
void dvar_cast_double_to_char(dvar* v);
void dvar_cast_double_to_bool(dvar* v);
void dvar_cast_double_to_string(dvar* v);
void dvar_cast_char_to_int(dvar* v);
void dvar_cast_char_to_double(dvar* v);
void dvar_cast_char_to_bool(dvar* v);
void dvar_cast_char_to_string(dvar* v);
void dvar_cast_bool_to_int(dvar* v);
void dvar_cast_bool_to_double(dvar* v);
void dvar_cast_bool_to_char(dvar* v);
void dvar_cast_bool_to_string(dvar* v);
void dvar_cast_string_to_int(dvar* v);
void dvar_cast_string_to_double(dvar* v);
void dvar_cast_string_to_char(dvar* v);
void dvar_cast_string_to_bool(dvar* v);

void dvar_op_postinc_int(dvar* a, dvar* res);
void dvar_op_postinc_double(dvar* a, dvar* res);
void dvar_op_postinc_char(dvar* a, dvar* res);
void dvar_op_postdec_int(dvar* a, dvar* res);
void dvar_op_postdec_double(dvar* a, dvar* res);
void dvar_op_postdec_char(dvar* a, dvar* res);
void dvar_op_preinc_int(dvar* a, dvar* res);
void dvar_op_preinc_double(dvar* a, dvar* res);
void dvar_op_preinc_char(dvar* a, dvar* res);
void dvar_op_predec_int(dvar* a, dvar* res);
void dvar_op_predec_double(dvar* a, dvar* res);
void dvar_op_predec_char(dvar* a, dvar* res);
void dvar_op_uplus_int(const dvar* a, dvar* res);
void dvar_op_uplus_double(const dvar* a, dvar* res);
void dvar_op_uplus_char(const dvar* a, dvar* res);
void dvar_op_uminus_int(const dvar* a, dvar* res);
void dvar_op_uminus_double(const dvar* a, dvar* res);
void dvar_op_uminus_char(const dvar* a, dvar* res);
void dvar_op_mul_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_mul_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_mul_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_div_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_div_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_div_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_mod_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_mod_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_bplus_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_bplus_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_bplus_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_bplus_string(const dvar* a, const dvar* b, dvar* res);
void dvar_op_bminus_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_bminus_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_bminus_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_gt_null(const dvar* a, const dvar* b, dvar* res);
void dvar_op_gt_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_gt_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_gt_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_gt_bool(const dvar* a, const dvar* b, dvar* res);
void dvar_op_gt_string(const dvar* a, const dvar* b, dvar* res);
void dvar_op_lt_null(const dvar* a, const dvar* b, dvar* res);
void dvar_op_lt_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_lt_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_lt_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_lt_bool(const dvar* a, const dvar* b, dvar* res);
void dvar_op_lt_string(const dvar* a, const dvar* b, dvar* res);
void dvar_op_geq_null(const dvar* a, const dvar* b, dvar* res);
void dvar_op_geq_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_geq_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_geq_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_geq_bool(const dvar* a, const dvar* b, dvar* res);
void dvar_op_geq_string(const dvar* a, const dvar* b, dvar* res);
void dvar_op_leq_null(const dvar* a, const dvar* b, dvar* res);
void dvar_op_leq_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_leq_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_leq_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_leq_bool(const dvar* a, const dvar* b, dvar* res);
void dvar_op_leq_string(const dvar* a, const dvar* b, dvar* res);
void dvar_op_eq_null(const dvar* a, const dvar* b, dvar* res);
void dvar_op_eq_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_eq_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_eq_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_eq_bool(const dvar* a, const dvar* b, dvar* res);
void dvar_op_eq_string(const dvar* a, const dvar* b, dvar* res);
void dvar_op_neq_null(const dvar* a, const dvar* b, dvar* res);
void dvar_op_neq_int(const dvar* a, const dvar* b, dvar* res);
void dvar_op_neq_double(const dvar* a, const dvar* b, dvar* res);
void dvar_op_neq_char(const dvar* a, const dvar* b, dvar* res);
void dvar_op_neq_bool(const dvar* a, const dvar* b, dvar* res);
void dvar_op_neq_string(const dvar* a, const dvar* b, dvar* res);
void dvar_op_assign_null(dvar* a, const dvar* b, dvar* res);
void dvar_op_assign_int(dvar* a, const dvar* b, dvar* res);
void dvar_op_assign_double(dvar* a, const dvar* b, dvar* res);
void dvar_op_assign_char(dvar* a, const dvar* b, dvar* res);
void dvar_op_assign_bool(dvar* a, const dvar* b, dvar* res);
void dvar_op_assign_string(dvar* a, const dvar* b, dvar* res);
void dvar_op_pluseq_int(dvar* a, const dvar* b, dvar* res);
void dvar_op_pluseq_double(dvar* a, const dvar* b, dvar* res);
void dvar_op_pluseq_char(dvar* a, const dvar* b, dvar* res);
void dvar_op_pluseq_string(dvar* a, const dvar* b, dvar* res);
void dvar_op_minuseq_int(dvar* a, const dvar* b, dvar* res);
void dvar_op_minuseq_double(dvar* a, const dvar* b, dvar* res);
void dvar_op_minuseq_char(dvar* a, const dvar* b, dvar* res);
void dvar_op_muleq_int(dvar* a, const dvar* b, dvar* res);
void dvar_op_muleq_double(dvar* a, const dvar* b, dvar* res);
void dvar_op_muleq_char(dvar* a, const dvar* b, dvar* res);
void dvar_op_diveq_int(dvar* a, const dvar* b, dvar* res);
void dvar_op_diveq_double(dvar* a, const dvar* b, dvar* res);
void dvar_op_diveq_char(dvar* a, const dvar* b, dvar* res);
void dvar_op_modeq_int(dvar* a, const dvar* b, dvar* res);
void dvar_op_modeq_char(dvar* a, const dvar* b, dvar* res);

void dvar_op_postinc(dvar* a, dvar* res);
void dvar_op_postdec(dvar* a, dvar* res);
void dvar_op_preinc(dvar* a, dvar* res);
void dvar_op_predec(dvar* a, dvar* res);
void dvar_op_uplus(const dvar* a, dvar* res);
void dvar_op_uminus(const dvar* a, dvar* res);
void dvar_op_lnot(const dvar* a, dvar* res);
void dvar_op_mul(const dvar* a, const dvar* b, dvar* res);
void dvar_op_div(const dvar* a, const dvar* b, dvar* res);
void dvar_op_mod(const dvar* a, const dvar* b, dvar* res);
void dvar_op_bplus(const dvar* a, const dvar* b, dvar* res);
void dvar_op_bminus(const dvar* a, const dvar* b, dvar* res);
void dvar_op_gt(const dvar* a, const dvar* b, dvar* res);
void dvar_op_lt(const dvar* a, const dvar* b, dvar* res);
void dvar_op_geq(const dvar* a, const dvar* b, dvar* res);
void dvar_op_leq(const dvar* a, const dvar* b, dvar* res);
void dvar_op_eq(const dvar* a, const dvar* b, dvar* res);
void dvar_op_neq(const dvar* a, const dvar* b, dvar* res);
void dvar_op_land(const dvar* a, const dvar* b, dvar* res);
void dvar_op_lor(const dvar* a, const dvar* b, dvar* res);
void dvar_op_assign(dvar* a, const dvar* b, dvar* res);
void dvar_op_pluseq(dvar* a, const dvar* b, dvar* res);
void dvar_op_minuseq(dvar* a, const dvar* b, dvar* res);
void dvar_op_muleq(dvar* a, const dvar* b, dvar* res);
void dvar_op_diveq(dvar* a, const dvar* b, dvar* res);
void dvar_op_modeq(dvar* a, const dvar* b, dvar* res);




void dvar_print(dvar* v);

#endif //!DVAR_H_
