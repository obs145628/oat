#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "err.h"
#include "str.h"
#include "path.h"


#define FS_BUFFER_LEN 1024
static char _fsBuffer[FS_BUFFER_LEN];

char* fsReadAllBytes(const char* path, size_t* size)
{
   FILE* f;
   if(!(f = fopen(path, "rb")))
      err("Unable to open file %s\n", path);

   fseek(f, 0, SEEK_END);
   size_t len = ftell(f);
   rewind(f);

   char* buffer = malloc(len);
   if(fread(buffer, 1, len, f) != len)
      err("Unable to read file %s (%zu / %zu)\n", path);

   fclose(f);

   if(size)
      *size = len;

   return buffer;
}

char* fsReadAllText(const char* path, size_t* size)
{
   FILE* f;
   if(!(f = fopen(path, "rb")))
      err("Unable to open file %s\n", path);

   fseek(f, 0, SEEK_END);
   size_t len = ftell(f);
   rewind(f);

   char* buffer = malloc(len + 1);
   if(fread(buffer, 1, len, f) != len)
      err("Unable to read file %s\n", path);

   buffer[len] = '\0';
   fclose(f);

   if(size)
      *size = len;

   return buffer;
}

void fsWriteBytes(const char* path, const char* data, size_t len)
{
   FILE* f;
   if(!(f = fopen(path, "wb")))
      err("Unable to open file %s\n", path);

   if(fwrite(data, 1, len, f) != len)
      err("Unable to write file %s\n", path);

   fclose(f);
}

void fsWriteText(const char* path, const char* content)
{
   fsWriteBytes(path, content, strlen(content));
}

void fsAppendBytes(const char* path, const char* data, size_t len)
{
   FILE* f;
   if(!(f = fopen(path, "ab")))
      err("fUnable to open file %s\n", path);

   if(fwrite(data, 1, len, f) != len)
      err("Unable to write file %s\n", path);

   fclose(f);
}

void fsAppendText(const char* path, const char* content)
{
   fsAppendBytes(path, content, strlen(content));
}


void fsCopy(const char* src, const char* dst)
{
   FILE *is, *os;
   size_t len;

   if(!(is = fopen(src, "rb")))
      err("Unable to open src file %s\n", src);
   if(!(os = fopen(dst, "wb")))
      err("Unable to open dst file %s\n", dst);

   while((len = fread(_fsBuffer, 1, FS_BUFFER_LEN, is)) != 0)
   {
      if(fwrite(_fsBuffer, 1, len, os) != len)
         err("Unable to write to dst file %s\n", dst);
   }
}

void fsMove(const char* src, const char* dst)
{
   if(rename(src, dst))
      err("Unable to move %s to %s\n", src, dst);
}

void fsRemove(const char* path)
{
   if(remove(path))
      err("Unable to remove %s\n", path);
}

FsStat fsStat(const char* path)
{
   struct stat s;
   if(lstat(path, &s))
      err("Unable to stat %s\n", path);
   return s;
}

int fsExists(const char* path)
{
   struct stat s;
   return !lstat(path, &s);
}

int fsIsFile(const char* path)
{
   struct stat s  = fsStat(path);
   return (s.st_mode & S_IFMT) == S_IFREG;
}

int fsIsDir(const char* path)
{
   struct stat s  = fsStat(path);
   return (s.st_mode & S_IFMT) == S_IFDIR;
}

SList* fsReadDir(const char* path)
{
   DIR* d;
   struct dirent* dir;

   if(!(d = opendir(path)))
      err("Unable to open dir %s\n", path);

   SList* l = SListNew();

   while((dir = readdir(d)) != NULL)
   {
      if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
         SListInsertBack(l, strClone(dir->d_name));
   }

   return l;
}

void fsMakeDir(const char* path, int mode)
{
   if(mkdir(path, mode) == -1)
      err("Unable to mkdir %s", path);
}

void fsRemoveDir(const char* path)
{
   DIR* d;
   struct dirent* dir;

   if(!(d = opendir(path)))
      err("Unable to open dir %s\n", path);

   while((dir = readdir(d)) != NULL)
   {
      if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
      {
         char* sub = pathJoin(path, dir->d_name);
         if(fsIsDir(sub))
            fsRemoveDir(sub);
         else
            fsRemove(sub);
         free(sub);
      }
   }

   fsRemove(path);
}

void fsCopyDir(const char* src, const char* dst)
{
   DIR* d;
   struct dirent* dir;

   if(!(d = opendir(src)))
      err("Unable to open src dir %s\n", src);

   fsMakeDir(dst, 0700); //0700 ?

   while((dir = readdir(d)) != NULL)
   {
      if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
      {
         char* srcSub = pathJoin(src, dir->d_name);
         char* dstSub = pathJoin(dst, dir->d_name);
         if(fsIsDir(srcSub))
            fsCopyDir(srcSub, dstSub);
         else
            fsCopy(srcSub, dstSub);
         free(srcSub);
         free(dstSub);
      }
   }
}

char* fsHomePath()
{
   const char* home;
   if((home =getenv("HOME")) == NULL)
      home = getpwuid(getuid())->pw_dir;
   return strClone(home);
}
