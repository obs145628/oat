#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "parray.h"

const size_t PARRAY_NFOUND = -1;


static size_t get_alloc_size(size_t arr_size)
{
        size_t size = PARRAY_DEFAULT_ALLOC;
        while (size < arr_size)
                size *= 2;
        return size;
}

static void add_items(s_parray* v, size_t n)
{
        size_t new_size = v->_size + n;
        size_t alloc_size = v->_alloc_size;

        while (new_size > alloc_size)
                alloc_size *= 2;

        if(v->_alloc_size != alloc_size)
        {
                v->_items = realloc(v->_items, alloc_size * sizeof (t_parrayv));
                v->_alloc_size = alloc_size;
        }

        v->_size = new_size;
}

static void remove_items(s_parray* v, size_t n)
{
        assert(n <= v->_size);
        size_t new_size = v->_size - n;
        size_t alloc_size = v->_alloc_size;

        while(alloc_size > PARRAY_DEFAULT_ALLOC && alloc_size > 4 * new_size)
                alloc_size /= 2;

        if(v->_alloc_size != alloc_size)
        {
                v->_items = realloc(v->_items, alloc_size * sizeof (t_parrayv));
                v->_alloc_size = alloc_size;
        }

        v->_size = new_size;
}

s_parray* parray_new()
{
        return parray_new_with_reserve(PARRAY_DEFAULT_ALLOC);
}

s_parray* parray_new_with_reserve(size_t n)
{
        s_parray* v = malloc(sizeof (s_parray));
        v->_size = 0;
        v->_alloc_size = n;
        v->_items = malloc(sizeof (t_parrayv) * n);
        return v;
}

s_parray* parray_new_with_size(size_t n)
{
        s_parray* v = parray_new_with_reserve(get_alloc_size(n));
        v->_size = n;
        return v;
}

s_parray* parray_new_fill(size_t n, t_parrayv val)
{
        s_parray* v = parray_new_with_size(n);

        for (size_t i = 0; i < n; ++i)
                v->_items[i] = val;

        return v;
}

s_parray* parray_new_from_it(t_parrayv* begin, t_parrayv* end)
{
        size_t n = end - begin;
        s_parray* v = parray_new_with_size(n);

        for (size_t i = 0; i < n; ++i)
                v->_items[i] = begin[i];

        return v;
}



void parray_free(s_parray *v)
{
        free(v->_items);
        free(v);
}


void parray_print(s_parray* v)
{
        for (size_t i = 0; i < v->_size; ++i)
                printf("%zu => %p\n", i, v->_items[i]);
}
s_parray* parray_clone(s_parray* v)
{
        return parray_new_from_it(v->_items,
                                  v->_items + v->_size);
}



size_t parray_size(s_parray* v)
{
        return v->_size;
}

int parray_empty(s_parray* v)
{
        return !v->_size;
}

t_parrayv parray_get(s_parray* v, size_t pos)
{
        assert(pos < v->_size);
        return v->_items[pos];
}

t_parrayv parray_get_front(s_parray* v)
{
        assert(v->_size > 0);
        return v->_items[0];
}

t_parrayv parray_get_back(s_parray* v)
{
        assert(v->_size > 0);
        return v->_items[v->_size - 1];
}

t_parrayv* parray_at(s_parray* v, size_t pos)
{
        assert(pos < v->_size);
        return v->_items + pos;
}

t_parrayv* parray_begin(s_parray* v)
{
        return v->_items;
}

t_parrayv* parray_end(s_parray* v)
{
        return v->_items + v->_size;
}

void parray_set(s_parray* v, size_t pos, t_parrayv value)
{
        assert(pos < v->_size);
        v->_items[pos] = value;
}

void parray_set_front(s_parray* v, t_parrayv value)
{
        assert(v->_size > 0);
        v->_items[0] = value;
}

void parray_set_back(s_parray* v, t_parrayv value)
{
        assert(v->_size > 0);
        v->_items[v->_size - 1] = value;
}

void parray_copy(s_parray* v, size_t pos, t_parrayv* begin, t_parrayv* end)
{
        size_t it_size = end - begin;
        assert(pos < v->_size);
        assert(pos + it_size < v->_size);

        memcpy(v->_items + pos, begin, it_size * sizeof (t_parrayv));
}

void parray_insert(s_parray* v, size_t pos, t_parrayv value)
{
        assert(pos <= v->_size);

        add_items(v, 1);
        memmove(v->_items + pos + 1, v->_items + pos,
                (v->_size - pos) * sizeof (t_parrayv));

        v->_items[pos] = value;
}

void parray_insert_it(s_parray* v, size_t pos,
                      t_parrayv* begin, t_parrayv *end)
{
        assert (pos <= v->_size);

        size_t it_size = end - begin;
        add_items(v, it_size);
        memmove(v->_items + pos + it_size, v->_items + pos,
                (v->_size - pos) * sizeof (t_parrayv));
        memcpy(v->_items  + pos, begin, it_size * sizeof (t_parrayv));
}

