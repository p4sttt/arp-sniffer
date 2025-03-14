#ifndef THREADS_H_
#define THREADS_H_

#include "common.h"
#include <pthread.h>
#include <stdatomic.h>

#define THREADS_COUNT 2

typedef void *(*thrd_func_t) (void *);

typedef struct task_s {
    struct task_s *next;
    thrd_func_t    func;
    void          *args;
} task_t;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    atomic_size_t   len;
    task_t         *head;
    task_t         *tail;
} tasks_queue_t;

typedef struct {
    pthread_t    *thrds;
    size_t        count;
    atomic_size_t free;
    atomic_bool   shutdown;
} thrd_pool_t;

extern pthread_mutex_t print_mutex;
extern thrd_pool_t     thrd_pool;
extern tasks_queue_t   tasks_queue;

/**/

i32   thrd_pool_init (size_t thrds_cnt);
void  thrd_pool_destroy (void);

i32   tasks_queue_init (void);
i32   tasks_queue_destroy (void);
i32   tasks_queue_add (tasks_queue_t *tq, task_t *task);
i32   get_next_task (tasks_queue_t *tq, task_t **retval);

void *worker (void *arg);

#endif
