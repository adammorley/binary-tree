#include <stdbool.h>
#include "../tree-node/tree_node.h"

#ifndef TREE_H
#define TREE_H

typedef struct tree tree;
struct tree {
    tree_node* r;
};

/*
    insert a data into a tree
*/
void tree_insert(tree* t, long d);

/*
    create a tree
*/
tree* tree_new();

/*
    print a tree
*/
void tree_print(tree* t);

/*
    remove a value, returns bool indicating whether removed
*/
bool tree_remove(tree* t, long d);

/*
    search for a value, returns the node
*/
tree_node* tree_search(tree* t, long d);

/*
    free all the nodes in the tree, and the tree
*/
void _tree_free(tree* t);

#include "static.h"

#endif //TREE_H
