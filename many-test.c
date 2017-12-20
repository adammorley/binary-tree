#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../log/log.h"
#include "../tree-node/node.h"

#include "tree.h"

#include "test-support.h"

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
    test_many();
    return 0;
}
