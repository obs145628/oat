#include "plist.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

const size_t PLIST_NFOUND = -1;

static s_plist_node* new_node(t_plistv value)
{
        s_plist_node* node = malloc(sizeof (s_plist_node));
        node->value = value;
        return node;
}

static void insert_before(s_plist_node* node, s_plist_node* insert)
{
        insert->prev = node->prev;
        insert->next = node;
        node->prev->next = insert;
        node->prev = insert;
}

static void remove_node(s_plist* s, s_plist_node* node)
{
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if(node == node->next)
                s->_front = 0;
        else if(s->_front == node)
                s->_front = node->next;
}

static s_plist_node* insert_first(s_plist* s, t_plistv value)
{
        s_plist_node* front = new_node(value);
        front->prev = front;
        front->next = front;
        s->_front = front;
        s->_size = 1;
        return front;
}

s_plist* plist_new()
{
        s_plist* s = malloc(sizeof (s_plist));
        s->_front = 0;
        s->_size = 0;
        return s;
}

s_plist* plist_new_from_it(t_plistv* begin, t_plistv* end)
{
        s_plist* s = plist_new();
        plist_push_it_back(s, begin, end);
        return s;
}

void plist_free(s_plist* s)
{
        plist_clear(s);
        free(s);
}

s_plist* plist_clone(s_plist* s)
{
        s_plist* clone = plist_new();
        s_plist_node* node = s->_front;
        for (size_t i = 0; i < s->_size; ++i)
        {
                plist_push_back(clone, node->value);
                node = node->next;
        }

        return clone;
}

void plist_print(s_plist* s)
{
        s_plist_node* front = s->_front;
        s_plist_node* node = s->_front;
        if (!front)
                return;

        do
        {
                printf("%p\n", node->value);
                node = node->next;
        }
        while (node != front);
}

size_t plist_size(s_plist* s)
{
        return s->_size;
}

int plist_empty(s_plist* s)
{
        return !s->_size;
}

void plist_clear(s_plist* s)
{
        s_plist_node* node = s->_front;

        for (size_t i = 0; i < s->_size; ++i)
        {
                s_plist_node* next = node->next;
                free(node);
                node = next;
        }

        s->_front = 0;
        s->_size = 0;
}

s_plist_node* plist_front(s_plist* s)
{
        assert(s->_front);
        return s->_front;
}

s_plist_node* plist_back(s_plist* s)
{
        assert(s->_front);
        return s->_front->prev;
}

void plist_move_front(s_plist* s, s_plist_node* node)
{
        s->_front = node;
}

s_plist_node* plist_insert_before(s_plist* s, s_plist_node* node,
                                   t_plistv value)
{
        s_plist_node* ins = new_node(value);
        insert_before(node, ins);

        if (s->_front == node)
                s->_front = ins;

        ++s->_size;
        return ins;
}

s_plist_node* plist_insert_after(s_plist* s, s_plist_node* node,
                                   t_plistv value)
{
        s_plist_node* ins = new_node(value);
        insert_before(node->next, ins);
        ++s->_size;
        return ins;
}

s_plist_node* plist_insert_it_before(s_plist* s, s_plist_node* node,
                                     t_plistv* begin, t_plistv* end)
{
        s_plist_node *ins = 0;
        for (t_plistv* it = begin; it != end; ++it)
                ins = plist_insert_before(s, node, *it);
        return ins;
}

s_plist_node* plist_insert_it_after(s_plist* s, s_plist_node* node,
                                     t_plistv* begin, t_plistv* end)
{
        if(begin == end)
                return 0;

        s_plist_node *ins = node;
        for (t_plistv* it = begin; it != end; ++it)
                ins = plist_insert_after(s, ins, *it);
        return ins;
}

t_plistv plist_pop(s_plist* s, s_plist_node* node)
{
        assert(s->_front);

        t_plistv value = node->value;
        remove_node(s, node);
        free(node);
        --s->_size;
        return value;
}

void plist_remove(s_plist* s, s_plist_node* node, size_t n)
{
        assert(s->_size >= n);

        for(size_t i = 0; i < n; ++i)
        {
                remove_node(s, node);
                s_plist_node* next = node->next;
                free(node);
                --s->_size;
                node = next;
        }

}

void plist_copy(s_plist* s, s_plist_node* node, t_plistv* begin, t_plistv* end)
{
        size_t it_size = end - begin;
        assert(s->_size >= it_size);
        for (size_t i = 0; i < it_size; ++i)
        {
                node->value = begin[i];
                node = node->next;
        }
}


