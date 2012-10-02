/* socksrv.c - aminisocket socket server process 
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <stdio.h>

#include "drv.h"
#include "timer.h"
#include "msgio.h"

static int init(void)
{
    /* setup timer */
    int err = timer_init();
    if(err != 0) {
        printf("ERROR: timer_init failed: %d\n", err);
        return err;
    }
    
    /* setup msg io */
    err = msgio_init();
    if(err != 0) {
        printf("ERROR: msgio_init failed: %d\n", err);
        return err;
    }
    
    return err;
}

static void cleanup(void)
{
    msgio_shutdown();
    timer_shutdown();
}

#define TICK_DELAY 5000000

static void main_loop(void)
{
    BOOL running = TRUE;
    ULONG sigMask = SIGBREAKF_CTRL_F | SIGBREAKF_CTRL_C;
    ULONG tickMask = timer_tick_mask();
    ULONG rxMask = drv_get_rx_sig_mask();
    ULONG msgMask = msgio_port_sig_mask();

    sigMask |= tickMask | rxMask | msgMask;
    timer_tick_start(TICK_DELAY);
    while(running) {
        ULONG sigGot = 0;
        sigGot = Wait(sigMask);
        
        /* rx signal */
        if(sigGot & rxMask) {
            drv_handle_rx();
        }

        /* tick timer */
        if(sigGot & tickMask) {
            drv_handle_tick();
            timer_tick_start(TICK_DELAY);
        }
        
        /* msg from lib? */
        if(sigGot & msgMask) {
            msg_t *msg;
            msg = msgio_get_msg();
            if(msg != NULL) {
                /* TODO: really handle message */
                printf("msg: got cmd %02x\n", msg->m_command);
                msg->m_result = msg->m_command;
                msgio_reply_msg(msg);
            } else {
                printf("msg: got NULL?\n");
            }
        }

        /* user break */
        if(sigGot & SIGBREAKF_CTRL_C) {
            running = FALSE;
        }
    }
    timer_tick_clear();
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

 