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
    size_t idx;

    printf ("ETH HEADER\n");
    printf ("\tDestication: ");
    for (idx = 0; idx < MAC_ADDR_SIZE; ++idx) {
        printf ("%02x%s",
                eth_hdr->dest[idx],
                idx == MAC_ADDR_SIZE - 1 ? "\n" : ":");
    }
    printf ("\tSource: ");
    for (idx = 0; idx < MAC_ADDR_SIZE; ++idx) {
        printf (
            "%02x%s", eth_hdr->src[idx], idx == MAC_ADDR_SIZE - 1 ? "\n" : ":");
    }
    printf ("\tType: 0x%02x%02x\n",
            eth_hdr->type_u.bytes[1],
            eth_hdr->type_u.bytes[0]);
}

void
print_arp_req (const struct arp_request *arp_req)
{
    size_t idx;

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
    printf ("\tSender MAC: ");
    for (idx = 0; idx < MAC_ADDR_SIZE; ++idx) {
        printf ("%02x%s",
                arp_req->sender_ha[idx],
                idx == MAC_ADDR_SIZE - 1 ? "\n" : ":");
    }
    printf ("\tSender IP: ");
    for (idx = 0; idx < IPV4_ADDR_SIZE; ++idx) {
        printf ("%02x%s",
                arp_req->sender_pa[idx],
                idx == IPV4_ADDR_SIZE - 1 ? "\n" : ".");
    }
    printf ("\tTarget MAC: ");
    for (idx = 0; idx < MAC_ADDR_SIZE; ++idx) {
        printf ("%02x%s",
                arp_req->target_ha[idx],
                idx == MAC_ADDR_SIZE - 1 ? "\n" : ":");
    }
    printf ("\tTarget IP: ");
    for (idx = 0; idx < IPV4_ADDR_SIZE; ++idx) {
        printf ("%02x%s",
                arp_req->target_pa[idx],
                idx == IPV4_ADDR_SIZE - 1 ? "\n" : ".");
    }
}
