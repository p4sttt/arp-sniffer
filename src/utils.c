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
            eth_hdr->utype.bytes[1],
            eth_hdr->utype.bytes[0]);
}

void
print_ipv4_arp_req_payload (const struct ipv4_arp_request *ipv4_arp_req)
{
    printf ("ARP PAYLOAD\n");
    printf ("\tSender MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            ipv4_arp_req->sender_ha[0],
            ipv4_arp_req->sender_ha[1],
            ipv4_arp_req->sender_ha[2],
            ipv4_arp_req->sender_ha[3],
            ipv4_arp_req->sender_ha[4],
            ipv4_arp_req->sender_ha[5]);
    printf ("\tSender IP: %d.%d.%d.%d\n",
            ipv4_arp_req->sender_pa[0],
            ipv4_arp_req->sender_pa[1],
            ipv4_arp_req->sender_pa[2],
            ipv4_arp_req->sender_pa[3]);
    printf ("\tTarget MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            ipv4_arp_req->target_ha[0],
            ipv4_arp_req->target_ha[1],
            ipv4_arp_req->target_ha[2],
            ipv4_arp_req->target_ha[3],
            ipv4_arp_req->target_ha[4],
            ipv4_arp_req->target_ha[5]);
    printf ("\tTarget IP: %d.%d.%d.%d\n",
            ipv4_arp_req->target_pa[0],
            ipv4_arp_req->target_pa[1],
            ipv4_arp_req->target_pa[2],
            ipv4_arp_req->target_pa[3]);
}

void
print_ipv6_arp_req_payload (const struct ipv6_arp_request *ipv6_arp_req)
{
    printf ("ARP PAYLOAD\n");
    printf ("\tSender MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            ipv6_arp_req->sender_ha[0],
            ipv6_arp_req->sender_ha[1],
            ipv6_arp_req->sender_ha[2],
            ipv6_arp_req->sender_ha[3],
            ipv6_arp_req->sender_ha[4],
            ipv6_arp_req->sender_ha[5]);
    printf ("\tSender IP: %02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
            ipv6_arp_req->sender_pa[0],
            ipv6_arp_req->sender_pa[1],
            ipv6_arp_req->sender_pa[2],
            ipv6_arp_req->sender_pa[3],
            ipv6_arp_req->sender_pa[4],
            ipv6_arp_req->sender_pa[5],
            ipv6_arp_req->sender_pa[6],
            ipv6_arp_req->sender_pa[7]);
    printf ("\tTarget MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            ipv6_arp_req->target_ha[0],
            ipv6_arp_req->target_ha[1],
            ipv6_arp_req->target_ha[2],
            ipv6_arp_req->target_ha[3],
            ipv6_arp_req->target_ha[4],
            ipv6_arp_req->target_ha[5]);
    printf ("\tTarget IP: %02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
            ipv6_arp_req->target_pa[0],
            ipv6_arp_req->sender_pa[1],
            ipv6_arp_req->sender_pa[2],
            ipv6_arp_req->sender_pa[3],
            ipv6_arp_req->sender_pa[4],
            ipv6_arp_req->sender_pa[5],
            ipv6_arp_req->sender_pa[6],
            ipv6_arp_req->sender_pa[7]);
}

void
print_arp_req (const struct arp_request *arp_req)
{
    printf ("ARP HEADER\n");
    printf ("\tHardware type: 0x%02x%02x\n",
            arp_req->uha_type.bytes[1],
            arp_req->uha_type.bytes[0]);
    printf ("\tProto type: 0x%02x%02x\n",
            arp_req->upr_type.bytes[1],
            arp_req->upr_type.bytes[0]);
    printf ("\tHardware size: %d\n", arp_req->ha_size);
    printf ("\tProto size: %d\n", arp_req->pr_size);
    printf ("\tOpcode: 0x%02x%02x\n",
            arp_req->uopcode.bytes[1],
            arp_req->uopcode.bytes[0]);
    if (arp_req->upr_type.val == ETH_P_IP)
        print_ipv4_arp_req_payload ((struct ipv4_arp_request *)arp_req);
    if (arp_req->upr_type.val == ETH_P_IPV6)
        print_ipv6_arp_req_payload ((struct ipv6_arp_request *)arp_req);
}
