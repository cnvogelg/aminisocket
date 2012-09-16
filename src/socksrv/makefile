# Makefile for aminisocket
BASE_DIR=..
include $(BASE_DIR)/common.mk

TARGET=socksrv
SRCS=socksrv.c pardrv.c parirq.asm
HDRS=
    
all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(VAMOS_RUN) smake $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f *.o
	
install: all
	cp $(TARGET) $(INSTALL_DIR)/
