#ifndef NODE_H
#define NODE_H

typedef struct node node;
struct node {
    node* l; // left
    node* r; // right
    node* p; // parent
    long d; // number
    /*
        balance factor
        height right - height left
    */
    unsigned long b;
};

/*
    create a node, return ownership
*/
node* node_new(long d);

#endif //NODE_H
