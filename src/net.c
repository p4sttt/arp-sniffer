#include "net.h"

#include "common.h"
#include "threads.h"
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

int
listener_sock_init (i32 *fd)
{
    if ((*fd = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ARP))) == -1) {
        perror ("socket");
        return -1;
    }

    return 0;
}

int
get_default_gateway (char *gw_ip, char *if_name)
{
    u64   dest, gateway;
    char  line[256];
    FILE *fp;

    fp = fopen (PROC_NET_ROUTE, "r");
    if (fp == NULL) {
        perror ("fopen");
        return -1;
    }

    /* skip table header */
    fgets (line, sizeof (line), fp);

    while (fgets (line, sizeof (line), fp)) {
        if (sscanf (line, "%15s %lx %lx", if_name, &dest, &gateway) == 3 &&
            dest == 0) {
            memcpy (gw_ip, &gateway, IPV4_ADDR_SIZE);
            fclose (fp);
            return 0;
        }
    }

    fclose (fp);
    return -1;
}

int
should_arp_passed (struct arp_request *arp_req)
{
    if (!memcmp (arp_req->sender_pa, arp_req->target_pa, IPV4_ADDR_SIZE) ||
        arp_req->opcode_u.val == 2)
        return -1;

    return 0;
}

void *
listen_arp (void *arg)
{
    struct spoof_args  *task_args;
    struct eth_header  *eth_hdr;
    struct arp_request *arp_req;
    ssize_t             recvsz;
    task_t             *task;
    i32                *fd = arg;
    u8                 *buffer;

    buffer = malloc (BUFFER_SIZE);
    if (buffer == NULL) {
        return NULL;
    }

    for (;;) {
        pthread_mutex_lock (&thrd_pool->mutex);
        while (thrd_pool->thrds_free == 0 && !thrd_pool->shutdown) {
            pthread_cond_wait (&thrd_pool->cond_listen, &thrd_pool->mutex);
        }
        pthread_mutex_unlock (&thrd_pool->mutex);

        recvsz = recv (*fd, buffer, BUFFER_SIZE, 0);
        if (recvsz == -1 && errno == EWOULDBLOCK) {
            return NULL;
        } else if (recvsz == -1) {
            continue;
        }

        eth_hdr = malloc (sizeof (*eth_hdr));
        if (eth_hdr == NULL) {
            free (buffer);
            return NULL;
        }

        arp_req = malloc (sizeof (*arp_req));
        if (arp_req == NULL) {
            free (buffer);
            free (eth_hdr);
            return NULL;
        }

        memcpy (eth_hdr, buffer, sizeof (*eth_hdr));
        memcpy (arp_req, buffer + sizeof (*eth_hdr), sizeof (*arp_req));
        eth_hdr_normalize (eth_hdr);
        arp_req_normalize (arp_req);

        pthread_mutex_lock (&print_mutex);
        print_eth_hdr (eth_hdr);
        print_arp_req (arp_req);
        pthread_mutex_unlock (&print_mutex);

        task = malloc (sizeof (*task));
        if (task == NULL) {
            free (buffer);
            free (eth_hdr);
            free (arp_req);
            return NULL;
        }

        task_args = malloc (sizeof (*task_args));
        if (task_args == NULL) {
            free (task);
            free (buffer);
            free (eth_hdr);
            free (arp_req);
            return NULL;
        }

        task_args->eth_hdr = eth_hdr;
        task_args->arp_req = arp_req;
        task->args         = task_args;
        task->func         = spoof;

        /* TODO: impleent unique ip address validation */
        pthread_mutex_lock (&thrd_pool->mutex);
        tasks_list_push (&thrd_pool->list, task);
        pthread_cond_signal (&thrd_pool->cond_work);
        pthread_mutex_unlock (&thrd_pool->mutex);
    }

    free (eth_hdr);
    free (arp_req);
    free (task);

    return NULL;
}

void *
spoof (void *arg)
{
    struct arp_request *arp = ((struct spoof_args *)arg)->arp_req;

    pthread_mutex_lock (&print_mutex);
    printf ("PROCESSING\n\t%d.%d.%d.%d\n",
            arp->target_pa[0],
            arp->target_pa[1],
            arp->target_pa[2],
            arp->target_pa[3]);
    pthread_mutex_unlock (&print_mutex);

    return NULL;
}
