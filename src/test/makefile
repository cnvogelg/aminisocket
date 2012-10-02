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
	cp $(TARGET) $(INSTALL_DIR)/
	cp run_test $(INSTALL_DIR)/
