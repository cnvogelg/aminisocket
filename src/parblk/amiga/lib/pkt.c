#include "pkt.h"

#include <exec/semaphores.h>
#include <exec/memory.h>
#include <proto/exec.h>

static UWORD max_pkt_size = 1024;
static int tx_pkt_size = 8;
static int rx_pkt_size = 8;

/* free and used list of rx/tx queue */
static struct List tx_free;
static struct List rx_free;
static struct SignalSemaphore tx_sem;
static struct SignalSemaphore rx_sem;

/* allocated memory for packets and rx buffers */
static ULONG pkts_size;
static packet_t *pkts;
static ULONG rx_pkt_data_size;
static UBYTE *rx_pkt_data;

void pkt_setup(int tx_size, int rx_size, UWORD max_pkt)
{
    tx_pkt_size = tx_size;
    rx_pkt_size = rx_size;
    max_pkt_size = max_pkt;       
}

BOOL pkt_init(void)
{
    packet_t *p;
    int i;
    UBYTE *pkt_data;
    
    /* allocate packets with nodes for tx and rx queue */
    int totalPkt = tx_pkt_size + rx_pkt_size;
    pkts_size = totalPkt * sizeof(packet_t);
    pkts = (packet_t *)AllocMem(pkts_size, MEMF_CLEAR);
    if(pkts == NULL) {
        return FALSE;
    }
    
    /* allocate packet data for rx packets */
    rx_pkt_data_size = rx_pkt_size * max_pkt_size;
    rx_pkt_data = (UBYTE *)AllocMem(rx_pkt_data_size, MEMF_CLEAR);
    if(rx_pkt_data == NULL) {
        return FALSE;
    }
    
    NewList(&tx_free);
    NewList(&rx_free);
    InitSemaphore(&tx_sem);
    InitSemaphore(&rx_sem);
    
    /* init tx queue packets and insert them into free list */
    p = pkts;
    for(i=0;i<tx_pkt_size;i++) {
        AddHead(&tx_free, &p->p_Node);
        p++;
    }
    
    /* init rx queue packets and insert them into free list 
       store the pre-allocated buffer as 
    */
    pkt_data = rx_pkt_data;
    for(i=0;i<rx_pkt_size;i++) {
        AddHead(&rx_free, &p->p_Node);
        p->p_OwnBuffer = pkt_data;
        p->p_OwnBufferSize = max_pkt_size;
        pkt_data += max_pkt_size;
        p++;
    }
    
    return TRUE;
}

void pkt_free(void)
{
    if(pkts != NULL) {
        FreeMem(pkts, pkts_size);
    }
    if(rx_pkt_data != NULL) {
        FreeMem(rx_pkt_data, rx_pkt_data_size);
    }
}

void reset_pkt(packet_t *p)
{
    p->p_DoneFunc = NULL;
    p->p_Retries = 0;
    p->p_ParPacket.p_Buffer = NULL;
}

packet_t *pkt_tx_alloc(void)
{
    packet_t *p = NULL;
    
    ObtainSemaphore(&tx_sem);
    if(!IsListEmpty(&tx_free)) {
        p = (packet_t *)RemHead(&tx_free);
    }
    ReleaseSemaphore(&tx_sem);

    if(p!=NULL) {
        reset_pkt(p);
    }
    return p;
}

void pkt_tx_release(packet_t *np)
{
    ObtainSemaphore(&tx_sem);
    AddTail(&tx_free, &np->p_Node);
    ReleaseSemaphore(&tx_sem);
}

packet_t *pkt_rx_alloc(void)
{
    packet_t *p = NULL;
    
    ObtainSemaphore(&rx_sem);
    if(!IsListEmpty(&rx_free)) {
        p = (packet_t *)RemHead(&rx_free);
    }
    ReleaseSemaphore(&rx_sem);

    if(p!=NULL) {
        reset_pkt(p);
    }
    return p;    
}

void pkt_rx_release(packet_t *np)
{
    ObtainSemaphore(&rx_sem);
    AddTail(&rx_free, &np->p_Node);
    ReleaseSemaphore(&rx_sem);
}
