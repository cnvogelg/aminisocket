#include "par.h"
#include "parblk.h"
#include "timer.h"
#include "pkt.h"

#include <exec/semaphores.h>
#include <proto/exec.h>
#include <string.h>

/* parameters */
static UWORD max_pkt_size = 1024;
static UWORD max_retries = 5;
static ULONG retry_timeout = 100000; /* in us */

static UWORD rx_err = 0;
static ULONG rx_bytes = 0;
static UWORD tx_err = 0;
static ULONG tx_bytes = 0;

static ULONG rx_sigmask;
static ULONG timer_sigmask;

static struct List tx_list;
static struct List rx_empty_list;
static struct List rx_filled_list;
static struct SignalSemaphore tx_sem;
static struct SignalSemaphore rx_empty_sem;
static struct SignalSemaphore rx_filled_sem;

int parblk_init(void)
{   
    BOOL ok;
    int result;
    
    /* setup parallel port */
    result = par_init(max_pkt_size, 0);
    if(result != PAR_INIT_OK) {
        return result;
    }
    
    /* setup timer */
    ok = timer_init();
    if(!ok) {
        return PARBLK_INIT_NO_TIMER;
    }
    
    /* setup mem */
    ok = pkt_init();
    if(!ok) {
        return PARBLK_INIT_NO_PKT;
    }
    
    rx_sigmask = par_get_rx_sig_mask();
    timer_sigmask = timer_tick_mask();
    
    NewList(&tx_list);
    NewList(&rx_empty_list);
    NewList(&rx_filled_list);
    InitSemaphore(&tx_sem);
    InitSemaphore(&rx_empty_sem);
    InitSemaphore(&rx_filled_sem);
    
    /* start ticker */
    timer_tick_start(retry_timeout);
    
    return PARBLK_INIT_OK;
}

void parblk_shutdown(void)
{
    timer_tick_clear();
    
    /* shutdown parallel port */
    par_shutdown();
    
    /* shutdown timer */
    timer_shutdown();
    
    /* free mem */
    pkt_free();
}

static void done_tx_packet(packet_t *p)
{
    /* call done func: must release packet! */
    pkt_done_func_t f = p->p_DoneFunc;
    if(f != NULL) {
        f(p);
    }
    /* otherwise release packet ourselves */
    else {
        pkt_tx_release(p);
    }
}

static BOOL handle_tx_queue(void)
{
    BOOL ok = TRUE;
    
    /* try to send queued packets */
    ObtainSemaphore(&tx_sem);
    while(!IsListEmpty(&tx_list)) {
        int error;
        
        /* get next packet */
        packet_t *p = (packet_t *)RemHead(&tx_list);
        
        /* try to send it */
        error = par_send(&p->p_ParPacket);
        
        /* packet ok */
        if(error == PAR_OK) {
            /* complete packet */
            done_tx_packet(p);
        }
        /* packet has error */
        else {
            p->p_Retries += 1;
            
            /* too many retries -> drop with error */
            if(p->p_Retries >= max_retries) {
                done_tx_packet(p);
            } 
            /* re-add packet for next retry */
            else {
                AddHead(&tx_list, &p->p_Node);
            }
            
            /* leave loop for now */
            ok = FALSE;
            break;
        }
    }
    ReleaseSemaphore(&tx_sem);
    
    return ok;
}

ULONG parblk_get_sig_mask(void)
{
    /* flag incoming packet and the timer tick */
    return rx_sigmask | timer_sigmask;
}

static packet_t *get_next_empty_rx_pkt(void)
{
    packet_t *p = NULL;
    
    /* check if rx_empty_list has a packet prepared for us */
    ObtainSemaphore(&rx_empty_sem);
    if(!IsListEmpty(&rx_empty_list)) {
        p = (packet_t *)RemHead(&rx_empty_list);
    }
    ReleaseSemaphore(&rx_empty_sem);
    
    /* if no packet is ready to be filled then use a new one */
    if(p == NULL) {
        p = pkt_rx_alloc();
    }
    
    return p;
}

