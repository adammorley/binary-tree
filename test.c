#include <assert.h>
#include <stdlib.h>
#include "tree.h"

int main() {
    node* t = node_new(5);
    assert(t->d == 5);
    assert(NULL == tree_remove(t, 5));
    t = node_new(5);
    tree_insert(t, 4);
    tree_insert(t, 3);
    tree_insert(t, 2);
    assert(t->b == -3); // not balanced
    assert(tree_search(t, 3)->d == 3);
    assert(t == tree_remove(t, 3));
    assert(NULL == tree_search(t, 3));
    _tree_free_N(t);
    return 0;
}
