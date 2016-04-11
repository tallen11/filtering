CC=clang
DEBUG=-g
CFLAGS=-Wall -Wextra -pedantic -std=c11 -lportaudio -lfftw3 -lm $(DEBUG)

all: voice.c
	$(CC) voice.c -o bin/voice $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf bin/voice
	rm -rf *.dSYM