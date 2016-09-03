/*
 * @file set of functions to manipulate numbers and strings
 */

#ifndef NUMBER_H_
# define NUMBER_H_

/*
 * Converts a long to it's string representation
 * The returned string is allocated
 * @param x - the number to convert
 */
char* longToStr(long x);

/*
 * Converts a double to it's string representation
 * The returned string is allocated
 * @param x - the number to convert
 */
char* doubleToStr(double x);


/*
 * Converts a string to a long
 * All charachters must be numbers (except a leading -)
 * @param  str - string that represents the number in base 10
 * @param  ok - if not null, will contains if the convertion succeeded
 */
long strToLong(const char* str, int* ok);

/*
 * Converts a string to a double
 * All charachters must be numbers (export fot the dot and a leading -)
 * @param  str - string that represents the float
 * @param  ok - if not null, will contains if the convertion succeeded
 */
double strToDouble(const char* str, int* ok);


#endif //!NUMBER_H_
