#ifndef PMAP_H_
# define PMAP_H_

# ifdef __cplusplus
extern "C"
{
# endif

# include <stddef.h>

typedef void* t_pmapk;
typedef void* t_pmapv;
typedef int (*f_pmap_cmp)(t_pmapk a, t_pmapk b);
typedef void (*f_pmap_foreach)(t_pmapk key, t_pmapv value);
typedef int (*f_pmap_pred)(t_pmapk key, t_pmapv value);


struct pmapnode
{
        t_pmapk key;
        t_pmapv value;
        struct pmapnode* left;
        struct pmapnode* right;
        struct pmapnode* parent;
        int isBlack;
};

typedef struct pmapnode s_pmap_node;

typedef struct
{
        s_pmap_node* root;
        s_pmap_node* leaf;
        f_pmap_cmp cmp;
        size_t size;
} s_pmap;

s_pmap* pmap_new(f_pmap_cmp cmp);

void pmap_free(s_pmap* s);
s_pmap* pmap_clone(s_pmap* s);
void pmap_extend(s_pmap* dst, s_pmap* src);

size_t pmap_size(s_pmap* s);
int pmap_empty(s_pmap* s);
int pmap_contains(s_pmap* s, t_pmapk key);
t_pmapv pmap_get(s_pmap* s, t_pmapk key);
int pmap_insert(s_pmap* s, t_pmapk key, t_pmapv value);
void pmap_set(s_pmap* s, t_pmapk key, t_pmapv value);
void pmap_clear(s_pmap* s);
int pmap_remove(s_pmap* s, t_pmapk key);


s_pmap_node *pmap_find(s_pmap *t, t_pmapk key);
s_pmap_node* pmap_min(s_pmap* t);
s_pmap_node* pmap_max(s_pmap* t);
s_pmap_node* pmap_begin(s_pmap* t);
s_pmap_node* pmap_end(s_pmap* t);
s_pmap_node* pmap_prev(s_pmap* s, s_pmap_node* node);
s_pmap_node* pmap_next(s_pmap* s, s_pmap_node* node);
t_pmapv pmap_key(s_pmap *t, s_pmap_node* node);
t_pmapv pmap_value(s_pmap *t, s_pmap_node* node);



void pmap_foreach(s_pmap* s, f_pmap_foreach f);
int pmap_every(s_pmap* s, f_pmap_pred f);
int pmap_some(s_pmap* s, f_pmap_pred f);
s_pmap* pmap_filter(s_pmap* s, f_pmap_pred f);
s_pmap_node* pmap_search(s_pmap* s, f_pmap_pred f);

# ifdef __cplusplus
}
# endif

#endif //!PMAP_H_
