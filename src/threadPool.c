// Copyright 2017 Johannes Heßling. All rights reserved.

#include "libUtil_Internal.h"
#include <pthread.h>

#define LOCK(obj)   pthread_mutex_lock(&(obj)->mutex);
#define UNLOCK(obj) pthread_mutex_unlock(&(obj)->mutex);


Channel_t ChannelNew() {
    const Channel_t chan = calloc(1, sizeof(struct Channel));
    assert_test(chan);
    chan->data = ListNew(NULL);
    assert_test(!pthread_mutex_init(&chan->mutex, NULL));
    assert_test(!pthread_cond_init(&chan->empty, NULL));
    return chan;
}

void ChannelFree(Channel_t *c) {
    assert_param(c && *c && (*c)->closed);
    LOCK(*c);
    ListFree(&(*c)->data);
    UNLOCK(*c);
    pthread_mutex_destroy(&(*c)->mutex);
    pthread_cond_destroy(&(*c)->empty);
    free(*c);
}

void ChanPut(Channel_t c, void *data) {
    assert_param(c && data && !c->closed);
    LOCK(c);
    ListAppend(c->data, data);
    pthread_cond_signal(&c->empty);
    UNLOCK(c);
}

void *ChanGet(Channel_t c) {
    assert_param(c);
    LOCK(c);
    while (ListIsEmpty(c->data)) {
        if (c->closed) {
            pthread_cond_broadcast(&c->empty);
            UNLOCK(c);
            return NULL;
        }
        pthread_cond_wait(&c->empty, &c->mutex);
    }
    void *data = ListGet(c->data, 0);
    ListDelete(c->data, 0);
    UNLOCK(c);
    return data;
}

void ChanClose(Channel_t c) {
    assert_param(c);
    LOCK(c);
    c->closed = true;
    pthread_cond_signal(&c->empty);
    while (!ListIsEmpty(c->data))
        pthread_cond_wait(&c->empty, &c->mutex);
    UNLOCK(c);
}

typedef struct Job {
    WorkFunc_t f;
    void *Args;
} *Job_t;

static void __threadHandler(Channel_t jobs) {
    while (true) {
        Job_t job = ChanGet(jobs);
        if (!job)
            return;
        job->f(job->Args);
        free(job);
    }
}

void ThreadPoolAddJob(ThreadPool_t tp, WorkFunc_t f, void *Args) {
    assert_param(tp && f);
    LOCK(tp);
    const Job_t job = calloc(1, sizeof(struct Job));
    assert_test(job);
    job->f = f, job->Args = Args;
    ChanPut(tp->work, job);
    UNLOCK(tp);
}

ThreadPool_t ThreadPoolNew(uint8_t NumThread) {
    assert_param(NumThread > 0);
    const ThreadPool_t tp = calloc(1, sizeof(struct ThreadPool));
    assert_test(tp);
    tp->work = ChannelNew();
    tp->threads = calloc(NumThread, sizeof(pthread_t));
    assert_test(tp->threads);
    assert_test(!pthread_mutex_init(&tp->mutex, NULL));
    tp->numThreads = NumThread;
    for (int i = 0; i < NumThread; i++)
        pthread_create(&tp->threads[i], NULL, (WorkFunc_t) __threadHandler, tp->work);
    return tp;
}


void ThreadPoolFree(ThreadPool_t *tp) {
    assert_param(tp && *tp);
    LOCK(*tp);
    ChanClose((*tp)->work);
    for (int i = 0; i < (*tp)->numThreads; i++)
        pthread_join((*tp)->threads[i], NULL);
    ChannelFree(&(*tp)->work);
    UNLOCK(*tp);
    pthread_mutex_destroy(&(*tp)->mutex);
    free((*tp)->threads);
    free(*tp);
}




