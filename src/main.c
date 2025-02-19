#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <endian.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "net.h"
#include "utils.h"

#define BUFFER_SIZE 1488

int
main (int argc, char *argv[])
{
    int                sockfd;
    ssize_t            recvsz;
    uint8_t           *buf;
    struct sockaddr_ll sll;
    struct eth_header  eth_hdr;
    struct arp_request arp_req;

    if ((sockfd = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ARP))) == -1) {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    memset (&sll, 0, sizeof (struct sockaddr_ll));
    sll.sll_family   = AF_PACKET;
    sll.sll_protocol = htons (ETH_P_ARP);
    sll.sll_ifindex  = 2;

    if (bind (sockfd, (struct sockaddr *)&sll, sizeof (sll)) == -1) {
        close (sockfd);
        perror ("bind");
        exit (EXIT_FAILURE);
    }

    printf ("Listening...\n");

    do {
        buf = malloc (BUFFER_SIZE);
        if (buf == NULL) {
            close (sockfd);
            perror ("alloc");
            exit (EXIT_FAILURE);
        }

        recvsz = recvfrom (sockfd, buf, BUFFER_SIZE, 0, NULL, NULL);

        if (recvsz == -1 && errno == EWOULDBLOCK) {
            close (sockfd);
            free (buf);
            perror ("recvfrom");
            exit (EXIT_FAILURE);
        } else if (recvsz == -1) {
            continue;
        }

        memset (&eth_hdr, 0, sizeof (eth_hdr));
        memset (&arp_req, 0, sizeof (arp_req));

        memcpy (&eth_hdr, buf, sizeof (eth_hdr));
        memcpy (&arp_req, buf + sizeof (eth_hdr), sizeof (arp_req));

        eth_hdr_normalize (&eth_hdr);
        arp_req_normalize (&arp_req);

        print_eth_hdr (&eth_hdr);
        print_arp_req (&arp_req);

        usleep (250);
    } while (1);

    close (sockfd);
    free (buf);

    return EXIT_SUCCESS;
}
