#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../log/log.h"
#include "../tree-node/tree_node.h"

#include "tree.h"
// tree internal function prototypes
#include "static.h"

void tree_insert(tree* t, long d) {
    tree_node* n = _get_root(t);
    tree_node* c = tree_node_new(d);
    if (n == NULL) {
        t->r = c;
        return;
    }
    if (n->d == d) return; // noop
    n = _insert_node(n, c);
    if (n->p == NULL) t->r = n;
}

tree* tree_new() {
    tree* t = malloc(sizeof(tree));
    Assert(t != NULL, __func__, "malloc error");
    t->r = NULL;
    return t;
}

static void _node_print(tree_node* n) {
    if (n != NULL) {
        printf("n: %lu, p: %lu, d: %li, l: %lu, r: %lu, b: %li\n", (unsigned long) n, (unsigned long) n->p, n->d, (unsigned long) n->l, (unsigned long) n->r, n->b);
        _node_print(n->l);
        _node_print(n->r);
    }
}

void tree_print(tree* t) {
    tree_node* n = _get_root(t);
    printf("r: %lu\n", (unsigned long) n);
    _node_print(n);
    printf("\n");
}

bool tree_remove(tree* t, long d) {
    tree_node* n = _get_root(t);
    if (n == NULL) return false;
    if (n->d == d && n->l == NULL && n->r == NULL) {
        t->r = NULL;
        free(n);
        return true;
    }
    n = tree_search(t, d);
    if (n == NULL) return false;

    tree_node* r;
    if (n->r == NULL && n->l == NULL) {
        r = _remove_no_children(n);
    } else if (n->r == NULL) {
        r = _remove_no_right_children(n);
    } else if (n->r != NULL && n->r->l == NULL) {
        r = _remove_right_no_left(n);
    } else if (n->r != NULL && n->r->l != NULL) {
        r = _remove_complex(n);
    } else Assert(false, __func__, "unhandled node removal");
    if (r == NULL) t->r = NULL;
    if (r->p != NULL) Assert(false, __func__, "root parent not NULL: %li", r->d);
    t->r = r;
    free(n);
    return true;
}

static tree_node* _tree_search(tree_node* n, long d) {
    if (n->d == d) return n;
    if (d < n->d && n->l != NULL) return _tree_search(n->l, d);
    if (d > n->d && n->r != NULL) return _tree_search(n->r, d);
    return NULL;
}
    
tree_node* tree_search(tree* t, long d) {
    tree_node* n = _get_root(t);
    if (n == NULL) return NULL;
    LOG_DEBUG("searching for: %li", __func__, d);
    return _tree_search(n, d);
}

void _tree_free(tree* t) {
    tree_node* n = _get_root(t);
    if (n == NULL) return;
    tree_node_free_recurse(n);
    free(t);
}

/*
    insert a new node into the tree starting at n
    maintaining binary search tree propety and
    rebalancing the tree if needed

    returns the top level node of the new tree
    so the caller can update the root pointer
    if needed
*/
STATIC tree_node* _insert_node(tree_node* n, tree_node* c) {
    if (c->d < n->d && n->l != NULL) return _insert_node(n->l, c);
    if (c->d > n->d && n->r != NULL) return _insert_node(n->r, c);
    LOG_DEBUG("inserting: %li", __func__, c->d);
    if (c->d < n->d && n->l == NULL) n->l = c;
    if (c->d > n->d && n->r == NULL) n->r = c;
    c->p = n;
    return _retrace_insert(c);
}

/* 
    node c was just inserted (and hence balance factor = 0)
    update its parent's balance factor, retracing up the tree
    rebalance if needed.
*/
STATIC tree_node* _retrace_insert(tree_node* c) {
    LOG_DEBUG("retracing insert at %li", __func__, c->d);
    tree_node* p = c;
    /*
        note break cases:
            if rebalance led to root page
            if tree is balanced (no height changes)
    */
    while (true) {
        p = p->p;
        _update_bf_insert(p, c);
        if (p->b == 2 || p->b == -2) p = _rebalance(p);
        if (p->p == NULL) break;
        if (p->b == 0) break;
        c = p;
    }
    LOG_DEBUG("new top: %li", __func__, p->d);
    return _up_to_root(p);
}

