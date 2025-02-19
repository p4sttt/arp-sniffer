#ifndef NET_H_
#define NET_H_

#include <stdint.h>

#define MAC_ADDR_SIZE  6
#define IPV4_ADDR_SIZE 4
#define IPV6_ADDR_SIZE 16
#define MAX_DATA_SIZE  48 /* 6 + 16 + 6 + 16 */

struct eth_header {
    uint8_t dest[MAC_ADDR_SIZE];
    uint8_t src[MAC_ADDR_SIZE];
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } utype;
};

struct arp_request {
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } uha_type;
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } upr_type;
    uint8_t ha_size;
    uint8_t pr_size;
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } uopcode;
    uint8_t data[MAX_DATA_SIZE];
};

struct ipv4_arp_request {
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } uha_type;
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } upr_type;
    uint8_t ha_size;
    uint8_t pr_size;
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } uopcode;
    uint8_t sender_ha[MAC_ADDR_SIZE];
    uint8_t sender_pa[IPV4_ADDR_SIZE];
    uint8_t target_ha[MAC_ADDR_SIZE];
    uint8_t target_pa[IPV4_ADDR_SIZE];
    uint8_t padding[28];
};

struct ipv6_arp_request {
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } uha_type;
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } upr_type;
    uint8_t ha_size;
    uint8_t pr_size;
    union {
        uint16_t val;
        uint8_t  bytes[2];
    } uopcode;
    uint8_t sender_ha[MAC_ADDR_SIZE];
    uint8_t sender_pa[IPV6_ADDR_SIZE];
    uint8_t target_ha[MAC_ADDR_SIZE];
    uint8_t target_pa[IPV6_ADDR_SIZE];
};

void arp_req_normalize (struct arp_request *arp_req);
void eth_hdr_normalize (struct eth_header *eth_hdr);

#endif
