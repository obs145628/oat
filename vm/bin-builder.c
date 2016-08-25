#include "bin-builder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "err.h"
#include "fs.h"
#include "dvar.h"

# define NO_ADDR ((t_vm_addr) -1)

typedef struct {
   const char* label;
   t_vm_addr len;
   t_vm_addr addr;
   char* data;
} s_bin_builder_data;

typedef struct {
   const char* label;
   t_vm_ins code;
   t_vm_addr addr;
   s_bin_builder_arg a1;
   s_bin_builder_arg a2;
   s_bin_builder_arg a3;
   s_bin_builder_arg a4;
   s_bin_builder_arg a5;
} s_bin_builder_ins;

typedef struct {
   const char* label;
   t_vm_addr addr;
} s_bin_builder_label_ref;

static s_bin_builder_arg arg_label_(const char* label)
{
   s_bin_builder_arg a;
   a.type = 0;
   a.p_label = label;
   return a;
}

static s_bin_builder_arg arg_saddr_(t_vm_saddr saddr)
{
   s_bin_builder_arg a;
   a.type = 2;
   a.p_saddr = saddr;
   return a;
}

static s_bin_builder_arg arg_int_(t_vm_int x)
{
   s_bin_builder_arg a;
   a.type = 3;
   a.p_int = x;
   return a;
}

static s_bin_builder_arg arg_double_(t_vm_double x)
{
   s_bin_builder_arg a;
   a.type = 4;
   a.p_double = x;
   return a;
}

static s_bin_builder_arg arg_char_(t_vm_char x)
{
   s_bin_builder_arg a;
   a.type = 5;
   a.p_char = x;
   return a;
}

static s_bin_builder_arg arg_bool_(t_vm_bool x)
{
   s_bin_builder_arg a;
   a.type = 6;
   a.p_bool = x;
   return a;
}


static s_bin_builder_data* bin_builder_data_new_(const char* label,
                                                 const char* src,
                                                 t_vm_addr len)
{
   s_bin_builder_data* bd = malloc(sizeof(s_bin_builder_data));
   bd->label = label;
   bd->len = len;
   bd->data = malloc(len);
   bd->addr = NO_ADDR;
   memcpy(bd->data, src, len);
   return bd;
}

static s_bin_builder_ins* bin_builder_ins_new_(const char* label, t_vm_ins code)
{
   s_bin_builder_ins* ins = malloc((sizeof(s_bin_builder_ins)));
   ins->label = label;
   ins->code = code;
   ins->addr = NO_ADDR;
   return ins;
}

static s_bin_builder_label_ref* bin_builder_label_ref_new_(const char* label,
                                                           t_vm_addr addr)
{
   s_bin_builder_label_ref* ref = malloc(sizeof(s_bin_builder_label_ref));
   ref->label = label;
   ref->addr = addr;
   return ref;
}

static s_bin_builder_ins* bin_builder_addi_(s_bin_builder* b,
                                            const char* label, t_vm_ins code)
{
   s_bin_builder_ins* ins = bin_builder_ins_new_(label, code);
   ins->a1.type = -1;
   ins->a2.type = -1;
   ins->a3.type = -1;
   ins->a4.type = -1;
   ins->a5.type = -1;
   parray_insert_back(b->ins_arr, ins);
   if(label)
   {
      errAssert(!pmap_contains(b->labels_map, (void*) label),
                "label %s already defined", label);
      pmap_insert(b->labels_map, (void*) label, &(ins->addr));
   }
   return ins;
}

static t_vm_addr binary_size_(s_bin_builder* b)
{
   t_vm_addr size = 0;
   s_bin_builder_data* data;
   s_bin_builder_ins* ins;

   size += sizeof(t_vm_addr);

   for(size_t i = 0; i < parray_size(b->data_arr); ++i)
   {
      data = (s_bin_builder_data*) parray_get(b->data_arr, i);
      size += data->len;
   }

   for(size_t i = 0; i < parray_size(b->ins_arr); ++i)
   {
      ins = (s_bin_builder_ins*) parray_get(b->ins_arr, i);
      size += VM_INS_INFOS[ins->code].size;
   }

   return size;
}

