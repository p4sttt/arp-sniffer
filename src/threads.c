#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
thrd_pool_t     thrd_pool   = {
          .thrds    = NULL,
          .count    = 0,
          .free     = 0,
          .shutdown = 1,
};

i32
thrd_pool_init (size_t thrds_cnt)
{
    size_t idx;

    thrd_pool.count    = thrds_cnt;
    thrd_pool.free     = thrds_cnt;
    thrd_pool.shutdown = 0;
    thrd_pool.thrds    = malloc (sizeof (*thrd_pool.thrds) * thrds_cnt);

    if (thrd_pool.thrds == NULL) {
        perror ("threads malloc");
        goto cleanup;
    }
    for (idx = 0; idx < thrds_cnt; ++idx) {
        if (pthread_create (&thrd_pool.thrds[idx], NULL, worker, NULL) != 0) {
            perror ("pthread_creat");
            goto cleanup;
        }
    }

    return 0;

cleanup:
    thrd_pool_destroy ();
    return -1;
}

void
thrd_pool_destroy (void)
{
    free (thrd_pool.thrds);
    thrd_pool.thrds    = NULL;
    thrd_pool.free     = 0;
    thrd_pool.count    = 0;
    thrd_pool.shutdown = 1;
}

i32
tasks_queue_create (void)
{
    if (pthread_mutex_init (&tasks_queue.mutex, NULL) != 0)
        return -1;

    tasks_queue.head = NULL;
    tasks_queue.tail = NULL;

    return 0;
}

i32
tasks_queue_destroy (void)
{
    task_t *curr;

    if (tasks_queue.head == NULL)
        return 0;

    pthread_mutex_lock (&tasks_queue.mutex);
    for (curr = tasks_queue.head->next; curr != NULL; curr = curr->next) {
        free (tasks_queue.head);
        tasks_queue.head = curr;
    }
    free (tasks_queue.head);
    pthread_mutex_unlock (&tasks_queue.mutex);

    return 0;
}

#if 0
i32
tasks_queue_remove (tasks_queue_t *tl, task_t **retval)
{
    pthread_mutex_lock (&tl->mutex);

    if (tl->head == NULL)
        return 0;

    *retval         = tl->head;
    (*retval)->next = NULL;
    tl->head        = tl->head->next;

    if (tl->head == NULL)
        tl->tail = NULL;

    pthread_mutex_unlock (&tl->mutex);

    return 1;
}
#endif

i32
tasks_queue_add (tasks_queue_t *tl, task_t *task)
{
    if (task == NULL) {
        return -1;
    }

    task->next = NULL;
    pthread_mutex_lock (&tl->mutex);

    if (tl->head == NULL) {
        tl->head = tl->tail = task;
    } else {
        tl->tail->next = task;
        tl->tail       = task;
    }

    tl->len++;
    pthread_cond_signal (&tl->cond);
    pthread_mutex_unlock (&tl->mutex);

    return 0;
}

i32
get_next_task (tasks_queue_t *tq, task_t **retval)
{
    task_t *prev;
    pthread_mutex_lock (&tq->mutex);

    while (tq->len == 0)
        pthread_cond_wait (&tq->cond, &tq->mutex);
    assert (tq->len != 0);

    if (tq->len == 1) {
        *retval  = tq->tail;
        tq->head = tq->tail = NULL;
    } else {
        prev = tq->head;
        while (prev->next != tq->tail)
            prev = prev->next;
        *retval        = tq->tail;
        tq->tail       = prev;
        tq->tail->next = NULL;
    }

    tq->len--;

    pthread_mutex_unlock (&tq->mutex);
    return 0;
}

void *
worker (void *arg)
{
    task_t *task;
    pthread_detach (pthread_self ());

    for (;;) {
        if (get_next_task (&task)) {
            /* process task */
            thrd_pool.free--;
            task->func (task->args);

            /* makr task as done */
            free (task->args);
            free (task);
            thrd_pool.free++;
        }
    }

    return NULL;
}
