#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../tree-node/node.h"
#include "tree.h"

STATIC void _insert_update_b(node* n);
STATIC void _remove_no_right_children(node* n);
STATIC void _remove_right_no_left(node* n);
STATIC void _remove_complex(node* n);

static node* _get_root(tree* t) {
    assert(t);
    return t->r;
}

static void _insert_node(node* n, long d) {
    if (d < n->d && n->l != NULL) return _insert_node(n->l, d);
    if (d > n->d && n->r != NULL) return _insert_node(n->r, d);
    node* c = node_new(d);
    if (d < n->d && n->l == NULL) n->l = c;
    if (d > n->d && n->r == NULL) n->r = c;
    c->p = n;
    _insert_update_b(c);
    //FIXME: rebalance
    return;
}

void tree_insert(tree* t, long d) {
    node* n = _get_root(t);
    if (n == NULL) {
        t->r = node_new(d);
        return;
    }
    if (n->d == d) return; // noop
    _insert_node(n, d);
}

tree* tree_new() {
    tree* t = malloc(sizeof(tree));
    if (t == NULL) assert(true);
    t->r = NULL;
    return t;
}

bool tree_remove(tree* t, long d) {
    node* n = _get_root(t);
    if (n == NULL) return false;
    if (n->d == d) {
        t->r = NULL;
        free(n);
        return true;
    }
    n = tree_search(t, d);
    if (n == NULL) return false;

    if (n->r == NULL) _remove_no_right_children(n);
    if (n->r != NULL && n->r->l == NULL) _remove_right_no_left(n);
    if (n->r != NULL && n->r->l != NULL) _remove_complex(n);
    free(t);
    //FIXME: rebalance
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

static void _tree_node_free(node* n) {
    if (n->l != NULL) _tree_node_free(n->l);
    if (n->r != NULL) _tree_node_free(n->r);
    free(n);
}

void _tree_free_N(tree* t) {
    node* n = _get_root(t);
    if (n == NULL) return;
    _tree_node_free(n);
    t->r = NULL;
}

STATIC void _insert_update_b(node* n) {
    node* p = n;
    node* c = n;
    while (p->p != NULL) {
        p = p->p;
        if (p->l == c) p->b -= 1;
        if (p->r == c) p->b += 1;
        c = c->p;
    }
}

STATIC void _remove_update_b(node* n) {
    node* p = n;
    node* c = n;
    while (p->p != NULL) {
        p = p->p;
        if (p->l == c) p->b += 1;
        if (p->r == c) p->b -= 1;
        c = c->p;
    }
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
STATIC void _remove_no_right_children(node* n) {
    n->l->p = n->p;
    if (n->p->l == n) n->p->l = n->l;
    if (n->p->r == n) n->p->r = n->l;
    _remove_update_b(n->l);
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
STATIC void _remove_right_no_left(node* n) {
    n->r->p = n->p;
    n->r->l = n->l;
    if (n->p->r == n) n->p->r = n->r;
    if (n->p->l == n) n->p->l = n->r;
    _remove_update_b(n->r);
}

/* node has right child, and child has left children
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

note some complexity: by pulling from under a node, a lot
of balance factors have to be updated, and the sign flips
upon just reaching the node (because the right has shrunk there
whereas other places, it's the left)
*/
STATIC void _remove_complex(node* n) {
    node* c = n;
    // find the unbusy left node
    while (c->l != NULL) {
        c = c->l;
    }

    // first, catch the child
    if (c->r != NULL) c->p->l = c->r;
    else c->p->l = NULL;

    // hook it into the right place, updating balance too
    c->p = n->p;
    c->b = n->b;
    if (n->p->r == n) n->p->r = c;
    if (n->p->l == n) n->p->r = c;

    // update the balance factors up to the node to be removed
    node* t = c;
    while (t->p != n) {
        t = t->p;
        t->b += 1;
    }

    // do the replaces
    n->l->p = c; c->l = n->l;
    n->r->p = c; c->r = n->r;

    // and update the balance factors to the root
    t = c;
    while (c->p != NULL) {
        c->b -= 1;
        c = c->p;
    }
}
