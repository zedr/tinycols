.PHONY: default all test clean

CC 			?= gcc
STD			:= c99

SHELL 		:= /bin/bash
SOURCES 	:= src/tinycols/*.c src/queue.c
INCDIR		:= include/
INCLUDES	= $(shell find $(INCDIR) -type f -name *.h)
INC			:= -I ${INCLUDES}
COMPILE		:= -g -DLINUX_TARGET -Wall -Werror -pedantic -std=${STD}
CFLAGS 		:= ${COMPILE}

default: build/tinycols

all: default test

build:
	@mkdir build

build/test_tinycols: build
	@${CC} ${CFLAGS} \
		-D_POSIX_C_SOURCE=199309L \
		-o build/test_tinycols \
		lib/*.c tests/test_tinycols.c ${SOURCES}

build/test_queue: build
	@${CC} ${CFLAGS} \
		-D_POSIX_C_SOURCE=199309L \
		-o build/test_queue \
		lib/*.c src/queue.c src/tinycols/*.c tests/test_queue.c

build/tinycols: build
	@${CC} ${CFLAGS} -std=gnu99 -lncurses \
		-o build/tinycols \
		src/main.c src/gfx.c ${SOURCES}

tests: build/test_tinycols build/test_queue

test:
	@build/test_tinycols
	@build/test_queue

clean:
	@rm -rf build