static t_vm_addr write_data_(s_bin_builder* b, char* buffer)
{
   t_vm_addr addr = sizeof(t_vm_addr);
   s_bin_builder_data* data;

   for(size_t i = 0; i < parray_size(b->data_arr); ++i)
   {
      data = (s_bin_builder_data*) parray_get(b->data_arr, i);
      data->addr = addr;
      memcpy(buffer + addr, data->data, data->len);
      addr += data->len;
   }

   return addr;
}

static t_vm_addr write_ins_arg_(s_bin_builder_arg arg, char* buffer,
                                t_vm_addr addr, s_parray* labels_refs)
{
   if(arg.type == 0) //label
   {
      parray_insert_back(labels_refs,
                         bin_builder_label_ref_new_(arg.p_label, addr));
      return addr + sizeof(t_vm_addr);
   }

   else if(arg.type == 1) //addr
   {
      memcpy(buffer + addr, &(arg.p_addr), sizeof(t_vm_addr));
      return addr + sizeof(t_vm_addr);
   }

   else if(arg.type == 2) //saddr
   {
      memcpy(buffer + addr, &(arg.p_saddr), sizeof(t_vm_saddr));
      return addr + sizeof(t_vm_saddr);
   }

   else if(arg.type == 3) //int
   {
      memcpy(buffer + addr, &(arg.p_int), sizeof(t_vm_int));
      return addr + sizeof(t_vm_int);
   }

   else if(arg.type == 4) //double
   {
      memcpy(buffer + addr, &(arg.p_double), sizeof(t_vm_double));
      return addr + sizeof(t_vm_double);
   }

   else if(arg.type == 5) //char
   {
      memcpy(buffer + addr, &(arg.p_char), sizeof(t_vm_char));
      return addr + sizeof(t_vm_char);
   }

   else if(arg.type == 6) //bool
   {
      memcpy(buffer + addr, &(arg.p_bool), sizeof(t_vm_bool));
      return addr + sizeof(t_vm_bool);
   }

   else
   {
      return addr;
   }
}

static t_vm_addr write_ins_(s_bin_builder* b, char* buffer, t_vm_addr addr,
                            s_parray* labels_refs)
{
   s_bin_builder_ins* ins;

   for(size_t i = 0; i < parray_size(b->ins_arr); ++i)
   {
      ins = (s_bin_builder_ins*) parray_get(b->ins_arr, i);
      ins->addr = addr;
      t_vm_ins code = ins->code;

      memcpy(buffer + addr, &code, sizeof(t_vm_ins));
      addr += sizeof(t_vm_ins);

      addr = write_ins_arg_(ins->a1, buffer, addr, labels_refs);
      addr = write_ins_arg_(ins->a2, buffer, addr, labels_refs);
      addr = write_ins_arg_(ins->a3, buffer, addr, labels_refs);
      addr = write_ins_arg_(ins->a4, buffer, addr, labels_refs);
      addr = write_ins_arg_(ins->a5, buffer, addr, labels_refs);
   }

   return addr;
}

static void write_refs_(s_bin_builder* b, char* buffer, s_parray* labels_refs)
{
   s_bin_builder_label_ref* ref;

   for(size_t i = 0; i < parray_size(labels_refs); ++i)
   {
      ref = (s_bin_builder_label_ref*) parray_get(labels_refs, i);

      errAssert(pmap_contains(b->labels_map,(void*) ref->label),
                "Undefined label: %s\n", ref->label);
      t_vm_addr* data = pmap_get(b->labels_map, (void*) ref->label);
      memcpy(buffer + ref->addr, data, sizeof(t_vm_addr));

      //printf("w ref %s %zu => %zu\n", ref->label, (size_t) ref->addr, (size_t) *data);;
   }
}


