/*
 * @file Date functions manipulation
 */

#ifndef DATE_H_
# define DATE_H_

/**
 * Returns the timestamp in ms
 */
long dateNow();

/**
 * Returns the timestamp at a certain date in ms
 */
long dateAt(long year, long month, long day, long h, long m, long s, long ms);

/**
 * Get all date composants from a timestamp in ms
 * If a pointer is null, the value is ignorer
 */
void dateGetLocale(long time, long* year, long* month, long* day,
                   long* h, long* m, long* s, long* ms);



#endif //!DATE_H_
