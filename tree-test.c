#include <assert.h>
#include <stdlib.h>
#include "tree.h"

int main() {
    node* t = node_new(5);
    assert(t->d == 5);
    return 0;
}
