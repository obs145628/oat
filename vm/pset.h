#ifndef PSTET_H_
# define PSTET_H_

# include <stddef.h>

typedef void* t_psetv;
typedef int (*f_pset_cmp)(t_psetv a, t_psetv b);
typedef void (*f_pset_foreach)(t_psetv value);
typedef int (*f_pset_pred)(t_psetv value);
typedef t_psetv (*f_pset_map)(t_psetv value);
typedef t_psetv (*f_pset_reduce)(t_psetv memo, t_psetv value);


struct psetnode
{
        t_psetv key;
        struct psetnode* left;
        struct psetnode* right;
        struct psetnode* parent;
        int isBlack;
};

typedef struct psetnode s_pset_node;

typedef struct
{
        s_pset_node* root;
        s_pset_node* leaf;
        f_pset_cmp cmp;
        size_t size;
} s_pset;

s_pset* pset_new(f_pset_cmp cmp);
s_pset* pset_new_from_it(f_pset_cmp cmp, t_psetv* begin, t_psetv* end);

void pset_free(s_pset* s);
s_pset* pset_clone(s_pset* s);
s_pset* pset_union(s_pset* s1, s_pset* s2);
s_pset* pset_intersection(s_pset* s1, s_pset* s2);

size_t pset_size(s_pset* s);
int pset_empty(s_pset* s);
int pset_contains(s_pset* s, t_psetv value);
int pset_insert(s_pset* s, t_psetv value);
void pset_insert_it(s_pset* s, t_psetv* begin, t_psetv* end);
void pset_clear(s_pset* s);
int pset_remove(s_pset* s, t_psetv value);


s_pset_node *pset_find(s_pset *t, t_psetv e);
s_pset_node* pset_min(s_pset* t);
s_pset_node* pset_max(s_pset* t);
s_pset_node* pset_begin(s_pset* t);
s_pset_node* pset_end(s_pset* t);
s_pset_node* pset_prev(s_pset* s, s_pset_node* node);
s_pset_node* pset_next(s_pset* s, s_pset_node* node);
t_psetv pset_value(s_pset *t, s_pset_node* node);

void pset_foreach(s_pset* s, f_pset_foreach f);
int pset_every(s_pset* s, f_pset_pred f);
int pset_some(s_pset* s, f_pset_pred f);
s_pset* pset_filter(s_pset* s, f_pset_pred f);
s_pset_node* pset_search(s_pset* s, f_pset_pred f);
s_pset* pset_map(s_pset* s, f_pset_map f);
t_psetv pset_reduce(s_pset* s, t_psetv memo, f_pset_reduce f);


#endif //!PSTET_H_
