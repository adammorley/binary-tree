#include "../tree-node/node.h"

#ifndef TREE_STATIC_H
#define TREE_STATIC_H
#ifdef _UNIT_TEST
#define STATIC
#else
#define STATIC static
#endif

#ifdef _UNIT_TEST
STATIC void Assert(bool condition, const char* func, const char* format, ...);

STATIC node* _get_root(tree* t);
STATIC node* _up_to_root(node* n);

STATIC node* _insert_node(node* n, node* c);

STATIC node* _retrace_insert(node* n);
STATIC void _update_bf_insert(node* p, node* c);

STATIC node* _rebalance(node* n);

STATIC node* _retrace_remove(node* n);

STATIC void _remove_splice(node* n, node* c, node* r);
STATIC node* _remove_no_children(node* n);
STATIC node* _remove_no_right_children(node* n);
STATIC node* _remove_right_no_left(node* n);
STATIC node* _remove_complex(node* n);

STATIC node* _right_right(node* X);
STATIC node* _right_left(node* X);
STATIC node* _left_left(node* X);
STATIC node* _left_right(node* X);

STATIC void _tree_node_free(node* n);
#endif // _UNIT_TEST

#endif //TREE_STATIC_H
