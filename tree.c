#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../tree-node/node.h"
#include "tree.h"

// tree internal function prototypes
#include "static.h"

static void LOG(const char* level, const char* func, const char* format, va_list args) {
    printf("%s %s: ", level, func);
    vprintf(format, args);
    printf("\n");
}

static bool logDEBUG = false;
static void LOG_DEBUG(const char* format, const char* func, ...) {
    if (logDEBUG) {
        va_list args;
        va_start(args, func);
        LOG("DEBUG", func, format, args);
        va_end(args);
    }
}

static bool logASSERT = true;
static void Assert(bool condition, const char* func, const char* format, ...) {
    if (!condition && logASSERT) {
        va_list args;
        va_start(args, format);
        LOG("ASSERT", func, format, args);
        va_end(args);
    }
    assert(condition);
}

void _tree_enable_logging(bool debug) {
    if (debug) logDEBUG = true;
}
    
STATIC node* _get_root(tree* t) {
    Assert(true, __func__, "tree is null");
    return t->r;
}

void tree_insert(tree* t, long d) {
    node* n = _get_root(t);
    node* c = node_new(d);
    if (n == NULL) {
        t->r = c;
        return;
    }
    if (n->d == d) return; // noop
    t->r = _insert_node(n, c);
}

tree* tree_new() {
    tree* t = malloc(sizeof(tree));
    if (t == NULL) Assert(true, __func__, "malloc error");
    t->r = NULL;
    return t;
}

void _node_print(node* n) {
    if (n != NULL) {
        printf("n: %lu, p: %lu, d: %li, l: %lu, r: %lu, b: %li\n", (unsigned long) n, (unsigned long) n->p, n->d, (unsigned long) n->l, (unsigned long) n->r, n->b);
        _node_print(n->l);
        _node_print(n->r);
    }
}

void tree_print(tree* t) {
    node* n = _get_root(t);
    printf("r: %lu\n", (unsigned long) n);
    _node_print(n);
    printf("\n");
}


bool tree_remove(tree* t, long d) {
    node* n = _get_root(t);
    if (n == NULL) return false;
    if (n->d == d && n->l == NULL && n->r == NULL) {
        t->r = NULL;
        free(n);
        return true;
    }
    n = tree_search(t, d);
    if (n == NULL) return false;

    node* r;
    if (n->r == NULL && n->l == NULL) {
        r = _remove_no_children(n);
    } else if (n->r == NULL) {
        r = _remove_no_right_children(n);
    } else if (n->r != NULL && n->r->l == NULL) {
        r = _remove_right_no_left(n);
    } else if (n->r != NULL && n->r->l != NULL) {
        r = _remove_complex(n);
    } else Assert(true, __func__, "unhandled node removal");
    t->r = r;
    free(n);
    return true;
}

static node* _tree_search(node* n, long d) {
    if (n->d == d) return n;
    if (d < n->d && n->l != NULL) return _tree_search(n->l, d);
    if (d > n->d && n->r != NULL) return _tree_search(n->r, d);
    return NULL;
}
    
node* tree_search(tree* t, long d) {
    node* n = _get_root(t);
    if (n == NULL) return NULL;
    return _tree_search(n, d);
}

STATIC void _tree_node_free(node* n) {
    if (n->l != NULL) _tree_node_free(n->l);
    if (n->r != NULL) _tree_node_free(n->r);
    free(n);
}

