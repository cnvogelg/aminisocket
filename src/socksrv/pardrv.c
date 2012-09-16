#include <proto/exec.h>
#include <proto/misc.h>
#include <proto/cia.h>

#include <resources/misc.h>
#include <hardware/cia.h>

#include "drv.h"

__far volatile struct CIA ciaa,ciab;

static int alloc_flags = 0;
static char *name = "pardrv";
static struct Library *cia_base;
struct Library *MiscBase;
static struct Interrupt irq;

struct state_s {
    UWORD flags;
};
static struct state_s state;

extern __asm void interrupt(register __a1 struct state_s *state);

#define HS_LINE     CIAF_PRTRPOUT
#define HS_REQUEST  CIAF_PRTRBUSY

#define SETCIAOUTPUT    ciab.ciapra |= CIAF_PRTRSEL; ciaa.ciaddrb = 0xFF
#define SETCIAINPUT     ciab.ciapra &= ~CIAF_PRTRSEL; ciaa.ciaddrb = 0x00
#define PARINIT         SETCIAINPUT; \
            ciab.ciaddra &= ~HS_LINE; \
            ciab.ciaddra |= HS_REQUEST | CIAF_PRTRSEL

#define TESTLINE(b)     (ciab.ciapra & HS_LINE)
#define SETREQUEST(b)   ciab.ciapra |= HS_REQUEST
#define CLEARREQUEST(b) ciab.ciapra &= HS_REQUEST

#define CLEARINT        SetICR(cia_base, CIAICRF_FLG)
#define DISABLEINT      AbleICR(cia_base, CIAICRF_FLG)
#define ENABLEINT       AbleICR(cia_base, CIAICRF_FLG | CIAICRF_SETCLR)

int drv_init(void)
{
    int err = DRV_ERR;
    
    if (MiscBase = OpenResource("misc.resource")) {
        if (cia_base = OpenResource("ciaa.resource")) {

            /* obtain exclusive access to the parallel hardware */
            if (!AllocMiscResource(MR_PARALLELPORT, name))
            {
                alloc_flags |= 1;
                if (!AllocMiscResource(MR_PARALLELBITS, name))
                {
                    alloc_flags |= 2;
                    
                    /* allocate irq */
                    irq.is_Node.ln_Type = NT_INTERRUPT;
                    irq.is_Node.ln_Pri  = 127;
                    irq.is_Node.ln_Name = name;
                    irq.is_Data         = (APTR)NULL;
                    irq.is_Code         = (VOID (*)())&interrupt;

                    Disable();
                    if (!AddICRVector(cia_base, CIAICRB_FLG, &irq))
                    {
                        DISABLEINT;
                        err = DRV_OK;
                    }
                    Enable();

                    if (err = DRV_OK) {
                        alloc_flags |= 4;
                        PARINIT;
                        CLEARREQUEST(pb);
                        CLEARINT;
                        ENABLEINT;
                    }
                }
            }
        }
    }
    return err;
}

void drv_shutdown(void)
{
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