s_bin_builder* bin_builder_new()
{
   s_bin_builder* b = malloc(sizeof(s_bin_builder));
   b->data_arr = parray_new();
   b->ins_arr = parray_new();
   b->labels_map = pmap_new((f_pmap_cmp) strcmp);
   b->start_label = "_main";
   return b;
}

void bin_builder_free(s_bin_builder* b)
{
   parray_foreach(b->data_arr, free);
   parray_free(b->data_arr);
   parray_foreach(b->ins_arr, free);
   parray_free(b->ins_arr);
   pmap_free(b->labels_map);
   free(b);
}

void bin_builder_add_data(s_bin_builder* b,
                       const char* label, const char* src, t_vm_addr len)
{
   s_bin_builder_data* bd = bin_builder_data_new_(label, src, len);
   parray_insert_back(b->data_arr, bd);

   if(label)
   {
      errAssert(!pmap_contains(b->labels_map, (void*) label),
                "label %s already defined", label);
      pmap_insert(b->labels_map, (void*) label, &(bd->addr));
   }
}

void bin_builder_add_int(s_bin_builder* b, const char* label, t_vm_int x)
{
   bin_builder_add_data(b, label, (const char*) (&x), sizeof(t_vm_int));
}

void bin_builder_add_double(s_bin_builder* b, const char* label, t_vm_double x)
{
   bin_builder_add_data(b, label, (const char*) (&x), sizeof(t_vm_double));
}

void bin_builder_add_char(s_bin_builder* b, const char* label, t_vm_char x)
{
   bin_builder_add_data(b, label, (const char*) (&x), sizeof(t_vm_char));
}

void bin_builder_add_bool(s_bin_builder* b, const char* label, t_vm_bool x)
{
   bin_builder_add_data(b, label, (const char*) (&x), sizeof(t_vm_bool));
}

void bin_builder_add_string(s_bin_builder* b, const char* label, const char* x)
{
   bin_builder_add_data(b, label, x, (t_vm_addr) strlen(x));
}

void bin_builder_add_space(s_bin_builder* b, const char* label, t_vm_addr len)
{
   char* src = calloc(1, len);
   bin_builder_add_data(b, label, src, len);
   free(src);
}

void bin_builder_add_var(s_bin_builder* b, const char* label)
{
   bin_builder_add_space(b, label, 3 * sizeof(dvar));
}


void bin_builder_addi_nop(s_bin_builder* b, const char* label)
{
   bin_builder_addi_(b, label, VM_INS_NOP);
}

void bin_builder_addi_jump(s_bin_builder* b, const char* label,
                           const char* addrLabel)
{
   bin_builder_addia1(b, label, VM_INS_JUMP, arg_label_(addrLabel));
}

void bin_builder_addi_cjump(s_bin_builder* b, const char* label,
                            t_vm_saddr saddr, const char* addrLabel)
{
   bin_builder_addia2(b, label, VM_INS_CJUMP,
                      arg_saddr_(saddr), arg_label_(addrLabel));
}
void bin_builder_addi_fjump(s_bin_builder* b, const char* label,
                            const char* addrLabel, t_vm_saddr saddr)
{
   bin_builder_addia2(b, label, VM_INS_FJUMP,
                      arg_label_(addrLabel), arg_saddr_(saddr));
}

void bin_builder_addi_fcall(s_bin_builder* b, const char* label,
                            t_vm_saddr saddr, t_vm_saddr pos, t_vm_int size)
{
   bin_builder_addia3(b, label, VM_INS_FCALL,
                      arg_saddr_(saddr), arg_saddr_(pos), arg_int_(size));
}

void bin_builder_addi_fret(s_bin_builder* b, const char* label)
{
   bin_builder_addi_(b, label, VM_INS_FRET);
}

void bin_builder_addi_bclear(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_saddr size)
{
   bin_builder_addia2(b, label, VM_INS_BCLEAR,
                      arg_saddr_(saddr), arg_saddr_(size));
}