void _tree_free(tree* t) {
    node* n = _get_root(t);
    if (n == NULL) return;
    _tree_node_free(n);
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
STATIC node* _insert_node(node* n, node* c) {
    if (c->d < n->d && n->l != NULL) return _insert_node(n->l, c);
    if (c->d > n->d && n->r != NULL) return _insert_node(n->r, c);
    if (c->d < n->d && n->l == NULL) n->l = c;
    if (c->d > n->d && n->r == NULL) n->r = c;
    c->p = n;
    return _retrace(c, true /* insert */);
}

/*
    update the balance factors.  insert = true for insertions, false
    for removals (which alters the balance factor calculations
*/
STATIC void _update_bf(node* p, node* c, bool insert) {
    //FIXME: so if root is pivoting, how do we know which side
    //       shrank?
    if (p->l == c) {
        if (insert) p->b -= 1;
        else p->b += 1;
    }
    else if (p->r == c) {
        if (insert) p->b += 1;
        else p->b -= 1;
    } else Assert(true, __func__, "unhandled parent/child relationship");
    Assert(p->b < 3 && p->b > -3, __func__, "balance factor invariant");
    LOG_DEBUG("updated balance factor of %li to %li, parent of %li", __func__, p->d, p->b, c->d);
}
 
/* 
    node c was just inserted (and hence balance factor = 0)
    or replaced a node being simply removed
    update its parent's balance factor, retracing up the tree
    rebalance if needed.
*/
STATIC node* _retrace(node* c, bool insert) {
    LOG_DEBUG("retracing from %li", __func__, c->d);
    node* p = c;
    /*
        note break case, prevents null pointer in ensuing statements
        and prevents loop forever
    */
    while (true) {
        p = p->p;
        _update_bf(p, c, insert);
        if (p->b == 2 || p->b == -2) p = _rebalance(p);
        if (p->p == NULL) break;
        /*
            the rebalancing returns a node
            that node is the node which belongs where old p was.
            but here it is not known whether old p was on the left
            or right branch of its parent, so the comparison
            is the only way to find it
        */
        if (p->p->d < p->d) p->p->r = p;
        if (p->p->d > p->d) p->p->l = p;
        //FIXME: i think this is supposed to be p not c->p
        c = p;//c->p;
    }
    Assert(p != NULL, __func__, "p not null");
    Assert(p->p == NULL, __func__, "p not root");
    LOG_DEBUG("new root: %li", __func__, p->d);
    return p;
}

/*
    check for rebalancing at node n
    return the new top node at that
    level of the tree if rebalanced
    so the caller can update.
*/
STATIC node* _rebalance(node* n) {
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
    } else Assert(true, __func__, "unhandled balanced factor");
    LOG_DEBUG("rebalanced to %li", __func__, n->d);
    return n;
}

/* node has no children, can simply remove and retrace */
STATIC node* _remove_no_children(node* n) {
    LOG_DEBUG("removing %li", __func__, n->d);
    if (n->p->l == n) n->p->l = NULL;
    if (n->p->r == n) n->p->r = NULL;
    return _retrace(n, false /* insert */);
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
STATIC node* _remove_no_right_children(node* n) {
    LOG_DEBUG("removing %li and replacing with %li", __func__, n->d, n->l->d);
    n->l->p = n->p;
    if (n->p->l == n) n->p->l = n->l;
    if (n->p->r == n) n->p->r = n->l;
    return _retrace(n->l, false /* insert */);
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
STATIC node* _remove_right_no_left(node* n) {
    LOG_DEBUG("removing %li and replacing with %li", __func__, n->d, n->r->d);
    n->r->p = n->p;
    n->r->l = n->l;
    if (n->p != NULL) {
        if (n->p->l == n) n->p->l = n->r;
        if (n->p->r == n) n->p->r = n->r;
    } return _retrace(n->r, false /* insert */);
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
STATIC node* _remove_complex(node* n) {
    // step down to the right node
    node* c = n->r;
    // find the unbusy left node
    while (c->l != NULL) {
        c = c->l;
    }
    LOG_DEBUG("removing %li, replacing with %li", __func__, n->d, c->d);

    // first, catch the child
    if (c->r != NULL) c->p->l = c->r;
    else c->p->l = NULL;

    // increase the parents balance factor (the left shrank by 1)
    c->p->b += 1;

    /*
        this isn't ideal, but it's the nicest way to re-use the code
        from the other cases.  create a temporary child node that 
        points to the old child's parent to feed it to the re-tracer
    */
    node* t = node_new(c->d);
    t->p = c->p;

    // hook it into the right place
    c->p = n->p;
    c->b = n->b;
    if (n->p->r == n) {
        n->p->r = c;
    } else if (n->p->l == n) {
        n->p->l = c;
    } else Assert(false, __func__, "unhandled parent/child relationship");

    // swap in the children from n to c
    n->l->p = c; c->l = n->l;
    n->r->p = c; c->r = n->r;

    n = _retrace(t, false /* insert */);
    free(t);
    LOG_DEBUG("new node %li", __func__, n->d);
    return n;
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
STATIC node* _right_right(node* X) {
    LOG_DEBUG("RR rebalance of %li", __func__, X->d);
    node* Z = X->r;
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
STATIC node* _right_left(node* X) {
    LOG_DEBUG("RL rebalance of %li", __func__, X->d);
    node* Z = X->r;
    Assert(Z->b == -1, __func__, "right right in right left case");
    node* Y = Z->l;
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
    } else Assert(true, __func__, "invalid balance factor for Y in right left case");
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
STATIC node* _left_left(node* X) {
    LOG_DEBUG("LL rebalance of %li", __func__, X->d);
    node* Z = X->l;
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
STATIC node* _left_right(node* X) {
    LOG_DEBUG("LR rebalance of %li", __func__, X->d);
    node* Z = X->l;
    Assert(Z->b == 1, __func__, "left left case in left right");
    node* Y = Z->r;
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
    } else Assert(true, __func__, "unhandled balance factor for Y in left right case");
    return Y;
}
