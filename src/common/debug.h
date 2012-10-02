/*
 * debug.h - debug macros
 */
#ifndef DEBUG_H
#define DEBUG_H

/* master switch to enable debugging */
#define DEBUG  1

#ifdef DEBUG

/* Prototypes for Delay, kprintf, dprintf. Or use proto/dos.h or functions.h. */
#include <clib/dos_protos.h>
void kprintf(UBYTE *fmt,...);
#define bug kprintf

#define D(x)  (x)

#else
 
#define D(x)

#endif
