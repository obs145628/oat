#include "bin-builder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "err.h"
#include "fs.h"

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

typedef t_vm_addr (*f_write_ins_)(s_bin_builder_ins* ins, char* buffer,
                                  t_vm_addr addr, s_parray* labels_refs);

static t_vm_addr write_ins_nop_(s_bin_builder_ins* ins, char* buffer,
                                t_vm_addr addr, s_parray* labels_refs)
{
   (void) ins;
   (void) buffer;
   (void) labels_refs;
   return addr;
}

static t_vm_addr write_ins_jump_(s_bin_builder_ins* ins, char* buffer,
                                t_vm_addr addr, s_parray* labels_refs)
{
   (void) buffer;

   parray_insert_back(labels_refs,
                      bin_builder_label_ref_new_(ins->a1.p_label, addr));

   return addr + sizeof(t_vm_addr);
}

static t_vm_addr write_ins_cjump_(s_bin_builder_ins* ins, char* buffer,
                                  t_vm_addr addr, s_parray* labels_refs)
{

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   parray_insert_back(labels_refs,
                      bin_builder_label_ref_new_(ins->a2.p_label, addr));
   addr += sizeof(t_vm_addr);

   return addr;
}

static t_vm_addr write_ins_fjump_(s_bin_builder_ins* ins, char* buffer,
                                  t_vm_addr addr, s_parray* labels_refs)
{
   parray_insert_back(labels_refs,
                      bin_builder_label_ref_new_(ins->a1.p_label, addr));
   addr += sizeof(t_vm_addr);

   memcpy(buffer + addr, &(ins->a2.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   return addr;
}

static t_vm_addr write_ins_fret_(s_bin_builder_ins* ins, char* buffer,
                                 t_vm_addr addr, s_parray* labels_refs)
{
   (void) ins;
   (void) buffer;
   (void) labels_refs;
   return addr;
}

static t_vm_addr write_ins_bclear_(s_bin_builder_ins* ins, char* buffer,
                                   t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);
   memcpy(buffer + addr, &(ins->a2.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   return addr;
}

static t_vm_addr write_ins_putnull_(s_bin_builder_ins* ins, char* buffer,
                                    t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   return addr;
}

static t_vm_addr write_ins_putint_(s_bin_builder_ins* ins, char* buffer,
                                   t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   memcpy(buffer + addr, &(ins->a2.p_int), sizeof(t_vm_int));
   addr += sizeof(t_vm_int);

   return addr;
}

static t_vm_addr write_ins_putdouble_(s_bin_builder_ins* ins, char* buffer,
                                      t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   memcpy(buffer + addr, &(ins->a2.p_double), sizeof(t_vm_double));
   addr += sizeof(t_vm_double);

   return addr;
}

static t_vm_addr write_ins_putchar_(s_bin_builder_ins* ins, char* buffer,
                                    t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   memcpy(buffer + addr, &(ins->a2.p_char), sizeof(t_vm_char));
   addr += sizeof(t_vm_char);

   return addr;
}

static t_vm_addr write_ins_putbool_(s_bin_builder_ins* ins, char* buffer,
                                    t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   memcpy(buffer + addr, &(ins->a2.p_bool), sizeof(t_vm_bool));
   addr += sizeof(t_vm_bool);

   return addr;
}

static t_vm_addr write_ins_putstring_(s_bin_builder_ins* ins, char* buffer,
                                      t_vm_addr addr, s_parray* labels_refs)
{
   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   parray_insert_back(labels_refs,
                      bin_builder_label_ref_new_(ins->a2.p_label, addr));
   addr += sizeof(t_vm_addr);

   memcpy(buffer + addr, &(ins->a3.p_int), sizeof(t_vm_int));
   addr += sizeof(t_vm_int);

   return addr;
}

static t_vm_addr write_ins_spup_(s_bin_builder_ins* ins, char* buffer,
                                 t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   return addr;
}

static t_vm_addr write_ins_spdown_(s_bin_builder_ins* ins, char* buffer,
                                   t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   return addr;
}

static t_vm_addr write_ins_syscall_(s_bin_builder_ins* ins, char* buffer,
                                    t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_int), sizeof(t_vm_int));
   addr += sizeof(t_vm_int);

   return addr;
}

static t_vm_addr write_ins_spe_opu_(s_bin_builder_ins* ins, char* buffer,
                                    t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   memcpy(buffer + addr, &(ins->a2.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   return addr;
}

static t_vm_addr write_ins_spe_opb_(s_bin_builder_ins* ins, char* buffer,
                                    t_vm_addr addr, s_parray* labels_refs)
{
   (void) labels_refs;

   memcpy(buffer + addr, &(ins->a1.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   memcpy(buffer + addr, &(ins->a2.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   memcpy(buffer + addr, &(ins->a3.p_saddr), sizeof(t_vm_saddr));
   addr += sizeof(t_vm_saddr);

   return addr;
}


static t_vm_addr write_ins_(s_bin_builder* b, char* buffer, t_vm_addr addr,
                            s_parray* labels_refs)
{
   static const f_write_ins_ FNS[] = {
      write_ins_nop_,
      write_ins_jump_,
      write_ins_cjump_,
      write_ins_fjump_,
      write_ins_fret_,
      write_ins_bclear_,
      write_ins_putnull_,
      write_ins_putint_,
      write_ins_putdouble_,
      write_ins_putchar_,
      write_ins_putbool_,
      write_ins_putstring_,
      write_ins_spup_,
      write_ins_spdown_,
      write_ins_syscall_,
      write_ins_spe_opu_,
      write_ins_spe_opu_,
      write_ins_spe_opu_,
      write_ins_spe_opu_,
      write_ins_spe_opu_,
      write_ins_spe_opu_,
      write_ins_spe_opu_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_,
      write_ins_spe_opb_
   };

   s_bin_builder_ins* ins;

   for(size_t i = 0; i < parray_size(b->ins_arr); ++i)
   {
      ins = (s_bin_builder_ins*) parray_get(b->ins_arr, i);
      ins->addr = addr;
      t_vm_ins code = ins->code;

      memcpy(buffer + addr, &code, sizeof(t_vm_ins));
      addr += sizeof(t_vm_ins);
      addr = FNS[code](ins, buffer, addr, labels_refs);
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


void bin_builder_addi_nop(s_bin_builder* b, const char* label)
{
   bin_builder_addi_(b, label, VM_INS_NOP);
}

void bin_builder_addi_jump(s_bin_builder* b, const char* label,
                           const char* addrLabel)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_JUMP);
   ins->a1.p_label = addrLabel;
}

void bin_builder_addi_cjump(s_bin_builder* b, const char* label,
                            t_vm_saddr saddr, const char* addrLabel)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_CJUMP);
   ins->a1.p_saddr = saddr;
   ins->a2.p_label = addrLabel;
}
void bin_builder_addi_fjump(s_bin_builder* b, const char* label,
                            const char* addrLabel, t_vm_saddr saddr)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_FJUMP);
   ins->a1.p_label = addrLabel;
   ins->a2.p_saddr = saddr;
}

void bin_builder_addi_fret(s_bin_builder* b, const char* label)
{
   bin_builder_addi_(b, label, VM_INS_FRET);
}

void bin_builder_addi_bclear(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_saddr size)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_BCLEAR);
   ins->a1.p_saddr = saddr;
   ins->a2.p_saddr = size;
}

void bin_builder_addi_putnull(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_PUTNULL);
   ins->a1.p_saddr = saddr;
}

