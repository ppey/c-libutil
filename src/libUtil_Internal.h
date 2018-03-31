// Copyright 2017 Johannes Heßling. All rights reserved.

#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../libUtilDataStructs.h"
#include "../libUtilTheading.h"

#define useAssertParam
#define useAssertTest

// Used for validation of function parameters
#ifdef useAssertParam
#define assert_param(condition) assert(condition)
#else
#define assert_param(condition) \
    if(!(condition)){ \
        DEBUG_PRINT("Wrong Parameters %s", "\n"); \
}
#endif

// Used everywhere else
#ifdef useAssertTest
#define assert_test(condition) assert(condition)
#else
#define assert_test(condition) ;
#endif

#define free(ptr) free((ptr)); (ptr)=NULL

typedef struct internalSlice {
    struct sliceData slice;
    struct memoryData {
        size_t capacity;
        uint8_t *raw_ptr;
        List_t refs;
    } *memoryRegion;
} *slice_internal_t;

#define MEM(s)  (((slice_internal_t) s)->memoryRegion)
#define SLC(s)  (((slice_internal_t) s)->slice)

#define LIST_ITERATE(l) l->head; n ; n = n->next
// for (ListNode_t ent= LIST_ITERATE(l));

#define SlICE_ITERATE(buffer) 0; i  <= buffer->len; i++
// for (size_t index= SlICE_ITERATE(s));

void ListTestAll();

void TestThread1();