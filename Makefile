CC=/usr/local/Cellar/llvm/5.0.0/bin/clang
CFLAGS=-I. -g

all: many-test test

clean:
	rm -f many-test test *.o

test: test.c test-support.c tree.c ../log/log.c ../tree-node/tree_node.c
	$(CC) -o test test.c test-support.c tree.c ../log/log.c ../tree-node/tree_node.c $(CFLAGS) -D_UNIT_TEST=1

many-test: many-test.c test-support.c test.c tree.c ../log/log.c ../tree-node/tree_node.c
	$(CC) -o many-test many-test.c test-support.c tree.c ../log/log.c ../tree-node/tree_node.c $(CFLAGS) -D_UNIT_TEST=1
