#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../log/log.h"
#include "../tree-node/node.h"

#include "tree.h"

void _check_tree(tree* t) {
    node* n = _get_root(t);
    node_check_tree(n);
}

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
    node_check_tree(n0);
    return n0;
}

node* right() {
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    n0->r = n1;
    n0->b = 1;
    n1->p = n0;
    node_check_tree(n0);
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
    node_check_tree(n0);
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
    node_check_tree(n1);
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
    node_check_tree(n5);
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
    node_check_tree(n1);
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
    node_check_tree(n3);
    return n3;
}

void validate_balance_simple(node* n) {
    assert(n->d == 1);
    assert(n->b == 0);
    assert(n->l->d == 0);
    assert(n->l->b == 0);
    assert(n->r->d == 2);
    assert(n->r->b == 0);
    node_check_tree(n);
}

void check_bf(node* n) {
    if (n == NULL) return;
    assert(n->b < 2 && n->b > -2);
    check_bf(n->l);
    check_bf(n->r);
}

void test__update_bf_insert() {
    printf("testing _update_bf_insert\n");
    node* p = node_new(5);
    node* c = node_new(7);
    p->r = c;
    c->p = p;
    _update_bf_insert(p, c);
    assert(p->b == 1);
    node_check_tree(p);
    _tree_node_free(p);
}

void test_simple() {
    printf("simple test\n");
    node* x = right_heavy();
    x = _right_right(x);
    node_check_tree(x);
    validate_balance_simple(x);
    _tree_node_free(x);

    x = left_heavy();
    x = _left_left(x);
    node_check_tree(x);
    validate_balance_simple(x);
    _tree_node_free(x);
}

void test_simple_rebalancing() {
    printf("simple rebalance test\n");
    node* x = right_heavy();
    x = _rebalance(x);
    node_check_tree(x);
    validate_balance_simple(x);
    _tree_node_free(x);

    x = left_heavy();
    x = _rebalance(x);
    node_check_tree(x);
    validate_balance_simple(x);
    _tree_node_free(x);
}

void test_retracing() {
    printf("insert retracing test\n");
    node* x = right();
    node* n = node_new(2);
    x->r->r = n;
    n->p = x->r;
    x = _retrace_insert(x->r->r);
    assert(x->d == 1 && x->b == 0 && x->p == NULL);
    assert(x->r->d == 2 && x->r->b == 0 && x->r->p == x && x->r->r == NULL && x->r->l == NULL);
    assert(x->l->d == 0 && x->l->b == 0 && x->l->p == x && x->l->r == NULL && x->r->l == NULL);
    node_check_tree(x);
    _tree_node_free(x);

    x = simple_balanced();
    x->r->r->r = node_new(5);
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
    node_check_tree(x);
    _tree_node_free(x);
}

void test__insert_node() {
    printf("node insertion test\n");
    node* x = simple_balanced();
    node* n = node_new(5);
    x = _insert_node(x, n);
    assert(x->d == 3 && x->b == 0 && x->l != NULL && x->r != NULL);
    assert(x->l->d == 1 && x->l->p == x && x->l->b == 0);
    assert(x->r->d == 4 && x->r->p == x && x->r->b == 1 && x->r->l == NULL);
    assert(x->l->l->d == 0 && x->l->l->b == 0 && x->l->l->p == x->l && x->l->l->l == NULL && x->l->l->r == NULL);
    assert(x->l->r->d == 2 && x->l->r->b == 0 && x->l->r->p == x->l && x->l->r->l == NULL && x->l->r->r == NULL);
    assert(x->r->r->d == 5 && x->r->r->b == 0 && x->r->r->p == x->r && x->r->r->l == NULL && x->r->r->r == NULL);
    node_check_tree(x);
    _tree_node_free(x);

    x = node_new(0);
    node* t;
    for (long i = 0; i <= 50; i++) {
        t = _insert_node(x, node_new(i));
        if (t->p == NULL) x = t;
    }
    node_check_tree(x);
    _tree_node_free(x);
}

long* make_nums(int cnt) {
    long* nums = malloc(sizeof(long) * (cnt+2));
    for (long i = 0; i < cnt; i++) {
        nums[(int) i] = i;
    }
    nums[cnt - 1] = LONG_MAX;
    nums[cnt] = LONG_MIN;
    return nums;
}
 
tree* create_big_tree(long* nums, int cnt) {
    tree* t = tree_new();
    for (int i = 0; i <= cnt; i++) {
        tree_insert(t, nums[i]);
    }
    for (int i = 0; i <= cnt; i++) {
        assert(tree_search(t, nums[i])->d == nums[i]);
    }
    node_check_tree(_get_root(t));
    return t;
}

