CFLAGS=-g -O2
LDFLAGS=
gx-load: gx-load.o
	gcc $(LDFLAGS) -o $@ $^
%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^
clean:
	rm -f gx-load gx-load.o
