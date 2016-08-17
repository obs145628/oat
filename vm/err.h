/*
 * @file set of functions to handle errors
 */

#ifndef ERR_H_
# define ERR_H_

# ifdef __cplusplus
extern "C"
{
# endif

void err(const char* format, ...);

void errAssert(int b, const char* format, ...);

# ifdef __cplusplus
}
# endif

#endif //!ERR_H_
