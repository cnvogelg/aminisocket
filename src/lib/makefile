# Makefile for socklib of aminisocket
BASE_DIR=..
include $(BASE_DIR)/common.mk

TARGET=bsdsocket.library
SRCS=bsdsocket.c mininetdb.c msgio.c
HDRS=mininetdb.h msgio.h

all: $(TARGET)
    
$(TARGET): $(SRCS) $(HDRS)
	$(VAMOS_RUN) smake $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f *.o
	
install: all
	cp bsdsocket.library $(INSTALL_DIR)/