void parray_insert_front(s_parray* v, t_parrayv value)
{
        parray_insert(v, 0, value);
}

void parray_insert_it_front(s_parray*v, t_parrayv* begin, t_parrayv* end)
{
        parray_insert_it(v, 0, begin, end);
}

void parray_insert_back(s_parray* v, t_parrayv value)
{
        parray_insert(v, v->_size, value);
}

void parray_insert_it_back(s_parray*v, t_parrayv* begin, t_parrayv* end)
{
        parray_insert_it(v, v->_size, begin, end);
}

void parray_remove(s_parray* v, size_t pos, size_t n)
{
        if (!n)
                return;

        assert(pos < v->_size);
        assert(pos + n <= v->_size);

        remove_items(v, n);
        memmove(v->_items + pos, v->_items + pos + n,
                (v->_size - pos) * sizeof (t_parrayv));
}

void parray_remove_front(s_parray* v, size_t n)
{
        assert(n <= v->_size);
        parray_remove(v, 0, n);
}

void parray_remove_back(s_parray* v, size_t n)
{
        assert(n <= v->_size);
        parray_remove(v, v->_size - n, n);
}

void parray_resize(s_parray* v, size_t size)
{
        if (size >= v->_size)
                add_items(v, size - v->_size);
        else
                remove_items(v, v->_size - size);
}

void parray_clear(s_parray* v)
{
        remove_items(v, v->_size);
}

void parray_reserve(s_parray* v, size_t size)
{
        size_t alloc_size = v->_alloc_size;
        while (size > alloc_size)
                alloc_size *= 2;

        if(v->_alloc_size != alloc_size)
        {
                v->_items = realloc(v->_items, alloc_size * sizeof (t_parrayv));
                v->_alloc_size = alloc_size;
        }
}

size_t parray_indexof(s_parray* v, t_parrayv value)
{
        for (size_t i = 0; i < v->_size; ++i)
                if (v->_items[i] == value)
                        return i;

        return PARRAY_NFOUND;
}

size_t parray_lastindexof(s_parray* v, t_parrayv value)
{
        for (size_t i = v->_size - 1; i < v->_size; --i)
                if (v->_items[i] == value)
                        return i;

        return PARRAY_NFOUND;
}

int parray_contains(s_parray* v, t_parrayv value)
{
        for( size_t i = 0; i < v->_size; ++i)
                if (v->_items[i] == value)
                        return 1;

        return 0;
}

s_parray* parray_slice(s_parray* v, size_t begin, size_t end)
{
        return parray_new_from_it(v->_items + begin, v->_items + end);
}

s_parray* parray_concat(s_parray* v1, s_parray* v2)
{
        size_t size1 = v1->_size;
        size_t size2 = v2->_size;

        s_parray* v = parray_new_with_size(size1 + size2);
        memcpy(v->_items, v1->_items, size1 * sizeof (t_parrayv));
        memcpy(v->_items + size1, v2->_items, size2 * sizeof (t_parrayv));
        return v;
}

void parray_foreach(s_parray* v, f_parray_foreach f)
{
        for (size_t i = 0; i < v->_size; ++i)
                f(v->_items[i]);
}

int parray_every(s_parray* v, f_parray_pred f)
{
        for (size_t i = 0; i < v->_size; ++i)
                if (!f(v->_items[i]))
                        return 0;

        return 1;
}

int parray_some(s_parray* v, f_parray_pred f)
{
        for (size_t i = 0; i < v->_size; ++i)
                if (f(v->_items[i]))
                        return 1;

        return 0;
}

s_parray* parray_filter(s_parray* v, f_parray_pred f)
{
        s_parray* res = parray_new();
        for (size_t i = 0; i < v->_size; ++i)
                if (f(v->_items[i]))
                        parray_insert_back(res, v->_items[i]);

        return res;
}

t_parrayv* parray_find(s_parray* v, f_parray_pred f)
{
        for (size_t i = 0; i < v->_size; ++i)
                if (f(v->_items[i]))
                        return v->_items + i;

        return 0;
}

size_t parray_findindex(s_parray* v, f_parray_pred f)
{
        for (size_t i = 0; i < v->_size; ++i)
                if (f(v->_items[i]))
                        return i;

        return PARRAY_NFOUND;
}

s_parray* parray_map(s_parray* v, f_parray_map f)
{
        s_parray* res = parray_new_with_size(v->_size);
        for (size_t i = 0; i < v->_size; ++i)
                res->_items[i] = f(v->_items[i]);

        return res;
}

t_parrayv parray_reduce(s_parray* v, t_parrayv memo, f_parray_reduce f)
{
        for (size_t i = 0; i < v->_size; ++i)
                memo = f(memo, v->_items[i]);

        return memo;
}

t_parrayv parray_reduceright(s_parray* v, t_parrayv memo, f_parray_reduce f)
{
        for (size_t i = v->_size - 1; i < v->_size; --i)
                memo = f(memo, v->_items[i]);

        return memo;
}