void bin_builder_addi_putnull(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr, t_vm_int mode)
{
   bin_builder_addia2(b, label, VM_INS_PUTNULL,
                      arg_saddr_(saddr), arg_int_(mode));
}

void bin_builder_addi_putint(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,  t_vm_int value)
{
   bin_builder_addia3(b, label, VM_INS_PUTINT,
                      arg_saddr_(saddr), arg_int_(mode), arg_int_(value));
}

void bin_builder_addi_putdouble(s_bin_builder* b, const char* label,
                                t_vm_saddr saddr, t_vm_int mode,
                                t_vm_double value)
{
   bin_builder_addia3(b, label, VM_INS_PUTDOUBLE,
                      arg_saddr_(saddr), arg_int_(mode), arg_double_(value));
}

void bin_builder_addi_putchar(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr, t_vm_int mode,
                              t_vm_char value)
{
   bin_builder_addia3(b, label, VM_INS_PUTCHAR,
                      arg_saddr_(saddr), arg_int_(mode), arg_char_(value));
}

void bin_builder_addi_putbool(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr, t_vm_int mode,
                              t_vm_bool value)
{
   bin_builder_addia3(b, label, VM_INS_PUTBOOL,
                      arg_saddr_(saddr), arg_int_(mode), arg_bool_(value));
}

void bin_builder_addi_putstring(s_bin_builder* b, const char* label,
                                t_vm_saddr saddr, t_vm_int mode,
                                const char* addrLabel, t_vm_int size)
{
   bin_builder_addia4(b, label, VM_INS_PUTSTRING,
                      arg_saddr_(saddr), arg_int_(mode), arg_label_(addrLabel),
                      arg_int_(size));
}

void bin_builder_addi_putfunction(s_bin_builder* b, const char* label,
                                  t_vm_saddr saddr, t_vm_int mode,
                                  const char* addrLabel)
{
      bin_builder_addia3(b, label, VM_INS_PUTFUNCTION,
                         arg_saddr_(saddr), arg_int_(mode),
                         arg_label_(addrLabel));
}

void bin_builder_addi_putsyscall(s_bin_builder* b, const char* label,
                                 t_vm_saddr saddr, t_vm_int mode,
                                 t_vm_int syscall)
{
   bin_builder_addia3(b, label, VM_INS_PUTSYSCALL,
                      arg_saddr_(saddr), arg_int_(mode), arg_int_(syscall));
}

void bin_builder_addi_putarr(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_saddr it, t_vm_int size)
{
   bin_builder_addia4(b, label, VM_INS_PUTARR,
                      arg_saddr_(saddr), arg_int_(mode),
                      arg_saddr_(it), arg_int_(size));
}

void bin_builder_addi_putset(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_saddr it, t_vm_int size)
{
   bin_builder_addia4(b, label, VM_INS_PUTSET,
                      arg_saddr_(saddr), arg_int_(mode),
                      arg_saddr_(it), arg_int_(size));
}

void bin_builder_addi_putmap(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_saddr it, t_vm_int size)
{
   bin_builder_addia4(b, label, VM_INS_PUTMAP,
                      arg_saddr_(saddr), arg_int_(mode),
                      arg_saddr_(it), arg_int_(size));
}

void bin_builder_addi_putobj(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_int id)
{
   bin_builder_addia3(b, label, VM_INS_PUTOBJ,
                      arg_saddr_(saddr), arg_int_(mode),
                      arg_int_(id));
}

void bin_builder_addi_putclass(s_bin_builder* b, const char* label,
                               t_vm_saddr saddr, t_vm_int mode,
                               t_vm_int id)
{
   bin_builder_addia3(b, label, VM_INS_PUTCLASS,
                      arg_saddr_(saddr), arg_int_(mode),
                      arg_int_(id));
}

void bin_builder_addi_putvar(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int mode,
                             t_vm_saddr src)
{
   bin_builder_addia3(b, label, VM_INS_PUTVAR,
                      arg_saddr_(saddr), arg_int_(mode), arg_saddr_(src));
}

