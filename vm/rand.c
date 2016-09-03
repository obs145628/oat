#include "rand.h"
#include <stdlib.h>
#include <limits.h>

s_rand* rand_new(unsigned long s)
{
   s_rand* r = malloc(sizeof(s_rand));
   r->_mti = 1;
   r->_mag01[0] = 0x0UL;
   r->_mag01[1] = RAND_MATRIX_A;

   r->_mt[0] = s & 0xffffffffUL;
   for (; r->_mti < RAND_N; ++r->_mti)
   {
      r->_mt[r->_mti] = (1812433253UL * (r->_mt[r->_mti - 1]
                                   ^ (r->_mt[r->_mti - 1] >> 30)) + r->_mti);
      r->_mt[r->_mti] &= 0xffffffffUL;
   }

   return r;
}

void rand_free(s_rand* r)
{
   free(r);
}

unsigned long rand_int32_get(s_rand* r)
{
   unsigned long y;

   if (r->_mti == RAND_N)
   {
      int kk;

      for (kk = 0; kk < RAND_N - RAND_M; ++kk)
      {
         y = (r->_mt[kk] & RAND_UPPER_MASK)
            | (r->_mt[kk + 1] & RAND_LOWER_MASK);
         r->_mt[kk] = r->_mt[kk + RAND_M] ^ (y >> 1) ^ r->_mag01[y & 0x1UL];
      }

      for (; kk < RAND_N - 1; ++kk)
      {
         y = (r->_mt[kk ] & RAND_UPPER_MASK)
            | (r->_mt[kk + 1 ] & RAND_LOWER_MASK);
         r->_mt[kk] = r->_mt[kk + ( RAND_M - RAND_N)] ^ (y >> 1)
            ^ r->_mag01 [y & 0x1UL];
      }

      y = (r->_mt[RAND_N - 1] & RAND_UPPER_MASK)
         | (r->_mt[0 ] & RAND_LOWER_MASK);
      r->_mt[RAND_N - 1] = r->_mt [RAND_M - 1] ^ (y >> 1)
         ^ r->_mag01[y & 0x1UL];

      r->_mti = 0;
   }

   y = r->_mt[r->_mti++];
   y ^= (y >> 11);
   y ^= (y << 7) & 0x9d2c5680UL;
   y ^= (y << 15) & 0xefc60000UL;
   y ^= (y >> 18);
   return y;
}

long rand_int31_get(s_rand* r)
{
   return ((long) rand_int32_get(r)) >> 1;
}

int rand_get_int(s_rand* r)
{
   int n = (int) (rand_int31_get(r));
   int blocks = sizeof (int) / 4;
   int shift = 32;

   for (int i = 1; i < blocks; ++i)
      n = (n << shift) | (int) (rand_int32_get(r));
   return n;
}

unsigned int rand_get_uint(s_rand* r)
{
   unsigned int n = 0;
   int blocks = sizeof (unsigned int) / 4;
   unsigned int shift = 32;

   for (int i = 0; i < blocks; ++i)
      n = (n << shift) | (unsigned int) (rand_int32_get(r));
   return n;
}

long rand_get_long(s_rand* r)
{
   long n = (long) (rand_int31_get(r));
   int blocks = sizeof (long) / 4;
   long shift = 32;

   for (int i = 1; i < blocks; ++i)
      n = (n << shift) | (long) (rand_int32_get(r));
   return n;
}

unsigned long rand_get_ulong(s_rand* r)
{
   unsigned long n = 0;
   int blocks = sizeof (unsigned long) / 4;
   unsigned long shift = 32;

   for (int i = 0; i < blocks; ++i)
      n = (n << shift) | (unsigned long) (rand_int32_get(r));
   return n;
}

int rand_get_bool(s_rand* r)
{
   return (int) (rand_int32_get(r) % 2);
}

char rand_get_char(s_rand* r)
{
   return (char) (rand_int32_get(r) % (CHAR_MAX + 1));
}

unsigned char rand_get_uchar(s_rand* r)
{
   return (unsigned char) (rand_int32_get(r) % (UCHAR_MAX + 1));
}

short rand_get_short(s_rand* r)
{
   return (short) (rand_int32_get(r) % (SHRT_MAX + 1));
}

unsigned short rand_get_ushort(s_rand* r)
{
   return (unsigned short) (rand_int32_get(r) % (USHRT_MAX + 1));
}

float rand_get_float(s_rand* r)
{
   return rand_int32_get(r)
      * ((float) (1) / (float) (4294967295));
}

double rand_get_double(s_rand* r)
{
   return rand_int32_get(r)
      * ((double) (1) / (double) (4294967295));
}
