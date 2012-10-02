/* msgio.h - message I/O between server and lib */

#ifndef MSGIO_H
#define MSGIO_H

#include <exec/types.h>
#include "msg.h"

extern int msgio_init(void);
extern void msgio_shutdown(void);

extern ULONG msgio_port_sig_mask(void);
extern msg_t *msgio_get_msg(void);
extern void msgio_reply_msg(msg_t *msg);

#endif
