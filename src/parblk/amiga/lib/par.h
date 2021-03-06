#ifndef PAR_H
#define PAR_H

#include <exec/types.h>

struct state_s {
    UWORD     s_Flags;
    ULONG     s_IntSigMask;
    struct Library * s_SysBase;
    struct Task    * s_ServerTask;
    UBYTE     s_UseCRC;
    UBYTE     s_TimeOut;
};

#define FLAGS_BIT_RX        1
#define FLAGS_BIT_ACK       2

#define FLAGS_MASK_RX       (1<<FLAGS_BIT_RX)
#define FLAGS_MASK_ACK      (1<<FLAGS_BIT_ACK)

struct par_packet_s {
    /* this is the header sent on wire */
    UWORD     p_Sync;       /* set by pario: 0x4201 or 0x4202 = NOCRC | CRC */
    UWORD     p_Size;       /* user: size to be sent or size that was read */
    UWORD     p_CRC;        /* set by pario: CRC16 or 0 if disabled */
    /* this additional info is only local */
    UWORD     p_BufferSize; /* user: max size to be filled by read */
    UBYTE *   p_Buffer;     /* user: data buffer to be transferred */
    int       p_Result;     /* returned: transfer result */
};

typedef struct par_packet_s par_packet_t;

/* p_Result values */
#define PAR_OK                          0
#define PAR_ERR_TIMEOUT                 -1  
#define PAR_ERR_COLLISION               -2
#define PAR_ERR_NO_LINE                 -3
#define PAR_ERR_BUFFER_TOO_SMALL        -4
#define PAR_ERR_CRC                     -5

/* init result */
#define PAR_INIT_OK             0
#define PAR_INIT_NO_SIGNAL      1
#define PAR_INIT_NO_PARPORT_RES 2
#define PAR_INIT_NO_PARBITS_RES 3
#define PAR_INIT_NO_CIA_IRQ     4

extern int par_init(UBYTE useCRC);
extern void par_shutdown(void);
extern ULONG par_get_rx_sig_mask(void);
extern int par_send(struct par_packet_s *pkt);
extern int par_recv(struct par_packet_s *pkt);

#endif
