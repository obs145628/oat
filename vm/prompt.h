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
 * @param ok - if not NULL, indicates if the convertion succeeded
 */
long promptInt(const char* question, int* ok);

#endif //!PROMPT_H_
