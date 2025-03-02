#include "utils.h"
#include "net.h"
#include <linux/if_ether.h>
#include <stdio.h>

void
reverse_bytes (uint8_t *arr, size_t size)
{
    size_t idx;
    for (idx = 0; idx < size / 2; ++idx) {
        uint8_t temp        = arr[idx];
        arr[idx]            = arr[size - idx - 1];
        arr[size - idx - 1] = temp;
    }
}

void
print_eth_hdr (const struct eth_header *eth_hdr)
{
    printf ("ETH HEADER\n");
    printf ("\tDestication: %02x:%02x:%02x:%02x:%02x:%02x\n",
            eth_hdr->dest[0],
            eth_hdr->dest[1],
            eth_hdr->dest[2],
            eth_hdr->dest[3],
            eth_hdr->dest[4],
            eth_hdr->dest[5]);
    printf ("\tSource: %02x:%02x:%02x:%02x:%02x:%02x\n",
            eth_hdr->src[0],
            eth_hdr->src[1],
            eth_hdr->src[2],
            eth_hdr->src[3],
            eth_hdr->src[4],
            eth_hdr->src[5]);
    printf ("\tType: 0x%02x%02x\n",
            eth_hdr->type_u.bytes[1],
            eth_hdr->type_u.bytes[0]);
}

void
print_arp_req (const struct arp_request *arp_req)
{
    printf ("ARP REQUEST\n");
    printf ("\tHardware type: 0x%02x%02x\n",
            arp_req->ha_type_u.bytes[1],
            arp_req->ha_type_u.bytes[0]);
    printf ("\tProto type: 0x%02x%02x\n",
            arp_req->pa_type_u.bytes[1],
            arp_req->pa_type_u.bytes[0]);
    printf ("\tHardware size: %d\n", arp_req->ha_size);
    printf ("\tProto size: %d\n", arp_req->pr_size);
    printf ("\tOpcode: 0x%02x%02x\n",
            arp_req->opcode_u.bytes[1],
            arp_req->opcode_u.bytes[0]);
    printf ("\tSender MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            arp_req->sender_ha[0],
            arp_req->sender_ha[1],
            arp_req->sender_ha[2],
            arp_req->sender_ha[3],
            arp_req->sender_ha[4],
            arp_req->sender_ha[5]);
    printf ("\tSender IP: %d.%d.%d.%d\n",
            arp_req->sender_pa[0],
            arp_req->sender_pa[1],
            arp_req->sender_pa[2],
            arp_req->sender_pa[3]);
    printf ("\tTarget MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            arp_req->target_ha[0],
            arp_req->target_ha[1],
            arp_req->target_ha[2],
            arp_req->target_ha[3],
            arp_req->target_ha[4],
            arp_req->target_ha[5]);
    printf ("\tTarget IP: %d.%d.%d.%d\n",
            arp_req->target_pa[0],
            arp_req->target_pa[1],
            arp_req->target_pa[2],
            arp_req->target_pa[3]);
}
