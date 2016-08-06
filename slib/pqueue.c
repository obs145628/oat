#include "pqueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static struct pqueuenode* new_node(t_pqueuev value, struct pqueuenode* next)
{
        struct pqueuenode* node = malloc(sizeof (struct pqueuenode));
        node->value = value;
        node->next = next;
        return node;
}

s_pqueue* pqueue_new()
{
        s_pqueue* s = malloc(sizeof (s_pqueue));
        s->_size = 0;
        s->_head = 0;
        return s;
}

s_pqueue* pqueue_new_from_it(t_pqueuev* begin, t_pqueuev* end)
{
        s_pqueue* s = pqueue_new();
        pqueue_push_it(s, begin, end);
        return s;
}

void pqueue_free(s_pqueue* s)
{
        struct pqueuenode* head = s->_head;
        struct pqueuenode* node = s->_head;

        if (head)
        {
                do
                {
                        struct pqueuenode* next = node->next;
                        free(node);
                        node = next;
                }
                while (node != head);
        }

        free(s);
}

s_pqueue* pqueue_clone(s_pqueue* s)
{
        s_pqueue* clone = pqueue_new();
        clone->_size = s->_size;
        struct pqueuenode** cnode = &(clone->_head);
        struct pqueuenode* snode = s->_head;
        if (!snode)
                return clone;

        do
        {
                *cnode = new_node(snode->value, clone->_head);
                cnode = &((*cnode)->next);
                snode = snode->next;
        }
        while (snode != s->_head);

        return clone;
}

void pqueue_print(s_pqueue* s)
{
        s = pqueue_clone(s);

        while(s->_head)
                printf("%p\n", pqueue_pop(s));

        pqueue_free(s);
}

size_t pqueue_size(s_pqueue* s)
{
        return s->_size;
}

int pqueue_empty(s_pqueue* s)
{
        return !s->_head;
}

t_pqueuev pqueue_get(s_pqueue* s)
{
        assert(s->_head);
        return s->_head->next->value;
}

t_pqueuev pqueue_pop(s_pqueue* s)
{
        assert(s->_head);
        struct pqueuenode* head = s->_head;
        struct pqueuenode* top = head->next;
        t_pqueuev value = top->value;

        if (head == top)
                s->_head = 0;
        else
                head->next = top->next;

        free(top);
        --s->_size;
        return value;
}

void pqueue_push(s_pqueue* s, t_pqueuev value)
{
        struct pqueuenode* head = s->_head;
        struct pqueuenode* node = new_node(value, 0);
        s->_head = node;

        if (!head)
        {
                node->next = node;
        }
        else
        {
                node->next = head->next;
                head->next = node;
        }

        ++s->_size;
}


void pqueue_push_it(s_pqueue* s, t_pqueuev* begin, t_pqueuev* end)
{
        for (t_pqueuev* it = begin; it != end; ++it)
                pqueue_push(s, *it);
}
