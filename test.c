#include <assert.h>
#include <stdlib.h>
#include "tree.h"

int main() {
    tree* t = tree_new();
    tree_insert(t, 5);
    assert(t->r->d == 5);
    assert(tree_remove(t, 5));
    tree_insert(t, 5);
    tree_insert(t, 4);
    tree_insert(t, 3);
    tree_insert(t, 2);
    assert(t->r->b == -3); // not balanced
    assert(tree_search(t, 3)->d == 3);
    assert(tree_remove(t, 3));
    assert(NULL == tree_search(t, 3));
    _tree_free_N(t);
    return 0;
}
