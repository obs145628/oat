#ifndef BIN_BUILDER_H_
# define BIN_BUILDER_H_

# ifdef __cplusplus
extern "C"
{
#  if 0
}
#  endif
# endif

# include "parray.h"
# include "pmap.h"
# include "const.h"




typedef struct {
   s_parray* data_arr;
   s_parray* ins_arr;
   s_pmap* labels_map;
   const char* start_label;
} s_bin_builder;

typedef struct {
   t_vm_int type;
   union {
      const char* p_label;
      t_vm_addr p_addr;
      t_vm_saddr p_saddr;
      t_vm_int p_int;
      t_vm_double p_double;
      t_vm_char p_char;
      t_vm_bool p_bool;
   };
} s_bin_builder_arg;

s_bin_builder* bin_builder_new();
void bin_builder_free(s_bin_builder* b);

void bin_builder_add_data(s_bin_builder* b,
                          const char* label, const char* src, t_vm_addr len);
void bin_builder_add_int(s_bin_builder* b, const char* label, t_vm_int x);
void bin_builder_add_double(s_bin_builder* b, const char* label, t_vm_double x);
void bin_builder_add_char(s_bin_builder* b, const char* label, t_vm_char x);
void bin_builder_add_bool(s_bin_builder* b, const char* label, t_vm_bool x);
void bin_builder_add_string(s_bin_builder* b, const char* label, const char* x);
void bin_builder_add_space(s_bin_builder* b, const char* label, t_vm_addr len);
void bin_builder_add_var(s_bin_builder* b, const char* label);

void bin_builder_addi_nop(s_bin_builder* b, const char* label);
void bin_builder_addi_jump(s_bin_builder* b, const char* label,
                           const char* addrLabel);
void bin_builder_addi_cjump(s_bin_builder* b, const char* label,
                            t_vm_saddr saddr, const char* addrLabel);
void bin_builder_addi_fjump(s_bin_builder* b, const char* label,
                            const char* addrLabel, t_vm_saddr faddr);
void bin_builder_addi_fcall(s_bin_builder* b, const char* label,
                            t_vm_saddr saddr, t_vm_saddr pos, t_vm_int size);
void bin_builder_addi_fret(s_bin_builder* b, const char* label);
void bin_builder_addi_bclear(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_saddr size);
void bin_builder_addi_putnull(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr, t_vm_int mode);
void bin_builder_addi_putint(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,  t_vm_int value);
void bin_builder_addi_putdouble(s_bin_builder* b, const char* label,
                                t_vm_saddr saddr, t_vm_int mode,
                                t_vm_double value);
void bin_builder_addi_putchar(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr, t_vm_int mode,
                              t_vm_char value);
void bin_builder_addi_putbool(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr, t_vm_int mode,
                              t_vm_bool value);
void bin_builder_addi_putstring(s_bin_builder* b, const char* label,
                                t_vm_saddr saddr, t_vm_int mode,
                                const char* addrLabel, t_vm_int size);
void bin_builder_addi_putfunction(s_bin_builder* b, const char* label,
                                  t_vm_saddr saddr, t_vm_int mode,
                                  const char* addrLabel);
void bin_builder_addi_putsyscall(s_bin_builder* b, const char* label,
                                 t_vm_saddr saddr, t_vm_int mode,
                                 t_vm_int syscall);
void bin_builder_addi_putarr(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_saddr it, t_vm_int size);
void bin_builder_addi_putset(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_saddr it, t_vm_int size);
void bin_builder_addi_putmap(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_saddr it, t_vm_int size);
void bin_builder_addi_putobj(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_int id);
void bin_builder_addi_putclass(s_bin_builder* b, const char* label,
                               t_vm_saddr saddr, t_vm_int mode,
                               t_vm_int id);
void bin_builder_addi_putvar(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_saddr src);
void bin_builder_addi_putref(s_bin_builder* b, const char* label,
                             t_vm_saddr dst, t_vm_saddr src);
void bin_builder_addi_copy(s_bin_builder* b, const char* label,
                            t_vm_saddr dst, t_vm_saddr src);
void bin_builder_addi_move(s_bin_builder* b, const char* label,
                           t_vm_saddr dst, t_vm_saddr src);
