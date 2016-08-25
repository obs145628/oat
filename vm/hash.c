#include "hash.h"
#include <string.h>

uint32_t hash_int(uint32_t x)
{
   return  x* UINT32_C(2654435761);
}

uint32_t hash_float(double x)
{
   return hash_data((char*)&x, sizeof(double));
}

uint32_t hash_pointer(const void *p)
{
   intptr_t val = (intptr_t) p;
   return hash_int((uint32_t) (val));
}

uint32_t hash_str(const char* s)
{
   return hash_data(s, strlen(s));
}

uint32_t hash_data(const char* src, size_t len)
{
   uint32_t hash = 5381;
   uint32_t c;

   for(size_t i = 0; i < len; ++i)
   {
      c = (uint32_t) (src[i]);
      hash = ((hash << 5) + hash) + c;
   }

   return hash;
}
