#include <assert.h>
#include <stdlib.h>
#include "tree.h"

int main() {
    node* t = node_new(5);
    assert(t->d == 5);
    tree_insert(t, 4);
    tree_insert(t, 3);
    tree_insert(t, 2);
    assert(t->b == -3); // not balanced
    _tree_free_N(t);




    return 0;
}
