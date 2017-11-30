CC=/usr/local/Cellar/gcc/7.2.0/bin/gcc-7
CFLAGS=-I. -g

all: node-test tree-test

clean:
	rm -f node-test tree-test

node-test: node-test.c node.c
	$(CC) -o node-test node-test.c node.c $(CFLAGS)

tree-test: tree-test.c tree.c node.c
	$(CC) -o tree-test tree-test.c tree.c node.c $(CFLAGS) -D_UNIT_TEST=1
