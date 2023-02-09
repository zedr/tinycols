.PHONY: default all test clean

CC 		?= gcc

SHELL 	= /bin/bash
CFLAGS 	= -g -DLINUX_TARGET -Wall -Werror -pedantic -std=c99

default: build/tinycols

all: default test

build:
	@mkdir build

build/test: build
	@${CC} ${CFLAGS} \
		-D_POSIX_C_SOURCE=199309L \
		lib/*.c tests/test_tinycols.c src/tinycols.c -o build/test

build/tinycols: build
	@${CC} ${CFLAGS} -std=gnu99 -lncurses -o build/tinycols \
		src/gfx.c src/tinycols.c src/main.c

test: build/test

clean:
	@rm -rf build
