#ifndef THREADS_H_
#define THREADS_H_

#include "common.h"
#include <pthread.h>

#define THREADS_COUNT 2

typedef void *(*thrd_func_t) (void *);

typedef struct task_s {
    struct task_s *next;
    thrd_func_t    func;
    void          *args;
} task_t;

typedef struct {
    pthread_mutex_t mutex;
    task_t         *head;
    task_t         *tail;
} tasks_list_t;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond_work;
    pthread_cond_t  cond_listen;
    tasks_list_t    list;
    pthread_t      *thrds;
    size_t          thrds_cnt;
    u32             thrds_free;
    u8              shutdown;
} thrd_pool_t;

extern pthread_mutex_t print_mutex;

/**/

i32   thrd_pool_create (thrd_pool_t *tp, size_t thrds_cnt);
i32   thrd_pool_destroy (thrd_pool_t *tp);

i32   tasks_list_create (tasks_list_t *tl);
i32   tasks_list_destroy (tasks_list_t *tl);
i32   tasks_list_pop (tasks_list_t *tl, task_t **retval);
i32   tasks_list_push (tasks_list_t *tl, task_t *task);
i32   tasks_list_empty (const tasks_list_t *tl);

void *worker (void *arg);

#endif
