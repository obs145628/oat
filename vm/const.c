#include "const.h"

# define OPU_SIZE (sizeof(t_vm_ins) + 2 * sizeof(t_vm_saddr))
# define OPB_SIZE (sizeof(t_vm_ins) + 3 * sizeof(t_vm_saddr))

const t_vm_insinfos VM_INS_INFOS[] = {
   {"nop", sizeof(t_vm_ins)},
   {"jump", sizeof(t_vm_ins) + sizeof(t_vm_addr)},
   {"cjump", sizeof(t_vm_ins) + sizeof(t_vm_saddr) + sizeof(t_vm_addr)},
   {"fjump", sizeof(t_vm_ins) + sizeof(t_vm_addr) + sizeof(t_vm_saddr)},
   {"fret", sizeof(t_vm_ins)},
   {"bclear", sizeof(t_vm_ins) + sizeof(t_vm_saddr) + sizeof(t_vm_saddr)},
   {"putnull", sizeof(t_vm_ins) + sizeof(t_vm_saddr)},
   {"putint", sizeof(t_vm_ins) + sizeof(t_vm_saddr) + sizeof(t_vm_int)},
   {"putdouble", sizeof(t_vm_ins) + sizeof(t_vm_saddr) + sizeof(t_vm_double)},
   {"putchar", sizeof(t_vm_ins) + sizeof(t_vm_saddr) + sizeof(t_vm_char)},
   {"putbool", sizeof(t_vm_ins) + sizeof(t_vm_saddr) + sizeof(t_vm_bool)},
   {"putstring", sizeof(t_vm_ins) + sizeof(t_vm_saddr) + sizeof(t_vm_addr)
    + sizeof(t_vm_int)},
   {"spup", sizeof(t_vm_ins) + sizeof(t_vm_saddr)},
   {"spdown", sizeof(t_vm_ins) + sizeof(t_vm_saddr)},
   {"syscall", sizeof(t_vm_ins) + sizeof(t_vm_int)},
   {"postinc", OPU_SIZE},
   {"postdec", OPU_SIZE},
   {"preinc", OPU_SIZE},
   {"predec", OPU_SIZE},
   {"uplus", OPU_SIZE},
   {"uminus", OPU_SIZE},
   {"lnot", OPU_SIZE},
   {"mul", OPB_SIZE},
   {"div", OPB_SIZE},
   {"mod", OPB_SIZE},
   {"bplus", OPB_SIZE},
   {"bminus", OPB_SIZE},
   {"gt", OPB_SIZE},
   {"lt", OPB_SIZE},
   {"geq", OPB_SIZE},
   {"leq", OPB_SIZE},
   {"eq", OPB_SIZE},
   {"neq", OPB_SIZE},
   {"land", OPB_SIZE},
   {"lor", OPB_SIZE},
   {"assign", OPB_SIZE},
   {"pluseq", OPB_SIZE},
   {"minuseq", OPB_SIZE},
   {"muleq", OPB_SIZE},
   {"diveq", OPB_SIZE},
   {"modeq", OPB_SIZE}
};
