/*
 * @file set of functions to handle errors
 */

#ifndef ERR_H_
# define ERR_H_

void err(const char* format, ...);

void errAssert(int b, const char* format, ...);

#endif //!ERR_H_
