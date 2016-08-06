#include "io.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "err.h"

#define NUM_BUFF_LEN 1024
#define FLOAT_PRECISION 1e-10

static FILE* io_w_file;

static size_t io_w_fileWriter(const char* data, size_t len)
{
   return fwrite(data, 1, len, io_w_file);
}

ioWrite_f ioFileWriter(FILE* f)
{
   io_w_file = f;
   return io_w_fileWriter;
}

static char* io_w_buffer;

static size_t io_w_bufferWriter(const char* data, size_t len)
{
   memcpy(io_w_buffer, data, len);
   io_w_buffer += len;
   return len;
}

ioWrite_f ioBufferWritter(char* buffer)
{
   io_w_buffer = buffer;
   return io_w_bufferWriter;
}


static size_t* io_w_counter;

static size_t io_w_counterWriter(const char* data, size_t len)
{
   if(data){}
   *io_w_counter += len;
   return len;
}

ioWrite_f ioCounterWritter(size_t* psize)
{
   io_w_counter = psize;
   *io_w_counter = 0;
   return io_w_counterWriter;
}



size_t ioStdoutWriter(const char* data, size_t len)
{
   return fwrite(data, 1, len, stdout);
}

size_t ioStderrWriter(const char* data, size_t len)
{
   return fwrite(data, 1, len, stderr);
}

size_t ioWriteBytes(ioWrite_f writer, const char* data, size_t len)
{
   if(writer(data, len) != len)
      err("Ouput error: invalid number of bytes written");
   return len;
}

size_t ioWriteChar(ioWrite_f writer, char c)
{
   return ioWriteBytes(writer, &c, 1);
}

size_t ioWriteString(ioWrite_f writer, const char* str)
{
   return ioWriteBytes(writer, str, strlen(str));
}

size_t ioWriteInt(ioWrite_f writer, long x)
{
   static char buffer[NUM_BUFF_LEN];
   int i = NUM_BUFF_LEN;
   size_t len = 0;

   if(x < 0)
   {
      len += ioWriteChar(writer, '-');
      x = -x;
   }

   do
   {
      char digit = (char) (x % 10);
      buffer[--i] = digit + '0';
      x /= 10;
   }   while(x);

   len += ioWriteBytes(writer, buffer + i, NUM_BUFF_LEN - i);
   return len;
}

size_t ioWriteFloat(ioWrite_f writer, double x)
{
   static char buffer[NUM_BUFF_LEN];
   int i = NUM_BUFF_LEN;
   size_t len = 0;

   if(x < 0)
   {
      len += ioWriteChar(writer, '-');
      x = -x;
   }

   double intPart = floor(x);
   double decPart = x - intPart;

   do
   {
      char digit = (char) fmod(intPart, 10.0);
      buffer[--i] = digit + '0';
      intPart = floor(intPart / 10.0);
   }   while(intPart > FLOAT_PRECISION);
   len += ioWriteBytes(writer, buffer + i, NUM_BUFF_LEN - i);

   if(decPart <= FLOAT_PRECISION)
      return len;

   len += ioWriteChar(writer, '.');

   do
   {
      decPart *= 10.0;
      double fDigit = floor(decPart);
      char digit = (char) fDigit;
      decPart -= fDigit;
      len += ioWriteChar(writer, digit + '0');
   }    while(decPart > FLOAT_PRECISION);
   return len;
}

size_t ioWritef(ioWrite_f writer, const char* format, ...)
{
   va_list args;
   va_start(args, format);
   size_t len = ioVwritef(writer, format, args);
   va_end(args);
   return len;
}

