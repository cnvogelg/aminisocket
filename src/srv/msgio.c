#include <proto/exec.h>
#include <proto/dos.h>
#include "msgio.h"

static struct MsgPort *port;

int msgio_init(void)
{
    /* make sure the port is unique */
    port = NULL;
    Forbid();
    if(!FindPort(SRV_PORT_NAME)) {
        port = CreatePort(SRV_PORT_NAME, 0L);
    }
    Permit();
    if(!port) {
        return -1;
    }
    return 0;
}

void msgio_shutdown(void)
{
    msg_t *msg;
    
    if(port) {
        /* reject all pending messages */
        Forbid();
        while(msg = (msg_t *)GetMsg(port)) {
            /* at least check size of message */
            if(msg->m_msg.mn_Length == sizeof(msg_t)) {
                msg->m_result = MSG_RESULT_NO_SERVER;
            }
            ReplyMsg((struct Message *)msg);
        }
        Permit();
    
        /* delete the port */
        DeletePort(port);
        port = NULL;
    }
}

ULONG msgio_port_sig_mask(void)
{
    if(port) {
        return 1UL << (port->mp_SigBit);
    } else {
        return 0;
    }
}

msg_t *msgio_get_msg(void)
{
    msg_t *msg;
    msg = (msg_t *)GetMsg(port);
    if(msg != NULL) {
        /* at least check size of message */
        if(msg->m_msg.mn_Length != sizeof(msg_t)) {
            printf("wrong size: %ld != %ld\n", msg->m_msg.mn_Length, sizeof(msg_t));
            return NULL;
        }
    }
    return msg;
}

void msgio_reply_msg(msg_t *msg)
{
    ReplyMsg((struct Message *)msg);
}
