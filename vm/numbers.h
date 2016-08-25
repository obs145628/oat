/*
 * @file set of functions to manipulate numbers and strings
 */

#ifndef NUMBER_H_
# define NUMBER_H_

/*
 * Converts a string to an unsigned integer
 * All charachters must be numbers
 * @param  str - string that represents the number in base 10
 * @param  ok - if not null, will contains if the convertion succeeded
 */
long strToUInt(const char* str, int* ok);

/*
 * Converts a string to an integer
 * All charachters must be numbers (except a leading -)
 * @param  str - string that represents the number in base 10
 * @param  ok - if not null, will contains if the convertion succeeded
 */
long strToInt(const char* str, int* ok);

/*
 * Converts an integer to it's string representation
 * The returned string is allocated
 * @param x - the number to convert
 */
char* intToStr(long x);

/*
 * Converts a string to an unsigned float
 * All charachters must be numbers (except for the dot)
 * @param  str - string that represents the float
 * @param  ok - if not null, will contains if the convertion succeeded
 */
double strToUFloat(const char* str, int* ok);

/*
 * Converts a string to a float
 * All charachters must be numbers (export fot the dot and a leading -)
 * @param  str - string that represents the float
 * @param  ok - if not null, will contains if the convertion succeeded
 */
double strToFloat(const char* str, int* ok);

/*
 * Converts a float to it's string representation
 * The returned string is allocated
 * @param x - the number to convert
 */
char* floatToStr(double x);


long strToLong(const char* str, int* ok);
double strToDouble(const char* str, int* ok);


#endif //!NUMBER_H_
