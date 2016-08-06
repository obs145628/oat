/**
 * @file function to manipulate 0 terminated char arrays
 */

#ifndef STR_H_
# define STR_H_

# include <stddef.h>
# include "slist.h"

# define STR_NFOUND ((size_t) -1)

/**
 * Allocates and returns a copy of a string
 */
char* strClone(const char* s);

/**
 * Allocates and returns a string using io format
 */
char* strFormat(const char* format, ...);

/**
 * Allocates and returns a new string using printf
 */
char* strPrintf(const char* format, ...);

/**
 * Allocate and retrns a string which is the concatenation od two strings
 */
char* strConcat(const char* s1, const char* s2);

/**
 * Returns the first index of the char in a string, or STR_NFOUND
 */
size_t strIndexOfChar(const char* s, char c);

/**
 * Returns the last index of the char in a string, or STR_NFOUND
 */
size_t strLastIndexOfChar(const char* s, char c);

/**
 * Allocates a new string containing data from s[pos] to s[pos + size - 1]
 */
char* strSubstring(const char* s, size_t pos, size_t size);

/**
 * Allocates a new string containing the n first characters of s
 */
char* strPrefix(const char* s, size_t n);

/**
 * Allocates a new string containing the n last charachets of s
 */
char* strSuffix(const char* s, size_t n);

/**
 * Allocates a new string which contains the last characthers of s from index
 * pos
 */
char* strSuffixFrom(const char* s, size_t pos);

/**
 * Allocates a new string which is the lowercase version of the first one
 */
char* strToLower(const char* s);

/**
 * Allocates a new string which is the uppercase version of the first one
 */
char* strToUpper(const char* s);


/**
 * Counts the number of occurences of a charachter in a string
 */
size_t strCountChar(const char* s, char c);

/**
 * Returns an allocated SList containing the tokens of a string,
 * split with a char
 */
SList* strSplitChar(const char* s, char sep);

/**
 * Returns a newly allocated string without blanck trails chars
 */
char* strTrim(const char* s);

/**
 * Allocates a new string containing an integer
 */
char* strFromInt(long x);

/**
 * Allocates a new string containing a float number
 */
char* strFromFloat(double x);




#endif //!STR_H_
