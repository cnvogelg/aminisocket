/* drv.h - driver interface for socket server */

#ifndef DRV_H
#define DRV_H

#include <exec/types.h>

#define DRV_OK    0
#define DRV_ERR   -1

extern int drv_init(void);
extern void drv_shutdown(void);
extern ULONG drv_get_rx_sig_mask(void);
extern void drv_handle_rx(void);

#endif
