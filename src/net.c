#include "net.h"

#include "common.h"
#include "utils.h"

#include <arpa/inet.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void
arp_req_normalize (struct arp_request *arp_req)
{
    arp_req->ha_type_u.val = ntohs (arp_req->ha_type_u.val);
    arp_req->pa_type_u.val = ntohs (arp_req->pa_type_u.val);
    arp_req->opcode_u.val  = ntohs (arp_req->opcode_u.val);
}

void
eth_hdr_normalize (struct eth_header *eth_hdr)
{
    eth_hdr->type_u.val = htons (eth_hdr->type_u.val);
}

void
listener_sock_init (i32 *fd)
{
    struct sockaddr_ll sll;

    if ((*fd = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ARP))) == -1) {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    memset (&sll, 0, sizeof (struct sockaddr_ll));
    sll.sll_family   = AF_PACKET;
    sll.sll_protocol = htons (ETH_P_ARP);
    sll.sll_ifindex  = 2;

    if (bind (*fd, (struct sockaddr *)&sll, sizeof (sll)) == -1) {
        close (*fd);
        perror ("bind");
        exit (EXIT_FAILURE);
    }

    return;
}

void *
listen_arp (void *arg)
{
    struct listen_arp_args *args = arg;
    struct eth_header      *eth_hdr;
    struct arp_request     *arp_req;
    ssize_t                 recvsz;
    task_t                 *task;
    u8                     *buffer;

    buffer = malloc (BUFFER_SIZE);
    if (buffer == NULL) {
        close (args->fd);
        return NULL;
    }

    for (;;) {
        pthread_mutex_lock (&args->tp->mutex);
        while (args->tp->thrds_free == 0 && !args->tp->shutdown) {
            pthread_cond_wait (&args->tp->cond_listen, &args->tp->mutex);
        }
        pthread_mutex_unlock (&args->tp->mutex);

        recvsz = recvfrom (args->fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (recvsz == -1 && errno == EWOULDBLOCK) {
            return NULL;
        } else if (recvsz == -1) {
            continue;
        }

        task    = malloc (sizeof (*task));

        eth_hdr = malloc (sizeof (*eth_hdr));
        arp_req = malloc (sizeof (*arp_req));

        /* TODO: create funcs for copy from buffer to struct */
        memcpy (eth_hdr, buffer, sizeof (*eth_hdr));
        memcpy (arp_req, buffer + sizeof (*eth_hdr), sizeof (*arp_req));

        eth_hdr_normalize (eth_hdr);
        arp_req_normalize (arp_req);

        print_eth_hdr (eth_hdr);

        task->args = malloc (sizeof (struct spoof_args));
        ((struct spoof_args *)task->args)->eth = eth_hdr;
        ((struct spoof_args *)task->args)->arp = arp_req;
        task->func                             = spoof;

        pthread_mutex_lock (&args->tp->mutex);
        tasks_list_push (&args->tp->list, task);
        pthread_cond_signal (&args->tp->cond_work);
        pthread_mutex_unlock (&args->tp->mutex);
    }

    return NULL;
}

void *
spoof (void *arg)
{
    struct spoof_args *args = (struct spoof_args *)arg;
    for (;;) {
        pthread_mutex_lock (&print_mutex);
        printf ("i will be spoofing %d.%d.%d.%d\n",
                args->arp->target_pa[0],
                args->arp->target_pa[1],
                args->arp->target_pa[2],
                args->arp->target_pa[3]);
        pthread_mutex_unlock (&print_mutex);

        sleep (100);
    }

    free (args->arp);
    free (args->eth);
    return NULL;
}