STATIC void _update_bf_insert(tree_node* p, tree_node* c) {
    Assert(p != NULL, __func__, "p null");
    if (p->l == c) p->b -= 1;
    else if (p->r == c) p->b += 1;
    else Assert(p->l == NULL && p->r == NULL, __func__, "unhandled parent/child relationship");
    Assert(p->b < 3 && p->b > -3, __func__, "balance factor invariant for %li", p->d);
    LOG_DEBUG("updated balance factor of %li to %li, parent of %li", __func__, p->d, p->b, c->d);
}
 
/* 
    node n just had a child removed, retrace and rebalance
*/
STATIC tree_node* _retrace_remove(tree_node* n) {
    LOG_DEBUG("retracing removal from %li", __func__, n->d);
    /*
        note break cases:
            if node removal was absorbed at n (combined with case immediately below)
            if after rebalance, removal was absorbed
            if node doesn't have parent (root node)
            otherwise, update balance factors, check asserts and loop
    */
    while (true) {
        if (n->b == 2 || n->b == -2) n = _rebalance(n);
        Assert(n->b < 2 && n->b > -2, __func__, "node %li balance factor out of range after rebalance", n->d);
        if (n->b == 1 || n->b == -1) break;
        if (n->p == NULL) break;
        if (n->p->d < n->d) n->p->b -= 1;
        else if (n->p->d > n->d) n->p->b += 1;
        else Assert(false, __func__, "unhandled parent/child relationship");
        Assert(n->p->b < 3 && n->p->b > -3, __func__, "balance factor invariant for %li", n->p->d);
        n = n->p;
    }
    Assert(n != NULL, __func__, "n null");
    LOG_DEBUG("new top: %li", __func__, n->d);

    return _up_to_root(n);
}

/*
    replace node c with x at p
*/
STATIC void _remove_splice(tree_node* p, tree_node* c, tree_node* x) {
    if (x != NULL) x->p = p;
    if (p == NULL) return;
    if (p->l == c) {
        p->l = x;
    } else if (p->r == c) {
        p->r = x;
    } else Assert(false, __func__, "unhandled parent/child case: %li, %li", p->d, c->d);
}

/*
    check for rebalancing at node n
    return the new top node at that
    level of the tree if rebalanced
    so the caller can update.
*/
STATIC tree_node* _rebalance(tree_node* n) {
    LOG_DEBUG("rebalancing %li", __func__, n->d);
    if (n->b == 2) { // right heavy
        if (n->r->b == 0 || n->r->b == 1) { // right right
            n = _right_right(n);
        } else { // right left, n->r->b == -1
            n = _right_left(n);
        }
    } else if (n->b == -2) { // left heavy
        if (n->l->b == -1 || n->l->b == 0) { // left left
            n = _left_left(n);
        } else { // left right, n->l->b == 1
            n = _left_right(n);
        }
    } else Assert(false, __func__, "asked to rebalance, but bf != 2");
    LOG_DEBUG("rebalanced to %li", __func__, n->d);
    if (n->p == NULL) return n;
    else if (n->p->d < n->d) n->p->r = n;
    else if (n->p->d > n->d) n->p->l = n;
    else Assert(false, __func__, "unhandled parent/child case: %li, %li", n->p->d, n->d);
    return n;
}

/*
    node has no children, can simply remove and retrace
    only need to retrace if tree height decreased
*/
STATIC tree_node* _remove_no_children(tree_node* c) {
    LOG_DEBUG("removing %li", __func__, c->d);
    tree_node* p = c->p;
    if (p == NULL) return NULL; // root page
    if (p->r == c) p->b -= 1;
    else p->b += 1;
    _remove_splice(p, c, NULL);
    return _retrace_remove(p);
}

