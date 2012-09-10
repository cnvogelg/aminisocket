# Makefile for aminisocket

all: bsdsocket.library test_sock
    
bsdsocket.library: bsdsocket.c
	vamos smake bsdsocket.library

test_sock: test_sock.c
	vamos smake test_sock

clean:
	rm -f bsdsocket.library
	rm -f test_sock
	rm -f *.o