void bin_builder_addi_putref(s_bin_builder* b, const char* label,
                             t_vm_saddr dst, t_vm_saddr src)
{
   bin_builder_addia2(b, label, VM_INS_PUTREF,
                      arg_saddr_(dst), arg_saddr_(src));
}

void bin_builder_addi_copy(s_bin_builder* b, const char* label,
                            t_vm_saddr dst, t_vm_saddr src)
{
      bin_builder_addia2(b, label, VM_INS_COPY,
                         arg_saddr_(dst), arg_saddr_(src));
}

void bin_builder_addi_move(s_bin_builder* b, const char* label,
                           t_vm_saddr dst, t_vm_saddr src)
{
      bin_builder_addia2(b, label, VM_INS_MOVE,
                         arg_saddr_(dst), arg_saddr_(src));
}


void bin_builder_addi_spup(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr)
{
   bin_builder_addia1(b, label, VM_INS_SPUP,
                      arg_saddr_(saddr));
}

void bin_builder_addi_spdown(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr)
{
   bin_builder_addia1(b, label, VM_INS_SPDOWN,
                      arg_saddr_(saddr));
}

void bin_builder_addi_syscall(s_bin_builder* b, const char* label,
                              t_vm_int value)
{
   bin_builder_addia1(b, label, VM_INS_SYSCALL,
                      arg_int_(value));
}

void bin_builder_addi_bind(s_bin_builder* b, const char* label,
                           t_vm_saddr dst, t_vm_saddr it, t_vm_int size)
{
   bin_builder_addia3(b, label, VM_INS_BIND,
                      arg_saddr_(dst), arg_saddr_(it), arg_int_(size));
}

void bin_builder_addi_load(s_bin_builder* b, const char* label,
                           t_vm_saddr dst, const char* srcLabel)
{
   bin_builder_addia2(b, label, VM_INS_LOAD,
                      arg_saddr_(dst), arg_label_(srcLabel));
}

void bin_builder_addi_store(s_bin_builder* b, const char* label,
                           t_vm_saddr src, const char* dstLabel)
{
   bin_builder_addia2(b, label, VM_INS_STORE,
                      arg_saddr_(src), arg_label_(dstLabel));
}

void bin_builder_addi_init(s_bin_builder* b, const char* label,
                           t_vm_saddr src, const char* dstLabel)
{
   bin_builder_addia2(b, label, VM_INS_INIT,
                      arg_saddr_(src), arg_label_(dstLabel));
}

static void addi_opu_(s_bin_builder* b, const char* label, t_vm_ins code,
                      t_vm_saddr a1, t_vm_saddr a2)
{
   bin_builder_addia2(b, label, code,
                      arg_saddr_(a1), arg_saddr_(a2));
}

void bin_builder_addi_postinc(s_bin_builder* b, const char* label,
                              t_vm_saddr a1, t_vm_saddr a2)
{
   addi_opu_(b, label, VM_INS_POSTINC, a1, a2);
}

void bin_builder_addi_postdec(s_bin_builder* b, const char* label,
                              t_vm_saddr a1, t_vm_saddr a2)
{
   addi_opu_(b, label, VM_INS_POSTDEC, a1, a2);
}

void bin_builder_addi_preinc(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2)
{
   addi_opu_(b, label, VM_INS_PREINC, a1, a2);
}

void bin_builder_addi_predec(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2)
{
   addi_opu_(b, label, VM_INS_PREDEC, a1, a2);
}

void bin_builder_addi_uplus(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2)
{
   addi_opu_(b, label, VM_INS_UPLUS, a1, a2);
}

void bin_builder_addi_uminus(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2)
{
   addi_opu_(b, label, VM_INS_UMINUS, a1, a2);
}

void bin_builder_addi_lnot(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2)
{
   addi_opu_(b, label, VM_INS_LNOT, a1, a2);
}

void bin_builder_addi_bnot(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2)
{
   addi_opu_(b, label, VM_INS_BNOT, a1, a2);
}


