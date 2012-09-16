# Makefile for aminisocket
BASE_DIR=..
include $(BASE_DIR)/common.mk

TARGET=test_sock
SRCS=test_sock.c
HDRS=
    
all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(VAMOS_RUN) smake $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f *.o
	
install: all
	cp test_sock $(INSTALL_DIR)/
