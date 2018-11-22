CC=gcc
cflags_test=-lSDL2 -o a.out

test:
	$(CC) sdl-test.c $(cflags_test)
	