s_plist_node* plist_push_front(s_plist* s, t_plistv value)
{
        if (s->_front)
                return plist_insert_before(s, s->_front, value);
        else
                return insert_first(s, value);
}

s_plist_node* plist_push_back(s_plist* s, t_plistv value)
{
        if(s->_front)
                return plist_insert_after(s, s->_front->prev, value);
        else
                return insert_first(s, value);
}

s_plist_node* plist_push_it_front(s_plist* s, t_plistv* begin, t_plistv* end)
{
        if (begin == end)
                return 0;

        plist_push_front(s, *begin);
        return plist_insert_it_after(s, s->_front, begin + 1, end);
}

s_plist_node* plist_push_it_back(s_plist* s, t_plistv* begin, t_plistv* end)
{
        if(begin == end)
                return 0;

        plist_push_back(s, *begin);
        return plist_insert_it_after(s, s->_front->prev, begin + 1, end);
}

t_plistv plist_get_front(s_plist* s)
{
        assert(s->_front);
        return s->_front->value;
}

t_plistv plist_get_back(s_plist* s)
{
        assert(s->_front);
        return s->_front->prev->value;
}

void plist_set_front(s_plist* s, t_plistv value)
{
        assert(s->_front);
        s->_front->value = value;
}

void plist_set_back(s_plist* s, t_plistv value)
{
        assert(s->_front);
        s->_front->prev->value = value;
}

t_plistv plist_pop_front(s_plist* s)
{
        assert(s->_front);
        return plist_pop(s, s->_front);
}

t_plistv plist_pop_back(s_plist* s)
{
        assert(s->_front);
        return plist_pop(s, s->_front->prev);
}

void plist_remove_front(s_plist* s, size_t n)
{
        assert(s->_size >= n);
        plist_remove(s, s->_front, n);
}

void plist_remove_back(s_plist* s, size_t n)
{
        assert(s->_size >= n);
        for (size_t i = 0; i < n; ++i)
                plist_pop(s, s->_front->prev);
}


s_plist_node* plist_at(s_plist* s, size_t pos)
{
        assert(pos < s->_size);
        s_plist_node* node = s->_front;

        for(size_t i = 0; i < pos; ++i)
                node = node->next;

        return node;
}

t_plistv plist_get_at(s_plist* s, size_t pos)
{
        assert(pos < s->_size);
        return plist_at(s, pos)->value;
}

void plist_set_at(s_plist* s, size_t pos, t_plistv value)
{
        assert(pos < s->_size);
        plist_at(s, pos)->value = value;
}

void plist_copy_at(s_plist* s, size_t pos, t_plistv* begin, t_plistv* end)
{
        assert(pos < s->_size);
        assert(pos + (end - begin) <= s->_size);
        plist_copy(s, plist_at(s, pos), begin, end);
}

void plist_insert_at(s_plist* s, size_t pos, t_plistv value)
{
        assert(pos <= s->_size);

        if(!s->_front)
                plist_push_front(s, value);
        else if(pos == s->_size)
                plist_push_back(s, value);
        else
                plist_insert_before(s, plist_at(s, pos), value);
}

void plist_insert_it_at(s_plist* s, size_t pos, t_plistv* begin, t_plistv* end)
{
        assert(pos <= s->_size);

        if(!s->_front)
                plist_push_it_front(s, begin, end);
        else if(pos == s->_size)
                plist_push_it_back(s, begin, end);
        else
                plist_insert_it_before(s, plist_at(s, pos), begin, end);
}

t_plistv plist_pop_at(s_plist* s, size_t pos)
{
        assert(pos < s->_size);
        return plist_pop(s, plist_at(s, pos));
}

void plist_remove_at(s_plist* s, size_t pos, size_t n)
{
        if(n == 0)
                return;

        assert(pos < s->_size);
        assert(pos + n <= s->_size);

        plist_remove(s, plist_at(s, pos), n);
}

void plist_resize(s_plist* s, size_t size)
{
        if(size < s->_size)
        {
                plist_remove_back(s, s->_size - size);
                return;
        }

        while(s->_size != size)
        {
                insert_before(s->_front, malloc(sizeof (s_plist_node)));
                ++s->_size;
        }
}

size_t plist_indexof(s_plist* v, t_plistv value)
{
        s_plist_node* node = v->_front;

        for (size_t i = 0; i < v->_size; ++i)
        {
                if (node->value == value)
                        return i;
                node = node->next;
        }

        return PLIST_NFOUND;
}

