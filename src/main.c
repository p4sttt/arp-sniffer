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

#include "net.h"
#include "options.h"
#include "threads.h"

int
main (int argc, char **argv)
{
    pthread_t listener_thrd;
    parse_options (argc, argv);

    thrd_pool = malloc (sizeof (*thrd_pool));
    if (thrd_pool == NULL) {
        exit (EXIT_FAILURE);
    }

    if (thrd_pool_create (thrd_pool, THREADS_COUNT) == -1) {
        free (thrd_pool);
        exit (EXIT_FAILURE);
    }

    pthread_create (&listener_thrd, NULL, listen_arp, &listener_thrd);
    pthread_join (listener_thrd, NULL);

    close (listener_thrd);
    close (listener_thrd);

    thrd_pool_destroy (thrd_pool);

    return EXIT_SUCCESS;
}
