// Copyright 2017 Johannes Heßling. All rights reserved.

#pragma once

#include "libUtilDataStructs.h"

typedef struct Channel {
    List_t data;
    bool closed;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
} *Channel_t;

Channel_t ChannelNew();

void ChannelJoinFree(Channel_t *c);

void ChanPut(Channel_t c, void *data);

void *ChanGet(Channel_t c);

void ChanClose(Channel_t c);


typedef void *(*WorkFunc_t)(void *Args);

typedef struct ThreadPool {
    Channel_t work;
    uint8_t numThreads;
    pthread_mutex_t mutex;
    pthread_t *threads;
} *ThreadPool_t;

ThreadPool_t ThreadPoolNew(uint8_t NumThread);

void ThreadPoolFree(ThreadPool_t *tp);

void ThreadPoolAddJob(ThreadPool_t tp, WorkFunc_t f, void *Args);
