#include "slist.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "str.h"

#define MIN_ALLOC 16

static void SListResize_(SList* l, size_t newLength)
{
   size_t newAllocated = l->allocated;
   while(newLength > newAllocated)
      newAllocated *= 2;

   if(newAllocated != l->allocated)
   {
      l->begin = realloc(l->begin, newAllocated * sizeof(char*));
      l->allocated = newAllocated;
   }

   l->end = l->begin + newLength;
   l->len = newLength;
}

SList* SListNew()
{
   SList* l = malloc(sizeof(SList));
   l->begin = malloc(MIN_ALLOC * sizeof(char*));
   l->end = l->begin;
   l->len = 0;
   l->allocated = MIN_ALLOC;
   return l;
}

void SListFree(SList* l)
{
   for(char** it = l->begin; it != l->end; ++it)
      free(*it);
   free(l->begin);
   free(l);
}

SList* SListClone(const SList* l)
{
   SList* clone = malloc(sizeof(SList));
   clone->begin = malloc(l->allocated * sizeof(char*));
   clone->end = clone->begin + l->len;
   clone->len = l->len;
   clone->allocated = l->allocated;

   for(size_t i = 0; i < l->len; ++i)
      clone->begin[i] = strClone(l->begin[i]);

   return clone;
}

void SListInsert(SList* l, size_t pos, char* s)
{
   assert(pos <= l->len);
   SListResize_(l, l->len + 1);

   memmove(l->begin + pos + 1, l->begin + pos,
           (l->len - pos - 1) * sizeof(char*));
   l->begin[pos] = s;
}

void SListInsertFront(SList* l, char* s)
{
   SListInsert(l, 0, s);
}

void SListInsertBack(SList* l, char* s)
{
   SListInsert(l, l->len, s);
}

char *SListPop(SList* l, size_t pos)
{
   assert(pos < l->len);
   char* s = l->begin[pos];

   memmove(l->begin + pos, l->begin + pos + 1,
           (l->len - pos - 1) * sizeof(char*));

   --l->len;
   --l->end;
   return s;
}

char* SListPopFront(SList* l)
{
   return SListPop(l, 0);
}

char* SListPopBack(SList* l)
{
   return SListPop(l, l->len - 1);
}

void SListRemove(SList* l, size_t pos)
{
   free(SListPop(l, pos));
}

void SListRemoveFront(SList* l)
{
   free(SListPop(l, 0));
}

void SListRemoveBack(SList* l)
{
   free(SListPop(l, l->len - 1));
}

void SListClear(SList* l)
{
   for(char** it = l->begin; it != l->end; ++it)
      free(*it);
   l->end = l->begin;
   l->len = 0;
}

char* SListJoin(SList* l, const char* sep)
{
   if(!l->len)
      return strClone("");

   size_t sepLen = strlen(sep);
   size_t len = sepLen * (l->len - 1);
   size_t* elemsLen = malloc(l->len * sizeof(size_t));

   for(size_t i = 0; i < l->len; ++i)
      len += (elemsLen[i] = strlen(l->begin[i]));

   char* res = malloc(len);
   char* it = res;

   for(size_t i = 0; i < l->len; ++i)
   {
      memcpy(it, l->begin[i], elemsLen[i]);
      it += elemsLen[i];
      if(i + 1 != l->len)
      {
         memcpy(it, sep, sepLen);
         it += sepLen;
      }
   }

   free(elemsLen);
   *it = '\0';
   return res;
}
