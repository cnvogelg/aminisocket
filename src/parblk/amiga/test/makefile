# Makefile for parblk.device
BASE_DIR=../../..
include $(BASE_DIR)/common.mk

TARGET=test_device test_parblk
SRCS=test_device.c test_parblk.c
LIBS=../lib/parblk.lib 
    
all: $(TARGET)

$(TARGET): $(SRCS) $(LIBS)
	$(VAMOS_RUN) smake $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f *.o
	
install: all
	cp $(TARGET) $(INSTALL_DIR)/
