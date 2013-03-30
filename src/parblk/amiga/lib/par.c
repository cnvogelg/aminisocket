#include <proto/exec.h>
#include <proto/misc.h>
#include <proto/cia.h>

#include <resources/misc.h>
#include <hardware/cia.h>

#include "par.h"
#include "timer.h"

/* external asm functions from pario.asm */
extern __asm void interrupt(register __a1 struct state_s *state);
extern __asm int hwsend(register __a0 struct par_packet_s *packet, register __a1 struct state_s *state);
extern __asm int hwrecv(register __a0 struct par_packet_s *packet, register __a1 struct state_s *state);

extern struct Library *SysBase;
extern __far volatile struct CIA ciaa,ciab;

static ULONG intSig = 0;
static int alloc_flags = 0;
static char *name = "pardrv";
struct Library *cia_base;
struct Library *MiscBase;
static struct Interrupt irq;
static struct state_s state;
static ULONG txTimeOut = 500000UL;
static ULONG rxTimeOut = 500000UL;

#define HS_REQUEST      CIAF_PRTRPOUT
#define HS_LINE         CIAF_PRTRBUSY

#define SETCIAOUTPUT    ciab.ciapra |= CIAF_PRTRSEL; ciaa.ciaddrb = 0xFF
#define SETCIAINPUT     ciab.ciapra &= ~CIAF_PRTRSEL; ciaa.ciaddrb = 0x00
#define PARINIT         SETCIAINPUT; \
                        ciab.ciaddra &= ~HS_LINE; \
                        ciab.ciaddra |= HS_REQUEST | CIAF_PRTRSEL

#define TESTLINE        (ciab.ciapra & HS_LINE)
#define SETREQUEST      ciab.ciapra |= HS_REQUEST
#define CLEARREQUEST    ciab.ciapra &= HS_REQUEST

#define CLEARINT        SetICR(cia_base, CIAICRF_FLG)
#define DISABLEINT      AbleICR(cia_base, CIAICRF_FLG)
#define ENABLEINT       AbleICR(cia_base, CIAICRF_FLG | CIAICRF_SETCLR)

extern ULONG par_get_rx_sig_mask(void)
{
    return state.s_IntSigMask;
}

int par_init(UWORD maxPacketSize, UBYTE useCRC)
{
    int result = PAR_INIT_OK;
    
    /* allocate signal for interrupt */
    intSig = AllocSignal(-1);
    if(intSig == -1) {
        return PAR_INIT_NO_SIGNAL;
    }
    alloc_flags |= 8;
    
    /* setup state structure */
    state.s_Flags = 0;
    state.s_IntSigMask = 1UL << intSig;
    state.s_SysBase = SysBase;
    state.s_ServerTask = FindTask(NULL);
    state.s_MaxPacketSize = maxPacketSize;
    state.s_UseCRC = useCRC;
    
    /* register our time out flag */
    timer_timeout_set_flag_ptr(&state.s_TimeOut);
    
    if (MiscBase = OpenResource("misc.resource")) {
        if (cia_base = OpenResource("ciaa.resource")) {

            /* obtain exclusive access to the parallel hardware */
            if (!AllocMiscResource(MR_PARALLELPORT, name))
            {
                alloc_flags |= 1;
                if (!AllocMiscResource(MR_PARALLELBITS, name))
                {
                    BOOL ok = FALSE;
                    
                    alloc_flags |= 2;
                    
                    /* allocate irq */
                    irq.is_Node.ln_Type = NT_INTERRUPT;
                    irq.is_Node.ln_Pri  = 127;
                    irq.is_Node.ln_Name = name;
                    irq.is_Data         = (APTR)&state;
                    irq.is_Code         = (VOID (*)())&interrupt;

                    Disable();
                    /* add an interrupt handler for FLAG = ACK */
                    if (!AddICRVector(cia_base, CIAICRB_FLG, &irq))
                    {
                        DISABLEINT;
                        ok = TRUE;
                    }
                    Enable();

                    /* finish setting up interrupt handler */
                    if (ok) {
                        alloc_flags |= 4;
                        
                        /* setup line */
                        PARINIT;
                        CLEARREQUEST;
                        
                        /* irq setup */
                        CLEARINT;
                        ENABLEINT;
                    } else {
                        result = PAR_INIT_NO_CIA_IRQ;
                    }
                } else {
                    result = PAR_INIT_NO_PARBITS_RES;
                }  
            } else {
                result = PAR_INIT_NO_PARPORT_RES;
            }
        }
    }
    return result;
}

void par_shutdown(void)
{
    if(alloc_flags & 8) {
        FreeSignal(intSig);
    }
    if (alloc_flags & 4) {
        DISABLEINT;
        CLEARINT;
        RemICRVector(cia_base, CIAICRB_FLG, &irq);
    }
    if (alloc_flags & 2) {
        FreeMiscResource(MR_PARALLELBITS);
    }
    if (alloc_flags & 1) {
        FreeMiscResource(MR_PARALLELPORT);
    }
    alloc_flags = 0;
}

int par_send(struct par_packet_s *pkt)
{
    int err;
    BOOL have_line = FALSE;
    
    if (!TESTLINE) /* is the line free ? */
    {
        SETREQUEST;
        /* is the line still free ? */
        if (!TESTLINE)  {
            have_line = TRUE;
        }                    
        else if (!(state.s_Flags & FLAGS_MASK_RX)) {
            /* reset line state */
            CLEARREQUEST;
        }
    }
    
    if(have_line) {
        if(!(state.s_Flags & FLAGS_MASK_RX)) {  
            int rc;
        
            timer_timeout_start(txTimeOut);
            rc = hwsend(pkt, &state);
            timer_timeout_clear();
            
            if(rc == 0) {
                err = PAR_ERR_TIMEOUT;
            } else {
                err = PAR_OK;
            } 
        } else {
            err = PAR_ERR_COLLISION;
        }
    } else {
        err = PAR_ERR_NO_LINE;
    }
    pkt->p_Result = err;
    return err;
}    

int par_recv(struct par_packet_s *pkt)
{
    int err = 0;
    int rc;
    int i;
    
    timer_timeout_start(rxTimeOut);
    rc = hwrecv(pkt, &state);
    timer_timeout_clear();

    if(rc == 0) {
        err = PAR_ERR_TIMEOUT;
    } else if(rc == 2) {
        err = PAR_ERR_CRC;
    } else {
        err = PAR_OK;
    }

    pkt->p_Result = err;
    return err;
}