/* node has no right children, can simply swap pointers at parent

    10
   /  \
  3   20*
     /  
    15

    to

    10
   /  \
  3   15

*/
STATIC tree_node* _remove_no_right_children(tree_node* n) {
    tree_node* p = n->p;
    tree_node* x = n->l;
    LOG_DEBUG("removing %li and replacing with %li", __func__, n->d, x->d);
    _remove_splice(p, n, x);
    if (p == NULL && x->p == NULL) return x; // root page
    return _retrace_remove(x);
}

/* node has right child, but child has no left children

    10
   /  \
  3   20
     /  \
    15  25*
          \
          30
            \
            40
    to

    10
   /  \
  3   20
     /  \
    15   30
           \
           40
*/
STATIC tree_node* _remove_right_no_left(tree_node* n) {
    tree_node* p = n->p;
    tree_node* x = n->r;
    LOG_DEBUG("removing %li and replacing with %li", __func__, n->d, x->d);
    x->l = n->l;
    x->b = n->b - 1;
    _remove_splice(p, n, x);
    if (p == NULL && x->p == NULL) return x; // root page
    return _retrace_remove(x);
}

/*
    node has right child, and child has left children
    must traverse down and re-home left most child
    but take care to not drop right child of
    left most child!

    10
   /  \
  3   20
     /  \
    15  25*
       /  \
      24  30
         /  \
        27  40
         \
         28

    to

    10
   /  \
  3   20
     /  \
    15  27
       /  \
      24  30
         /  \
        28  40
*/
STATIC tree_node* _remove_complex(tree_node* n) {
    // step down to the right node
    tree_node* c = n->r;
    // find the unbusy left node
    while (c->l != NULL) {
        c = c->l;
    }
    LOG_DEBUG("removing %li, replacing with %li", __func__, n->d, c->d);

    // first, catch the child
    if (c->r != NULL) c->p->l = c->r;
    else c->p->l = NULL;

    // track the old parent for c, and note the height shrink
    tree_node* p = c->p;
    p->b += 1;

    // hook c into the right place
    c->b = n->b;
    _remove_splice(n->p, n, c);

    // swap in the children from n to c
    n->l->p = c;
    c->l = n->l;
    n->r->p = c;
    c->r = n->r;

    LOG_DEBUG("new node %li", __func__, c->d);
    // and retrace
    if (p == NULL && c->p == NULL) return c; // root page
    return _retrace_remove(p);
}

