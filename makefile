# Makefile for aminisocket

all: bsdsocket.library test_sock

SRCS=bsdsocket.c mininetdb.c
HDRS=mininetdb.h
    
bsdsocket.library: $(SRCS) $(HDRS)
	vamos smake bsdsocket.library

test_sock: test_sock.c
	vamos smake test_sock

clean:
	rm -f bsdsocket.library
	rm -f test_sock
	rm -f *.o
	
install: all
	cp test_sock bsdsocket.library shared/
