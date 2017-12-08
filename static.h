#include "../tree-node/node.h"

#ifndef TREE_STATIC_H
#define TREE_STATIC_H
#ifdef _UNIT_TEST
#define STATIC
#else
#define STATIC static
#endif

#ifdef _UNIT_TEST
STATIC node* _insert_node(node* n, node* c);
STATIC node* _insert_rebalance(node* n);
STATIC node* _rebalance(node* n);
STATIC void _remove_update_b(node* n);
STATIC void _remove_no_right_children(node* n);
STATIC void _remove_right_no_left(node* n);
STATIC void _remove_complex(node* n);
STATIC node* _right_right(node* X);
STATIC node* _right_left(node* X);
STATIC node* _left_left(node* X);
STATIC node* _left_right(node* X);
#endif // _UNIT_TEST

#endif //TREE_STATIC_H
