#ifndef TIMER_H
#define TIMER_H

#include <exec/types.h>

extern int timer_init(void);
extern void timer_shutdown(void);

extern void timer_timeout_set_flag_ptr(UBYTE *flag);
extern void timer_timeout_start(ULONG timeout);
extern void timer_timeout_clear(void);

extern ULONG timer_tick_mask(void);
extern void timer_tick_start(ULONG timeout);
extern void timer_tick_clear(void);

#endif
