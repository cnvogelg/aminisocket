#include "proto/exec.h"
#include "proto/dos.h"
#include <stdio.h>
#include <bsdsocket.h>
#include <errno.h>

struct Library *SocketBase;

int main()
{
        int sd;
        
        SocketBase = OpenLibrary("bsdsocket.library",2);
        if (SocketBase)
        {   
            printf("Library Opened Sucessfully.\n");
            
            puts("Set errno pointer");            
            SetErrnoPtr(&errno, sizeof errno);

            puts("calling socket");
            sd = socket(AF_INET, SOCK_STREAM, 0);
            printf("sd=%d\n", sd);

            puts("closing lib");
            CloseLibrary(SocketBase);
            printf("Library Closed.\n");
        }
        return 0;
}
