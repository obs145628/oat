#include "pstack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static struct pstacknode* new_node(t_pstackv value, struct pstacknode* next)
{
        struct pstacknode* node = malloc(sizeof (struct pstacknode));
        node->value = value;
        node->next = next;
        return node;
}

s_pstack* pstack_new()
{
        s_pstack* s = malloc(sizeof (s_pstack));
        s->_size = 0;
        s->_head = 0;
        return s;
}

s_pstack* pstack_new_from_it(t_pstackv* begin, t_pstackv* end)
{
        s_pstack* s = pstack_new();
        pstack_push_it(s, begin, end);
        return s;
}

void pstack_free(s_pstack* s)
{
        struct pstacknode* node = s->_head;
        while (node)
        {
                struct pstacknode* next = node->next;
                free(node);
                node = next;
        }

        free(s);
}

s_pstack* pstack_clone(s_pstack* s)
{
        s_pstack* clone = pstack_new();
        clone->_size = s->_size;

        struct pstacknode** cnode = &(clone->_head);

        struct pstacknode* snode = s->_head;
        while (snode)
        {
                *cnode = new_node(snode->value, 0);
                cnode = &((*cnode)->next);
                snode = snode->next;
        }

        return clone;
}

void pstack_print(s_pstack* s)
{
        s = pstack_clone(s);

        while(s->_head)
                printf("%p\n", pstack_pop(s));

        pstack_free(s);
}

size_t pstack_size(s_pstack* s)
{
        return s->_size;
}

int pstack_empty(s_pstack* s)
{
        return !s->_head;
}

t_pstackv pstack_get(s_pstack* s)
{
        assert(s->_head);
        return s->_head->value;
}

t_pstackv pstack_pop(s_pstack* s)
{
        assert(s->_head);
        struct pstacknode* head = s->_head;
        t_pstackv value = head->value;

        s->_head = head->next;
        free(head);
        --s->_size;
        return value;
}

void pstack_push(s_pstack* s, t_pstackv value)
{
        s->_head = new_node(value, s->_head);
        ++s->_size;
}


void pstack_push_it(s_pstack* s, t_pstackv* begin, t_pstackv* end)
{
        for (t_pstackv* it = begin; it != end; ++it)
                pstack_push(s, *it);
}
