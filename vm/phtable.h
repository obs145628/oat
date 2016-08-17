#ifndef PHTABLE_H_
# define PHTABLE_H_

# include <stddef.h>

# define PHTABLE_END (0)

typedef void* t_phtablek;
typedef void* t_phtablev;
typedef int (*f_phtable_cmp)(t_phtablek a, t_phtablek b);
typedef size_t (*f_phtable_hash)(t_phtablek key);
typedef void (*f_phtable_foreach)(t_phtablek key, t_phtablev value);
typedef int (*f_phtable_pred)(t_phtablek key, t_phtablev value);


struct phtablenode
{
        size_t hash;
        t_phtablek key;
        t_phtablev value;
        struct phtablenode* next;
};

typedef struct phtablenode s_phtable_node;

typedef struct
{
        s_phtable_node** table;
        f_phtable_hash hash;
        f_phtable_cmp cmp;
        size_t size;
        size_t count;
} s_phtable;

s_phtable* phtable_new(size_t size, f_phtable_hash hash, f_phtable_cmp cmp);

void phtable_free(s_phtable* s);
s_phtable* phtable_clone(s_phtable* s);
void phtable_extend(s_phtable* dst, s_phtable* src);

size_t phtable_size(s_phtable* s);
int phtable_empty(s_phtable* s);
int phtable_contains(s_phtable* s, t_phtablek key);
t_phtablev phtable_get(s_phtable* s, t_phtablek key);
int phtable_insert(s_phtable* s, t_phtablek key, t_phtablev value);
void phtable_set(s_phtable* s, t_phtablek key, t_phtablev value);
void phtable_clear(s_phtable* s);
int phtable_remove(s_phtable* s, t_phtablek key);


s_phtable_node *phtable_find(s_phtable *t, t_phtablek key);
s_phtable_node* phtable_begin(s_phtable* t);
s_phtable_node* phtable_next(s_phtable* s, s_phtable_node* node);
t_phtablev phtable_key(s_phtable_node* node);
t_phtablev phtable_value(s_phtable_node* node);



void phtable_foreach(s_phtable* s, f_phtable_foreach f);
int phtable_every(s_phtable* s, f_phtable_pred f);
int phtable_some(s_phtable* s, f_phtable_pred f);
s_phtable* phtable_filter(s_phtable* s, f_phtable_pred f);
s_phtable_node* phtable_search(s_phtable* s, f_phtable_pred f);

#endif //!PHTABLE_H_
