#ifndef DVAR_STR_H_
# define DVAR_STR_H_

# include <stddef.h>
# include "const.h"

struct dvar;

struct dvar_str {
   t_vm_int len;
   struct dvar *chars;
};
typedef struct dvar_str dvar_str;

dvar_str* dvar_str_new(const char* str, t_vm_int len);
void dvar_str_free(dvar_str* s);
dvar_str* dvar_str_copy(const dvar_str* s);
dvar_str* dvar_str_move(dvar_str* s);

t_vm_int dvar_str_cmp(const dvar_str* s1, const dvar_str* s2);
t_vm_bool dvar_str_to_bool(const dvar_str* s);
char* dvar_str_to_string(const dvar_str* s);
uint32_t dvar_str_to_hash(const dvar_str* s);

void dvar_str_concat(dvar_str* a, const dvar_str* b);
dvar_str* dvar_str_sub(const dvar_str* s, t_vm_int begin, t_vm_int size);

struct dvar* c__str__empty(struct dvar* l, t_vm_int n);
struct dvar* c__str__size(struct dvar* l, t_vm_int n);
struct dvar* c__str__substr(struct dvar* l, t_vm_int n);
struct dvar* c__str__is_int(struct dvar* l, t_vm_int n);
struct dvar* c__str__to_int(struct dvar* l, t_vm_int n);
struct dvar* c__str__is_double(struct dvar* l, t_vm_int n);
struct dvar* c__str__to_double(struct dvar* l, t_vm_int n);

#endif //!DVAR_STR_H_
