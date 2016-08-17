#ifndef CONST_H_
# define CONST_H_

# ifdef __cplusplus
extern "C"
{
# endif

# include <stddef.h>
# include <stdint.h>

# define TRUE (1)
# define FALSE (0)
# define NPOS ((size_t) -1)

typedef int64_t oat_int;
typedef double oat_double;
typedef uint8_t oat_bool;

typedef uint32_t t_vm_addr;
typedef uint16_t t_vm_ins;
typedef uint16_t t_vm_saddr;
typedef oat_int t_vm_int;
typedef oat_double t_vm_double;
typedef char t_vm_char;
typedef oat_bool t_vm_bool;

typedef struct {
   const char* name;
   t_vm_addr size;
} t_vm_insinfos;

# define VM_INS_NOP (0)
# define VM_INS_JUMP (1)
# define VM_INS_CJUMP (2)
# define VM_INS_FJUMP (3)
# define VM_INS_FRET (4)
# define VM_INS_BCLEAR (5)
# define VM_INS_PUTNULL (6)
# define VM_INS_PUTINT (7)
# define VM_INS_PUTDOUBLE (8)
# define VM_INS_PUTCHAR (9)
# define VM_INS_PUTBOOL (10)
# define VM_INS_PUTSTRING (11)
# define VM_INS_SPUP (12)
# define VM_INS_SPDOWN (13)
# define VM_INS_SYSCALL (14)
# define VM_INS_POSTINC (15)
# define VM_INS_POSTDEC (16)
# define VM_INS_PREINC (17)
# define VM_INS_PREDEC (18)
# define VM_INS_UPLUS (19)
# define VM_INS_UMINUS (20)
# define VM_INS_LNOT (21)
# define VM_INS_MUL (22)
# define VM_INS_DIV (23)
# define VM_INS_MOD (24)
# define VM_INS_BPLUS (25)
# define VM_INS_BMINUS (26)
# define VM_INS_GT (27)
# define VM_INS_LT (28)
# define VM_INS_GEQ (29)
# define VM_INS_LEQ (30)
# define VM_INS_EQ (31)
# define VM_INS_NEQ (32)
# define VM_INS_LAND (33)
# define VM_INS_LOR (34)
# define VM_INS_ASSIGN (35)
# define VM_INS_PLUSEQ (36)
# define VM_INS_MINUSEQ (37)
# define VM_INS_MULEQ (38)
# define VM_INS_DIVEQ (39)
# define VM_INS_MODEQ (40)


# define VM_NB_INS (41)

# define VM_SYSCALL_EXIT (0)
# define VM_SYSCALL_KPRINT (1)
# define VM_SYSCALL_PRINT (2)

# define VM_NB_SYSCALL (3)

extern const t_vm_insinfos VM_INS_INFOS[];



# ifdef __cplusplus
}
# endif


#endif //!CONST_H_
