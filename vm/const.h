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
# define VM_INS_PUTARR (15)
# define VM_INS_PUTSET (16)
# define VM_INS_PUTMAP (17)
# define VM_INS_PUTOBJ (18)
# define VM_INS_PUTCLASS (19)
# define VM_INS_PUTVAR (20)
# define VM_INS_PUTREF (21)
# define VM_INS_COPY (22)
# define VM_INS_MOVE (23)
# define VM_INS_SPUP (24)
# define VM_INS_SPDOWN (25)
# define VM_INS_SYSCALL (26)
# define VM_INS_BIND (27)
# define VM_INS_LOAD (28)
# define VM_INS_STORE (29)
# define VM_INS_INIT (30)
# define VM_INS_POSTINC (31)
# define VM_INS_POSTDEC (32)
# define VM_INS_PREINC (33)
# define VM_INS_PREDEC (34)
# define VM_INS_UPLUS (35)
# define VM_INS_UMINUS (36)
# define VM_INS_LNOT (37)
# define VM_INS_BNOT (38)
# define VM_INS_MUL (39)
# define VM_INS_DIV (40)
# define VM_INS_MOD (41)
# define VM_INS_BPLUS (42)
# define VM_INS_BMINUS (43)
# define VM_INS_GT (44)
# define VM_INS_LT (45)
# define VM_INS_GEQ (46)
# define VM_INS_LEQ (47)
# define VM_INS_EQ (48)
# define VM_INS_NEQ (49)
# define VM_INS_LAND (50)
# define VM_INS_LOR (51)
# define VM_INS_LSHIFT (52)
# define VM_INS_RSHIFT (53)
# define VM_INS_BAND (54)
# define VM_INS_BXOR (55)
# define VM_INS_BOR (56)
# define VM_INS_ASSIGN (57)
# define VM_INS_PLUSEQ (58)
# define VM_INS_MINUSEQ (59)
# define VM_INS_MULEQ (60)
# define VM_INS_DIVEQ (61)
# define VM_INS_MODEQ (62)
# define VM_INS_LSHIFTEQ (63)
# define VM_INS_RSHIFTEQ (64)
# define VM_INS_BANDEQ (65)
# define VM_INS_BXOREQ (66)
# define VM_INS_BOREQ (67)
# define VM_INS_SUBSCRIPT (68)
# define VM_INS_TERNARY (69)
# define VM_INS_MEMBER (70)
# define VM_INS_DEFCLASS (71)
# define VM_INS_DEFFIELD (72)
# define VM_INS_DEFSFIELD (73)
# define VM_INS_DEFEND (74)
# define VM_INS_SSUPER (75)
# define VM_INS_SETFSCOPE (76)
# define VM_INS_SETFLINE (77)

# define VM_NB_INS (78)

