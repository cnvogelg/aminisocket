# Makefile for parblk.device
BASE_DIR=../../..
include $(BASE_DIR)/common.mk

TARGET=parblk.lib
SRCS=crc16.asm par.c parblk.c pario.asm timer.c pkt.c
HDRS=par.h parblk.h timer.h pkt.h
    
all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(VAMOS_RUN) smake $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f *.o
	
install: all
