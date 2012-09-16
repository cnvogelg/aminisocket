/* socksrv.c - aminisocket socket server process 
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <stdio.h>

#include "drv.h"

int main()
{
    int err;
  
    puts("socksrv started");
    err = drv_init();
    if(err == DRV_OK) {
        puts("driver ok");
    
        drv_shutdown();
    } else {
        puts("ERROR: init driver");
    }
}

 