/*
 * @file set of functions to handle errors
 */

#ifndef ERR_H_
# define ERR_H_

# ifdef __cplusplus
extern "C"
{
#  if 0
}
#  endif
# endif

# include <stddef.h>

void err_set_scope_name(const char* scope_name, size_t len);
void err_set_line_(size_t line);


void err(const char* format, ...);

void errAssert(int b, const char* format, ...);

# ifdef __cplusplus
}
# endif

#endif //!ERR_H_
