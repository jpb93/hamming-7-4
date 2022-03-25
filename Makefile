CC=gcc
CFLAGS=-Wall -O2

all: raid diar test

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f a.out *.part? *.2 *._* *.exe *.--*