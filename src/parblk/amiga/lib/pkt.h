#ifndef PKT_H
#define PKT_H

#include <exec/types.h>
#include <exec/lists.h>
#include "par.h"

struct packet_s;
typedef void (*pkt_done_func_t)(struct packet_s *pkt);

/* packet with node */
struct packet_s {
  struct Node p_Node;
  struct par_packet_s p_ParPacket;
  APTR p_OwnBuffer;
  APTR p_UserData;
  pkt_done_func_t p_DoneFunc;
  UWORD p_Retries;
  UWORD p_OwnBufferSize;
};
typedef struct packet_s packet_t;

extern void pkt_setup(int tx_size, int rx_size, UWORD max_pkt);
extern BOOL pkt_init(void);
extern void pkt_free(void);

extern packet_t *pkt_tx_alloc(void);
extern void pkt_tx_release(packet_t *p);

extern packet_t *pkt_rx_alloc(void);
extern void pkt_rx_release(packet_t *p);

#endif