size_t plist_lastindexof(s_plist* v, t_plistv value)
{
        if (!v->_front)
                return PLIST_NFOUND;

        s_plist_node* node = v->_front->prev;

        for (size_t i = v->_size - 1; i < v->_size; --i)
        {
                if (node->value == value)
                        return i;
                node = node->prev;
        }

        return PLIST_NFOUND;
}

s_plist_node* plist_nodeof(s_plist* s, t_plistv value)
{
        s_plist_node* node = s->_front;

        for (size_t i = 0; i < s->_size; ++i)
        {
                if (node->value == value)
                        return node;
                node = node->next;
        }

        return 0;
}

s_plist_node* plist_lastnodeof(s_plist* s, t_plistv value)
{
        if (!s->_front)
                return 0;

        s_plist_node* node = s->_front->prev;

        for (size_t i = s->_size - 1; i < s->_size; --i)
        {
                if (node->value == value)
                        return node;
                node = node->prev;
        }

        return 0;
}

int plist_contains(s_plist* v, t_plistv value)
{
        s_plist_node* node = v->_front;

        for (size_t i = 0; i < v->_size; ++i)
        {
                if (node->value == value)
                        return 1;
                node = node->next;
        }

        return 0;
}

s_plist* plist_slice(s_plist* v, size_t begin, size_t end)
{
        size_t l_size = end - begin;
        assert(begin < v->_size);
        assert(end <= v->_size);
        s_plist* l = plist_new();

        s_plist_node* node = plist_at(v, begin);
        for (size_t i = 0; i < l_size; ++i)
        {
                plist_push_back(l, node->value);
                node = node->next;
        }

        return l;
}

s_plist* plist_concat(s_plist* v1, s_plist* v2)
{
        s_plist* l = plist_clone(v1);

        s_plist_node* node = v2->_front;
        for (size_t i = 0; i < v2->_size; ++i)
        {
                plist_push_back(l, node->value);
                node = node->next;
        }

        return l;
}

void plist_foreach(s_plist* v, f_plist_foreach f)
{
        s_plist_node* node = v->_front;
        for (size_t i = 0; i < v->_size; ++i)
        {
                f(node->value);
                node = node->next;
        }
}

int plist_every(s_plist* v, f_plist_pred f)
{
        s_plist_node* node = v->_front;
        for (size_t i = 0; i < v->_size; ++i)
        {
                if (!f(node->value))
                        return 0;
                node = node->next;
        }

        return 1;
}

int plist_some(s_plist* v, f_plist_pred f)
{
        s_plist_node* node = v->_front;
        for (size_t i = 0; i < v->_size; ++i)
        {
                if (f(node->value))
                        return 1;
                node = node->next;
        }

        return 0;
}

s_plist* plist_filter(s_plist* v, f_plist_pred f)
{
        s_plist* l = plist_new();

        s_plist_node* node = v->_front;
        for (size_t i = 0; i < v->_size; ++i)
        {
                if (f(node->value))
                        plist_push_back(l, node->value);
                node = node->next;
        }

        return l;
}

s_plist_node* plist_find(s_plist* v, f_plist_pred f)
{
        s_plist_node* node = v->_front;
        for (size_t i = 0; i < v->_size; ++i)
        {
                if (f(node->value))
                        return node;
                node = node->next;
        }

        return 0;
}

size_t plist_findindex(s_plist* v, f_plist_pred f)
{
        s_plist_node* node = v->_front;
        for (size_t i = 0; i < v->_size; ++i)
        {
                if (f(node->value))
                        return i;
                node = node->next;
        }

        return PLIST_NFOUND;
}

s_plist* plist_map(s_plist* v, f_plist_map f)
{
        s_plist* l = plist_new();

        s_plist_node* node = v->_front;
        for (size_t i = 0; i < v->_size; ++i)
        {
                plist_push_back(l, f(node->value));
                node = node->next;
        }

        return l;
}

t_plistv plist_reduce(s_plist* v, t_plistv memo, f_plist_reduce f)
{
        s_plist_node* node = v->_front;
        for (size_t i = 0; i < v->_size; ++i)
        {
                memo = f(memo, node->value);
                node = node->next;
        }

        return memo;
}

t_plistv plist_reduceright(s_plist* v, t_plistv memo, f_plist_reduce f)
{
        if (!v->_front)
                return memo;

        s_plist_node* node = v->_front->prev;
        for (size_t i = 0; i < v->_size; ++i)
        {
                memo = f(memo, node->value);
                node = node->prev;
        }

        return memo;
}
