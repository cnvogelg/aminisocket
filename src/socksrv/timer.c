#include <proto/exec.h>
#include <proto/dos.h>
#include "timer.h"

static struct MsgPort *timeoutPort;
static struct timerequest timeoutReq;
static struct Library *TimerBase;
static ULONG oldExcept;
static APTR oldExceptCode;
static APTR oldExceptData;
static struct Task *myTask;
static volatile UBYTE *timeoutFlag;

static struct MsgPort *tickPort;
static struct timerequest tickReq;

static ULONG __asm __saveds exceptcode(register __d0 ULONG sigmask, register __a1 UBYTE *flag)
{
    /* remove the I/O Block from the port */
    WaitIO((struct IORequest*)&timeoutReq);
    /* set flag */
    *flag = 0xff;
    /* re-enable the signal */
    return sigmask;
}

int timer_init(void)
{
    ULONG sigmask;
    
    /* create port for timer replies */
    timeoutPort = CreatePort(NULL,0);
    if(timeoutPort == NULL) {
        return -1;
    }
    
    /* create port for tick replies */
    tickPort = CreatePort(NULL,0);
    if(tickPort == NULL) {
        return -1;
    }
    
    /* timer.device */
    if (OpenDevice("timer.device", UNIT_MICROHZ, (struct IORequest*)&timeoutReq, 0))
    {
        return -1;
    }
    timeoutReq.tr_node.io_Flags = IOF_QUICK;
    timeoutReq.tr_node.io_Message.mn_ReplyPort = timeoutPort;
    timeoutReq.tr_node.io_Command = TR_ADDREQUEST;
    TimerBase = (struct Library *)timeoutReq.tr_node.io_Device;
    
    /* clone request */
    tickReq.tr_node.io_Flags = IOF_QUICK;
    tickReq.tr_node.io_Message.mn_ReplyPort = tickPort;
    tickReq.tr_node.io_Command = TR_ADDREQUEST;
    tickReq.tr_node.io_Device = timeoutReq.tr_node.io_Device;
    tickReq.tr_node.io_Unit = timeoutReq.tr_node.io_Unit;
    
    /* set exception for timer signal */
    myTask = FindTask(NULL);
    oldExcept = SetExcept(0, 0xffffffff);
    oldExceptCode = myTask->tc_ExceptCode;
    oldExceptData = myTask->tc_ExceptData;
    myTask->tc_ExceptCode = (APTR)&exceptcode;
    myTask->tc_ExceptData = (APTR)NULL; /* will be set later */
    sigmask = 1L << timeoutPort->mp_SigBit;
    SetSignal(0, sigmask);
    SetExcept(sigmask, sigmask);
    
    return 0;
}

void timer_shutdown(void)
{
    /* remove exception */
    if(myTask) {
        SetExcept(0, 0xffffffff);
        myTask->tc_ExceptCode = oldExceptCode;
        myTask->tc_ExceptData = oldExceptData;
        SetExcept(oldExcept, 0xffffffff);
    }
    
    /* close timer device */
    if (TimerBase)
    {
        WaitIO((struct IORequest*)&timeoutReq);
        CloseDevice((struct IORequest*)&timeoutReq);
    }

    /* close tick port */
    if(tickPort != NULL) {
        DeletePort(tickPort);
    }
    
    /* close port */
    if(timeoutPort != NULL) {
        DeletePort(timeoutPort);
    }
}

void timer_timeout_set_flag_ptr(UBYTE *flag)
{
    timeoutFlag = flag;
    myTask->tc_ExceptData = (APTR)flag;
    *flag = 0xff;
}

void timer_timeout_start(ULONG timeout)
{
    //while(!timeoutFlag) Delay(1L);
    timeoutReq.tr_time.tv_secs = 2;
    timeoutReq.tr_time.tv_micro = timeout;
    *timeoutFlag = 0;
    SendIO((struct IORequest*)&timeoutReq);
}

void timer_timeout_clear(void)
{
    AbortIO((struct IORequest*)&timeoutReq);
}

ULONG timer_tick_mask(void)
{
    return 1UL << tickPort->mp_SigBit;
}

void timer_tick_start(ULONG timeout)
{
    tickReq.tr_time.tv_secs    = 0;
    tickReq.tr_time.tv_micro   = timeout;
    SendIO((struct IORequest*)&tickReq);
}

void timer_tick_clear(void)
{
    AbortIO((struct IORequest*)&timeoutReq);
    WaitIO((struct IORequest*)&timeoutReq);
}

