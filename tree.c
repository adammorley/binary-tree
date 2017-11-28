#include <assert.h>
#include <stdlib.h>
#include "tree.h"

void tree_insert(node* n, long d) {
    if (n->d == d) return; // noop
    if (d < n->d && n->l != NULL) return tree_insert(n->l, d);
    if (d > n->d && n->r != NULL) return tree_insert(n->r, d);
    node* c = node_new(d);
    if (d < n->d && n->l == NULL) n->l = c;
    if (d > n->d && n->r == NULL) n->r = c;
    //FIXME: rebalance
    c->p = n;
}

node* tree_remove(node* n, long d) {
    if (n->p == NULL && n->d == d) return NULL;
    node* t = tree_search(n, d);
    if (t == NULL) return n;

    // node has no right children, can simply swap pointers at parent
    if (t->r == NULL) {
        t->l->p = t->p;
        if (t->p->d < d) t->p->r = t->l;
        if (t->p->d > d) t->p->l = t->l;
    }

    // node has right child, but child has no left children
    if (t->r != NULL && t->r->l == NULL) {
        t->r->p = t->p;
        t->r->l = t->l;
        if (t->p->d < d) t->p->r = t->r;
        if (t->p->d > d) t->p->l = t->r;
    }

    // node has right child, and child has left children
    // must traverse down and re-home left most child
    // but take care to not drop right child of
    // left most child!
    if (t->r != NULL && t->r->l != NULL) {
        node* c = t;
        // find the unbusy left node
        while (c->l != NULL) {
            c = c->l;
        }
        if (c->r != NULL) c->p->l = c->r;
        else c->p->l = NULL;
        c->p = t->p;
        if (t->p->d < d) t->p->r = c;
        if (t->p->d > d) t->p->l = c;
        t->l->p = c; c->l = t->l;
        t->r->p = c; c->r = t->r;
    }
    free(t);
    return n;
}

node* tree_search(node* n, long d) {
    if (n->d == d) return n;
    if (d < n->d && n->l != NULL) return tree_search(n->l, d);
    if (d > n->d && n->r != NULL) return tree_search(n->r, d);
    return NULL;
}
