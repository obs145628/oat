#include "pmap.h"
#include <stdio.h>
#include <stdlib.h>
#include "err.h"

#include <assert.h>


static void rotateLeft(s_pmap *t, s_pmap_node *x)
{
    s_pmap_node *y = x->right;

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

static void rotateRight(s_pmap *t, s_pmap_node *x)
{
    s_pmap_node *y = x->left;

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

static s_pmap_node* nodeClone(s_pmap_node *node,
                                 s_pmap_node *oldLeaf,
                                 s_pmap_node  *newLeaf)
{
    s_pmap_node *clone = malloc(sizeof(s_pmap_node));
    clone->isBlack = node->isBlack;
    clone->key = node->key;
    clone->value = node->value;

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

static void nodeFree(s_pmap_node *node, s_pmap_node *leaf)
{
    if(node->left != leaf)
        nodeFree(node->left, leaf);
    if(node->right != leaf)
        nodeFree(node->right, leaf);
    free(node);
}

static s_pmap_node* nodeGetMin(s_pmap_node *node, s_pmap_node *leaf)
{
    if(node == leaf)
        return node;
    while(node->left != leaf)
        node = node->left;
    return node;
}

static s_pmap_node* nodeGetMax(s_pmap_node *node, s_pmap_node *leaf)
{
    if(node == leaf)
        return node;
    while(node->right != leaf)
        node = node->right;
    return node;
}

static void insertFixup(s_pmap *t, s_pmap_node *z)
{
    while(!z->parent->isBlack)
    {
        if(z->parent == z->parent->parent->left)
        {
            s_pmap_node *y = z->parent->parent->right;
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
            s_pmap_node *y = z->parent->parent->left;
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

static void nodeReplace(s_pmap *t, s_pmap_node *n1, s_pmap_node *n2)
{
    if(n1->parent == t->leaf)
        t->root = n2;

    else if(n1 == n1->parent->left)
        n1->parent->left = n2;
    else
        n1->parent->right = n2;

    n2->parent = n1->parent;
}

static void removeFixup(s_pmap *t, s_pmap_node *x)
{
    while(x != t->root && x->isBlack)
    {
        if(x == x->parent->left)
        {
            s_pmap_node *w = x->parent->right;
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
            s_pmap_node *w = x->parent->left;
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

static void nodeRemove(s_pmap *t, s_pmap_node *z)
{
    s_pmap_node *x;
    s_pmap_node *y = z;
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




s_pmap* pmap_new(f_pmap_cmp cmp)
{
   s_pmap* s = malloc(sizeof(s_pmap));
   s->cmp = cmp;
   s->leaf = malloc(sizeof(s_pmap_node));
   s->leaf->isBlack = 1;
   s->leaf->left = 0;
   s->leaf->right = 0;
   s->root = s->leaf;
   return s;
}

void pmap_free(s_pmap* s)
{
   if(s->root != s->leaf)
      nodeFree(s->root, s->leaf);
   free(s->leaf);
   free(s);
}

s_pmap* pmap_clone(s_pmap* s)
{
   s_pmap* clone = pmap_new(s->cmp);
   clone->size = s->size;
   if(s->root != s->leaf)
   {
      clone->root = nodeClone(s->root, s->leaf, clone->leaf);
      clone->root->parent = clone->leaf;
   }
   return clone;
}

void pmap_extend(s_pmap* dst, s_pmap* src)
{
    for(s_pmap_node* it = nodeGetMin(src->root, src->leaf);
        it != src->leaf; it = pmap_next(src, it))
    {
        pmap_set(dst, it->key, it->value);
    }
}

size_t pmap_size(s_pmap* s)
{
   return s->size;
}

int pmap_empty(s_pmap* s)
{
   return !s->size;
}

int pmap_contains(s_pmap* s, t_pmapk key)
{
   return pmap_find(s, key) != s->leaf;
}

t_pmapv pmap_get(s_pmap* s, t_pmapk key)
{
    s_pmap_node* node = pmap_find(s, key);
    if(node == s->leaf)
        err("pmap: key not found");
    return node->value;
}

int pmap_insert(s_pmap* s, t_pmapk key, t_pmapv value)
{
    s_pmap_node *parent = s->leaf;
    s_pmap_node **node = &(s->root);

    while(*node != s->leaf)
    {
        s_pmap_node *elem = *node;
        int val = s->cmp(key, elem->key);
        if(val < 0)
            node = &(elem->left);
        else if(val > 0)
            node = &(elem->right);
        else
            return 0;
        parent = elem;
    }

    *node = malloc(sizeof(s_pmap_node));
    (*node)->key = key;
    (*node)->value = value;
    (*node)->left = s->leaf;
    (*node)->right = s->leaf;
    (*node)->parent = parent;
    (*node)->isBlack = 0;
    insertFixup(s, *node);
    ++s->size;
    return 1;
}

void pmap_set(s_pmap* s, t_pmapk key, t_pmapv value)
{
    s_pmap_node *parent = s->leaf;
    s_pmap_node **node = &(s->root);

    while(*node != s->leaf)
    {
        s_pmap_node *elem = *node;
        int val = s->cmp(key, elem->key);
        if(val < 0)
            node = &(elem->left);
        else if(val > 0)
            node = &(elem->right);
        else
        {
            elem->value = value;
            return;
        }
        parent = elem;
    }

    *node = malloc(sizeof(s_pmap_node));
    (*node)->key = key;
    (*node)->value = value;
    (*node)->left = s->leaf;
    (*node)->right = s->leaf;
    (*node)->parent = parent;
    (*node)->isBlack = 0;
    insertFixup(s, *node);
    ++s->size;
}

void pmap_clear(s_pmap* s)
{
   if(s->root != s->leaf)
      nodeFree(s->root, s->leaf);

   s->root = s->leaf;
   s->size = 0;
}

int pmap_remove(s_pmap* s, t_pmapk key)
{
   s_pmap_node *node = s->root;

    while(node != s->leaf)
    {
        int val = s->cmp(key, node->key);
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


s_pmap_node *pmap_find(s_pmap *t, t_pmapk key)
{
    s_pmap_node *node = t->root;
    while(node != t->leaf)
    {
        int val = t->cmp(key, node->key);
        if(val < 0)
            node = node->left;
        else if(val > 0)
            node = node->right;
        else
            return node;
    }

    return t->leaf;
}

s_pmap_node* pmap_min(s_pmap* t)
{
   return t->root == t->leaf ? t->leaf : nodeGetMin(t->root, t->leaf);
}

s_pmap_node* pmap_max(s_pmap* t)
{
   return t->root == t->leaf ? t->leaf : nodeGetMax(t->root, t->leaf);
}

s_pmap_node* pmap_begin(s_pmap* t)
{
   return t->root == t->leaf ? t->leaf : nodeGetMin(t->root, t->leaf);
}

s_pmap_node* pmap_end(s_pmap* t)
{
   return t->leaf;
}

s_pmap_node* pmap_prev(s_pmap* s, s_pmap_node* node)
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
        s_pmap_node *parent = node->parent;
        while(parent != s->leaf && node == parent->left)
        {
            node = parent;
            parent = node->parent;
        }
        return parent;
    }
}

s_pmap_node* pmap_next(s_pmap* s, s_pmap_node* node)
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
        s_pmap_node *parent = node->parent;
        while(parent != s->leaf && node == parent->right)
        {
            node = parent;
            parent = node->parent;
        }
        return parent;
    }
}

t_pmapv pmap_key(s_pmap *t, s_pmap_node* node)
{
    if(t->leaf == node)
        err("Impossible to access value of null node");
    return node->key;
}

t_pmapv pmap_value(s_pmap *t, s_pmap_node* node)
{
    if(t->leaf == node)
        err("Impossible to access value of null node");
    return node->value;
}


void pmap_foreach(s_pmap* s, f_pmap_foreach f)
{
    for(s_pmap_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pmap_next(s, it))
    {
        f(it->key, it->value);
    }
}

int pmap_every(s_pmap* s, f_pmap_pred f)
{
    for(s_pmap_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pmap_next(s, it))
    {
        if(!f(it->key, it->value))
            return 0;
    }
    return 1;
}

int pmap_some(s_pmap* s, f_pmap_pred f)
{
    for(s_pmap_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pmap_next(s, it))
    {
        if(f(it->key, it->value))
            return 1;
    }
    return 0;
}

s_pmap* pmap_filter(s_pmap* s, f_pmap_pred f)
{
    s_pmap* res = pmap_new(s->cmp);

    for(s_pmap_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pmap_next(s, it))
    {
        if(f(it->key, it->value))
            pmap_insert(res, it->key, it->value);
    }

    return res;
}

s_pmap_node* pmap_search(s_pmap* s, f_pmap_pred f)
{
    for(s_pmap_node* it = nodeGetMin(s->root, s->leaf);
        it != s->leaf; it = pmap_next(s, it))
    {
        if(f(it->key, it->value))
            return it;
    }

    return s->leaf;
}
