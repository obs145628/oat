#ifndef DVAR_H_
# define DVAR_H_

# ifdef __cplusplus
extern "C"
{
#  if 0
}
#  endif
# endif

# include "const.h"

# define DVAR_TNOT (-1)
# define DVAR_TNULL (0)
# define DVAR_TINT (1)
# define DVAR_TDOUBLE (2)
# define DVAR_TCHAR (3)
# define DVAR_TBOOL (4)
# define DVAR_TSTR (5)
# define DVAR_TFUN (6)

# define DVAR_TREF (88)

# define DVAR_MVAR (1)
# define DVAR_MTCONST (2)
# define DVAR_MCONST (3)

# define DVAR_NB_TYPES (7)

struct dvar_fun;
struct dvar_str;

struct dvar {
   t_vm_type type;
   t_vm_mode mode;
   union {
      t_vm_int v_int;
      t_vm_double v_double;
      t_vm_char v_char;
      t_vm_bool v_bool;
      struct dvar_str* v_str;
      struct dvar_fun* v_fun;
      struct dvar* v_ref;
   };
};
typedef struct dvar dvar;

typedef void (*f_dvar_a1)(dvar* v);
typedef void (*f_dvar_a2)(dvar* v1, dvar* v2);
typedef void (*f_dvar_a3)(dvar* v1, dvar* v2, dvar* v3);
typedef void (*f_dvar_a4)(dvar* v1, dvar* v2, dvar* v3, dvar* v4);


void dvar_init(dvar* v);
void dvar_init_null(dvar* v, t_vm_mode mode);
void dvar_init_int(dvar* v, t_vm_mode mode, t_vm_int x);
void dvar_init_double(dvar* v, t_vm_mode mode, t_vm_double x);
void dvar_init_char(dvar* v, t_vm_mode mode, t_vm_char x);
void dvar_init_bool(dvar* v, t_vm_mode mode, t_vm_bool x);
void dvar_init_str(dvar* v, t_vm_mode mode, const char* x, t_vm_int len);
void dvar_init_function(dvar* v, t_vm_mode mode, t_vm_addr addr);
void dvar_init_syscall(dvar* v, t_vm_mode mode, t_vm_int syscall);

void dvar_clear(dvar* v);
void dvar_bclear(dvar* begin, dvar* end);
t_vm_bool dvar_to_bool(const dvar* v);
char* dvar_to_str(const dvar* v);


void dvar_putnull(dvar* v, t_vm_mode mode);
void dvar_putint(dvar* v, t_vm_mode mode, t_vm_int x);
void dvar_putdouble(dvar* v, t_vm_mode mode, t_vm_double x);
void dvar_putchar(dvar* v, t_vm_mode mode, t_vm_char x);
void dvar_putbool(dvar* v, t_vm_mode mode, t_vm_bool x);
void dvar_putstring(dvar* v, t_vm_mode mode, const char* x, t_vm_int len);
void dvar_putfunction(dvar* v, t_vm_mode mode, t_vm_addr addr);
void dvar_putsyscall(dvar* v, t_vm_mode mode, t_vm_int syscall);
void dvar_putvar(dvar* v, t_vm_mode mode, dvar* src);
void dvar_putref(dvar* dst, dvar* src);

void dvar_copy(dvar* dst, const dvar* src);
void dvar_move(dvar* dst, dvar* src);
void dvar_bind(dvar* dst, const dvar* begin, const dvar* end);

void dvar_postinc(dvar* op, dvar* res);
void dvar_postdec(dvar* op, dvar* res);
void dvar_preinc(dvar* op, dvar* res);
void dvar_predec(dvar* op, dvar* res);
void dvar_uplus(const dvar* op, dvar* res);
void dvar_uminus(const dvar* op, dvar* res);
void dvar_lnot(const dvar* op, dvar* res);
void dvar_bnot(const dvar* op, dvar* res);

void dvar_mul(const dvar* a, const dvar* b, dvar* res);
void dvar_div(const dvar* a, const dvar* b, dvar* res);
void dvar_mod(const dvar* a, const dvar* b, dvar* res);
void dvar_bplus(const dvar* a, const dvar* b, dvar* res);
void dvar_bminus(const dvar* a, const dvar* b, dvar* res);
void dvar_gt(const dvar* a, const dvar* b, dvar* res);
void dvar_lt(const dvar* a, const dvar* b, dvar* res);
void dvar_geq(const dvar* a, const dvar* b, dvar* res);
void dvar_leq(const dvar* a, const dvar* b, dvar* res);
void dvar_eq(const dvar* a, const dvar* b, dvar* res);
void dvar_neq(const dvar* a, const dvar* b, dvar* res);
void dvar_land(const dvar* a, const dvar* b, dvar* res);
void dvar_lor(const dvar* a, const dvar* b, dvar* res);
void dvar_lshift(const dvar* a, const dvar* b, dvar* res);
void dvar_rshift(const dvar* a, const dvar* b, dvar* res);
void dvar_band(const dvar* a, const dvar* b, dvar* res);
void dvar_bxor(const dvar* a, const dvar* b, dvar* res);
void dvar_bor(const dvar* a, const dvar* b, dvar* res);
void dvar_assign(dvar* a, const dvar* b, dvar* res);
void dvar_pluseq(dvar* a, const dvar* b, dvar* res);
void dvar_minuseq(dvar* a, const dvar* b, dvar* res);
void dvar_muleq(dvar* a, const dvar* b, dvar* res);
void dvar_diveq(dvar* a, const dvar* b, dvar* res);
void dvar_modeq(dvar* a, const dvar* b, dvar* res);
void dvar_lshifteq(dvar* a, const dvar* b, dvar* res);
void dvar_rshifteq(dvar* a, const dvar* b, dvar* res);
void dvar_bandeq(dvar* a, const dvar* b, dvar* res);
void dvar_bxoreq(dvar* a, const dvar* b, dvar* res);
void dvar_boreq(dvar* a, const dvar* b, dvar* res);

void dvar_ternary(const dvar* a, const dvar* b, const dvar* c, dvar* d);
void dvar_subscript(const dvar* a, const dvar* b, dvar* c);
void dvar_member(const dvar* v, const char* str, t_vm_int len, dvar* res);

void dvar_print(const dvar* a);


# ifdef __cplusplus
}
# endif

#endif //!DVAR_H_
