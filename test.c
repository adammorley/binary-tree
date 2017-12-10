#include <assert.h>
#include <limits.h>
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

node* right_left_heavy() {
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    node* n2 = node_new(2);
    node* n3 = node_new(3);
    node* n4 = node_new(4);
    node* n5 = node_new(5);
    node* n6 = node_new(6);
    n1->l = n0;
    n0->p = n1;
    n1->r = n5;
    n5->p = n1;
    n5->l = n3;
    n3->p = n5;
    n3->l = n2;
    n2->p = n3;
    n3->r = n4;
    n4->p = n3;
    n5->r = n6;
    n6->p = n5;
    n1->b = 2;
    n5->b = -1;
    return n1;
}

node* left_right_heavy() {
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    node* n2 = node_new(2);
    node* n3 = node_new(3);
    node* n4 = node_new(4);
    node* n5 = node_new(5);
    node* n6 = node_new(6);
    n5->r = n6;
    n6->p = n5;
    n5->l = n1;
    n1->p = n5;
    n1->l = n0;
    n0->p = n1;
    n1->r = n3;
    n3->p = n1;
    n3->l = n2;
    n2->p = n3;
    n3->r = n4;
    n4->p = n3;
    n5->b = -2;
    n1->b = 1;
    return n5;
}

node* simple_balanced() {
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

// where lopsided is right left and left right
node* lopsided_balanced() {
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    node* n2 = node_new(2);
    node* n3 = node_new(3);
    node* n4 = node_new(4);
    node* n5 = node_new(5);
    node* n6 = node_new(6);
    n3->l = n1;
    n1->p = n3;
    n1->l = n0;
    n0->p = n1;
    n1->r = n2;
    n2->p = n1;
    n3->r = n5;
    n5->p = n3;
    n5->l = n4;
    n4->p = n5;
    n5->r = n6;
    n6->p = n5;
    return n3;
}

void validate_balance_simple(node* n) {
    assert(n->d == 1);
    assert(n->b == 0);
    assert(n->l->d == 0);
    assert(n->l->b == 0);
    assert(n->r->d == 2);
    assert(n->r->b == 0);
}

void test_lopsided() {
    node* x = right_left_heavy();
    x = _right_left(x);
    node* y = lopsided_balanced();
    assert(node_compare_recurse(x, y));

    x = left_right_heavy();
    x = _left_right(x);
    y = lopsided_balanced();
    assert(node_compare_recurse(x, y));
    _tree_node_free(x);
    _tree_node_free(y);
}

void check_bf(node* n) {
    if (n == NULL) return;
    assert(n->b < 2 && n->b > -2);
    check_bf(n->l);
    check_bf(n->r);
}

void test__update_bf() {
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
    _tree_node_free(p);
}

void test_simple() {
    node* x = right_heavy();
    x = _right_right(x);
    validate_balance_simple(x);

    x = left_heavy();
    x = _left_left(x);
    validate_balance_simple(x);
    _tree_node_free(x);
}

void test_simple_rebalancing() {
    node* x = right_heavy();
    x = _rebalance(x);
    validate_balance_simple(x);

    x = left_heavy();
    x = _rebalance(x);
    validate_balance_simple(x);
    _tree_node_free(x);
}

void test_retracing() {
    node* x = right();
    node* n = node_new(2);
    x->r->r = n;
    n->p = x->r;
    x = _retrace(x->r->r, true);
    assert(x->d == 1 && x->b == 0 && x->p == NULL);
    assert(x->r->d == 2 && x->r->b == 0 && x->r->p == x && x->r->r == NULL && x->r->l == NULL);
    assert(x->l->d == 0 && x->l->b == 0 && x->l->p == x && x->l->r == NULL && x->r->l == NULL);

    x = simple_balanced();
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
    _tree_node_free(x);
}

void test__insert_node() {
    node* x = simple_balanced();
    node* n = node_new(5);
    x = _insert_node(x, n);
    assert(x->d == 3 && x->b == 0 && x->l != NULL && x->r != NULL);
    assert(x->l->d == 1 && x->l->p == x && x->l->b == 0);
    assert(x->r->d == 4 && x->r->p == x && x->r->b == 1 && x->r->l == NULL);
    assert(x->l->l->d == 0 && x->l->l->b == 0 && x->l->l->p == x->l && x->l->l->l == NULL && x->l->l->r == NULL);
    assert(x->l->r->d == 2 && x->l->r->b == 0 && x->l->r->p == x->l && x->l->r->l == NULL && x->l->r->r == NULL);
    assert(x->r->r->d == 5 && x->r->r->b == 0 && x->r->r->p == x->r && x->r->r->l == NULL && x->r->r->r == NULL);

    x = node_new(0);
    for (long i = 0; i <= 50; i++) {
        x = _insert_node(x, node_new(i));
    }
    _tree_node_free(x);
}

void test_simple_tree() {
    tree* t = tree_new();
    int cnt = 500;
    long nums[500];
    nums[0] = LONG_MAX;
    nums[1] = LONG_MIN;
    for (long i = 2; i <= (500 - 1); i++) {
        nums[(int) i] = i;
    }
    for (int i = 0; i <= sizeof(nums) / sizeof(long); i++) {
        tree_insert(t, nums[i]);
    }
    for (int i = 0; i <= sizeof(nums) / sizeof(long); i++) {
        assert(tree_search(t, nums[i])->d == nums[i]);
    }
    check_bf(_get_root(t));
    _tree_free(t);
}

int main() {
    test__update_bf();

    test_simple();
    test_simple_rebalancing();

    test_retracing();

    test__insert_node();

    test_simple_tree();

    test_lopsided();


    // test removal, especially _remove_complex


    return 0;
}
