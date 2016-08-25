#include "dvar.h"
#include <stdlib.h>
#include "vm-dvar-utils.h"
#include "dvar-class.h"
#include "dvar-obj.h"
#include "date.h"


void dvar_date_init()
{
   vm_define_begin(VM_CLASS_DATE, "Date", DVAR_CLASS_NOPARENT);
   vm_define_method("constructor", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_CONSTRUCTOR);
   vm_define_method("destructor", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_DESTRUCTOR);
   vm_define_method("getTime", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_GETTIME);
   vm_define_method("getYear", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_GETYEAR);
   vm_define_method("getMonth", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_GETMONTH);
   vm_define_method("getDay", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_GETDAY);
   vm_define_method("getHours", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_GETHOURS);
   vm_define_method("getMinutes", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_GETMINUTES);
   vm_define_method("getSeconds", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_GETSECONDS);
   vm_define_method("getMilliseconds", DVAR_CLASS_VPUBLIC, FALSE,
                    VM_SYSCALL_DATE_GETMILLISECONDS);

   vm_define_method("now", DVAR_CLASS_VPUBLIC, TRUE,
                    VM_SYSCALL_DATE_NOW);

   vm_define_end_();
}

struct dvar* c__date__constructor(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_obj_val* o = l->v_obj->val;

   long* time = malloc(sizeof(long));

   if(n == 1)
   {
      *time = dateNow();
      o->extra = time;
      return l;
   }

   long year = 0;
   long month = 0;
   long day = 0;
   long hours = 0;
   long minutes = 0;
   long seconds = 0;
   long ms = 0;

   if(n > 1)
      year = (long) dvar_get_int(l+1, "Date(): year must be a number");
   if(n > 2)
      month = (long) dvar_get_int(l+2, "Date(): month must be a number");
   if(n > 3)
      day = (long) dvar_get_int(l+3, "Date(): day must be a number");
   if(n > 4)
      hours = (long) dvar_get_int(l+4, "Date(): hours must be a number");
   if(n > 5)
      minutes = (long) dvar_get_int(l+5, "Date(): minutes must be a number");
   if(n > 6)
      seconds = (long) dvar_get_int(l+6, "Date(): seconds must be a number");
   if(n > 7)
      ms = (long) dvar_get_int(l+7, "Date(): milliseconds must be a number");

   *time = dateAt(year, month, day, hours, minutes, seconds, ms);
   o->extra = time;
   return l;
}

struct dvar* c__date__destructor(struct dvar* l, t_vm_int n)
{
   (void) n;
   free(l->v_obj->val->extra);
   return l;
}

struct dvar* c__date__now(struct dvar* l, t_vm_int n)
{
   (void) n;
   dvar_putint(l, DVAR_MVAR, (t_vm_int) dateNow());
   return l;
}

struct dvar* c__date__get_time(struct dvar* l, t_vm_int n)
{
   (void) n;

   long* time = l->v_obj->val->extra;
   dvar_putint(l, DVAR_MVAR, (t_vm_int) (*time));
   return l;
}

struct dvar* c__date__get_year(struct dvar* l, t_vm_int n)
{
   (void) n;

   long* time = l->v_obj->val->extra;
   long res;
   dateGetLocale(*time, &res, 0, 0, 0, 0, 0, 0);

   dvar_putint(l, DVAR_MVAR, (t_vm_int) (res));
   return l;
}

struct dvar* c__date__get_month(struct dvar* l, t_vm_int n)
{
   (void) n;

   long* time = l->v_obj->val->extra;
   long res;
   dateGetLocale(*time, 0, &res, 0, 0, 0, 0, 0);

   dvar_putint(l, DVAR_MVAR, (t_vm_int) (res));
   return l;
}

struct dvar* c__date__get_day(struct dvar* l, t_vm_int n)
{
   (void) n;

   long* time = l->v_obj->val->extra;
   long res;
   dateGetLocale(*time, 0, 0, &res, 0, 0, 0, 0);

   dvar_putint(l, DVAR_MVAR, (t_vm_int) (res));
   return l;
}

struct dvar* c__date__get_hours(struct dvar* l, t_vm_int n)
{
   (void) n;

   long* time = l->v_obj->val->extra;
   long res;
   dateGetLocale(*time, 0, 0, 0, &res, 0, 0, 0);

   dvar_putint(l, DVAR_MVAR, (t_vm_int) (res));
   return l;
}

struct dvar* c__date__get_minutes(struct dvar* l, t_vm_int n)
{
   (void) n;

   long* time = l->v_obj->val->extra;
   long res;
   dateGetLocale(*time, 0, 0, 0, 0, &res, 0, 0);

   dvar_putint(l, DVAR_MVAR, (t_vm_int) (res));
   return l;
}

struct dvar* c__date__get_seconds(struct dvar* l, t_vm_int n)
{
   (void) n;

   long* time = l->v_obj->val->extra;
   long res;
   dateGetLocale(*time, 0, 0, 0, 0, 0, &res, 0);

   dvar_putint(l, DVAR_MVAR, (t_vm_int) (res));
   return l;
}

struct dvar* c__date__get_milliseconds(struct dvar* l, t_vm_int n)
{
   (void) n;

   long* time = l->v_obj->val->extra;
   long res;
   dateGetLocale(*time, 0, 0, 0, 0, 0, 0, &res);

   dvar_putint(l, DVAR_MVAR, (t_vm_int) (res));
   return l;
}
