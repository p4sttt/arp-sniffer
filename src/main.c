#include <arpa/inet.h>
#include <endian.h>
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

#include "common.h"
#include "net.h"
#include "options.h"
#include "threads.h"

int
main (int argc, char *argv[])
{
#if 0
    int                sockfd;
    ssize_t            recvsz;
    uint8_t           *buf;
    struct sockaddr_ll sll;
    struct eth_header  eth_hdr;
    struct arp_request arp_req;

    parse_options (argc, argv);

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

    buf = malloc (BUFFER_SIZE);
    if (buf == NULL) {
        close (sockfd);
        perror ("alloc");
        exit (EXIT_FAILURE);
    }

    do {
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

        print_arp_req (&arp_req);

        usleep (250);
    } while (1);

    close (sockfd);
    free (buf);

    return EXIT_SUCCESS;
#endif
    struct listen_arp_args listen_args;
    i32                    listener_fd;
    pthread_t              listener_thrd;
    thrd_pool_t           *thrd_pool;

    parse_options (argc, argv);

    thrd_pool = malloc (sizeof (*thrd_pool));
    if (thrd_pool == NULL) {
        exit (EXIT_FAILURE);
    }

    if (thrd_pool_create (thrd_pool, THREADS_COUNT) == -1) {
        free (thrd_pool);
        exit (EXIT_FAILURE);
    }

    listener_sock_init (&listener_fd);
    listen_args.fd = listener_fd;
    listen_args.tp = thrd_pool;

    pthread_create (&listener_thrd, NULL, listen_arp, (void *)&listen_args);
    pthread_join (listener_thrd, NULL);

    thrd_pool_destroy (thrd_pool);

    return EXIT_SUCCESS;
}
