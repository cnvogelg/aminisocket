/* socksrv.c - aminisocket socket server process 
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <stdio.h>

#include "drv.h"
#include "timer.h"

static int init(void)
{
    int err = timer_init();
    return err;
}

static void cleanup(void)
{
    timer_shutdown();
}

static void main_loop(void)
{
    BOOL running = TRUE;
    ULONG sigMask = SIGBREAKF_CTRL_F | SIGBREAKF_CTRL_C;
    
    // test
    volatile UBYTE *flag = timer_start(500);
    printf("flag %02x\n", (int)*flag);
    
    while(running) {
        ULONG sigGot = 0;
        sigGot = Wait(sigMask);
                
        
        
        if(sigGot & SIGBREAKF_CTRL_C) {
            running = FALSE;
        }
    }
    
    printf("flag %02x\n", (int)*flag);
    timer_clear();
    
}

int main()
{
    int err;
  
    puts("socksrv started");
    err = init();
    if(err == 0) {
        puts("init ok");
        err = drv_init();
        if(err == DRV_OK) {
            puts("driver ok");
            main_loop();
            puts("done");
        } else {
            puts("ERROR: driver init");
        }
        drv_shutdown();
    } else {
        puts("ERROR: init");
    }
    cleanup();
    puts("ready");
}

 