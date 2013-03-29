# Makefile for parblk.device
BASE_DIR=../../..
include $(BASE_DIR)/common.mk

TARGET=test_device
SRCS=test_device.c
HDRS=
    
all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(VAMOS_RUN) smake $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f *.o
	
install: all
	cp $(TARGET) $(INSTALL_DIR)/
