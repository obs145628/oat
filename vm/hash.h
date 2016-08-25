#ifndef HASH_H_
# define HASH_H_

#include <stdint.h>
#include <stddef.h>

uint32_t hash_int(uint32_t x);
uint32_t hash_float(double x);
uint32_t hash_pointer(const void *p);
uint32_t hash_str(const char* s);
uint32_t hash_data(const char* src, size_t len);

#endif //!HASH_H_
