#ifndef DEVOPS_H
#define DEVOPS_H

#include <exec/types.h>

extern BOOL dev_init(void);
extern void dev_shutdown(void);

extern long dev_read(APTR buffer, long size);
extern long dev_write(APTR buffer, long size);

#endif