/*
    rebalance tree methods

    there are four unbalanced states:
    right right (the right side is right heavy)
    right left (...)
    left left
    left right

    each of the cases returns the new top node, so the caller
    can check if this is the new tree root.

    pretty pictures:

    right right (right side is right heavy, rotate left):

        X b=2
       / \
      t0  Z b=[0,1]
         / \
        t1  t2

    pivot to:

          Z b=[-1,0]
         / \
b=[0,1] X   t2
       / \
      t0 t1
*/
STATIC tree_node* _right_right(tree_node* X) {
    LOG_DEBUG("RR rebalance of %li", __func__, X->d);
    tree_node* Z = X->r;
    Assert(Z->b != -1, __func__, "right left in right right case");
    Z->p = X->p;
    X->p = Z;

    X->r = Z->l;
    if (X->r) X->r->p = X;

    Z->l = X;
    if (Z->b == 0) {
        LOG_DEBUG("delete from %li L", __func__, X->d);
        X->b = 1;
        Z->b = -1;
    } else {
        LOG_DEBUG("insert R or delete L from %li", __func__, Z->d);
        X->b = 0;
        Z->b = 0;
    }
    return Z;
}
/*
    right left:

          X b=2
         / \
        t0  Z b=-1
           / \
 b=-1,0,1 Y  t3
         / \
        t1 t2

    pivot to:

           Y b=0
          / \
 b=-1,0  X   Z b=0,1
        / \  / \
      t0 t1 t2 t3
*/
STATIC tree_node* _right_left(tree_node* X) {
    LOG_DEBUG("RL rebalance of %li", __func__, X->d);
    tree_node* Z = X->r;
    Assert(Z->b == -1, __func__, "right right in right left case");
    tree_node* Y = Z->l;
    Y->p = X->p;
    Z->p = Y;
    X->p = Y;

    X->r = Y->l;
    if (X->r) X->r->p = X;
    Z->l = Y->r;
    if (Z->l) Z->l->p = Z;

    Y->l = X;
    Y->r = Z;
    if (Y->b == 0) {
        LOG_DEBUG("delete from %li L", __func__, X->d);
        X->b = 0;
        Z->b = 0;
    } else if (Y->b == -1) {
        LOG_DEBUG("insert on %li L", __func__, Y->d);
        X->b = 0;
        Z->b = 1;
        Y->b = 0;
    } else if (Y->b == 1) {
        LOG_DEBUG("insert on %li R", __func__, Y->d);
        X->b = -1;
        Z->b = 0;
        Y->b = 0;
    } else Assert(false, __func__, "invalid balance factor for Y in right left case");
    return Y;
}
/*
    left left:

           X b=-2
          / \
b=[-1,0] Z   t2
        / \
       t0 t1

    pivot to:

        Z b=[0,1]
       / \
      t0  X b=[-1,0]
         / \
        t1  t2
*/
STATIC tree_node* _left_left(tree_node* X) {
    LOG_DEBUG("LL rebalance of %li", __func__, X->d);
    tree_node* Z = X->l;
    Assert(Z->b != 1, __func__, "left right case in left left");
    Z->p = X->p;
    X->p = Z;

    X->l = Z->r;
    if (X->l) X->l->p = X;

    Z->r = X;
    if (Z->b == 0) {
        LOG_DEBUG("delete from %li R", __func__, X->d);
        X->b = -1;
        Z->b = 1;
    } else {
        LOG_DEBUG("insert L or delete R from %li", __func__, Z->d);
        X->b = 0;
        Z->b = 0;
    }
    return Z;
}
/*
    left right:

        X b=-2
       / \
  b=1 Z   t3
     / \
    t0  Y b=[-1,0,1]
       / \
      t1 t2

    pivot to:

        Y b=0
       / \
b=0,1 Z   X b=-1,0
     / \ / \
   t0 t1 t2 t3
*/
STATIC tree_node* _left_right(tree_node* X) {
    LOG_DEBUG("LR rebalance of %li", __func__, X->d);
    tree_node* Z = X->l;
    Assert(Z->b == 1, __func__, "left left case in left right");
    tree_node* Y = Z->r;
    Y->p = X->p;
    Z->p = Y;
    X->p = Y;

    X->l = Y->r;
    if (X->l) X->l->p = X;
    Z->r = Y->l;
    if (Z->r) Z->r->p = Z;

    Y->r = X;
    Y->l = Z;
    if (Y->b == 0) {
        LOG_DEBUG("delete from %li R", __func__, X->d);
        X->b = 0;
        Z->b = 0;
    } else if (Y->b == -1) {
        LOG_DEBUG("insert on %li L", __func__, Y->d);
        X->b = 0;
        Z->b = 1;
        Y->b = 0;
    } else if (Y->b == 1) {
        LOG_DEBUG("insert on %li R", __func__, Y->d);
        X->b = -1;
        Z->b = 0;
        Y->b = 0;
    } else Assert(false, __func__, "unhandled balance factor for Y in left right case");
    return Y;
}

/*
    some helper functions
*/
STATIC tree_node* _get_root(tree* t) {
    Assert(t != NULL, __func__, "tree is null");
    return t->r;
}

STATIC tree_node* _up_to_root(tree_node* n) {
    if (n == NULL) return NULL;
    while (n->p != NULL) {
        n = n->p;
    }
    LOG_DEBUG("root: %li", __func__, n->d);
    return n;
}