void bin_builder_addi_spup(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr);
void bin_builder_addi_spdown(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr);
void bin_builder_addi_syscall(s_bin_builder* b, const char* label,
                              t_vm_int value);
void bin_builder_addi_bind(s_bin_builder* b, const char* label,
                           t_vm_saddr dst, t_vm_saddr it, t_vm_int size);
void bin_builder_addi_load(s_bin_builder* b, const char* label,
                           t_vm_saddr dst, const char* srcLabel);
void bin_builder_addi_store(s_bin_builder* b, const char* label,
                           t_vm_saddr src, const char* dstLabel);
void bin_builder_addi_init(s_bin_builder* b, const char* label,
                           t_vm_saddr src, const char* dstLabel);

void bin_builder_addi_postinc(s_bin_builder* b, const char* label,
                              t_vm_saddr a1, t_vm_saddr a2);
void bin_builder_addi_postdec(s_bin_builder* b, const char* label,
                              t_vm_saddr a1, t_vm_saddr a2);
void bin_builder_addi_preinc(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2);
void bin_builder_addi_predec(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2);
void bin_builder_addi_uplus(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2);
void bin_builder_addi_uminus(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2);
void bin_builder_addi_lnot(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2);
void bin_builder_addi_bnot(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2);
void bin_builder_addi_mul(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_div(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_mod(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_bplus(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_bminus(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_gt(s_bin_builder* b, const char* label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_lt(s_bin_builder* b, const char* label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_geq(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_leq(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_eq(s_bin_builder* b, const char* label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_neq(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_land(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_lor(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_lshift(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_rshift(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_band(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_bxor(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_bor(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_assign(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_pluseq(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_minuseq(s_bin_builder* b, const char* label,
                              t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_muleq(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_diveq(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_modeq(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_lshifteq(s_bin_builder* b, const char* label,
                               t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_rshiftqeq(s_bin_builder* b, const char* label,
                                t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_bandeq(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_bxoreq(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_boreq(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_subscript(s_bin_builder* b, const char* label,
                                t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);
void bin_builder_addi_ternary(s_bin_builder* b, const char* label,
                              t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3,
                              t_vm_saddr a4);
void bin_builder_addi_member(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, const char* strLabel,
                             t_vm_int size,  t_vm_saddr res);
void bin_builder_addi_defclass(s_bin_builder* b, const char* label,
                               const char* nameLabel, t_vm_int len,
                               t_vm_int id, t_vm_int parent);
void bin_builder_addi_deffield(s_bin_builder* b, const char* label,
                               t_vm_int id,
                               const char* nameLabel, t_vm_int len,
                               t_vm_int type, t_vm_saddr value);
void bin_builder_addi_defsfield(s_bin_builder* b, const char* label,
                                t_vm_int id,
                                const char* nameLabel, t_vm_int len,
                                t_vm_int type, t_vm_saddr value);
void bin_builder_addi_defend(s_bin_builder* b, const char* label,
                               t_vm_int id);
void bin_builder_addi_ssuper(s_bin_builder* b, const char* label,
                             t_vm_saddr dst);
void bin_builder_addi_setfscope(s_bin_builder* b, const char* label,
                                const char* nameLabel, t_vm_int len);
void bin_builder_addi_setfline(s_bin_builder* b, const char* label,
                               t_vm_int line);

void bin_builder_addia0(s_bin_builder* b, const char* label, t_vm_ins code);
void bin_builder_addia1(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1);
void bin_builder_addia2(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2);
void bin_builder_addia3(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2,
                        s_bin_builder_arg a3);
void bin_builder_addia4(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2,
                        s_bin_builder_arg a3, s_bin_builder_arg a4);
void bin_builder_addia5(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2,
                        s_bin_builder_arg a3, s_bin_builder_arg a4,
                        s_bin_builder_arg a5);

void bin_builder_add_opu(s_bin_builder* b, const char* label, t_vm_ins code,
                         t_vm_saddr a1, t_vm_saddr a2);
void bin_builder_add_opb(s_bin_builder* b, const char* label, t_vm_ins code,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3);

int bin_builder_save(s_bin_builder* b, const char* outPath);

# ifdef __cplusplus
}
# endif

#endif //!BIN_BUILDER_H_
