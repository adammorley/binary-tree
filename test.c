#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../log/log.h"
#include "../tree-node/tree_node.h"

#include "tree.h"
#include "test-support.h"

void _check_tree(tree* t) {
    tree_node* n = _get_root(t);
    tree_node_check(n);
}

tree_node* right_heavy() {
    tree_node* n0 = tree_node_new(0);
    tree_node* n1 = tree_node_new(1);
    tree_node* n2 = tree_node_new(2);
    n0->r = n1;
    n1->p = n0;
    n1->r = n2;
    n2->p = n1;
    n0->b = 2;
    n1->b = 1;
    tree_node_check(n0);
    return n0;
}

tree_node* right() {
    tree_node* n0 = tree_node_new(0);
    tree_node* n1 = tree_node_new(1);
    n0->r = n1;
    n0->b = 1;
    n1->p = n0;
    tree_node_check(n0);
    return n0;
}

tree_node* left_heavy() {
    tree_node* n0 = tree_node_new(2);
    tree_node* n1 = tree_node_new(1);
    tree_node* n2 = tree_node_new(0);
    n0->l = n1;
    n1->p = n0;
    n1->l = n2;
    n2->p = n1;
    n0->b = -2;
    n1->b = -1;
    tree_node_check(n0);
    return n0;
}

tree_node* right_left_heavy() {
    tree_node* n0 = tree_node_new(0);
    tree_node* n1 = tree_node_new(1);
    tree_node* n2 = tree_node_new(2);
    tree_node* n3 = tree_node_new(3);
    tree_node* n4 = tree_node_new(4);
    tree_node* n5 = tree_node_new(5);
    tree_node* n6 = tree_node_new(6);
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
    tree_node_check(n1);
    return n1;
}

tree_node* left_right_heavy() {
    tree_node* n0 = tree_node_new(0);
    tree_node* n1 = tree_node_new(1);
    tree_node* n2 = tree_node_new(2);
    tree_node* n3 = tree_node_new(3);
    tree_node* n4 = tree_node_new(4);
    tree_node* n5 = tree_node_new(5);
    tree_node* n6 = tree_node_new(6);
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
    tree_node_check(n5);
    return n5;
}

tree_node* simple_balanced() {
    tree_node* n0 = tree_node_new(0);
    tree_node* n1 = tree_node_new(1);
    tree_node* n2 = tree_node_new(2);
    tree_node* n3 = tree_node_new(3);
    tree_node* n4 = tree_node_new(4);
    n1->l = n0;
    n0->p = n1;
    n1->r = n3;
    n3->p = n1;
    n3->l = n2;
    n2->p = n3;
    n3->r = n4;
    n4->p = n3;

    n1->b = 1;
    tree_node_check(n1);
    return n1;
}

// where lopsided is right left and left right
tree_node* lopsided_balanced() {
    tree_node* n0 = tree_node_new(0);
    tree_node* n1 = tree_node_new(1);
    tree_node* n2 = tree_node_new(2);
    tree_node* n3 = tree_node_new(3);
    tree_node* n4 = tree_node_new(4);
    tree_node* n5 = tree_node_new(5);
    tree_node* n6 = tree_node_new(6);
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
    tree_node_check(n3);
    return n3;
}

void validate_balance_simple(tree_node* n) {
    assert(n->d == 1);
    assert(n->b == 0);
    assert(n->l->d == 0);
    assert(n->l->b == 0);
    assert(n->r->d == 2);
    assert(n->r->b == 0);
    tree_node_check(n);
}

void check_bf(tree_node* n) {
    if (n == NULL) return;
    assert(n->b < 2 && n->b > -2);
    check_bf(n->l);
    check_bf(n->r);
}

void test__update_bf_insert() {
    printf("testing _update_bf_insert\n");
    tree_node* p = tree_node_new(5);
    tree_node* c = tree_node_new(7);
    p->r = c;
    c->p = p;
    _update_bf_insert(p, c);
    assert(p->b == 1);
    tree_node_check(p);
    tree_node_free_recurse(p);
}

void test_simple() {
    printf("simple test\n");
    tree_node* x = right_heavy();
    x = _right_right(x);
    tree_node_check(x);
    validate_balance_simple(x);
    tree_node_free_recurse(x);

    x = left_heavy();
    x = _left_left(x);
    tree_node_check(x);
    validate_balance_simple(x);
    tree_node_free_recurse(x);
}

void test_simple_rebalancing() {
    printf("simple rebalance test\n");
    tree_node* x = right_heavy();
    x = _rebalance(x);
    tree_node_check(x);
    validate_balance_simple(x);
    tree_node_free_recurse(x);

    x = left_heavy();
    x = _rebalance(x);
    tree_node_check(x);
    validate_balance_simple(x);
    tree_node_free_recurse(x);
}