static void addi_opb_(s_bin_builder* b, const char* label, t_vm_ins code,
                      t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   bin_builder_addia3(b, label, code,
                      arg_saddr_(a1), arg_saddr_(a2), arg_saddr_(a3));
}

void bin_builder_addi_mul(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_MUL, a1, a2, a3);
}

void bin_builder_addi_div(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_DIV, a1, a2, a3);
}

void bin_builder_addi_mod(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_MOD, a1, a2, a3);
}

void bin_builder_addi_bplus(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_BPLUS, a1, a2, a3);
}

void bin_builder_addi_bminus(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_BMINUS, a1, a2, a3);
}

void bin_builder_addi_gt(s_bin_builder* b, const char* label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_GT, a1, a2, a3);
}

void bin_builder_addi_lt(s_bin_builder* b, const char* label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_LT, a1, a2, a3);
}

void bin_builder_addi_geq(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_GEQ, a1, a2, a3);
}

void bin_builder_addi_leq(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_LEQ, a1, a2, a3);
}

void bin_builder_addi_eq(s_bin_builder* b, const char* label,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_EQ, a1, a2, a3);
}

void bin_builder_addi_neq(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_NEQ, a1, a2, a3);
}

void bin_builder_addi_land(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_LAND, a1, a2, a3);
}

void bin_builder_addi_lor(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_LOR, a1, a2, a3);
}

void bin_builder_addi_lshift(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_LSHIFT, a1, a2, a3);
}

void bin_builder_addi_rshift(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_RSHIFT, a1, a2, a3);
}

void bin_builder_addi_band(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_BAND, a1, a2, a3);
}

void bin_builder_addi_bxor(s_bin_builder* b, const char* label,
                           t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_BXOR, a1, a2, a3);
}

void bin_builder_addi_bor(s_bin_builder* b, const char* label,
                          t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_BOR, a1, a2, a3);
}

void bin_builder_addi_assign(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_ASSIGN, a1, a2, a3);
}

void bin_builder_addi_pluseq(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_PLUSEQ, a1, a2, a3);
}

void bin_builder_addi_minuseq(s_bin_builder* b, const char* label,
                              t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_MINUSEQ, a1, a2, a3);
}

void bin_builder_addi_muleq(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_MULEQ, a1, a2, a3);
}

void bin_builder_addi_diveq(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_DIVEQ, a1, a2, a3);
}

void bin_builder_addi_modeq(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_MODEQ, a1, a2, a3);
}

void bin_builder_addi_lshifteq(s_bin_builder* b, const char* label,
                               t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_LSHIFTEQ, a1, a2, a3);
}

void bin_builder_addi_rshiftqeq(s_bin_builder* b, const char* label,
                                t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_RSHIFTEQ, a1, a2, a3);
}

void bin_builder_addi_bandeq(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_BANDEQ, a1, a2, a3);
}

void bin_builder_addi_bxoreq(s_bin_builder* b, const char* label,
                             t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_BXOREQ, a1, a2, a3);
}
void bin_builder_addi_boreq(s_bin_builder* b, const char* label,
                            t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_BOREQ, a1, a2, a3);
}

void bin_builder_addi_subscript(s_bin_builder* b, const char* label,
                                t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, VM_INS_SUBSCRIPT, a1, a2, a3);
}

void bin_builder_addi_ternary(s_bin_builder* b, const char* label,
                              t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3,
                              t_vm_saddr a4)
{
   bin_builder_addia4(b, label, VM_INS_TERNARY,
                      arg_saddr_(a1), arg_saddr_(a2),
                      arg_saddr_(a3), arg_saddr_(a4));
}

void bin_builder_addi_member(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, const char* strLabel,
                             t_vm_int size,  t_vm_saddr res)
{
   bin_builder_addia4(b, label, VM_INS_MEMBER,
                      arg_saddr_(saddr), arg_label_(strLabel),
                      arg_int_(size), arg_saddr_(res));
}

