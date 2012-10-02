/* msgio.h - message I/O between server and lib */

#ifndef MSGIO_H
#define MSGIO_H

#include <exec/types.h>
#include "msg.h"

extern msg_t *msg;
extern int msgio_ready;

extern int msgio_init(void);
extern void msgio_shutdown(void);

extern int msgio_xfer_msg(msg_t *msg);

#endif
