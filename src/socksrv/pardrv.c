#include "par.h"
#include "drv.h"

static char pkt_data[256];

int drv_init(void)
{
    return par_init(256 - sizeof(struct packet_s), 0);
}

void drv_shutdown(void)
{
    par_shutdown();
}

ULONG drv_get_rx_sig_mask(void)
{
    return par_get_rx_sig_mask();
}

void drv_handle_rx(void)
{    
    struct packet_s *pkt = (struct packet_s *)pkt_data;
    int status = par_recv(pkt);
    printf("rx: %d\n", status);
    if(status == 1) {
        int status = par_send(pkt);
        printf("tx: %d\n", status);
    }
}
