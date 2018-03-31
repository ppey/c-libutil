// Copyright 2017 Johannes Heßling. All rights reserved.


#include "libUtil_Internal.h"

#define INITIAL_SIZE 64

Stack_t StackNew() {
    const Stack_t s = calloc(1, sizeof(struct Stack));
    assert_test(s);
    s->stackMem = SliceNewAllocate(INITIAL_SIZE);
    return s;
}

void StackFree(Stack_t *s) {
    assert_param(s && *s);
    SliceFree(&((*s)->stackMem));
    free(*s);
}

inline void StackPush(Stack_t s, void *addr) {
    assert_param(s && addr);
    SliceAppend(s->stackMem, (void *) &addr, sizeof(void *));
}

inline void *StackPop(Stack_t s) {
    assert_param(s);
    if (!s->stackMem->len) {
        SliceClear(s->stackMem);
        return NULL;
    }
    void *addr = NULL;
    Slice_t block = SliceLastAligned(s->stackMem, sizeof(void *), 0);
    addr = *((void **) block->ptr);
    s->stackMem->len -= sizeof(void *);
    SliceFree(&block);
    return addr;
}

inline void StackForEach(Stack_t s, ForeachFunc_t func, void *OptArgs) {
    assert_param(s);
    for (size_t i = 0; i < StackLen(s); i++) {
        Slice_t slc = SliceFirstAligned(s->stackMem, sizeof(void *), i);
        func(*((void **) slc->ptr), OptArgs);
        SliceFree(&slc);
    }
    return;
}

inline size_t StackLen(Stack_t s) {
    assert_param(s);
    return s->stackMem->len / sizeof(void *);
}

inline bool StackIsEmpty(Stack_t s) {
    return StackLen(s) == 0;
}



