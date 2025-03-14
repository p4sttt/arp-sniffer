#ifndef NET_H_
#define NET_H_

#include "threads.h"

#define MAC_ADDR_SIZE  6
#define IPV4_ADDR_SIZE 4
#define BUFFER_SIZE    60

#define PROC_NET_ROUTE "/proc/net/route"

#define BITSET_FIELD(type, name)   \
    union {                        \
        type val;                  \
        u8   bytes[sizeof (type)]; \
    } name##_u

struct eth_header {
    u8 dest[MAC_ADDR_SIZE];
    u8 src[MAC_ADDR_SIZE];
    BITSET_FIELD (u16, type);
};

struct arp_request {
    BITSET_FIELD (u16, ha_type);
    BITSET_FIELD (u16, pa_type);
    u8 ha_size;
    u8 pr_size;
    BITSET_FIELD (u16, opcode);
    u8 sender_ha[MAC_ADDR_SIZE];
    u8 sender_pa[IPV4_ADDR_SIZE];
    u8 target_ha[MAC_ADDR_SIZE];
    u8 target_pa[IPV4_ADDR_SIZE];
};

struct listen_arp_args {
    thrd_pool_t *tp;
    i32          fd;
};

struct spoof_args {
    struct eth_header  *eth_hdr;
    struct arp_request *arp_req;
};

void  arp_req_normalize (struct arp_request *arp_req);
void  eth_hdr_normalize (struct eth_header *eth_hdr);

int   listener_sock_init (i32 *fd);
int   get_default_gateway (char *gateway_ip, char *ifname);
int   should_arp_passed (struct arp_request *arp_req);

void *listen_arp (void *arg);
void *spoof (void *arg);

#endif