size_t ioVwritef(ioWrite_f writer, const char* format, va_list ap)
{
   size_t len = 0;
   int percent = 0;
   int ival;
   char cval;
   double dval;
   float fval;
   long lval;
   const char *sval;

   for(const char* it = format; *it; ++it)
   {
      char c = *it;
      if(!percent)
      {
         if(c == '%')
            percent = 1;
         else
            len += ioWriteChar(writer, c);
      }

      else
      {
         if(c == 'b')
         {
            ival = va_arg(ap, int);
            if(ival)
               len += ioWriteBytes(writer, "true", 4);
            else
               len += ioWriteBytes(writer, "false", 5);
         }

         else if(c == 'c')
         {
            cval = va_arg(ap, int);
            len += ioWriteChar(writer, cval);
         }

         else if(c == 'd')
         {
            dval = va_arg(ap, double);
            len += ioWriteFloat(writer, dval);
         }

         else if(c == 'f')
         {
            fval = va_arg(ap, double);
            len += ioWriteFloat(writer, fval);
         }

         else if(c == 'i')
         {
            ival = va_arg(ap, int);
            len += ioWriteInt(writer, ival);
         }

         else if(c == 'l')
         {
            lval = va_arg(ap, long);
            len += ioWriteInt(writer, lval);
         }

         else if(c == 's')
         {
            sval = va_arg(ap, const char*);
            len += ioWriteString(writer, sval);
         }

         else if(c == '%')
         {
            len += ioWriteChar(writer, '%');
         }

         else
         {
            err("format: use of unnown symbol symbol %c\n", c);
         }

         percent = 0;
      }
   }

   return len;
}


size_t ioOutf(const char* format, ...)
{
   va_list args;
   va_start(args, format);
   size_t len = ioVwritef(ioStdoutWriter, format, args);
   va_end(args);
   return len;
}

size_t ioErrf(const char* format, ...)
{
   va_list args;
   va_start(args, format);
   size_t len = ioVwritef(ioStderrWriter, format, args);
   va_end(args);
   return len;
}

size_t ioFwritef(FILE* f, const char* format, ...)
{
   va_list args;
   va_start(args, format);
   size_t len = ioVwritef(ioFileWriter(f), format, args);
   va_end(args);
   return len;
}

size_t ioSwritef(char* buffer, const char* format, ...)
{
   va_list args;
   va_start(args, format);
   ioWrite_f writer = ioBufferWritter(buffer);
   size_t len = ioVwritef(writer, format, args);
   len += ioWriteBytes(writer, "\0", 1);
   va_end(args);
   return len;
}







static FILE* io_r_file;

static size_t io_r_fileReader(char* buffer, size_t len)
{
   return fread(buffer, 1, len, io_r_file);
}

ioRead_f ioFileReader(FILE* f)
{
   io_r_file = f;
   return io_r_fileReader;
}


static const char* io_r_buffer;

static size_t io_r_bufferReader(char* buffer, size_t len)
{
   memcpy(buffer, io_r_buffer, len);
   io_r_buffer += len;
   return len;
}

ioRead_f ioBufferReader(const char* buffer)
{
   io_r_buffer = buffer;
   return io_r_bufferReader;
}

static const char* io_r_str;

static size_t io_r_strReader(char* buffer, size_t len)
{
   size_t n = 0;
   while(*io_r_str != '\0' && n < len)
   {
      *(buffer++) = *(io_r_str)++;
      ++n;
   }
   return n;
}

ioRead_f ioStringReader(const char* str)
{
   io_r_str = str;
   return io_r_strReader;
}

static size_t* io_r_counter;

static size_t io_r_counterReader(char* buffer, size_t len)
{
   if(buffer){}
   *io_r_counter += len;
   return len;
}

ioRead_f ioCounterReader(size_t* psize)
{
   io_r_counter = psize;
   *io_r_counter = 0;
   return io_r_counterReader;
}

size_t ioStdinReader(char* buffer, size_t len)
{
   return fread(buffer, 1, len, stdin);
}


size_t ioReadBytes(ioRead_f reader, char* dst, size_t len)
{
   if(reader(dst, len) != len)
      err("Input error: invalid number of bytes read");
   return len;
}

size_t ioReadString(ioRead_f reader, char* dst, size_t len)
{
   ioReadBytes(reader, dst, len);
   dst[len] = '\0';
   return len;
}

size_t ioReadBytesUntil(ioRead_f reader, char* dst, char c)
{
   char it;
   size_t len = 0;
   do
   {
      len += ioReadBytes(reader, &it, 1);
      *(dst++) = it;
   } while(it != c);
   return len;
}

size_t ioReadStringUntil(ioRead_f reader, char* dst, char c)
{
   size_t len = ioReadBytesUntil(reader, dst, c);
   dst[len] = '\0';
   return len;
}
