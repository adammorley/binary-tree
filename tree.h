#include <stdbool.h>
#include "../tree-node/node.h"

#ifndef TREE_H
#define TREE_H

typedef struct tree tree;
struct tree {
    node* r;
};

/*
    insert a node or data into a tree
*/
void tree_insert(tree* t, long d);

/*
    create a tree
*/
tree* tree_new();

/*
    remove a value, returns bool indicating whether removed
*/
bool tree_remove(tree* t, long d);

/*
    search for a value, returns count of times value stored
*/
node* tree_search(tree* t, long d);

/*
    free all the nodes in the tree
*/
void _tree_free_N(tree* t);

#ifdef _UNIT_TEST
#define STATIC
#else
#define STATIC static
#endif

#ifdef _UNIT_TEST
void _insert_update_b(node* node);
void _remove_update_b(node* node);
void _remove_no_right_children(node* node);
void _remove_right_no_left(node* node);
void _remove_complex(node* node);
#endif

#endif //TREE_H
