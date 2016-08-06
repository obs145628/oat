#ifndef PLIST_H_
# define PLIST_H_

# include <stddef.h>

typedef void* t_plistv;
typedef void (*f_plist_foreach)(t_plistv value);
typedef int (*f_plist_pred)(t_plistv value);
typedef t_plistv (*f_plist_map)(t_plistv value);
typedef t_plistv (*f_plist_reduce)(t_plistv memo, t_plistv value);

extern const size_t PLIST_NFOUND;

typedef struct plist_node
{
        t_plistv value;
        struct plist_node* prev;
        struct plist_node* next;
} s_plist_node;

typedef struct
{
        s_plist_node* _front;
        size_t _size;
} s_plist;

s_plist* plist_new();
s_plist* plist_new_from_it(t_plistv* begin, t_plistv* end);

void plist_free(s_plist* s);
s_plist* plist_clone(s_plist* s);
void plist_print(s_plist* s);

size_t plist_size(s_plist* s);
int plist_empty(s_plist* s);
void plist_clear(s_plist* s);

s_plist_node* plist_front(s_plist* s);
s_plist_node* plist_back(s_plist* s);
void plist_move_front(s_plist* s, s_plist_node* node);
s_plist_node* plist_insert_before(s_plist* s, s_plist_node* node,
                                   t_plistv value);
s_plist_node* plist_insert_after(s_plist* s, s_plist_node* node,
                                   t_plistv value);
s_plist_node* plist_insert_it_before(s_plist* s, s_plist_node* node,
                                     t_plistv* begin, t_plistv* end);
s_plist_node* plist_insert_it_after(s_plist* s, s_plist_node* node,
                                     t_plistv* begin, t_plistv* end);
t_plistv plist_pop(s_plist* s, s_plist_node* node);
void plist_remove(s_plist* s, s_plist_node* node, size_t n);
void plist_copy(s_plist* s, s_plist_node* node, t_plistv* begin, t_plistv* end);

s_plist_node* plist_push_front(s_plist* s, t_plistv value);
s_plist_node* plist_push_back(s_plist* s, t_plistv value);
s_plist_node* plist_push_it_front(s_plist* s, t_plistv* begin, t_plistv* end);
s_plist_node* plist_push_it_back(s_plist* s, t_plistv* begin, t_plistv* end);
t_plistv plist_get_front(s_plist* s);
t_plistv plist_get_back(s_plist* s);
void plist_set_front(s_plist* s, t_plistv value);
void plist_set_back(s_plist* s, t_plistv value);
t_plistv plist_pop_front(s_plist* s);
t_plistv plist_pop_back(s_plist* s);
void plist_remove_front(s_plist* s, size_t n);
void plist_remove_back(s_plist* s, size_t n);

s_plist_node* plist_at(s_plist* s, size_t pos);
t_plistv plist_get_at(s_plist* s, size_t pos);
void plist_set_at(s_plist* s, size_t pos, t_plistv value);
void plist_copy_at(s_plist* s, size_t pos, t_plistv* begin, t_plistv* end);
void plist_insert_at(s_plist* s, size_t pos, t_plistv value);
void plist_insert_it_at(s_plist* s, size_t pos, t_plistv* begin, t_plistv* end);
t_plistv plist_pop_at(s_plist* s, size_t pos);
void plist_remove_at(s_plist* s, size_t pos, size_t n);
void plist_resize(s_plist* s, size_t size);

size_t plist_indexof(s_plist* v, t_plistv value);
size_t plist_lastindexof(s_plist* v, t_plistv value);
s_plist_node* plist_nodeof(s_plist* s, t_plistv value);
s_plist_node* plist_lastnodeof(s_plist* s, t_plistv value);
int plist_contains(s_plist* v, t_plistv value);
s_plist* plist_slice(s_plist* v, size_t begin, size_t end);
s_plist* plist_concat(s_plist* v1, s_plist* v2);

void plist_foreach(s_plist* v, f_plist_foreach f);
int plist_every(s_plist* v, f_plist_pred f);
int plist_some(s_plist* v, f_plist_pred f);
s_plist* plist_filter(s_plist* v, f_plist_pred f);
s_plist_node* plist_find(s_plist* v, f_plist_pred f);
size_t plist_findindex(s_plist* v, f_plist_pred f);
s_plist* plist_map(s_plist* v, f_plist_map f);
t_plistv plist_reduce(s_plist* v, t_plistv memo, f_plist_reduce f);
t_plistv plist_reduceright(s_plist* v, t_plistv memo, f_plist_reduce f);


#endif //!PLIST_H_
