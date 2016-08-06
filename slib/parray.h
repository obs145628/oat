#ifndef PARRAY_H_
# define PARRAY_H_

# include <stddef.h>

# define PARRAY_DEFAULT_ALLOC 16

typedef void* t_parrayv;
typedef void (*f_parray_foreach)(t_parrayv value);
typedef int (*f_parray_pred)(t_parrayv value);
typedef t_parrayv (*f_parray_map)(t_parrayv value);
typedef t_parrayv (*f_parray_reduce)(t_parrayv memo, t_parrayv value);

extern const size_t PARRAY_NFOUND;

typedef struct
{
        size_t _size;
        size_t _alloc_size;
        t_parrayv* _items;
} s_parray;

s_parray* parray_new();
s_parray* parray_new_with_reserve(size_t n);
s_parray* parray_new_with_size(size_t n);
s_parray* parray_new_fill(size_t n, t_parrayv val);
s_parray* parray_new_from_it(t_parrayv* begin, t_parrayv* end);

void parray_free(s_parray* v);
void parray_print(s_parray* v);
s_parray* parray_clone(s_parray* v);

size_t parray_size(s_parray* v);
int parray_empty(s_parray* v);

t_parrayv parray_get(s_parray* v, size_t pos);
t_parrayv parray_get_front(s_parray* v);
t_parrayv parray_get_back(s_parray* v);
t_parrayv* parray_at(s_parray* v, size_t pos);
t_parrayv* parray_begin(s_parray* v);
t_parrayv* parray_end(s_parray* v);
void parray_set(s_parray* v, size_t pos, t_parrayv value);
void parray_set_front(s_parray* v, t_parrayv value);
void parray_set_back(s_parray* v, t_parrayv value);
void parray_copy(s_parray* v, size_t pos, t_parrayv* begin, t_parrayv* end);

void parray_insert(s_parray* v, size_t pos, t_parrayv value);
void parray_insert_it(s_parray* v, size_t pos,
                      t_parrayv* begin, t_parrayv *end);
void parray_insert_front(s_parray* v, t_parrayv value);
void parray_insert_it_front(s_parray*v, t_parrayv* begin, t_parrayv* end);
void parray_insert_back(s_parray* v, t_parrayv value);
void parray_insert_it_back(s_parray*v, t_parrayv* begin, t_parrayv* end);
void parray_remove(s_parray* v, size_t pos, size_t n);
void parray_remove_front(s_parray* v, size_t n);
void parray_remove_back(s_parray* v, size_t n);
void parray_resize(s_parray* v, size_t size);
void parray_clear(s_parray* v);
void parray_reserve(s_parray* v, size_t size);

size_t parray_indexof(s_parray* v, t_parrayv value);
size_t parray_lastindexof(s_parray* v, t_parrayv value);
int parray_contains(s_parray* v, t_parrayv value);
s_parray* parray_slice(s_parray* v, size_t begin, size_t end);
s_parray* parray_concat(s_parray* v1, s_parray* v2);

void parray_foreach(s_parray* v, f_parray_foreach f);
int parray_every(s_parray* v, f_parray_pred f);
int parray_some(s_parray* v, f_parray_pred f);
s_parray* parray_filter(s_parray* v, f_parray_pred f);
t_parrayv* parray_find(s_parray* v, f_parray_pred f);
size_t parray_findindex(s_parray* v, f_parray_pred f);
s_parray* parray_map(s_parray* v, f_parray_map f);
t_parrayv parray_reduce(s_parray* v, t_parrayv memo, f_parray_reduce f);
t_parrayv parray_reduceright(s_parray* v, t_parrayv memo, f_parray_reduce f);



#endif //PARRAY_H_
