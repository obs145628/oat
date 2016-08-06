#ifndef PQUEUE_H_
# define PQUEUE_H_

# include <stddef.h>

typedef void* t_pqueuev;

struct pqueuenode
{
        t_pqueuev value;
        struct pqueuenode* next;

};

typedef struct
{
        struct pqueuenode* _head;
        size_t _size;
} s_pqueue;

s_pqueue* pqueue_new();
s_pqueue* pqueue_new_from_it(t_pqueuev* begin, t_pqueuev* end);

void pqueue_free(s_pqueue* s);
s_pqueue* pqueue_clone(s_pqueue* s);
void pqueue_print(s_pqueue* s);

size_t pqueue_size(s_pqueue* s);
int pqueue_empty(s_pqueue* s);
t_pqueuev pqueue_get(s_pqueue* s);
t_pqueuev pqueue_pop(s_pqueue* s);
void pqueue_push(s_pqueue* s, t_pqueuev value);
void pqueue_push_it(s_pqueue* s, t_pqueuev* begin, t_pqueuev* end);



#endif //!PQUEUE_H_
