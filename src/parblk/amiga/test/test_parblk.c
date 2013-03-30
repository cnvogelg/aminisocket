/* test_parblk */

#include <stdio.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include "parblk.h"

static BOOL send_pkt = TRUE;
static BOOL running = TRUE;
static char data[] = "ABCDABCD";
static char in[] = "........";

static void rx_done(packet_t *p)
{
    int result = p->p_ParPacket.p_Result;
    printf("rx_done: size=%d result=%d\n", p->p_ParPacket.p_Size, result);
    if(result != PAR_OK) {
        running = FALSE;
    } else {
        send_pkt = TRUE;
    }
}

static void tx_done(packet_t *p)
{
    int result = p->p_ParPacket.p_Result;
    printf("tx_done: size=%d result=%d\n", p->p_ParPacket.p_Size, result);
    if(result != PAR_OK) {
        running = FALSE;
    } else {
        packet_t *p;
        
        /* receive result */
        puts("rx_go!");
        p = parblk_read(in,8,rx_done);
        if(p == NULL) {
            puts("FAILED: read!");
            running = FALSE;
        }
    }
}

static void main_loop(void)
{
    ULONG sigMask = SIGBREAKF_CTRL_C;
    ULONG parblkMask = parblk_get_sig_mask();

    sigMask |= parblkMask;
    while(running) {
        ULONG sigGot;
        
        /* send a new packet */
        if(send_pkt) {
            packet_t * p = parblk_write(data,8,tx_done);
            if(p == NULL) {
                puts("FAILED: write!");
                break;
            }
            send_pkt = FALSE;
            puts("send");
        }
        
        sigGot = Wait(sigMask);
        printf("sig: %08x\n", sigGot);
        
        /* parblk signal */
        if(sigGot & parblkMask) {
            parblk_handle_sig(sigGot);
        }
        
        /* user break */
        if(sigGot & SIGBREAKF_CTRL_C) {
            running = FALSE;
        }
    }
}

int main()
{
    int err;
  
    puts("parblk init");
    err = parblk_init();
    if(err == PARBLK_INIT_OK) {
        puts("init ok");
        main_loop();
        parblk_shutdown();
    } else {
        printf("ERROR: parblk_init failed: %d\n", err);
    }
    puts("ready");
}

 