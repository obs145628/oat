/*
 * Struct to handle string edition
 */

#ifndef DSTR_H_
# define DSTR_H_

# include <stddef.h>
# include "io.h"

typedef struct {
   char* s;
   size_t len;
   size_t alen;
} dstr;

void dstr_init(dstr* ds);
void dstr_clear(dstr* ds);
void dstr_reserve(dstr* ds, size_t len);
char* dstr_at(dstr* ds, size_t pos);
char* dstr_end(dstr* ds);
void dstr_assign(dstr* ds, const char* cs, size_t len);
void dstr_insert(dstr* ds, size_t pos, const char* cs, size_t len);
void dstr_prepend(dstr* ds, const char* cs, size_t len);
void dstr_append(dstr* ds, const char* cs, size_t len);
void dstr_write(dstr* ds, size_t pos, const char* cs, size_t len);
void dstr_erase(dstr* ds, size_t pos, size_t len);

ioWrite_f dstrWriter(dstr* ds);



# endif
