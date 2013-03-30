/* test_parblk */

#include <stdio.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include "parblk.h"

static void main_loop(void)
{
    BOOL running = TRUE;
    ULONG sigMask = SIGBREAKF_CTRL_C;
    ULONG parblkMask = parblk_get_sig_mask();

    sigMask |= parblkMask;
    while(running) {
        ULONG sigGot = 0;
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

 