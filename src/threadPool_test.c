// Copyright 2017 Johannes Heßling. All rights reserved.

#include "libUtil_Internal.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define SCALE 10000
#define ARRINIT 2000

char *pi_digits(int digits) {
    int carry = 0;
    char pi[digits + 1];
    memset(pi, 0, digits + 1);
    int arr[digits + 1];
    for (int i = 0; i <= digits; ++i)
        arr[i] = ARRINIT;
    for (int i = digits, j = 0; i > 0; i -= 14, j += 4) {
        int sum = 0;
        for (int j = i; j > 0; --j) {
            sum = sum * j + SCALE * arr[j];
            arr[j] = sum % (j * 2 - 1);
            sum /= j * 2 - 1;
        }
        sprintf(pi + j, "%04d", carry + sum / SCALE);
        carry = sum % SCALE;
    }
    return strdup(pi);
}

void *Thread(void *l) {
    sleep((unsigned int) rand() % 4);
    printf("Started Thread %ld %s\n", pthread_self() % 100, (char *) l);
    //   char *pi = pi_digits(100000);
    //  printf("Thread %ld %s -> pi : %.8s...\n", pthread_self() % 100, (char *) l, pi);
    //  free(pi);
    return NULL;
}

void TestThread1() {
    srand((unsigned int) time(NULL));
    ThreadPool_t tp = ThreadPoolNew(4);
    char *l[] = {"Test1", "Test2", "Test3", "Test4", "Test5", "Test6", "Test7", "Test8", "Test9", "Test10", "Test11",
                 "Test12", "Test13", "Test14", "Test15", "Test16", "Test17", "Test18", "Test19", "Test20", "Test21",
                 "Test22"};
    for (int i = 0; i < 22; i++) {
        ThreadPoolAddJob(tp, Thread, l[i]);
    }
    ThreadPoolFree(&tp);
}