void test_retracing() {
    printf("insert retracing test\n");
    tree_node* x = right();
    tree_node* n = tree_node_new(2);
    x->r->r = n;
    n->p = x->r;
    x = _retrace_insert(x->r->r);
    assert(x->d == 1 && x->b == 0 && x->p == NULL);
    assert(x->r->d == 2 && x->r->b == 0 && x->r->p == x && x->r->r == NULL && x->r->l == NULL);
    assert(x->l->d == 0 && x->l->b == 0 && x->l->p == x && x->l->r == NULL && x->r->l == NULL);
    tree_node_check(x);
    tree_node_free_recurse(x);

    x = simple_balanced();
    x->r->r->r = tree_node_new(5);
    x->r->r->r->p = x->r->r;
    x->r->r->b = 0;
    x->b = 1;
    x = _retrace_insert(x->r->r->r);
    assert(x->d == 3 && x->b == 0 && x->l != NULL && x->r != NULL);
    assert(x->l->d == 1 && x->l->p == x && x->l->b == 0);
    assert(x->r->d == 4 && x->r->p == x && x->r->b == 1 && x->r->l == NULL);
    assert(x->l->l->d == 0 && x->l->l->b == 0 && x->l->l->p == x->l && x->l->l->l == NULL && x->l->l->r == NULL);
    assert(x->l->r->d == 2 && x->l->r->b == 0 && x->l->r->p == x->l && x->l->r->l == NULL && x->l->r->r == NULL);
    assert(x->r->r->d == 5 && x->r->r->b == 0 && x->r->r->p == x->r && x->r->r->l == NULL && x->r->r->r == NULL);
    tree_node_check(x);
    tree_node_free_recurse(x);
}

void test__insert_node() {
    printf("node insertion test\n");
    tree_node* x = simple_balanced();
    tree_node* n = tree_node_new(5);
    x = _insert_node(x, n);
    assert(x->d == 3 && x->b == 0 && x->l != NULL && x->r != NULL);
    assert(x->l->d == 1 && x->l->p == x && x->l->b == 0);
    assert(x->r->d == 4 && x->r->p == x && x->r->b == 1 && x->r->l == NULL);
    assert(x->l->l->d == 0 && x->l->l->b == 0 && x->l->l->p == x->l && x->l->l->l == NULL && x->l->l->r == NULL);
    assert(x->l->r->d == 2 && x->l->r->b == 0 && x->l->r->p == x->l && x->l->r->l == NULL && x->l->r->r == NULL);
    assert(x->r->r->d == 5 && x->r->r->b == 0 && x->r->r->p == x->r && x->r->r->l == NULL && x->r->r->r == NULL);
    tree_node_check(x);
    tree_node_free_recurse(x);

    x = tree_node_new(0);
    for (long i = 1; i <= 50; i++) {
        x = _insert_node(x, tree_node_new(i));
    }
    tree_node_check(x);
    tree_node_free_recurse(x);
}

void test_big_tree() {
    printf("testing big tree\n");
    int cnt = 5000;
    long* nums = make_nums(cnt);
    tree* t = create_big_tree(nums, cnt);
    check_bf(_get_root(t));
    tree_node_check(_get_root(t));
    free(nums);
    _tree_free(t);
}

void test_lopsided() {
    printf("lopsided tree test\n");
    tree_node* x = right_left_heavy();
    x = _right_left(x);
    tree_node* y = lopsided_balanced();
    assert(tree_node_compare_recurse(x, y));
    tree_node_check(x);
    tree_node_check(y);
    tree_node_free_recurse(x);
    tree_node_free_recurse(y);

    x = left_right_heavy();
    x = _left_right(x);
    y = lopsided_balanced();
    assert(tree_node_compare_recurse(x, y));
    tree_node_check(x);
    tree_node_check(y);
    tree_node_free_recurse(x);
    tree_node_free_recurse(y);
}

void test_insert_correctness() {
    printf("insertion correctness test\n");
    tree* t = tree_new();
    int cnt = 5000;
    long nums[cnt];
    nums[0] = LONG_MAX;
    nums[1] = LONG_MIN;
    for (long i = 2; i <= (cnt - 1); i++) {
        nums[(int) i] = i;
    }
    for (int i = 0; i <= sizeof(nums) / sizeof(long); i++) {
        tree_insert(t, nums[i]);
    }
    for (int i = 0; i <= sizeof(nums) / sizeof(long); i++) {
        assert(tree_search(t, nums[i])->d == nums[i]);
    }
    check_bf(_get_root(t));
    // technically the above has checked everything, but let's
    // do this for correctness
    tree_node_check(_get_root(t));
    _tree_free(t);
}

