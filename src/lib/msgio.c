#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/memory.h>
#include "msgio.h"
#include "debug.h"

static struct MsgPort *port;
static struct MsgPort *reply_port;
msg_t *msg;
int msgio_ready = 0;

int msgio_init(void)
{
    /* already initialized */
    if(msgio_ready > 0) {
        msgio_ready++;
        return 0;
    }
    
    /* try to find port */
    D(bug("finding port\n"));    
    port = FindPort(SRV_PORT_NAME);
    if(!port) {
        return -1;
    }
    D(bug("found port\n"));
    
    /* create a reply port */
    reply_port = CreatePort(0L, 0L);
    if(!reply_port) {
        return -2;
    }
    D(bug("created port\n"));
    
    /* alloc a message */
    msg = (msg_t *)AllocMem(sizeof(msg_t), MEMF_CLEAR);
    if(!msg) {
        return -3;
    }
    D(bug("got mem\n"));
    msg->m_msg.mn_ReplyPort = reply_port;
    
    msgio_ready++;
    return 0;
}

void msgio_shutdown(void)
{
    msgio_ready--;
    
    /* not last one? */
    if(msgio_ready > 0) {
        return;
    }
    
    /* free message */
    if(msg) {
        FreeMem(msg, sizeof(msg_t));
        msg = NULL;
    }
    
    /* free reply port */
    if(reply_port) {
        DeletePort(reply_port);
        reply_port = NULL;
    }
}

int msgio_xfer_msg(msg_t *msg)
{
    /* not initialized? */
    if(msgio_ready == 0) {
        return -1;
    }
    
    /* pre-init result */
    msg->m_result = 0;
    msg->m_msg.mn_Length = sizeof(msg_t);
    
    /* send message to socksrv port */
    D(bug("put msg\n"));
    PutMsg(port, (struct Message *)msg);
    
    /* now wait for the result */
    D(bug("get msg\n"));
    while(!(msg = (msg_t *)GetMsg(reply_port))) {
        D(bug("wait for msg\n"));
        WaitPort(reply_port);
    }
    
    return 0;
}


