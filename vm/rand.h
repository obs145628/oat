#ifndef RAND_H_
# define RAND_H_

# define RAND_N 624
# define RAND_M 397
# define RAND_MATRIX_A 0x9908b0dfUL
# define RAND_UPPER_MASK 0x80000000UL
# define RAND_LOWER_MASK 0x7fffffffUL

typedef struct {
   unsigned long _mt[RAND_N];
   int _mti;
   unsigned long _mag01[2];
} s_rand;

s_rand* rand_new(unsigned long s);
void rand_free(s_rand* r);

unsigned long rand_int32_get(s_rand* r);
long rand_int31_get(s_rand* r);

int rand_get_int(s_rand* r);
unsigned int rand_get_uint(s_rand* r);
long rand_get_long(s_rand* r);
unsigned long rand_get_ulong(s_rand* r);
int rand_get_bool(s_rand* r);
char rand_get_char(s_rand* r);
unsigned char rand_get_uchar(s_rand* r);
short rand_get_short(s_rand* r);
unsigned short rand_get_ushort(s_rand* r);
float rand_get_float(s_rand* r);
double rand_get_double(s_rand* r);

#endif //!RAND_H_