void test__remove_no_right_children() {
    printf("testing _remove_no_right_children\n");
    tree_node* n0 = tree_node_new(0);
    tree_node* n1 = tree_node_new(1);
    tree_node* n2 = tree_node_new(2);
    tree_node* n3 = tree_node_new(3);

    n1->l = n0;
    n0->p = n1;
    n1->r = n3;
    n3->p = n1;
    n3->l = n2;
    n2->p = n3;
    n3->b = -1;
    n1->b = 1;
    tree_node_check(n1);

    tree* t = tree_new();
    tree_insert(t, 1);
    tree_insert(t, 0);
    tree_insert(t, 2);
    tree_node_check(_get_root(t));

    _remove_no_right_children(n3);
    free(n3);
    tree_node* x = _get_root(t);
    tree_node_check(n1);
    assert(tree_node_compare_recurse(n1, x));
    tree_node_free_recurse(n1);
    _tree_free(t);
}

void test__remove_right_no_left() {
    printf("testing _remove_right_no_left\n");
    tree_node* n_1 = tree_node_new(-1);
    tree_node* n0 = tree_node_new(0);
    tree_node* n1 = tree_node_new(1);
    tree_node* n2 = tree_node_new(2);
    tree_node* n3 = tree_node_new(3);
    tree_node* n4 = tree_node_new(4);
    tree_node* n5 = tree_node_new(5);

    n1->l = n0;
    n0->p = n1;
    n0->l = n_1;
    n_1->p = n0;
    n1->r = n3;
    n3->p = n1;
    n3->l = n2;
    n2->p = n3;
    n3->r = n4;
    n4->p = n3;
    n4->r = n5;
    n5->p = n4;
    n4->b = 1;
    n3->b = 1;
    n1->b = 1;
    n0->b = -1;
    tree_node_check(n1);
   
    tree* t = tree_new();
    tree_insert(t, 1);
    tree_insert(t, 0);
    tree_insert(t, 2);
    tree_insert(t, 3);
    tree_insert(t, -1);
    tree_insert(t, 5);
    tree_node_check(_get_root(t));

    _remove_right_no_left(n4);
    free(n4);
    tree_node_check(n1);
    assert(tree_node_compare_recurse(n1, _get_root(t)));
    tree_node_free_recurse(n1);
    _tree_free(t);
}

struct node_con {
    tree_node* root;
    tree_node* removal;
};

struct node_con complex_tree() {
    tree_node* n_4 = tree_node_new(-4);
    tree_node* n_3 = tree_node_new(-3);
    tree_node* n_2 = tree_node_new(-2);
    tree_node* n_1 = tree_node_new(-1);
    tree_node* n0 = tree_node_new(0);
    tree_node* n1 = tree_node_new(1);
    tree_node* n2 = tree_node_new(2);
    tree_node* n3 = tree_node_new(3);
    tree_node* n4 = tree_node_new(4);
    tree_node* n5 = tree_node_new(5);
    tree_node* n6 = tree_node_new(6);
    tree_node* n7 = tree_node_new(7);
    tree_node* n8 = tree_node_new(8);

    n_3->l = n_4;
    n_4->p = n3;
    n_3->b = -1;
    n_2->l = n_3;
    n_3->p = n_2;
    n_2->r = n_1;
    n_1->p = n_2;
    n_2->b = -1;
    n0->l = n_2;
    n_2->p = n0;
    n0->b = 1;
    n0->r = n3;
    n3->p = n0;
    n3->l = n2;
    n2->p = n3;
    n2->l = n1;
    n1->p = n2;
    n2->b = -1;
    n3->r = n5;
    n5->p = n3;
    n3->b = 1;
    n5->l = n4;
    n4->p = n5;
    n5->r = n7;
    n5->b = 1;
    n7->p = n5;
    n7->l = n6;
    n6->p = n7;
    n7->r = n8;
    n8->p = n7;

    struct node_con con;
    con.root = n0;
    con.removal = n3;
    tree_node_check(n0);
    return con;
}

void test__remove_complex() {
    printf("testing _remove_complex\n");
    struct node_con con = complex_tree();
    tree_node* n = con.root;
    tree_node* r = con.removal;
    _remove_complex(r);
    free(r);
    
    assert(n->d == 0);
    assert(n->r->d == 4); // remove
    assert(n->r->r->d == 7); // rebalance
    tree_node_check(n);
    tree_node_free_recurse(n);

    tree* t = tree_new();
    for (long i = 0; i <= 50; i++) {
        tree_insert(t, i);
    }
    tree_node* d = tree_search(t, 39);
    _remove_complex(d);
    free(d);
    tree_node_check(_get_root(t));
    _tree_free(t);
}

int main() {

    test__update_bf_insert();

    test_simple();
    test_simple_rebalancing();

    test_retracing();

    test__insert_node();

    test_big_tree();

    test_lopsided();

    test_insert_correctness();

    test__remove_no_right_children();

    test__remove_right_no_left();

    test__remove_complex();

    return 0;
}