BOOL parblk_handle_sig(ULONG sigmask)
{   
    /* an incoming packet is signalled */
    if(sigmask & rx_sigmask) {
        packet_t *p;
        int error;
        
        /* get next packet for incoming data */
        p = get_next_empty_rx_pkt();
        
        /* fatal: no packet to fill */
        if(p == NULL) {
            return FALSE;
        }
        
        /* make sure buffer is valid 
           use pre-allocated buffer if none is set
        */
        if(p->p_ParPacket.p_Buffer == NULL) {
            p->p_ParPacket.p_Buffer = p->p_OwnBuffer;
        }
        
        /* try to recevie from parallel */
        error = par_recv(&p->p_ParPacket);
        
        /* add to rx_filled_list */
        ObtainSemaphore(&rx_filled_sem);
        AddTail(&rx_filled_list, &p->p_Node);
        ReleaseSemaphore(&rx_filled_sem);
    }
    
    /* an incoming timer tick */
    if(sigmask & timer_sigmask) {
        /* try to send out packets */
        handle_tx_queue();
        
        /* start next */
        timer_tick_start(retry_timeout);
    }
       
    return TRUE;
}

packet_t *parblk_write(APTR buffer, UWORD size, UWORD flags, pkt_done_func_t func)
{
    packet_t *p;
    BOOL ok;
    
    /* allocate a new tx packet */
    p = pkt_tx_alloc();
    if(p == NULL) {
        /* fatal: no packet to allocate */
        return NULL;
    }
    
    /* prepare packet */
    p->p_ParPacket.p_Size = size;
    p->p_ParPacket.p_Flags = flags;
    p->p_ParPacket.p_Buffer = buffer;
    p->p_DoneFunc = func;
    
    /* first try to flush send queue if anything is still to send
       otherwise do nothing. return FALSE if sent had error
     */
    ok = handle_tx_queue();

    /* try to send immediately if queue was processed ok */
    if(ok) {
        int error = par_send(&p->p_ParPacket);
        if(error == PAR_OK) {
            /* directly call done func */
            func(p);
        }
        /* push on list for later */
        else {
            ok = FALSE;
            p->p_Retries += 1;
        }
    }
    
    /* if sent op was not possible then put in tx queue */
    if(!ok) {
        ObtainSemaphore(&tx_sem);
        AddTail(&tx_list, &p->p_Node);
        ReleaseSemaphore(&tx_sem);
    }
    
    return p;
}

static packet_t *get_next_filled_rx_pkt(void)
{
    packet_t *p = NULL;
    
    /* check if rx_empty_list has a packet prepared for us */
    ObtainSemaphore(&rx_filled_sem);
    if(!IsListEmpty(&rx_filled_list)) {
        p = (packet_t *)RemHead(&rx_filled_list);
    }
    ReleaseSemaphore(&rx_filled_sem);
        
    return p;
}

packet_t *parblk_read(APTR buffer, UWORD size, pkt_done_func_t func)
{
    packet_t *p;
    
    /* is there already a received packet? */
    p = get_next_filled_rx_pkt();
    if(p != NULL) {
        UWORD pkt_size = p->p_ParPacket.p_Size;
        /* uh oh, buffer is too small! */
        if(pkt_size > size) {
            p->p_ParPacket.p_Result = PAR_ERR_BUFFER_TOO_SMALL;
        }
        /* copy buffer */
        else {
            memcpy(buffer, p->p_ParPacket.p_Buffer, pkt_size);
        }
        /* directly reply with done call */
        func(p);
    }
    /* need to queue a receive */
    else {
        /* allocate a new rx packet */
        p = pkt_rx_alloc();
        if(p == NULL) {
            /* fatal: no packet to allocate */
            return NULL;
        }
    
        /* prepare packet */
        p->p_ParPacket.p_Size = size;
        p->p_ParPacket.p_Buffer = buffer;
        p->p_DoneFunc = func;
        
        /* add to free queue */
        ObtainSemaphore(&rx_empty_sem);
        AddTail(&rx_empty_list, &p->p_Node);
        ReleaseSemaphore(&rx_empty_sem);
    }
    
    return p;
}



