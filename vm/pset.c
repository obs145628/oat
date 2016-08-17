#include "pset.h"
#include <stdio.h>
#include <stdlib.h>
#include "err.h"


static void rotateLeft(s_pset *t, s_pset_node *x)
{
    s_pset_node *y = x->right;

    x->right = y->left;
    if(y->left != t->leaf)
        y->left->parent = x;

    y->parent = x->parent;
    if(x->parent == t->leaf)
        t->root = y;
    else if(x->parent->left == x)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

static void rotateRight(s_pset *t, s_pset_node *x)
{
    s_pset_node *y = x->left;

    x->left = y->right;
    if(y->right != t->leaf)
        y->right->parent = x;

    y->parent = x->parent;
    if(x->parent == t->leaf)
        t->root = y;
    else if(x->parent->left == x)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->right = x;
    x->parent = y;
}

static s_pset_node* nodeClone(s_pset_node *node,
                                 s_pset_node *oldLeaf,
                                 s_pset_node  *newLeaf)
{
    s_pset_node *clone = malloc(sizeof(s_pset_node));
    clone->isBlack = node->isBlack;
    clone->key = node->key;

    if(node->left == oldLeaf)
        clone->left = newLeaf;
    else
    {
        clone->left = nodeClone(node->left, oldLeaf, newLeaf);
        clone->left->parent = clone;
    }

    if(node->right == oldLeaf)
        clone->right = newLeaf;
    else
    {
        clone->right = nodeClone(node->right, oldLeaf, newLeaf);
        clone->right->parent = clone;
    }

    return clone;
}

static void nodeFree(s_pset_node *node, s_pset_node *leaf)
{
    if(node->left != leaf)
        nodeFree(node->left, leaf);
    if(node->right != leaf)
        nodeFree(node->right, leaf);
    free(node);
}

static s_pset_node* nodeGetMin(s_pset_node *node, s_pset_node *leaf)
{
    if(node == leaf)
        return node;
    while(node->left != leaf)
        node = node->left;
    return node;
}

static s_pset_node* nodeGetMax(s_pset_node *node, s_pset_node *leaf)
{
    if(node == leaf)
        return node;
    while(node->right != leaf)
        node = node->right;
    return node;
}

static void insertFixup(s_pset *t, s_pset_node *z)
{
    while(!z->parent->isBlack)
    {
        if(z->parent == z->parent->parent->left)
        {
            s_pset_node *y = z->parent->parent->right;
            if(!y->isBlack)
            {
                z->parent->isBlack = 1;
                y->isBlack = 1;
                z->parent->parent->isBlack = 0;
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->right)
                {
                    z = z->parent;
                    rotateLeft(t, z);
                }
                z->parent->isBlack = 1;
                z->parent->parent->isBlack = 0;
                rotateRight(t, z->parent->parent);
            }
        }

        else
        {
            s_pset_node *y = z->parent->parent->left;
            if(!y->isBlack)
            {
                z->parent->isBlack = 1;
                y->isBlack = 1;
                z->parent->parent->isBlack = 0;
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->left)
                {
                    z = z->parent;
                    rotateRight(t, z);
                }
                z->parent->isBlack = 1;
                z->parent->parent->isBlack = 0;
                rotateLeft(t, z->parent->parent);
            }
        }
    }

    t->root->isBlack = 1;
}

static void nodeReplace(s_pset *t, s_pset_node *n1, s_pset_node *n2)
{
    if(n1->parent == t->leaf)
        t->root = n2;

    else if(n1 == n1->parent->left)
        n1->parent->left = n2;
    else
        n1->parent->right = n2;

    n2->parent = n1->parent;
}

