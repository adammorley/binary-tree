#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "../tree-node/tree_node.h"

#include "tree.h"

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
    tree_node_check(_get_root(t));
    return t;
}
