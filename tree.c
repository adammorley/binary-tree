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
STATIC void Assert(bool condition, const char* func, const char* format, ...) {
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
    Assert(t != NULL, __func__, "tree is null");
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
    n = _insert_node(n, c);
    if (n->p == NULL) t->r = n;
}

tree* tree_new() {
    tree* t = malloc(sizeof(tree));
    Assert(t != NULL, __func__, "malloc error");
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
    //FIXME: what about root page?
    if (n->r == NULL && n->l == NULL) {
        r = _remove_no_children(n);
    } else if (n->r == NULL) {
        r = _remove_no_right_children(n);
    } else if (n->r != NULL && n->r->l == NULL) {
        r = _remove_right_no_left(n);
    } else if (n->r != NULL && n->r->l != NULL) {
        r = _remove_complex(n);
    } else Assert(false, __func__, "unhandled node removal");
    if (r->p == NULL) t->r = r;
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
    LOG_DEBUG("searching for: %li", __func__, d);
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
STATIC node* _retrace_insert(node* c) {
    LOG_DEBUG("retracing insert at %li", __func__, c->d);
    node* p = c;
    /*
        note break case, prevents null pointer in ensuing statements
        and prevents loop forever
    */
    while (true) {
        p = p->p;
        _update_bf_insert(p, c);
        if (p->b == 2 || p->b == -2) p = _rebalance(p);
        if (p->p == NULL) break;
        // must re-home new top node
        //if (p->p->d < p->d) p->p->r = p;
        //if (p->p->d > p->d) p->p->l = p;
        if (p->b == 0) break;
        c = p;
    }
    Assert(p != NULL, __func__, "p null");
    LOG_DEBUG("new top: %li", __func__, p->d);
    return p;
}

STATIC void _update_bf_insert(node* p, node* c) {
    if (p->l == c) p->b -= 1;
    else if (p->r == c) p->b += 1;
    else Assert(p->l == NULL && p->r == NULL, __func__, "unhandled parent/child relationship");
    Assert(p->b < 3 && p->b > -3, __func__, "balance factor invariant for %li", p->d);
    LOG_DEBUG("updated balance factor of %li to %li, parent of %li", __func__, p->d, p->b, c->d);
}
 
/* 
    node n just had a child removed, retrace and rebalance
*/
STATIC node* _retrace_remove(node* n) {
    LOG_DEBUG("retracing removal from %li", __func__, n->d);
    /*
        note break case, prevents null pointer in ensuing statements
        and prevents loop forever
    */
    while (true) {
        // previously balanced node is now unbalanced but height unchanged
        if (n->b == 1 || n->b == -1) break;
        if (n->b == 2 || n->b == -2) n = _rebalance(n);
        if (n->b == 1 || n->b == -1) break;
        if (n->p == NULL) break;
        if (n->p->d < n->d) n->p->b -= 1;
        else if (n->p->d > n->d) n->p->b += 1;
        else Assert(false, __func__, "unhandled parent/child relationship");
        Assert(n->b < 2 && n->b > -2, __func__, "node %li balance factor out of range after rebalance", n->d);
        Assert(n->p->b < 3 && n->p->b > -3, __func__, "balance factor invariant for %li", n->p->d);
        n = n->p;
    }
    Assert(n != NULL, __func__, "n null");
    LOG_DEBUG("new top: %li", __func__, n->d);
    return n;
}

/*
    replace node c with x and update bf at p
    returns true if retracing can be skipped because deletion was
    absorbed
*/
STATIC void _remove_splice(node* p, node* c, node* x) {
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
    } else Assert(false, __func__, "unhandled balance factor");
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
STATIC node* _remove_no_children(node* c) {
    LOG_DEBUG("removing %li", __func__, c->d);
    node* p = c->p;
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
STATIC node* _remove_no_right_children(node* n) {
    node* p = n->p;
    node* x = n->l;
    LOG_DEBUG("removing %li and replacing with %li", __func__, n->d, x->d);
    x->p = n->p;
    _remove_splice(p, n, x);
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
STATIC node* _remove_right_no_left(node* n) {
    node* p = n->p;
    node* x = n->r;
    LOG_DEBUG("removing %li and replacing with %li", __func__, n->d, x->d);
    x->p = p;
    x->l = n->l;
    x->b = n->b - 1;
    _remove_splice(p, n, x);
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

    /*
        if parent was balanced, don't need to retrace (height didn't change)
        but if the parent is unbalanced, and it's right side is unbalanced,
            then just rebalance the parent (since the child was moved)
            and no retracing needed since the height of the tree
            remains the same
        if the parent's parent was balanced, only need to update parent and
            parent's parent, as rebalance not needed
        otherwise, have to update parent and retrace (see _retrace later)
    */
    node* p = NULL;
    if (c->p->b == 0) {
        c->p->b += 1;
    } else if (c->p->b == 1 && c->p->r->b == 0) {
        c->p->b += 1;
        n->r = _rebalance(c->p); 
    } else if (c->p->p->b == 0) {
        c->p->b += 1;
        c->p->p->b -= 1;
    } else {
        c->p->b += 1;
        p = c->p; // retrace happens before return
    }

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

    if (p) c = _retrace_remove(p);
    LOG_DEBUG("new node %li", __func__, c->d);
    return c;
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
    } else Assert(false, __func__, "unhandled balance factor for Y in left right case");
    return Y;
}
