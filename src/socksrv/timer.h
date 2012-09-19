#ifndef TIMER_H
#define TIMER_H

#include <exec/types.h>

extern int timer_init(void);
extern void timer_shutdown(void);
extern volatile UBYTE *timer_start(ULONG timeout);
extern void timer_clear(void);

#endif
