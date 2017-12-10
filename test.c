#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../tree-node/node.h"
#include "tree.h"

node* right_heavy() {
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    node* n2 = node_new(2);
    n0->r = n1;
    n1->p = n0;
    n1->r = n2;
    n2->p = n1;
    n0->b = 2;
    n1->b = 1;
    return n0;
}

node* right() {
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    n0->r = n1;
    n0->b = 1;
    n1->p = n0;
    return n0;
}

node* left_heavy() {
    node* n0 = node_new(2);
    node* n1 = node_new(1);
    node* n2 = node_new(0);
    n0->l = n1;
    n1->p = n0;
    n1->l = n2;
    n2->p = n1;
    n0->b = -2;
    n1->b = -1;
    return n0;
}

node* balanced() {
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    node* n2 = node_new(2);
    node* n3 = node_new(3);
    node* n4 = node_new(4);
    n1->l = n0;
    n0->p = n1;
    n1->r = n3;
    n3->p = n1;
    n3->l = n2;
    n2->p = n3;
    n3->r = n4;
    n4->p = n3;

    n1->b = 1;
    return n1;
}

void validate_balance(node* n) {
    assert(n->d == 1);
    assert(n->b == 0);
    assert(n->l->d == 0);
    assert(n->l->b == 0);
    assert(n->r->d == 2);
    assert(n->r->b == 0);
}


int main() {
    node* p = node_new(5);
    node* c = node_new(7);
    p->r = c;
    c->p = p;
    _update_bf(p, c, true);
    assert(p->b == 1);

    p = node_new(5);
    c = node_new(7);
    p->r = c;
    p->b = 1;
    c->p = p;
    _update_bf(p, c, false);
    assert(p->b == 0);

    node* x = right_heavy();
    x = _right_right(x);
    validate_balance(x);

    x = left_heavy();
    x = _left_left(x);
    validate_balance(x);

    x = right_heavy();
    x = _rebalance(x);
    validate_balance(x);

    x = left_heavy();
    x = _rebalance(x);
    validate_balance(x);

    x = right();
    node* n = node_new(2);
    x->r->r = n;
    n->p = x->r;
    x = _retrace(x->r->r, true);
    assert(x->d == 1 && x->b == 0 && x->p == NULL);
    assert(x->r->d == 2 && x->r->b == 0 && x->r->p == x && x->r->r == NULL && x->r->l == NULL);
    assert(x->l->d == 0 && x->l->b == 0 && x->l->p == x && x->l->r == NULL && x->r->l == NULL);

    x = balanced();
    x->r->r->r = node_new(5);
    x->r->r->r->p = x->r->r;
    x->r->r->b = 0;
    x->b = 1;
    x = _retrace(x->r->r->r, true);
    assert(x->d == 3 && x->b == 0 && x->l != NULL && x->r != NULL);
    assert(x->l->d == 1 && x->l->p == x && x->l->b == 0);
    assert(x->r->d == 4 && x->r->p == x && x->r->b == 1 && x->r->l == NULL);
    assert(x->l->l->d == 0 && x->l->l->b == 0 && x->l->l->p == x->l && x->l->l->l == NULL && x->l->l->r == NULL);
    assert(x->l->r->d == 2 && x->l->r->b == 0 && x->l->r->p == x->l && x->l->r->l == NULL && x->l->r->r == NULL);
    assert(x->r->r->d == 5 && x->r->r->b == 0 && x->r->r->p == x->r && x->r->r->l == NULL && x->r->r->r == NULL);

    x = balanced();
    printf("\n");
    n = node_new(5);
    x = _insert_node(x, n);
    assert(x->d == 3 && x->b == 0 && x->l != NULL && x->r != NULL);
    assert(x->l->d == 1 && x->l->p == x && x->l->b == 0);
    assert(x->r->d == 4 && x->r->p == x && x->r->b == 1 && x->r->l == NULL);
    assert(x->l->l->d == 0 && x->l->l->b == 0 && x->l->l->p == x->l && x->l->l->l == NULL && x->l->l->r == NULL);
    assert(x->l->r->d == 2 && x->l->r->b == 0 && x->l->r->p == x->l && x->l->r->l == NULL && x->l->r->r == NULL);
    assert(x->r->r->d == 5 && x->r->r->b == 0 && x->r->r->p == x->r && x->r->r->l == NULL && x->r->r->r == NULL);

    x = node_new(0);
    for (long i = 0; i <= 50; i++) {
        x=_insert_node(x, node_new(i));
    }
    _node_print(x);

    // add test that creates an array of numbers
    // insert the numbers
    // traverse tree and make sure numbers are there, ticking them off
    // make sure bf < 2 || -2

    /*validate_rr(rr);
    tree* t = tree_new();
    tree_insert(t, 5);
    assert(t->r->d == 5);
    assert(tree_remove(t, 5));
    assert(t->r == NULL);
    tree_insert(t, 0);
    tree_insert(t, 1);
    tree_insert(t, 2);
    losing the root flip somehow
    for (long i = 0; i <= 2; i++) {
        tree_print(t);
        tree_insert(t, i);
    }
    tree_print(t);
    assert(tree_search(t, 3)->d == 3);
    assert(tree_remove(t, 3));
    assert(NULL == tree_search(t, 3));
    _tree_free_N(t);*/

    return 0;
}
