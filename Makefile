CC=/usr/local/Cellar/llvm/5.0.0/bin/clang
CFLAGS=-I. -g
DEPS=../log/log.c ../queue/queue.c ../tree-node/tree_node.c

all: many-test test

clean:
	rm -f many-test test *.o

test: test.c test-support.c tree.c $(DEPS)
	$(CC) -o test test.c test-support.c tree.c $(DEPS) $(CFLAGS) -D_UNIT_TEST=1

many-test: many-test.c test-support.c test.c tree.c $(DEPS)
	$(CC) -o many-test many-test.c test-support.c tree.c $(DEPS) $(CFLAGS) -D_UNIT_TEST=1