static void removeFixup(s_pset *t, s_pset_node *x)
{
    while(x != t->root && x->isBlack)
    {
        if(x == x->parent->left)
        {
            s_pset_node *w = x->parent->right;
            if(!w->isBlack)
            {
                w->isBlack = 1;
                x->parent->isBlack = 0;
                rotateLeft(t, x->parent);
                w = x->parent->right;
            }
            if(w->left->isBlack && w->right->isBlack)
            {
                w->isBlack = 0;
                x = x->parent;
            }
            else
            {
                if(w->right->isBlack)
                {
                    w->left->isBlack = 1;
                    w->isBlack = 0;
                    rotateRight(t, w);
                    w = x->parent->right;
                }
                w->isBlack = x->parent->isBlack;
                x->parent->isBlack = 1;
                w->right->isBlack = 1;
                rotateLeft(t, x->parent);
                x = t->root;
            }
        }

        else
        {
            s_pset_node *w = x->parent->left;
            if(!w->isBlack)
            {
                w->isBlack = 1;
                x->parent->isBlack = 0;
                rotateRight(t, x->parent);
                w = x->parent->left;
            }
            if(w->left->isBlack && w->right->isBlack)
            {
                w->isBlack = 0;
                x = x->parent;
            }
            else
            {
                if(w->left->isBlack)
                {
                    w->right->isBlack = 1;
                    w->isBlack = 0;
                    rotateLeft(t, w);
                    w = x->parent->left;
                }
                w->isBlack = x->parent->isBlack;
                x->parent->isBlack = 1;
                w->left->isBlack = 1;
                rotateRight(t, x->parent);
                x = t->root;
            }
        }
    }
    x->isBlack = 1;
}

