CFLAGS=-std=c11 -Wall -Wextra -Wno-unused-parameter -Og -g

SRC=$(wildcard *.c)
OUT=$(patsubst %.c,%,$(SRC))

.PHONY: all
all: $(OUT)

.PHONY: clean
clean:
	rm -f *.o $(OUT)
