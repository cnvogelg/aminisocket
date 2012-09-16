/* drv.h - driver interface for socket server */

#ifndef DRV_H
#define DRV_H

#define DRV_OK    0
#define DRV_ERR   -1

extern int drv_init(void);
extern void drv_shutdown(void);

#endif
