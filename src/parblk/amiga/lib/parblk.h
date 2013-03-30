#ifndef PARBLK_H
#define PARBLK_H

#include <exec/types.h>
#include "pkt.h"

/* init result: add those of PKT_INIT_* */
#define PARBLK_INIT_OK           0
#define PARBLK_INIT_NO_TIMER     10
#define PARBLK_INIT_NO_PKT       11

extern int parblk_init(void);
extern void parblk_shutdown(void);

extern ULONG parblk_get_sig_mask(void);
extern BOOL parblk_handle_sig(ULONG sigmask);

extern packet_t *parblk_read(APTR buffer, UWORD size, pkt_done_func_t func);
extern packet_t *parblk_write(APTR buffer, UWORD size, UWORD flags, pkt_done_func_t func);

#endif
