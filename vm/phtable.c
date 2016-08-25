#include "phtable.h"
#include <stdlib.h>
#include <assert.h>
#include "err.h"

static s_phtable_node* node_clone(s_phtable_node* node)
{
   s_phtable_node* clone = malloc(sizeof(s_phtable_node));
   clone->hash = node->hash;
   clone->key = node->key;
   clone->value = node->value;
   clone->next = node->next ? node_clone(node->next) : 0;
   return clone;
}

s_phtable* phtable_new(size_t size, f_phtable_hash hash, f_phtable_cmp cmp)
{
   assert(size > 0);
   s_phtable* t = malloc(sizeof(s_phtable));
   t->table = calloc(size, sizeof(s_phtable_node*));
   t->size = size;
   t->hash = hash;
   t->cmp = cmp;
   t->count = 0;
   return t;
}

void phtable_free(s_phtable* s)
{
   phtable_clear(s);
   free(s);
}

s_phtable* phtable_clone(s_phtable* s)
{
   s_phtable* clone = phtable_new(s->size, s->hash, s->cmp);
   clone->count = s->count;
   for(size_t i = 0; i < s->size; ++i)
      if(s->table[i])
         clone->table[i] = node_clone(s->table[i]);
   return clone;
}

void phtable_extend(s_phtable* dst, s_phtable* src)
{
   for(s_phtable_node* it = phtable_begin(src); it; it = phtable_next(src, it))
   {
      phtable_set(dst, it->key, it->value);
   }
}

size_t phtable_size(s_phtable* s)
{
   return s->count;
}

int phtable_empty(s_phtable* s)
{
   return !s->count;
}

int phtable_contains(s_phtable* s, t_phtablek key)
{
   return !!phtable_find(s, key);
}

t_phtablev phtable_get(s_phtable* s, t_phtablek key)
{
   s_phtable_node* node = phtable_find(s, key);
   if(!node)
      err("hashtable doesn't contains this key");
   return node->value;
}

int phtable_insert(s_phtable* s, t_phtablek key, t_phtablev value)
{
   uint32_t hash = s->hash(key) % s->size;
   s_phtable_node* head = s->table[hash];
   s_phtable_node* node = head;

   while(node)
   {
      if(!s->cmp(node->key, key))
         return 0;
      node = node->next;
   }

   s_phtable_node* elem = malloc(sizeof(s_phtable_node));
   elem->hash = hash;
   elem->key = key;
   elem->value = value;
   elem->next = head;
   s->table[hash] = elem;
   ++s->count;
   return 1;
}

void phtable_set(s_phtable* s, t_phtablek key, t_phtablev value)
{
   uint32_t hash = s->hash(key) % s->size;
   s_phtable_node* head = s->table[hash];
   s_phtable_node* node = head;

   while(node)
   {
      if(!s->cmp(node->key, key))
      {
         node->value = value;
         return;
      }
      node = node->next;
   }

   s_phtable_node* elem = malloc(sizeof(s_phtable_node));
   elem->hash = hash;
   elem->key = key;
   elem->value = value;
   elem->next = head;
   s->table[hash] = elem;
   ++s->count;
}

void phtable_clear(s_phtable* s)
{
   for(size_t i = 0; i < s->size; ++i)
   {
      s_phtable_node* node = s->table[i];
      while(node)
      {
         s_phtable_node* next = node->next;
         free(node);
         node = next;
      }
      s->table[i] = NULL;
   }
   s->count = 0;
}

int phtable_remove(s_phtable* s, t_phtablek key)
{
   uint32_t hash = s->hash(key) % s->size;
   s_phtable_node** pnode = s->table + hash;

   while(*pnode)
   {
      s_phtable_node* node = *pnode;
      if(!s->cmp(node->key, key))
      {
         *pnode = node->next;
         --s->count;
         free(node);
         return 1;
      }
      pnode = &(node->next);
   }

   assert(phtable_find(s, key));
   return 0;
}

s_phtable_node *phtable_find(s_phtable *t, t_phtablek key)
{
   s_phtable_node* node = t->table[t->hash(key) % t->size];
   while(node && t->cmp(node->key, key))
      node = node->next;
   return node;
}


s_phtable_node* phtable_begin(s_phtable* t)
{
   for(size_t i = 0; i < t->size; ++i)
      if(t->table[i])
         return t->table[i];
   return NULL;
}

s_phtable_node* phtable_next(s_phtable* s, s_phtable_node* node)
{
   if(node->next)
      return node->next;

   for(size_t i = node->hash + 1; i < s->size; ++i)
      if(s->table[i])
         return s->table[i];
   return NULL;
}

t_phtablev phtable_key(s_phtable_node* node)
{
   if(!node)
      err("Unable to get key of undefined hashmap node");
   return node->key;
}

t_phtablev phtable_value(s_phtable_node* node)
{
   if(!node)
      err("Unable to get value of undefined hashmap node");
   return node->value;
}

void phtable_foreach(s_phtable* s, f_phtable_foreach f)
{
   for(s_phtable_node* it = phtable_begin(s); it; it = phtable_next(s, it))
   {
      f(it->key, it->value);
   }
}

int phtable_every(s_phtable* s, f_phtable_pred f)
{
   for(s_phtable_node* it = phtable_begin(s); it; it = phtable_next(s, it))
   {
      if(!f(it->key, it->value))
         return 0;
   }
   return 1;
}

int phtable_some(s_phtable* s, f_phtable_pred f)
{
   for(s_phtable_node* it = phtable_begin(s); it; it = phtable_next(s, it))
   {
      if(f(it->key, it->value))
         return 1;
   }
   return 0;
}

s_phtable* phtable_filter(s_phtable* s, f_phtable_pred f)
{
   s_phtable* s2 = phtable_new(s->size, s->hash, s->cmp);
   for(s_phtable_node* it = phtable_begin(s); it; it = phtable_next(s, it))
   {
      if(f(it->key, it->value))
         phtable_insert(s2, it->key, it->value);
   }

   return s2;
}

s_phtable_node* phtable_search(s_phtable* s, f_phtable_pred f)
{
   for(s_phtable_node* it = phtable_begin(s); it; it = phtable_next(s, it))
   {
      if(f(it->key, it->value))
         return it;
   }

   return NULL;
}
