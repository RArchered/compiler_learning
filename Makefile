CFLAGS=-std=c11 -g -static

9cc: 9cc.c
	cc -std=c11 -g -static 9cc.c -o ./build/9cc

test: 9cc
	./test/test.sh

clean:
	rm 9cc *.o *~ tmp*
	rm -r build

.PHONY: test clean