#include "par.h"
#include "drv.h"

int drv_init(void)
{
    return par_init();
}

void drv_shutdown(void)
{
    par_shutdown();
}

ULONG drv_get_rx_sig_mask(void)
{
    return par_get_rx_sig_mask();
}
