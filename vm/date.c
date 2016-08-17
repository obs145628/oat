#include "date.h"
#include <time.h>
#include <inttypes.h>
#include "err.h"

long dateNow()
{
   long ms;
   long s;
   struct timespec spec;

   clock_gettime(CLOCK_REALTIME, &spec);
   s = (long) spec.tv_sec;
   ms = (long) (spec.tv_nsec / 1.0e6);
   return s * 1000 + ms;
}


long dateAt(long year, long month, long day, long h, long m, long s, long ms)
{
   time_t time;
   struct tm date;

   date.tm_year = year - 1900;
   date.tm_mon = month - 1;
   date.tm_mday = day;
   date.tm_hour = h;
   date.tm_min = m;
   date.tm_sec = s;

   if((time = mktime(&date)) == -1)
      err("Invalid date format");
   return ((long)time) * 1000 + ms;
}


void dateGetLocale(long ltime, long* year, long* month, long* day,
                   long* h, long* m, long* s, long* ms)
{
   time_t time = (time_t) (ltime / 1000);
   struct tm* date = localtime(&time);

   if(year)
      *year = date->tm_year + 1900;
   if(month)
      *month = date->tm_mon + 1;
   if(day)
      *day = date->tm_mday;
   if(h)
      *h = date->tm_hour;
   if(m)
      *m = date->tm_min;
   if(s)
      *s = date->tm_sec;
   if(ms)
      *ms = ltime % 1000;
}
