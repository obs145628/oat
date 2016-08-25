#ifndef DVAR_DATE_H_
# define DVAR_DATE_H_

# include "const.h"

struct dvar;

void dvar_date_init();

struct dvar* c__date__constructor(struct dvar* l, t_vm_int n);
struct dvar* c__date__destructor(struct dvar* l, t_vm_int n);
struct dvar* c__date__now(struct dvar* l, t_vm_int n);
struct dvar* c__date__get_time(struct dvar* l, t_vm_int n);
struct dvar* c__date__get_year(struct dvar* l, t_vm_int n);
struct dvar* c__date__get_month(struct dvar* l, t_vm_int n);
struct dvar* c__date__get_day(struct dvar* l, t_vm_int n);
struct dvar* c__date__get_hours(struct dvar* l, t_vm_int n);
struct dvar* c__date__get_minutes(struct dvar* l, t_vm_int n);
struct dvar* c__date__get_seconds(struct dvar* l, t_vm_int n);
struct dvar* c__date__get_milliseconds(struct dvar* l, t_vm_int n);

#endif //!DVAR_DATE_H_
