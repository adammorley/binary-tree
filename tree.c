#include <stdlib.h>
#include "tree.h"

STATIC void _insert_update_b(node* n);
STATIC void _remove_no_right_children(node* n);
STATIC void _remove_right_no_left(node* n);
STATIC void _remove_complex(node* n);

void tree_insert(node* n, long d) {
    if (n->d == d) return; // noop
    if (d < n->d && n->l != NULL) return tree_insert(n->l, d);
    if (d > n->d && n->r != NULL) return tree_insert(n->r, d);
    node* c = node_new(d);
    if (d < n->d && n->l == NULL) n->l = c;
    if (d > n->d && n->r == NULL) n->r = c;
    c->p = n;
    _insert_update_b(c);
    //FIXME: rebalance
}

node* tree_remove(node* n, long d) {
    if (n->p == NULL && n->d == d) {
        free(n);
        return NULL;
    }
    node* t = tree_search(n, d);
    if (t == NULL) return n;

    if (t->r == NULL) _remove_no_right_children(t);
    if (t->r != NULL && t->r->l == NULL) _remove_right_no_left(t);
    if (t->r != NULL && t->r->l != NULL) _remove_complex(t);
    free(t);
    //FIXME: rebalance
    return n;
}

node* tree_search(node* n, long d) {
    if (n->d == d) return n;
    if (d < n->d && n->l != NULL) return tree_search(n->l, d);
    if (d > n->d && n->r != NULL) return tree_search(n->r, d);
    return NULL;
}

void _tree_free_N(node* n) {
    if (n->l != NULL) _tree_free_N(n->l);
    if (n->r != NULL) _tree_free_N(n->r);
    free(n);
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