void bin_builder_addi_defclass(s_bin_builder* b, const char* label,
                               const char* nameLabel, t_vm_int len,
                               t_vm_int id)
{
   bin_builder_addia3(b, label, VM_INS_DEFCLASS,
                      arg_label_(nameLabel), arg_int_(len) ,arg_int_(id));
}

void bin_builder_addi_deffield(s_bin_builder* b, const char* label,
                               t_vm_int id,
                               const char* nameLabel, t_vm_int len,
                               t_vm_int type, t_vm_saddr value)
{
   bin_builder_addia5(b, label, VM_INS_DEFFIELD,
                      arg_int_(id),  arg_label_(nameLabel), arg_int_(len),
                      arg_int_(type), arg_saddr_(value));
}

void bin_builder_addi_defsfield(s_bin_builder* b, const char* label,
                                t_vm_int id,
                                const char* nameLabel, t_vm_int len,
                                t_vm_int type, t_vm_saddr value)
{
   bin_builder_addia5(b, label, VM_INS_DEFSFIELD,
                      arg_int_(id),  arg_label_(nameLabel), arg_int_(len),
                      arg_int_(type), arg_saddr_(value));
}

void bin_builder_addi_defend(s_bin_builder* b, const char* label,
                               t_vm_int id)
{
      bin_builder_addia1(b, label, VM_INS_DEFEND,
                         arg_int_(id));
}

void bin_builder_addia0(s_bin_builder* b, const char* label, t_vm_ins code)
{
   bin_builder_addi_(b, label, code);
}

void bin_builder_addia1(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, code);
   ins->a1 = a1;
}

void bin_builder_addia2(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, code);
   ins->a1 = a1;
   ins->a2 = a2;
}

void bin_builder_addia3(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2,
                        s_bin_builder_arg a3)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, code);
   ins->a1 = a1;
   ins->a2 = a2;
   ins->a3 = a3;
}

void bin_builder_addia4(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2,
                        s_bin_builder_arg a3, s_bin_builder_arg a4)
{
      s_bin_builder_ins* ins = bin_builder_addi_(b, label, code);
   ins->a1 = a1;
   ins->a2 = a2;
   ins->a3 = a3;
   ins->a4 = a4;
}

void bin_builder_addia5(s_bin_builder* b, const char* label, t_vm_ins code,
                        s_bin_builder_arg a1, s_bin_builder_arg a2,
                        s_bin_builder_arg a3, s_bin_builder_arg a4,
                        s_bin_builder_arg a5)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, code);
   ins->a1 = a1;
   ins->a2 = a2;
   ins->a3 = a3;
   ins->a4 = a4;
   ins->a5 = a5;
}

void bin_builder_add_opu(s_bin_builder* b, const char* label, t_vm_ins code,
                         t_vm_saddr a1, t_vm_saddr a2)
{
   addi_opu_(b, label, code, a1, a2);
}

void bin_builder_add_opb(s_bin_builder* b, const char* label, t_vm_ins code,
                         t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   addi_opb_(b, label, code, a1, a2, a3);
}



void bin_builder_save(s_bin_builder* b, const char* outPath)
{
   size_t len = binary_size_(b);
   char* buffer = malloc(len);
   s_parray* labels_refs = parray_new();
   t_vm_addr addr;

   addr = write_data_(b, buffer);
   addr = write_ins_(b, buffer, addr, labels_refs);

   errAssert((size_t) addr == len,
             "Unknown error when creating binary file: len = %zu, addr = %zu",
             len, (size_t) addr);

   errAssert(pmap_contains(b->labels_map, (void*) b->start_label),
             "Undefined start label: %s\n", b->start_label);

   memcpy(buffer, pmap_get(b->labels_map, (void*)b->start_label),
          sizeof(t_vm_addr));

   write_refs_(b, buffer, labels_refs);

   parray_foreach(labels_refs, free);
   parray_free(labels_refs);
   fsWriteBytes(outPath, buffer, len);
}
