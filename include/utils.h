#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <unistd.h>

#include "net.h"

void reverse_bytes (uint8_t *arr, size_t size);

void print_eth_hdr (const struct eth_header *eth_hdr);
void print_arp_req (const struct arp_request *arp_req);

#endif
