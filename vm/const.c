#include "const.h"

# define S_INS (sizeof(t_vm_ins))
# define S_ADDR (sizeof(t_vm_addr))
# define S_SADDR (sizeof(t_vm_saddr))
# define S_INT (sizeof(t_vm_int))
# define S_DOUBLE (sizeof(t_vm_double))
# define S_CHAR (sizeof(t_vm_char))
# define S_BOOL (sizeof(t_vm_bool))


# define S_OPU (S_INS + 2 * S_SADDR)
# define S_OPB (S_INS + 3 * S_SADDR)

const t_vm_insinfos VM_INS_INFOS[] = {
   {"nop", S_INS},
   {"jump", S_INS + S_ADDR},
   {"cjump", S_INS + S_SADDR + S_ADDR},
   {"fjump", S_INS + S_ADDR + S_SADDR},
   {"fcall", S_INS + S_SADDR + S_SADDR + S_INT},
   {"fret", S_INS},
   {"bclear", S_INS + S_SADDR + S_SADDR},
   {"putnull", S_INS + S_SADDR +  S_INT},
   {"putint", S_INS + S_SADDR + S_INT + S_INT},
   {"putdouble", S_INS + S_SADDR + S_INT + S_DOUBLE},
   {"putchar", S_INS + S_SADDR + S_INT + S_CHAR},
   {"putbool", S_INS + S_SADDR + S_INT + S_BOOL},
   {"putstring", S_INS + S_SADDR + S_INT + S_ADDR + S_INT},
   {"putfunction", S_INS + S_SADDR + S_INT + S_ADDR},
   {"putsyscall", S_INS + S_SADDR + S_INT + S_INT},
   {"putvar", S_INS + S_SADDR + S_INT + S_SADDR},
   {"putref", S_INS + S_SADDR + S_SADDR},
   {"copy", S_INS + S_SADDR + S_SADDR},
   {"move", S_INS + S_SADDR + S_SADDR},
   {"spup", S_INS + S_SADDR},
   {"spdown", S_INS + S_SADDR},
   {"syscall", S_INS + S_INT},
   {"bind", S_INS + S_SADDR + S_SADDR + S_INT},
   {"load", S_INS + S_SADDR + S_ADDR},
   {"store", S_INS + S_SADDR + S_ADDR},
   {"init", S_INS + S_SADDR + S_ADDR},
   {"postinc", S_OPU},
   {"postdec", S_OPU},
   {"preinc", S_OPU},
   {"predec", S_OPU},
   {"uplus", S_OPU},
   {"uminus", S_OPU},
   {"lnot", S_OPU},
   {"bnot", S_OPU},
   {"mul", S_OPB},
   {"div", S_OPB},
   {"mod", S_OPB},
   {"bplus", S_OPB},
   {"bminus", S_OPB},
   {"gt", S_OPB},
   {"lt", S_OPB},
   {"geq", S_OPB},
   {"leq", S_OPB},
   {"eq", S_OPB},
   {"neq", S_OPB},
   {"land", S_OPB},
   {"lor", S_OPB},
   {"lshift", S_OPB},
   {"rshift", S_OPB},
   {"band", S_OPB},
   {"bxor", S_OPB},
   {"bor", S_OPB},
   {"assign", S_OPB},
   {"pluseq", S_OPB},
   {"minuseq", S_OPB},
   {"muleq", S_OPB},
   {"diveq", S_OPB},
   {"modeq", S_OPB},
   {"lshifteq", S_OPB},
   {"rshifteq", S_OPB},
   {"bandeq", S_OPB},
   {"bxoreq", S_OPB},
   {"boreq", S_OPB},
   {"subscript", S_OPB},
   {"ternary", S_INS + 4 * S_SADDR},
   {"member", S_INS + S_SADDR + S_ADDR + S_INT + S_SADDR}
};