static void nodeRemove(s_pset *t, s_pset_node *z)
{
    s_pset_node *x;
    s_pset_node *y = z;
    int yIsBlack = y->isBlack;

    if(z->left == t->leaf)
    {
        x = z->right;
        nodeReplace(t, z, z->right);
    }
    else if(z->right == t->leaf)
    {
        x = z->left;
        nodeReplace(t, z, z->left);
    }

    else
    {
        y = nodeGetMin(z->right, t->leaf);
        yIsBlack = y->isBlack;
        x = y->right;

        if(y->parent == z)
        {
            x->parent = y;
        }
        else
        {
            nodeReplace(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        nodeReplace(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->isBlack = z->isBlack;
    }

    if(yIsBlack)
        removeFixup(t, x);

    free(z);
}




s_pset* pset_new(f_pset_cmp cmp)
{
   s_pset* s = malloc(sizeof(s_pset));
   s->cmp = cmp;
   s->leaf = malloc(sizeof(s_pset_node));
   s->leaf->isBlack = 1;
   s->leaf->left = 0;
   s->leaf->right = 0;
   s->root = s->leaf;
   return s;
}

s_pset* pset_new_from_it(f_pset_cmp cmp, t_psetv* begin, t_psetv* end)
{
   s_pset* s = pset_new(cmp);
   pset_insert_it(s, begin, end);
   return s;
}

void pset_free(s_pset* s)
{
   if(s->root != s->leaf)
      nodeFree(s->root, s->leaf);
   free(s->leaf);
   free(s);
}

s_pset* pset_clone(s_pset* s)
{
   s_pset* clone = pset_new(s->cmp);
   clone->size = s->size;
   if(s->root != s->leaf)
   {
      clone->root = nodeClone(s->root, s->leaf, clone->leaf);
      clone->root->parent = clone->leaf;
   }
   return clone;
}

s_pset* pset_union(s_pset* s1, s_pset* s2)
{
    s_pset* s = pset_clone(s1);
    for(s_pset_node* it = pset_begin(s2);
        it != pset_end(s2); it = pset_next(s2, it))
    {
        pset_insert(s, it->key);
    }
    return s;
}

s_pset* pset_intersection(s_pset* s1, s_pset* s2)
{

    s_pset* s = pset_new(s1->cmp);

    for(s_pset_node* it = pset_begin(s1);
        it != pset_end(s1); it = pset_next(s1, it))
    {
        if(pset_contains(s2, it->key))
            pset_insert(s, it->key);
    }

    return s;
}

size_t pset_size(s_pset* s)
{
   return s->size;
}

int pset_empty(s_pset* s)
{
   return !s->size;
}

int pset_contains(s_pset* s, t_psetv value)
{
   return pset_find(s, value) != s->leaf;
}

int pset_insert(s_pset* s, t_psetv value)
{
    s_pset_node *parent = s->leaf;
    s_pset_node **node = &(s->root);

    while(*node != s->leaf)
    {
        s_pset_node *elem = *node;
        int val = s->cmp(value, elem->key);
        if(val < 0)
            node = &(elem->left);
        else if(val > 0)
            node = &(elem->right);
        else
            return 0;
        parent = elem;
    }

    *node = malloc(sizeof(s_pset_node));
    (*node)->key = value;
    (*node)->left = s->leaf;
    (*node)->right = s->leaf;
    (*node)->parent = parent;
    (*node)->isBlack = 0;
    insertFixup(s, *node);
    ++s->size;
    return 1;
}

void pset_insert_it(s_pset* s, t_psetv* begin, t_psetv* end)
{
   for(t_psetv* it = begin; it != end; ++it)
      pset_insert(s, *it);
}

void pset_clear(s_pset* s)
{
   if(s->root != s->leaf)
      nodeFree(s->root, s->leaf);

   s->root = s->leaf;
   s->size = 0;
}

int pset_remove(s_pset* s, t_psetv value)
{
   s_pset_node *node = s->root;

    while(node != s->leaf)
    {
        int val = s->cmp(value, node->key);
        if(val < 0)
           node = node->left;
        else if(val > 0)
           node = node->right;
        else
        {
           nodeRemove(s, node);
           --s->size;
           return 1;
        }
    }

    return 0;
}


s_pset_node *pset_find(s_pset *t, t_psetv e)
{
    s_pset_node *node = t->root;
    while(node != t->leaf)
    {
        int val = t->cmp(e, node->key);
        if(val < 0)
            node = node->left;
        else if(val > 0)
            node = node->right;
        else
            return node;
    }

    return t->leaf;
}

s_pset_node* pset_min(s_pset* t)
{
   return t->root == t->leaf ? t->leaf : nodeGetMin(t->root, t->leaf);
}

s_pset_node* pset_max(s_pset* t)
{
   return t->root == t->leaf ? t->leaf : nodeGetMax(t->root, t->leaf);
}

s_pset_node* pset_begin(s_pset* t)
{
   return t->root == t->leaf ? t->leaf : nodeGetMin(t->root, t->leaf);
}

s_pset_node* pset_end(s_pset* t)
{
   return t->leaf;
}

s_pset_node* pset_prev(s_pset* s, s_pset_node* node)
{
    if(node->left != s->leaf)
    {
        node = node->left;
        while(node->right != s->leaf)
            node = node->right;
        return node;
    }
    else
    {
        s_pset_node *parent = node->parent;
        while(parent != s->leaf && node == parent->left)
        {
            node = parent;
            parent = node->parent;
        }
        return parent;
    }
}

s_pset_node* pset_next(s_pset* s, s_pset_node* node)
{
    if(node->right != s->leaf)
    {
        node = node->right;
        while(node->left != s->leaf)
            node = node->left;
        return node;
    }
    else
    {
        s_pset_node *parent = node->parent;
        while(parent != s->leaf && node == parent->right)
        {
            node = parent;
            parent = node->parent;
        }
        return parent;
    }
}

t_psetv pset_value(s_pset *t, s_pset_node* node)
{
    if(t->leaf == node)
        err("Impossible to access value of null node");
    return node->key;
}


void pset_foreach(s_pset* s, f_pset_foreach f)
{
    for(s_pset_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pset_next(s, it))
    {
        f(it->key);
    }
}

int pset_every(s_pset* s, f_pset_pred f)
{
    for(s_pset_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pset_next(s, it))
    {
        if(!f(it->key))
            return 0;
    }
    return 1;
}

int pset_some(s_pset* s, f_pset_pred f)
{
    for(s_pset_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pset_next(s, it))
    {
        if(f(it->key))
            return 1;
    }
    return 0;
}

s_pset* pset_filter(s_pset* s, f_pset_pred f)
{
    s_pset* res = pset_new(s->cmp);

    for(s_pset_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pset_next(s, it))
    {
        if(f(it->key))
            pset_insert(res, it->key);
    }

    return res;
}

s_pset_node* pset_search(s_pset* s, f_pset_pred f)
{
    for(s_pset_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pset_next(s, it))
    {
        if(f(it->key))
            return it;
    }

    return s->leaf;
}

s_pset* pset_map(s_pset* s, f_pset_map f)
{
    s_pset* res = pset_new(s->cmp);

    for(s_pset_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pset_next(s, it))
    {
        pset_insert(res, f(it->key));
    }

    return res;
}

t_psetv pset_reduce(s_pset* s, t_psetv memo, f_pset_reduce f)
{
    for(s_pset_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pset_next(s, it))
    {
        memo = f(memo, it->key);
    }
    return memo;
}
