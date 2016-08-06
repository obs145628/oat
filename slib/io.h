/*
 * Set of basic io functions
 */

#ifndef IO_H_
# define IO_H_

# include <stddef.h>
# include <stdarg.h>
# include <stdio.h>

typedef size_t (*ioWrite_f)(const char* data, size_t len);
typedef size_t (*ioRead_f)(char* buffer, size_t len);

ioWrite_f ioFileWriter(FILE* f);
ioWrite_f ioBufferWritter(char* buffer);
ioWrite_f ioCounterWritter(size_t* psize);

size_t ioStdoutWriter(const char* data, size_t len);
size_t ioStderrWriter(const char* data, size_t len);

size_t ioWriteBytes(ioWrite_f writer, const char* data, size_t len);
size_t ioWriteChar(ioWrite_f writer, char c);
size_t ioWriteString(ioWrite_f writer, const char* str);
size_t ioWriteInt(ioWrite_f writer, long x);
size_t ioWriteFloat(ioWrite_f writer, double x);

/*
 * %b (int) : write 'true' or 'false'
 * %c (char) : write a char
 * %d (double) : write a double
 * %f (float) : write a float
 * %i (int) : write an integer
 * %l (long) : write a long
 * %s (const char *) : write a string
 * %% (void) : write '%'
 */
size_t ioWritef(ioWrite_f writer, const char* format, ...);

size_t ioVwritef(ioWrite_f writer, const char* format, va_list ap);

size_t ioOutf(const char* format, ...);
size_t ioErrf(const char* format, ...);
size_t ioFwritef(FILE* f, const char* format, ...);
size_t ioSwritef(char* buffer, const char* format, ...);

ioRead_f ioFileReader(FILE* f);
ioRead_f ioBufferReader(const char* buffer);
ioRead_f ioStringReader(const char* str);
ioRead_f ioCounterReader(size_t* psize);


size_t ioStdinReader(char* buffer, size_t len);

size_t ioReadBytes(ioRead_f reader, char* dst, size_t len);
size_t ioReadString(ioRead_f reader, char* dst, size_t len);
size_t ioReadBytesUntil(ioRead_f reader, char* dst, char c);
size_t ioReadStringUntil(ioRead_f reader, char* dst, char c);

#endif //!IO_H_
