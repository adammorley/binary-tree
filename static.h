#include "../tree-node/tree_node.h"

#ifndef TREE_STATIC_H
#define TREE_STATIC_H
#ifdef _UNIT_TEST
#define STATIC
#else
#define STATIC static
#endif

#ifdef _UNIT_TEST
STATIC void Assert(bool condition, const char* func, const char* format, ...);

STATIC tree_node* _get_root(tree* t);
STATIC tree_node* _up_to_root(tree_node* n);

STATIC tree_node* _insert_node(tree_node* n, tree_node* c);

STATIC tree_node* _retrace_insert(tree_node* n);
STATIC void _update_bf_insert(tree_node* p, tree_node* c);

STATIC tree_node* _rebalance(tree_node* n);

STATIC tree_node* _retrace_remove(tree_node* n);

STATIC void _remove_splice(tree_node* n, tree_node* c, tree_node* r);
STATIC tree_node* _remove_no_children(tree_node* n);
STATIC tree_node* _remove_no_right_children(tree_node* n);
STATIC tree_node* _remove_right_no_left(tree_node* n);
STATIC tree_node* _remove_complex(tree_node* n);

STATIC tree_node* _right_right(tree_node* X);
STATIC tree_node* _right_left(tree_node* X);
STATIC tree_node* _left_left(tree_node* X);
STATIC tree_node* _left_right(tree_node* X);
#endif // _UNIT_TEST

#endif //TREE_STATIC_H
