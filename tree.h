#include <stdbool.h>
#include "node.h"

#ifndef TREE_H
#define TREE_H

/*
    insert a node or data into a tree
*/
void tree_insert(node* n, long d);

/*
    remove a value
*/
node* tree_remove(node* n, long d);

/*
    search for a value, returns count of times value stored
*/
node* tree_search(node* n, long d);

#endif //TREE_H
