#ifndef PSTACK_H_
# define PSTACK_H_

# include <stddef.h>

typedef void* t_pstackv;

struct pstacknode
{
        t_pstackv value;
        struct pstacknode* next;

};

typedef struct
{
        struct pstacknode* _head;
        size_t _size;
} s_pstack;

s_pstack* pstack_new();
s_pstack* pstack_new_from_it(t_pstackv* begin, t_pstackv* end);

void pstack_free(s_pstack* s);
s_pstack* pstack_clone(s_pstack* s);
void pstack_print(s_pstack* s);

size_t pstack_size(s_pstack* s);
int pstack_empty(s_pstack* s);
t_pstackv pstack_get(s_pstack* s);
t_pstackv pstack_pop(s_pstack* s);
void pstack_push(s_pstack* s, t_pstackv value);
void pstack_push_it(s_pstack* s, t_pstackv* begin, t_pstackv* end);



#endif //!PSTACK_H_
