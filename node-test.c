#include <assert.h>
#include <stdlib.h>
#include "node.h"

int main() {
    node* n0 = node_new(5);
    assert(sizeof(node) == sizeof(n0->l) + sizeof(n0->r) + sizeof(n0->p) + sizeof(n0->d) + sizeof(n0->b));
    assert(n0->b == 0);
    assert(n0->d == 5);
    assert(n0->l == NULL);
    assert(n0->r == NULL);

    node* n1 = node_new(6);
    n0->r = n1;
    assert(n0->r->d == 6);
    free(n0);
    free(n1);
}
