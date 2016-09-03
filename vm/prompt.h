#ifndef PROMPT_H_
# define PROMPT_H_

/**
 * Allocates and returns a new string containing line from stdin
 * The '\n' sign is ignored
 * @param question - string displayed in stdout if not NULL
 * @param dft - default response if not NULL, displayed in brackets
 */
char* promptLine(const char* question, const char* dft);

/**
 * Read a whole line from stdin and tries to convert it to integer
 * @param question - string displayed in stdout if not NULL
 * @param dft - pointer to default response if not NULL, displayed in brackets
 * @param ok - if not NULL, indicates if the convertion succeeded
 */
long promptInt(const char* question, long* dft, int* ok);

/**
 * Read a whole line from stdin and tries to convert it to double
 * @param question - string displayed in stdout if not NULL
 * @param dft - pointer to default response if not NULL, displayed in brackets
 * @param ok - if not NULL, indicates if the convertion succeeded
 */
double promptDouble(const char* question, double* dft, int* ok);

/**
 * Read a whole line from stdin and tries to convert it to char
 * @param question - string displayed in stdout if not NULL
 * @param dft - pointer to default response if not NULL, displayed in brackets
 * @param ok - if not NULL, indicates if the convertion succeeded
 */
char promptChar(const char* question, char* dft, int* ok);

/**
 * Read a whole line from stdin and tries to convert it to bool
 * y/y = 1, n/N = 0
 * @param question - string displayed in stdout if not NULL
 * @param dft - pointer to default response if not NULL, displayed in brackets
 * @param ok - if not NULL, indicates if the convertion succeeded
 */
int promptBool(const char* question, int* dft, int* ok);

#endif //!PROMPT_H_
