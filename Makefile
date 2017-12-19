CC=/usr/local/Cellar/llvm/5.0.0/bin/clang
CFLAGS=-I. -g

all: test

clean:
	rm -f test *.o

test: test.c tree.c ../log/log.c ../tree-node/node.c
	$(CC) -o test test.c tree.c ../log/log.c ../tree-node/node.c $(CFLAGS) -D_UNIT_TEST=1
