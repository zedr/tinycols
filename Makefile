.PHONY: default all test debug clean

CC 		?= gcc
STD		:= c99
SHELL 		:= /usr/bin/env bash
SOURCES 	:= src/tinycols/*.c src/queue.c
INCDIR		:= include/
INCLUDES	= $(shell find $(INCDIR) -type f -name *.h)
INC		:= -I ${INCLUDES}
COMPILE		:= -DLINUX_TARGET -Wall -Werror -pedantic -std=${STD}
CFLAGS 		:= ${COMPILE}
DBG_CFLAGS	:= -g -p -fsanitize=address -fsanitize=undefined

default: build/tinycols

debug: build/tinycols-debug

all: default test

build:
	@mkdir build

build/test_tinycols: build
	@${CC} ${CFLAGS} ${DBG_CFLAGS} \
		-D_POSIX_C_SOURCE=199309L \
		-o build/test_tinycols \
		lib/*.c tests/test_tinycols.c ${SOURCES}

build/test_queue: build
	@${CC} ${CFLAGS} ${DBG_CFLAGS} \
		-D_POSIX_C_SOURCE=199309L \
		-o build/test_queue \
		lib/*.c src/queue.c src/tinycols/*.c tests/test_queue.c

build/tinycols: build
	@${CC} ${CFLAGS} \
		-std=gnu99 \
		-lncurses \
		-o $@ \
		src/main.c src/gfx.c ${SOURCES}

build/tinycols-dbg: build
	@${CC} ${CFLAGS} ${DBG_CFLAGS} \
		-std=gnu99 \
		-lncurses \
		-o $@ \
		src/main.c src/gfx.c ${SOURCES}

build/tinycols-debug: build
	@${CC} ${CFLAGS} ${DBG_CFLAGS} -std=gnu99 -lncurses \
		-o build/tinycols-debug \
		src/main.c src/gfx.c ${SOURCES}

tests: build/test_tinycols build/test_queue

test:
	@build/test_tinycols
	@build/test_queue

debug: build/tinycols-dbg

clean:
	@rm -rf build
