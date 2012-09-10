#include "proto/exec.h"
#include "proto/dos.h"
#include <stdio.h>
#include <bsdsocket.h>

struct Library *SocketBase;

int main()
{
        int sd;
        
        SocketBase = OpenLibrary("bsdsocket.library",2);
        if (SocketBase)
        {   
            printf("Library Opened Sucessfully.\n");

            sd = socket(AF_INET, SOCK_STREAM, 0);
            printf("sd=%d\n", sd);

            CloseLibrary(SocketBase);
            printf("Library Closed.\n");
        }
        return 0;
}
