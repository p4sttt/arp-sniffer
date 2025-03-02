#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

i32
thrd_pool_create (thrd_pool_t *tp, size_t thrds_cnt)
{
    size_t i;

    tp->thrds = malloc (sizeof (pthread_t) * thrds_cnt);
    if (!tp->thrds) {
        return -1;
    }

    tp->thrds_cnt  = thrds_cnt;
    tp->thrds_free = thrds_cnt;
    tp->shutdown   = 0;
    tasks_list_create (&tp->list);

    if (pthread_mutex_init (&tp->mutex, NULL) != 0) {
        free (tp->thrds);
        tp->thrds = NULL;
        return -1;
    }

    if (pthread_cond_init (&tp->cond_work, NULL) != 0) {
        free (tp->thrds);
        tp->thrds = NULL;
        return -1;
    }

    if (pthread_cond_init (&tp->cond_listen, NULL) != 0) {
        free (tp->thrds);
        tp->thrds = NULL;
        return -1;
    }

    for (i = 0; i < thrds_cnt; ++i) {
        if (pthread_create (&tp->thrds[i], NULL, worker, tp) != 0) {
            pthread_cond_destroy (&tp->cond_listen);
            pthread_cond_destroy (&tp->cond_work);
            pthread_mutex_destroy (&tp->mutex);
            free (tp->thrds);
            tp->thrds = NULL;
            return -1;
        }
    }

    return 0;
}

i32
thrd_pool_destroy (thrd_pool_t *tp)
{
    size_t i;

    pthread_mutex_lock (&tp->mutex);
    tp->shutdown = 1;
    pthread_cond_broadcast (&tp->cond_work);
    pthread_mutex_unlock (&tp->mutex);

    pthread_mutex_lock (&tp->mutex);
    tasks_list_destroy (&tp->list);
    pthread_mutex_unlock (&tp->mutex);

    for (i = 0; i < tp->thrds_cnt; i++) {
        pthread_join (tp->thrds[i], NULL);
    }

    pthread_mutex_destroy (&tp->mutex);
    pthread_cond_destroy (&tp->cond_work);
    pthread_cond_destroy (&tp->cond_listen);
    free (tp->thrds);

    return 0;
}

i32
tasks_list_create (tasks_list_t *tl)
{
    if (pthread_mutex_init (&tl->mutex, NULL) != 0) {
        return -1;
    }

    tl->head = NULL;
    tl->tail = NULL;

    return 0;
}

i32
tasks_list_destroy (tasks_list_t *tl)
{
    task_t *curr;

    if (tl->head == NULL) {
        return 0;
    }

    pthread_mutex_lock (&tl->mutex);
    for (curr = tl->head->next; curr != NULL; curr = curr->next) {
        free (tl->head);
        tl->head = curr;
    }
    free (tl->head);
    pthread_mutex_unlock (&tl->mutex);

    return 0;
}

i32
tasks_list_pop (tasks_list_t *tl, task_t **retval)
{
    if (tl->head == 0) {
        return 0;
    }

    pthread_mutex_lock (&tl->mutex);
    *retval  = tl->head;
    tl->head = tl->head->next;

    if (tl->head == NULL) {
        tl->tail = tl->head;
    }

    pthread_mutex_unlock (&tl->mutex);

    return 1;
}

i32
tasks_list_push (tasks_list_t *tl, task_t *task)
{
    pthread_mutex_lock (&tl->mutex);

    if (tl->head == NULL) {
        tl->head = tl->tail = task;
    } else {
        tl->head->next = task;
        tl->head       = tl->head->next;
    }

    pthread_mutex_unlock (&tl->mutex);

    return 0;
}

i32
tasks_list_empty (const tasks_list_t *tl)
{
    return (tl->head == NULL);
}

void *
worker (void *arg)
{
    thrd_pool_t *tp = (thrd_pool_t *)arg;
    task_t      *task;

    while (1) {
        pthread_mutex_lock (&tp->mutex);

        while (tasks_list_empty (&tp->list) && !tp->shutdown) {
            pthread_cond_wait (&tp->cond_work, &tp->mutex);
        }

        if (tp->shutdown && tasks_list_empty (&tp->list)) {
            pthread_mutex_unlock (&tp->mutex);
            return NULL;
        }

        tasks_list_pop (&tp->list, &task);
        tp->thrds_free--;
        pthread_mutex_unlock (&tp->mutex);

        if (task) {
            task->func (task->args);
            free (task->args);
            free (task);
            task = NULL;
        }

        pthread_mutex_lock (&tp->mutex);
        tp->thrds_free++;
        pthread_mutex_unlock (&tp->mutex);
    }

    return NULL;
}
