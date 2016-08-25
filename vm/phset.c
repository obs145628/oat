#include "phset.h"
#include <stdlib.h>
#include <assert.h>
#include "err.h"

static s_phset_node* node_clone(s_phset_node* node)
{
   s_phset_node* clone = malloc(sizeof(s_phset_node));
   clone->hash = node->hash;
   clone->key = node->key;
   clone->next = node->next ? node_clone(node->next) : 0;
   return clone;
}

s_phset* phset_new(size_t size, f_phset_hash hash, f_phset_cmp cmp)
{
   assert(size > 0);
   s_phset* t = malloc(sizeof(s_phset));
   t->table = calloc(size, sizeof(s_phset_node*));
   t->size = size;
   t->hash = hash;
   t->cmp = cmp;
   t->count = 0;
   return t;
}

void phset_free(s_phset* s)
{
   phset_clear(s);
   free(s);
}

s_phset* phset_clone(s_phset* s)
{
   s_phset* clone = phset_new(s->size, s->hash, s->cmp);
   clone->count = s->count;
   for(size_t i = 0; i < s->size; ++i)
      if(s->table[i])
         clone->table[i] = node_clone(s->table[i]);
   return clone;
}

void phset_extend(s_phset* dst, s_phset* src)
{
   for(s_phset_node* it = phset_begin(src); it; it = phset_next(src, it))
      phset_insert(dst, it->key);
}

size_t phset_size(s_phset* s)
{
   return s->count;
}

int phset_empty(s_phset* s)
{
   return !s->count;
}

int phset_contains(s_phset* s, t_phsetk key)
{
   return !!phset_find(s, key);
}

int phset_insert(s_phset* s, t_phsetk key)
{
   uint32_t hash = s->hash(key) % s->size;
   s_phset_node* head = s->table[hash];
   s_phset_node* node = head;

   while(node)
   {
      if(!s->cmp(node->key, key))
         return 0;
      node = node->next;
   }

   s_phset_node* elem = malloc(sizeof(s_phset_node));
   elem->hash = hash;
   elem->key = key;
   elem->next = head;
   s->table[hash] = elem;
   ++s->count;
   return 1;
}

void phset_clear(s_phset* s)
{
   for(size_t i = 0; i < s->size; ++i)
   {
      s_phset_node* node = s->table[i];
      while(node)
      {
         s_phset_node* next = node->next;
         free(node);
         node = next;
      }
      s->table[i] = NULL;
   }
   s->count = 0;
}

int phset_remove(s_phset* s, t_phsetk key)
{
   uint32_t hash = s->hash(key) % s->size;
   s_phset_node** pnode = s->table + hash;

   while(*pnode)
   {
      s_phset_node* node = *pnode;
      if(!s->cmp(node->key, key))
      {
         *pnode = node->next;
         --s->count;
         free(node);
         return 1;
      }
      pnode = &(node->next);
   }

   return 0;
}

s_phset_node *phset_find(s_phset *t, t_phsetk key)
{
   s_phset_node* node = t->table[t->hash(key) % t->size];
   while(node && t->cmp(node->key, key))
      node = node->next;
   return node;
}


s_phset_node* phset_begin(s_phset* t)
{
   for(size_t i = 0; i < t->size; ++i)
      if(t->table[i])
         return t->table[i];
   return NULL;
}

s_phset_node* phset_next(s_phset* s, s_phset_node* node)
{
   if(node->next)
      return node->next;

   for(size_t i = node->hash + 1; i < s->size; ++i)
      if(s->table[i])
         return s->table[i];
   return NULL;
}

t_phsetv phset_key(s_phset_node* node)
{
   if(!node)
      err("Unable to get key of undefined hashmap node");
   return node->key;
}

void phset_foreach(s_phset* s, f_phset_foreach f)
{
   for(s_phset_node* it = phset_begin(s); it; it = phset_next(s, it))
   {
      f(it->key);
   }
}

int phset_every(s_phset* s, f_phset_pred f)
{
   for(s_phset_node* it = phset_begin(s); it; it = phset_next(s, it))
   {
      if(!f(it->key))
         return 0;
   }
   return 1;
}

int phset_some(s_phset* s, f_phset_pred f)
{
   for(s_phset_node* it = phset_begin(s); it; it = phset_next(s, it))
   {
      if(f(it->key))
         return 1;
   }
   return 0;
}

s_phset* phset_filter(s_phset* s, f_phset_pred f)
{
   s_phset* s2 = phset_new(s->size, s->hash, s->cmp);
   for(s_phset_node* it = phset_begin(s); it; it = phset_next(s, it))
   {
      if(f(it->key))
         phset_insert(s2, it->key);
   }

   return s2;
}

s_phset_node* phset_search(s_phset* s, f_phset_pred f)
{
   for(s_phset_node* it = phset_begin(s); it; it = phset_next(s, it))
   {
      if(f(it->key))
         return it;
   }

   return NULL;
}
