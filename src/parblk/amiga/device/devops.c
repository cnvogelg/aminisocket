#include "devops.h"

#include <proto/dos.h>
#include <dos.h>

static long input;
static long output;

BOOL dev_init(void)
{
    input = Open("con:0/0/400/100/Input", MODE_NEWFILE);
    if (input == NULL) {
        return FALSE;
    }
    output = Open("con:0/110/400/100/Output", MODE_NEWFILE);
    if (output == NULL)
    {
        Close(input);
        return FALSE;
    }
    return TRUE;
}

void dev_shutdown(void)
{
    if(input != NULL) {
        Close(input);
    }
    if(output != NULL) {
        Close(output);
    }
}

long dev_read(APTR buffer, long size)
{
    return Read(input, buffer, size);
}

long dev_write(APTR buffer, long size)
{
    return Write(output, buffer, size);
}
