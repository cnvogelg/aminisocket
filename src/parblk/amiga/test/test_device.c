/* Driver program to test example.device */
/* Copyright (c) 1993 SAS Institute, Inc, Cary, NC, USA */
/* All Rights Reserved */

#include <exec/devices.h>
#include <devices/parblk.h>
#include <proto/exec.h>

void main(void)
{
    struct IOExtParBlk *ior;
    struct MsgPort *msg_port;
    char buff[10];

    if (msg_port = CreatePort(0,0))
    {
        if (ior = (struct IOExtParBlk *) 
                       CreateExtIO(msg_port, sizeof(struct IOExtParBlk)))
        {
            if (OpenDevice("parblk.device",-1,(struct IORequest*)ior,0) == 0)
            {
                ior->IOParBlk.io_Data = buff;
                while(1)
                {
                    ior->IOParBlk.io_Command = CMD_READ;
                    ior->IOParBlk.io_Length = 1;
                    DoIO((struct IORequest *)ior);
                    if (ior->IOParBlk.io_Actual < 1)
                    {
                        CloseDevice((struct IORequest *)ior);
                        break;
                    }
                    ior->IOParBlk.io_Command = CMD_WRITE;
                    ior->IOParBlk.io_Length = 1;
                    DoIO((struct IORequest *)ior);
                }
            }
            DeleteExtIO((struct IORequest *)ior);
        }
        DeletePort(msg_port);
    }
}
