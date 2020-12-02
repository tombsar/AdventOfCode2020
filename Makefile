CFLAGS=-std=c11 -Wall -Wextra -Wno-unused-parameter -march=native

SRC=$(wildcard *.c)
OUT=$(patsubst %.c,%,$(SRC))

.PHONY: debug
debug: CFLAGS+=-Og -g
debug: all

.PHONY: fast
fast: CFLAGS+=-DNDEBUG -O2
fast: all

.PHONY: all
all: $(OUT)

.PHONY: clean
clean:
	rm -f *.o $(OUT)
