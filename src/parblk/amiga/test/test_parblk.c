/* test_parblk */

#include <stdio.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <exec/memory.h>
#include <stdlib.h>

#include "parblk.h"

static BOOL send_pkt = TRUE;
static BOOL recv_pkt = FALSE;
static BOOL running = TRUE;
static BOOL verbose = FALSE;

static BYTE *alloc_buffer(int size)
{
    BYTE *mem;
    BYTE *ptr;
    int i;
    
    mem = AllocMem(size, MEMF_CLEAR | MEMF_PUBLIC);
    if(mem == NULL) {
        return NULL;
    }
    
    ptr = mem;
    for(i=0;i<size;i++) {
        int j = i % 26;
        *ptr++ = j + 'A';
    }
    return mem;
}

static void free_buffer(BYTE *mem, int size)
{
    if(mem != NULL) {
        FreeMem(mem, size);
    }
}

static void rx_done(packet_t *p)
{
    int result = p->p_ParPacket.p_Result;
    if(verbose) {
        printf("rx_done: size=%d result=%d\n", p->p_ParPacket.p_Size, result);
    }
    if(result != PAR_OK) {
        running = FALSE;
    } else {
        send_pkt = TRUE;
    }
}

static void tx_done(packet_t *p)
{
    int result = p->p_ParPacket.p_Result;
    if(verbose) {
        printf("tx_done: size=%d result=%d\n", p->p_ParPacket.p_Size, result);
    }
    if(result != PAR_OK) {
        running = FALSE;
    } else {
        recv_pkt = TRUE;
    }
}

static void main_loop(int size, BOOL tx_start)
{
    BYTE *mem;
    ULONG sigMask = SIGBREAKF_CTRL_C;
    ULONG parblkMask = parblk_get_sig_mask();

    if(tx_start) {
        send_pkt = TRUE;
        recv_pkt = FALSE;
    } else {
        send_pkt = FALSE;
        recv_pkt = TRUE;
    }

    mem = alloc_buffer(size);
    if(mem == NULL) {
        return;
    }

    sigMask |= parblkMask;
    while(running) {
        ULONG sigGot;
        
        /* send a new packet */
        if(send_pkt) {
            packet_t * p = parblk_write(mem,size,tx_done);
            if(p == NULL) {
                puts("FAILED: write!");
                break;
            }
            send_pkt = FALSE;
            if(verbose) {
                puts("send");
            }
        }
        
        /* recv a new packet */
        if(recv_pkt) {
            packet_t *p = parblk_read(mem,size,rx_done);
            if(p == NULL){
                puts("FAILED: read!");
                running = FALSE;
            }
            recv_pkt = FALSE;
            if(verbose) {
                puts("recv");
            }
        }
        
        sigGot = Wait(sigMask);
        //printf("sig: %08x\n", sigGot);
        
        /* parblk signal */
        if(sigGot & parblkMask) {
            parblk_handle_sig(sigGot);
        }
        
        /* user break */
        if(sigGot & SIGBREAKF_CTRL_C) {
            running = FALSE;
        }
    }
    
    free_buffer(mem, size);
}

int main(int argc, char **argv)
{
    int err;
    int i;
    int size = 1024;
    BOOL tx_start = FALSE;
  
    /* parse args */
    for(i=1;i<argc;i++) {
        if(argv[i][0]=='-') {
            switch(argv[i][1]) {
                case 't':
                    tx_start = TRUE;
                    break;
                case 'b':
                    size = atoi(argv[i+1]);
                    i++;
                    break;
                case 'v':
                    verbose = TRUE;
                    break;
            }
        }
    }
  
    printf("test_parblk init: size=%d tx_start=%d\n", size, tx_start);
    err = parblk_init();
    if(err == PARBLK_INIT_OK) {
        puts("init ok");
        main_loop(size, tx_start);
        parblk_shutdown();
    } else {
        printf("ERROR: parblk_init failed: %d\n", err);
    }
    puts("ready");
}

 