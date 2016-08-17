/*
 * Functions to manipulates path string, don't do i/o opreations
 */

#ifndef PATH_H_
# define PATH_H_

/*
 * Allocates a new string containing the last portion of a path
 */
char* pathBasename(const char* s);

/*
 *Allocates a new string containing the directory name of a path
 */
char* pathDirname(const char* s);

/*
 * Allocates a new string containing the extension of the path
 */
char* pathExtname(const char* s);

/*
 * Checks if a path is absoute
 */
int pathIsAbsolute(const char* s);

/*
 * Normalize the given path by resolving '..' and '.' segments
 * Returns a newly allocated string
 */
char* pathNormalize(const char* s);

/*
 * Returns pathNormalize(s1 + '/' + s2)
 */
char* pathJoin(const char* s1, const char* s2);

# endif//!PATH_H_
