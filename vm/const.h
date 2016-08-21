#ifndef CONST_H_
# define CONST_H_

# ifdef __cplusplus
extern "C"
{
#  if 0
}
#  endif
# endif

# include <stddef.h>
# include <stdint.h>

# define TRUE (1)
# define FALSE (0)
# define NPOS ((size_t) -1)

typedef uint32_t t_vm_addr;
typedef uint16_t t_vm_ins;
typedef uint16_t t_vm_saddr;
typedef int8_t t_vm_type;
typedef int8_t t_vm_mode;
typedef int64_t t_vm_int;
typedef double t_vm_double;
typedef char t_vm_char;
typedef uint8_t t_vm_bool;

typedef struct {
   const char* name;
   t_vm_addr size;
} t_vm_insinfos;

# define VM_INS_NOP (0)
# define VM_INS_JUMP (1)
# define VM_INS_CJUMP (2)
# define VM_INS_FJUMP (3)
# define VM_INS_FCALL (4)
# define VM_INS_FRET (5)
# define VM_INS_BCLEAR (6)
# define VM_INS_PUTNULL (7)
# define VM_INS_PUTINT (8)
# define VM_INS_PUTDOUBLE (9)
# define VM_INS_PUTCHAR (10)
# define VM_INS_PUTBOOL (11)
# define VM_INS_PUTSTRING (12)
# define VM_INS_PUTFUNCTION (13)
# define VM_INS_PUTSYSCALL (14)
# define VM_INS_PUTVAR (15)
# define VM_INS_PUTREF (16)
# define VM_INS_COPY (17)
# define VM_INS_MOVE (18)
# define VM_INS_SPUP (19)
# define VM_INS_SPDOWN (20)
# define VM_INS_SYSCALL (21)
# define VM_INS_BIND (22)
# define VM_INS_LOAD (23)
# define VM_INS_STORE (24)
# define VM_INS_INIT (25)
# define VM_INS_POSTINC (26)
# define VM_INS_POSTDEC (27)
# define VM_INS_PREINC (28)
# define VM_INS_PREDEC (29)
# define VM_INS_UPLUS (30)
# define VM_INS_UMINUS (31)
# define VM_INS_LNOT (32)
# define VM_INS_BNOT (33)
# define VM_INS_MUL (34)
# define VM_INS_DIV (35)
# define VM_INS_MOD (36)
# define VM_INS_BPLUS (37)
# define VM_INS_BMINUS (38)
# define VM_INS_GT (39)
# define VM_INS_LT (40)
# define VM_INS_GEQ (41)
# define VM_INS_LEQ (42)
# define VM_INS_EQ (43)
# define VM_INS_NEQ (44)
# define VM_INS_LAND (45)
# define VM_INS_LOR (46)
# define VM_INS_LSHIFT (47)
# define VM_INS_RSHIFT (48)
# define VM_INS_BAND (49)
# define VM_INS_BXOR (50)
# define VM_INS_BOR (51)
# define VM_INS_ASSIGN (52)
# define VM_INS_PLUSEQ (53)
# define VM_INS_MINUSEQ (54)
# define VM_INS_MULEQ (55)
# define VM_INS_DIVEQ (56)
# define VM_INS_MODEQ (57)
# define VM_INS_LSHIFTEQ (58)
# define VM_INS_RSHIFTEQ (59)
# define VM_INS_BANDEQ (60)
# define VM_INS_BXOREQ (61)
# define VM_INS_BOREQ (62)
# define VM_INS_SUBSCRIPT (63)
# define VM_INS_TERNARY (64)
# define VM_INS_MEMBER (65)

# define VM_NB_INS (66)

# define VM_SYSCALL_NO (-1)
# define VM_SYSCALL_EXIT (0)
# define VM_SYSCALL_KPRINT (1)
# define VM_SYSCALL_PRINT (2)
# define VM_SYSCALL_TOSTRING (3)

# define VM_NB_SYSCALL (4)

extern const t_vm_insinfos VM_INS_INFOS[];



# ifdef __cplusplus
}
# endif


#endif //!CONST_H_
