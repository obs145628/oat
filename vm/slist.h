/*
 * @file SList struct definiton
 */

#ifndef SLIST_H_
# define SLIST_H_

# include <stddef.h>

typedef struct {
   char** begin;
   char** end;
   size_t len;
   size_t allocated;
} SList;

SList* SListNew();
SList* SListNewFromIt(char** begin, char** end);
void SListFree(SList* l);
SList* SListClone(const SList* l);

void SListInsert(SList* l, size_t pos, char* s);
void SListInsertFront(SList* l, char* s);
void SListInsertBack(SList* l, char* s);
char* SListPop(SList* l, size_t pos);
char* SListPopFront(SList* l);
char* SListPopBack(SList* l);
void SListRemove(SList* l, size_t pos);
void SListRemoveFront(SList* l);
void SListRemoveBack(SList* l);
void SListClear(SList* l);

char* SListJoin(SList* l, const char* sep);

# endif
