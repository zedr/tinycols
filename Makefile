all: test game

test:
	@cc -g -DLINUX_TARGET -Wall -pedantic core.c tests/*.c -o test

game:
	@cc -g -DLINUX_TARGET -lncurses -std=gnu99 -Wall -pedantic *.c -o game

clean:
	@rm -f game test