void test_big_tree() {
    printf("testing big tree\n");
    int cnt = 5000;
    long* nums = make_nums(cnt);
    tree* t = create_big_tree(nums, cnt);
    check_bf(_get_root(t));
    node_check_tree(_get_root(t));
    free(nums);
    _tree_free(t);
}

void test_lopsided() {
    printf("lopsided tree test\n");
    node* x = right_left_heavy();
    x = _right_left(x);
    node* y = lopsided_balanced();
    assert(node_compare_recurse(x, y));
    node_check_tree(x);
    node_check_tree(y);
    _tree_node_free(x);
    _tree_node_free(y);

    x = left_right_heavy();
    x = _left_right(x);
    y = lopsided_balanced();
    assert(node_compare_recurse(x, y));
    node_check_tree(x);
    node_check_tree(y);
    _tree_node_free(x);
    _tree_node_free(y);
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
    node_check_tree(_get_root(t));
    _tree_free(t);
}

void test__remove_no_right_children() {
    printf("testing _remove_no_right_children\n");
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    node* n2 = node_new(2);
    node* n3 = node_new(3);

    n1->l = n0;
    n0->p = n1;
    n1->r = n3;
    n3->p = n1;
    n3->l = n2;
    n2->p = n3;
    n3->b = -1;
    n1->b = 1;
    node_check_tree(n1);

    tree* t = tree_new();
    tree_insert(t, 1);
    tree_insert(t, 0);
    tree_insert(t, 2);
    node_check_tree(_get_root(t));

    _remove_no_right_children(n3);
    node* x = _get_root(t);
    node_check_tree(n1);
    assert(node_compare_recurse(n1, x));
    _tree_node_free(n1);
    _tree_free(t);
}

void test__remove_right_no_left() {
    printf("testing _remove_right_no_left\n");
    node* n_1 = node_new(-1);
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    node* n2 = node_new(2);
    node* n3 = node_new(3);
    node* n4 = node_new(4);
    node* n5 = node_new(5);

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
    node_check_tree(n1);
   
    tree* t = tree_new();
    tree_insert(t, 1);
    tree_insert(t, 0);
    tree_insert(t, 2);
    tree_insert(t, 3);
    tree_insert(t, -1);
    tree_insert(t, 5);
    node_check_tree(_get_root(t));

    _remove_right_no_left(n4);
    node_check_tree(n1);
    assert(node_compare_recurse(n1, _get_root(t)));
    _tree_node_free(n1);
    _tree_free(t);
}

struct node_con {
    node* root;
    node* removal;
};

struct node_con complex_tree() {
    node* n_4 = node_new(-4);
    node* n_3 = node_new(-3);
    node* n_2 = node_new(-2);
    node* n_1 = node_new(-1);
    node* n0 = node_new(0);
    node* n1 = node_new(1);
    node* n2 = node_new(2);
    node* n3 = node_new(3);
    node* n4 = node_new(4);
    node* n5 = node_new(5);
    node* n6 = node_new(6);
    node* n7 = node_new(7);
    node* n8 = node_new(8);

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
    node_check_tree(n0);
    return con;
}

void test__remove_complex() {
    printf("testing _remove_complex\n");
    struct node_con con = complex_tree();
    node* n = con.root;
    node* r = con.removal;
    _remove_complex(r);
    
    assert(n->d == 0);
    assert(n->r->d == 4); // remove
    assert(n->r->r->d == 7); // rebalance
    node_check_tree(n);
    _tree_node_free(n);

    tree* t = tree_new();
    for (long i = 0; i <= 50; i++) {
        tree_insert(t, i);
    }
    node* d = tree_search(t, 39);
    _remove_complex(d);
    node_check_tree(_get_root(t));
    _tree_free(t);
}

void test_many_internal(int cnt) {
    long* nums = make_nums(cnt);

    // test even removal
    tree* t = create_big_tree(nums, cnt);
    node_check_tree(_get_root(t));
    for (long i = 0; i <= cnt - 2; i += 2) {
        assert(tree_remove(t, i));
        node_check_tree(_get_root(t));
    }
    _tree_free(t);

    // test odd removal
    t = create_big_tree(nums, cnt);
    node_check_tree(_get_root(t));
    for (long i = 1; i <= cnt - 2; i += 2) {
        assert(tree_remove(t, i));
        node_check_tree(_get_root(t));
    }
    _tree_free(t);
    free(nums);
}

void test_many() {
    printf("testing many inserts/deletes\n");
    for (int i = 1; i <= 999; i++) {
        test_many_internal(i);
    }
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

    test_many();

    return 0;
}
