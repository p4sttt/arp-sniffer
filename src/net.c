#include "net.h"
#include <netinet/in.h>

void
arp_req_normalize (struct arp_request *arp_req)
{
    arp_req->uha_type.val = ntohs (arp_req->uha_type.val);
    arp_req->upr_type.val = ntohs (arp_req->upr_type.val);
    arp_req->uopcode.val  = ntohs (arp_req->uopcode.val);
}

void
eth_hdr_normalize (struct eth_header *eth_hdr)
{
    eth_hdr->utype.val = htons (eth_hdr->utype.val);
}
