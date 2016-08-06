#include "dstr.h"
#include <stdlib.h>
#include <string.h>
#include "err.h"

#define DSTR_MIN_SIZE 16

void dstr_init(dstr* ds)
{
   ds->s = malloc(DSTR_MIN_SIZE);
   *(ds->s) = '\0';
   ds->alen = DSTR_MIN_SIZE;
   ds->len = 0;
}

void dstr_clear(dstr* ds)
{
   free(ds->s);
   dstr_init(ds);
}

void dstr_reserve(dstr* ds, size_t len)
{
   size_t newLen = ds->alen;
   while(len > newLen)
      newLen *= 2;

   if(ds->alen != newLen)
   {
      ds->s = realloc(ds->s, newLen);
      ds->alen = newLen;
   }
}

char* dstr_at(dstr* ds, size_t pos)
{
   errAssert(pos <= ds->len, "dstr_at: invalid pos %zu on dstr of size %zu\n",
             pos, ds->len);

   return ds->s + pos;
}

char* dstr_end(dstr* ds)
{
   return ds->s + ds->len;
}

void dstr_assign(dstr* ds, const char* cs, size_t len)
{
   size_t newLen = ds->alen;
   while(len > newLen)
      newLen *= 2;

   if(ds->alen != newLen)
   {
      free(ds->s);
      ds->s = malloc(newLen);
      ds->alen = newLen;
   }

   memcpy(ds->s, cs, len);
   ds->s[len] = '\0';
   ds->len = len;
}

void dstr_insert(dstr* ds, size_t pos, const char* cs, size_t len)
{
   errAssert(pos <= ds->len,
             "dstr_insert: invalid pos %zu on dstr of size %zu\n",
             pos, ds->len);

   dstr_reserve(ds, ds->len + len + 1);
   memmove(ds->s + pos + len, ds->s + pos, ds->len + 1 - pos);
   memcpy(ds->s + pos, cs, len);
   ds->len += len;
}

void dstr_prepend(dstr* ds, const char* cs, size_t len)
{
   dstr_insert(ds, 0, cs, len);
}

void dstr_append(dstr* ds, const char* cs, size_t len)
{
   dstr_insert(ds, ds->len, cs, len);
}

void dstr_erase(dstr* ds, size_t pos, size_t len)
{
   if(!len)
      return;

   errAssert(pos < ds->len,
             "dstr_erase: invalid pos %zu on dstr of size %zu\n",
             pos, ds->len);

   errAssert(pos + len <= ds->len,
             "dstr_erase: invalid arguments %zu, %zu on dstr of size %zu\n",
             pos, len, ds->len);

   memmove(ds->s + pos, ds->s + pos + len, ds->len + 1 - len - pos);
   ds->len -= len;
}

void dstr_write(dstr* ds, size_t pos, const char* cs, size_t len)
{
   errAssert(pos > ds->len,
             "dstr_write: invalid pos %zu on dstr of size %zu\n",
             pos, ds->len);

   dstr_reserve(ds, pos + len + 1);

   memcpy(ds->s + pos, cs, len);

   size_t end = pos + len;
   if(end >ds->len)
   {
      ds->len = end;
      ds->s[end] = '\0';
   }
}


static dstr* dstr_w_dst;

static size_t dstr_w_write(const char* data, size_t len)
{
   dstr_append(dstr_w_dst, data, len);
   return len;
}

ioWrite_f dstrWriter(dstr* ds)
{
   dstr_w_dst = ds;
   return dstr_w_write;
}
