#include <stdio.h>

#include "par.h"
#include "drv.h"

static char pkt_data[256];
static UWORD rx_err = 0;
static ULONG rx_bytes = 0;
static UWORD tx_err = 0;
static ULONG tx_bytes = 0;

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
    if(status == 1) {
        rx_bytes += pkt->p_Size;

        status = par_send(pkt);
        if(status == 1) {
            tx_bytes += pkt->p_Size;
        } else {
            printf("TX error: %d\n", status);
            tx_err ++;
        }
    } else {
        printf("RX error: %d\n", status);
        rx_err ++;
    }
}

void drv_handle_tick(void)
{
    printf("rx:%lu,%u tx:%lu,%u\n", rx_bytes, rx_err, tx_bytes, tx_err);
}
