.PHONY: default all test debug clean rpm

CC 		?= gcc
STD		:= c99
SHELL 		:= /usr/bin/env bash
SOURCES 	:= src/tinycols/*.c src/queue.c
INCDIR		:= include/
INCLUDES	= $(shell find $(INCDIR) -type f -name *.h)
INC		:= -I ${INCLUDES}
COMPILE		:= -DLINUX_TARGET -Wall -pedantic -std=${STD} -fPIE
CFLAGS 		:= ${COMPILE}
DBG_CFLAGS	:= -g -p -Werror -fsanitize=address -fsanitize=undefined

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
		-pie \
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

NAME		:= tinycols
VERSION		:= $(shell grep -oP '(?<=TINYCOLS_VERSION ")[\d.]+' src/main.c)
RPM_TOPDIR	:= $(CURDIR)/build/rpm

version:
	@echo $(NAME)-$(VERSION)

rpm: build/$(NAME)-$(VERSION).tar.gz
	@sed -i "s/^Version:.*/Version:        $(VERSION)/" $(NAME).spec
	@cp build/$(NAME)-$(VERSION).tar.gz .
	@mkdir -p $(RPM_TOPDIR)/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
	@cp build/$(NAME)-$(VERSION).tar.gz $(RPM_TOPDIR)/SOURCES/
	rpmbuild -ba \
		--define "_topdir $(RPM_TOPDIR)" \
		$(NAME).spec

build/$(NAME)-$(VERSION).tar.gz: build
	git archive --format=tar.gz --prefix=$(NAME)-$(VERSION)/ HEAD \
		-o build/$(NAME)-$(VERSION).tar.gz