void bin_builder_addi_putint(s_bin_builder* b, const char* label,
                             t_vm_saddr saddr, t_vm_int value)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_PUTINT);
   ins->a1.p_saddr = saddr;
   ins->a2.p_int = value;
}

void bin_builder_addi_putdouble(s_bin_builder* b, const char* label,
                                t_vm_saddr saddr, t_vm_double value)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_PUTDOUBLE);
   ins->a1.p_saddr = saddr;
   ins->a2.p_double = value;
}

void bin_builder_addi_putchar(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr, t_vm_char value)
{
      s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_PUTCHAR);
   ins->a1.p_saddr = saddr;
   ins->a2.p_char = value;
}

void bin_builder_addi_putbool(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr, t_vm_bool value)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_PUTBOOL);
   ins->a1.p_saddr = saddr;
   ins->a2.p_bool = value;
}

void bin_builder_addi_putstring(s_bin_builder* b, const char* label,
                                t_vm_saddr saddr, const char* addrLabel,
                                t_vm_int size)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_PUTSTRING);
   ins->a1.p_saddr = saddr;
   ins->a2.p_label = addrLabel;
   ins->a3.p_int = size;
}

void bin_builder_addi_spup(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_SPUP);
   ins->a1.p_saddr = saddr;
}

void bin_builder_addi_spdown(s_bin_builder* b, const char* label,
                              t_vm_saddr saddr)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_SPDOWN);
   ins->a1.p_saddr = saddr;
}

void bin_builder_addi_syscall(s_bin_builder* b, const char* label,
                              t_vm_int value)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, VM_INS_SYSCALL);
   ins->a1.p_int = value;
}

static void addi_opu_(s_bin_builder* b, const char* label, t_vm_ins code,
                      t_vm_saddr a1, t_vm_saddr a2)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, code);
   ins->a1.p_saddr = a1;
   ins->a2.p_saddr = a2;
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


static void addi_opb_(s_bin_builder* b, const char* label, t_vm_ins code,
                      t_vm_saddr a1, t_vm_saddr a2, t_vm_saddr a3)
{
   s_bin_builder_ins* ins = bin_builder_addi_(b, label, code);
   ins->a1.p_saddr = a1;
   ins->a2.p_saddr = a2;
   ins->a3.p_saddr = a3;
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