# define VM_SYSCALL_NO (-1)
# define VM_SYSCALL_EXIT (0)
# define VM_SYSCALL_KPRINT (1)
# define VM_SYSCALL_PRINT (2)
# define VM_SYSCALL_TOSTRING (3)
# define VM_SYSCALL_DEFAULT_CONSTRUCTOR (4)
# define VM_SYSCALL_RECT_CONSTRUCTOR (5)
# define VM_SYSCALL_RECT_GET_X (6)
# define VM_SYSCALL_RECT_GET_Y (7)
# define VM_SYSCALL_RECT_GET_WIDTH (8)
# define VM_SYSCALL_RECT_GET_HEIGHT (9)
# define VM_SYSCALL_MAP_SUB_LIST (10)
# define VM_SYSCALL_RECT_SET_X (11)
# define VM_SYSCALL_RECT_SET_Y (12)
# define VM_SYSCALL_RECT_SET_WIDTH (13)
# define VM_SYSCALL_RECT_SET_HEIGHT (14)
# define VM_SYSCALL_DEFAULT_DESTRUCTOR (15)
# define VM_SYSCALL_DATE_CONSTRUCTOR (16)
# define VM_SYSCALL_DATE_DESTRUCTOR (17)
# define VM_SYSCALL_DATE_GETTIME (18)
# define VM_SYSCALL_DATE_GETYEAR (19)
# define VM_SYSCALL_DATE_GETMONTH (20)
# define VM_SYSCALL_DATE_GETDAY (21)
# define VM_SYSCALL_DATE_GETHOURS (22)
# define VM_SYSCALL_DATE_GETMINUTES (23)
# define VM_SYSCALL_DATE_GETSECONDS (24)
# define VM_SYSCALL_DATE_GETMILLISECONDS (25)
# define VM_SYSCALL_CHAR_IS_PRINTABLE (26)
# define VM_SYSCALL_CHAR_IS_DIGIT (27)
# define VM_SYSCALL_CHAR_IS_LOWER (28)
# define VM_SYSCALL_CHAR_IS_UPPER (29)
# define VM_SYSCALL_STR_EMPTY (30)
# define VM_SYSCALL_STR_SIZE (31)
# define VM_SYSCALL_STR_SUBSTR (32)
# define VM_SYSCALL_STR_IS_INT (33)
# define VM_SYSCALL_STR_TO_INT (34)
# define VM_SYSCALL_STR_IS_DOUBLE (35)
# define VM_SYSCALL_STR_TO_DOUBLE (36)
# define VM_SYSCALL_FUN_BIND (37)
# define VM_SYSCALL_FUN_BINDA (38)
# define VM_SYSCALL_ARR_EMPTY (39)
# define VM_SYSCALL_ARR_SIZE (40)
# define VM_SYSCALL_ARR_PUSH_FRONT (41)
# define VM_SYSCALL_ARR_PUSH_BACK (42)
# define VM_SYSCALL_ARR_INSERT (43)
# define VM_SYSCALL_ARR_POP_FRONT (44)
# define VM_SYSCALL_ARR_POP_BACK (45)
# define VM_SYSCALL_ARR_REMOVE (46)
# define VM_SYSCALL_ARR_TO_SET (47)
# define VM_SYSCALL_SET_EMPTY (48)
# define VM_SYSCALL_SET_SIZE (49)
# define VM_SYSCALL_SET_CONTAINS (50)
# define VM_SYSCALL_SET_INSERT (51)
# define VM_SYSCALL_SET_REMOVE (52)
# define VM_SYSCALL_SET_TO_ARRAY (53)
# define VM_SYSCALL_MAP_EMPTY (54)
# define VM_SYSCALL_MAP_SIZE (55)
# define VM_SYSCALL_MAP_CONTAINS (56)
# define VM_SYSCALL_MAP_REMOVE (57)
# define VM_SYSCALL_MAP_KEYS (58)
# define VM_SYSCALL_MAP_VALUES (59)
# define VM_SYSCALL_DATE_NOW (60)
# define VM_SYSCALL_TYPENAME (61)
# define VM_SYSCALL_IS_NULL (62)
# define VM_SYSCALL_IS_INT (63)
# define VM_SYSCALL_IS_DOUBLE (64)
# define VM_SYSCALL_IS_CHAR (65)
# define VM_SYSCALL_IS_BOOL (66)
# define VM_SYSCALL_IS_STRING (67)
# define VM_SYSCALL_IS_FUNCTION (68)
# define VM_SYSCALL_IS_ARRAY (69)
# define VM_SYSCALL_IS_SET (70)
# define VM_SYSCALL_IS_MAP (71)
# define VM_SYSCALL_IS_OBJECT (72)
# define VM_SYSCALL_IS_CLASS (73)
# define VM_SYSCALL_IS_INSTANCE_OF (74)
# define VM_SYSCALL_STR_INDEX_OF (75)
# define VM_SYSCALL_STR_LAST_INDEX_OF (76)
# define VM_SYSCALL_STR_CONTAINS (77)
# define VM_SYSCALL_STR_REPLACE (78)
# define VM_SYSCALL_ARR_INDEX_OF (79)
# define VM_SYSCALL_ARR_LAST_INDEX_OF (80)
# define VM_SYSCALL_ARR_CONTAINS (81)
# define VM_SYSCALL_ARRAY_ITERATOR_CONSTRUCTOR (82)
# define VM_SYSCALL_ARRAY_ITERATOR_DESTRUCTOR (83)
# define VM_SYSCALL_ARRAY_ITERATOR_IS_END (84)
# define VM_SYSCALL_ARRAY_ITERATOR_NEXT (85)
# define VM_SYSCALL_ARRAY_ITERATOR_GET_KEY (86)
# define VM_SYSCALL_ARRAY_ITERATOR_GET_VALUE (87)
# define VM_SYSCALL_ARR_IT (88)
# define VM_SYSCALL_SET_ITERATOR_CONSTRUCTOR (89)
# define VM_SYSCALL_SET_ITERATOR_DESTRUCTOR (90)
# define VM_SYSCALL_SET_ITERATOR_IS_END (91)
# define VM_SYSCALL_SET_ITERATOR_NEXT (92)
# define VM_SYSCALL_SET_ITERATOR_GET_KEY (93)
# define VM_SYSCALL_SET_ITERATOR_GET_VALUE (94)
# define VM_SYSCALL_SET_IT (95)
# define VM_SYSCALL_MAP_ITERATOR_CONSTRUCTOR (96)
# define VM_SYSCALL_MAP_ITERATOR_DESTRUCTOR (97)
# define VM_SYSCALL_MAP_ITERATOR_IS_END (98)
# define VM_SYSCALL_MAP_ITERATOR_NEXT (99)
# define VM_SYSCALL_MAP_ITERATOR_GET_KEY (100)
# define VM_SYSCALL_MAP_ITERATOR_GET_VALUE (101)
# define VM_SYSCALL_MAP_IT (102)
# define VM_SYSCALL_MIN (103)
# define VM_SYSCALL_MAX (104)
# define VM_SYSCALL_ABS (105)
# define VM_SYSCALL_ROUND (106)
# define VM_SYSCALL_FLOOR (107)
# define VM_SYSCALL_CEIL (108)
# define VM_SYSCALL_FMOD (109)
# define VM_SYSCALL_EXP (110)
# define VM_SYSCALL_LOG (111)
# define VM_SYSCALL_POW (112)
# define VM_SYSCALL_SQRT (113)
# define VM_SYSCALL_SIN (114)
# define VM_SYSCALL_COS (115)
# define VM_SYSCALL_TAN (116)
# define VM_SYSCALL_ASIN (117)
# define VM_SYSCALL_ACOS (118)
# define VM_SYSCALL_ATAN (119)
# define VM_SYSCALL_SINH (120)
# define VM_SYSCALL_COSH (121)
# define VM_SYSCALL_TANH (122)
# define VM_SYSCALL_ASINH (123)
# define VM_SYSCALL_ACOSH (124)
# define VM_SYSCALL_ATANH (125)
# define VM_SYSCALL_RANDOM_GENERATOR_CONSTRUCTOR (126)
# define VM_SYSCALL_RANDOM_GENERATOR_DESTRUCTOR (127)
# define VM_SYSCALL_RANDOM_GENERATOR_GET_BOOL (128)
# define VM_SYSCALL_RANDOM_GENERATOR_GET_INT (129)
# define VM_SYSCALL_RANDOM_GENERATOR_GET_DOUBLE (130)
# define VM_SYSCALL_RANDOM_GENERATOR_GET_CHAR (131)
# define VM_SYSCALL_PROMPT_LINE (132)
# define VM_SYSCALL_PROMPT_INT (133)
# define VM_SYSCALL_PROMPT_DOUBLE (134)
# define VM_SYSCALL_PROMPT_CHAR (135)
# define VM_SYSCALL_PROMPT_BOOL (136)



# define VM_NB_SYSCALL (137)

# define VM_CLASS_RECT (0)
# define VM_CLASS_DATE (1)
# define VM_CLASS_ARRAY_ITERATOR (2)
# define VM_CLASS_SET_ITERATOR (3)
# define VM_CLASS_MAP_ITERATOR (4)
# define VM_CLASS_RANDOM_GENERATOR (5)

# define VM_NB_CLASS (6)

extern const t_vm_insinfos VM_INS_INFOS[];



# ifdef __cplusplus
}
# endif


#endif //!CONST_H_
