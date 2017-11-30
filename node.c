#include <stdlib.h>
#include "node.h"

// returns ownership
node* node_new(long d) {
    node* n = malloc(sizeof(node));
    n->l = NULL;
    n->r = NULL;
    n->p = NULL;
    n->d = d;
    n->b = 0;
    return n;
}
