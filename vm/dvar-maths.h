#ifndef DVAR_MATHS_H_
# define DVAR_MATHS_H_

# include "const.h"

struct dvar;

struct dvar* syscall_min(struct dvar* l, t_vm_int n);
struct dvar* syscall_max(struct dvar* l, t_vm_int n);
struct dvar* syscall_abs(struct dvar* l, t_vm_int n);
struct dvar* syscall_round(struct dvar* l, t_vm_int n);
struct dvar* syscall_floor(struct dvar* l, t_vm_int n);
struct dvar* syscall_ceil(struct dvar* l, t_vm_int n);
struct dvar* syscall_fmod(struct dvar* l, t_vm_int n);
struct dvar* syscall_exp(struct dvar* l, t_vm_int n);
struct dvar* syscall_log(struct dvar* l, t_vm_int n);
struct dvar* syscall_pow(struct dvar* l, t_vm_int n);
struct dvar* syscall_sqrt(struct dvar* l, t_vm_int n);
struct dvar* syscall_sin(struct dvar* l, t_vm_int n);
struct dvar* syscall_cos(struct dvar* l, t_vm_int n);
struct dvar* syscall_tan(struct dvar* l, t_vm_int n);
struct dvar* syscall_asin(struct dvar* l, t_vm_int n);
struct dvar* syscall_acos(struct dvar* l, t_vm_int n);
struct dvar* syscall_atan(struct dvar* l, t_vm_int n);
struct dvar* syscall_sinh(struct dvar* l, t_vm_int n);
struct dvar* syscall_cosh(struct dvar* l, t_vm_int n);
struct dvar* syscall_tanh(struct dvar* l, t_vm_int n);
struct dvar* syscall_asinh(struct dvar* l, t_vm_int n);
struct dvar* syscall_acosh(struct dvar* l, t_vm_int n);
struct dvar* syscall_atanh(struct dvar* l, t_vm_int n);

#endif //!DVAR_MATHS_H_
