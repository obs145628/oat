#ifndef PHSET_H_
# define PHSET_H_

# include <stddef.h>
# include <stdint.h>

# define PHSET_END (0)

typedef void* t_phsetk;
typedef void* t_phsetv;
typedef int (*f_phset_cmp)(t_phsetk a, t_phsetk b);
typedef uint32_t (*f_phset_hash)(t_phsetk key);
typedef void (*f_phset_foreach)(t_phsetk key);
typedef int (*f_phset_pred)(t_phsetk key);


struct phsetnode
{
        uint32_t hash;
        t_phsetk key;
        struct phsetnode* next;
};

typedef struct phsetnode s_phset_node;

typedef struct
{
        s_phset_node** table;
        f_phset_hash hash;
        f_phset_cmp cmp;
        size_t size;
        size_t count;
} s_phset;

s_phset* phset_new(size_t size, f_phset_hash hash, f_phset_cmp cmp);

void phset_free(s_phset* s);
s_phset* phset_clone(s_phset* s);
void phset_extend(s_phset* dst, s_phset* src);

size_t phset_size(s_phset* s);
int phset_empty(s_phset* s);
int phset_contains(s_phset* s, t_phsetk key);
int phset_insert(s_phset* s, t_phsetk key);
void phset_clear(s_phset* s);
int phset_remove(s_phset* s, t_phsetk key);


s_phset_node *phset_find(s_phset *t, t_phsetk key);
s_phset_node* phset_begin(s_phset* t);
s_phset_node* phset_next(s_phset* s, s_phset_node* node);
t_phsetv phset_key(s_phset_node* node);

void phset_foreach(s_phset* s, f_phset_foreach f);
int phset_every(s_phset* s, f_phset_pred f);
int phset_some(s_phset* s, f_phset_pred f);
s_phset* phset_filter(s_phset* s, f_phset_pred f);
s_phset_node* phset_search(s_phset* s, f_phset_pred f);

#endif //!PHSET_H_
