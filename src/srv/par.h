#ifndef PAR_H
#define PAR_H

#include <exec/types.h>

struct state_s {
    UWORD     s_Flags;
    ULONG     s_IntSigMask;
    struct Library * s_SysBase;
    struct Task    * s_ServerTask;
    UWORD     s_MaxPacketSize;
    UBYTE     s_UseCRC;
    UBYTE     s_TimeOut;
};

#define FLAGS_BIT_RX        1
#define FLAGS_BIT_ACK       2

#define FLAGS_MASK_RX       (1<<FLAGS_BIT_RX)
#define FLAGS_MASK_ACK      (1<<FLAGS_BIT_ACK)

struct packet_s {
    UWORD     p_Sync;
    UWORD     p_Size;
    UWORD     p_CRC;
};

extern int par_init(UWORD maxPacketSize, UBYTE useCRC);
extern void par_shutdown(void);
extern ULONG par_get_rx_sig_mask(void);
extern int par_send(struct packet_s *pkt);
extern int par_recv(struct packet_s *pkt);

#endif
