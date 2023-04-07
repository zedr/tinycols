.PHONY: default all test clean

CC 			?= gcc

SHELL 		= /bin/bash
CFLAGS 		= -g -DLINUX_TARGET -Wall -Werror -pedantic -std=c99
CSOURCES 	= src/tinycols.c src/queue.c

default: build/tinycols

all: default test

build:
	@mkdir build

build/test_tinycols: build
	@${CC} ${CFLAGS} \
		-D_POSIX_C_SOURCE=199309L \
		-o build/test_tinycols \
		lib/*.c tests/test_tinycols.c ${CSOURCES}

build/test_queue: build
	@${CC} ${CFLAGS} \
		-D_POSIX_C_SOURCE=199309L \
		-o build/test_queue \
		lib/*.c tests/test_queue.c ${CSOURCES}

build/tinycols: build
	@${CC} ${CFLAGS} -std=gnu99 -lncurses \
		-o build/tinycols \
		src/main.c src/gfx.c ${CSOURCES}

test: build/test_tinycols build/test_queue

clean:
	@rm -rf build
