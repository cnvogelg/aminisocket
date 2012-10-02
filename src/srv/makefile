# Makefile for aminisocket
BASE_DIR=..
include $(BASE_DIR)/common.mk

TARGET=socksrv
SRCS=socksrv.c pardrv.c par.c pario.asm crc16.asm timer.c msgio.c
HDRS=timer.h drv.h par.h msgio.h
    
all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(VAMOS_RUN) smake $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f *.o
	
install: all
	cp $(TARGET) $(INSTALL_DIR)/
	cp run_socksrv $(INSTALL_DIR)/
