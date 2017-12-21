#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../log/log.h"
#include "../tree-node/tree_node.h"

#include "tree.h"
#include "test-support.h"

void test_many_loop(int start, int check, int cnt, long* nums) {
    tree* t = create_big_tree(nums, cnt);
    tree_node_check(_get_root(t));
    for (long i = start; i <= cnt - 2; i += 2) {
        assert(tree_remove(t, i));
        tree_node_check(_get_root(t));
    }
    for (long i = check; i <= cnt - 2; i += 2) {
        assert(tree_search(t, i));
    }
    assert(tree_search(t, LONG_MIN));
    assert(tree_search(t, LONG_MAX));
    _tree_free(t);
}

void test_many_internal(int cnt) {
    long* nums = make_nums(cnt);

    // test even and odd removal
    test_many_loop(0, 1, cnt, nums);
    test_many_loop(1, 0, cnt, nums);
    free(nums);
}

void test_many() {
    printf("testing many inserts/deletes\n");
    for (int i = 1; i <= 999; i++) {
        test_many_internal(i);
    }
}

int main() {
    test_many();
    return 0;
}
