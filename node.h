#ifndef NODE_H
#define NODE_H

typedef struct node node;
struct node {
    node* l;
    node* r;
    node* p;
    long d;
};

/*
    create a node, return ownership
*/
node* node_new(long d);

#endif //NODE_H
