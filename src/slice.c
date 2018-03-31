// Copyright 2017 Johannes Heßling. All rights reserved.

#include "libUtil_Internal.h"
#include <math.h>

#define  GROW_FACTOR 1.5


static inline bool __listDel(void *addrInList, void *addrToDel) {
    return addrInList == addrToDel;
}

typedef struct {
    uint8_t *old_raw_ptr;
    size_t old_cap;
    uint8_t *new_raw_ptr;
} __refsArgs_t;

static inline bool __updateSliceReferences(void *addr, void *args) {
    const __refsArgs_t *a = args;
    if (*(uint8_t **) addr >= a->old_raw_ptr && *(uint8_t **) addr <= a->old_raw_ptr + a->old_cap)
        *(uint8_t **) addr = a->new_raw_ptr + (*(uint8_t **) addr - a->old_raw_ptr);
    return false;
}

static inline void __sliceGrow(Slice_t s, size_t newSize) {
    void *new_raw_ptr = realloc(MEM(s)->raw_ptr, newSize + 1); // +1 to prevent overflows due to missing '\0';
    assert_test(new_raw_ptr);

    const __refsArgs_t args = {MEM(s)->raw_ptr, MEM(s)->capacity, new_raw_ptr};
    ListForeachSearch(MEM(s)->refs, __updateSliceReferences, (void *) &args);

    MEM(s)->raw_ptr = new_raw_ptr;
    memset(MEM(s)->raw_ptr + MEM(s)->capacity, 0, newSize - MEM(s)->capacity + 1);
    MEM(s)->capacity = newSize;
}

Slice_t SliceNewAllocate(size_t capacity) {
    const slice_internal_t s = calloc(1, sizeof(struct internalSlice));
    MEM(s) = calloc(1, sizeof(struct memoryData));
    assert_test(MEM(s) && s);

    MEM(s)->capacity = capacity;
    MEM(s)->raw_ptr = calloc(1, capacity + 1);
    MEM(s)->refs = ListNew(NULL);
    assert_test(MEM(s)->raw_ptr);

    SLC(s).ptr = MEM(s)->raw_ptr;
    ListAppend(MEM(s)->refs, (void *) &SLC(s).ptr);
    return (Slice_t) s;
}

inline Slice_t SliceNewView(Slice_t s, size_t offset, size_t len) {
    assert_param(s && offset <= s->len);
    const slice_internal_t ns = calloc(1, sizeof(struct internalSlice));
    assert_test(ns);
    MEM(ns) = MEM(s);
    SLC(ns).ptr = s->ptr + offset;
    SLC(ns).len = len;
    ListAppend(MEM(s)->refs, (void *) &SLC(ns).ptr);
    return (Slice_t) ns;
}

inline void SliceFree(Slice_t *s) {
    assert_param(s && *s);
    ListForeachDelete(MEM(*s)->refs, __listDel, &(*s)->ptr);
    if (!ListLen(MEM(*s)->refs)) {
        ListFree(&MEM(*s)->refs);
        free(MEM(*s)->raw_ptr);
        free(MEM(*s));
    }
    free(*s);
    *s = NULL;
}

inline void SliceClear(Slice_t s) {
    assert_param(s);
    s->ptr = MEM(s)->raw_ptr;
    memset(s->ptr, 0, MEM(s)->capacity);
    s->len = 0;
}

inline void SliceSet(Slice_t s, uint8_t c) {
    assert_param(s);
    memset(s->ptr, c, s->len);
}

inline void SliceWrite(Slice_t s, size_t offset, uint8_t *ptr, size_t len) {
    assert_param(s && offset <= s->len);
    if (offset + len > MEM(s)->capacity)
        __sliceGrow(s, (size_t) round((offset + len) * GROW_FACTOR));
    assert_test(offset + len <= MEM(s)->capacity);
    memcpy(s->ptr + offset, ptr, len);
    if (offset + len > s->len)
        s->len = offset + len;
}

inline void SliceAppend(Slice_t s, uint8_t *ptr, size_t len) {
    SliceWrite(s, s->len, ptr, len);
}

inline void SliceCopy(Slice_t dest, Slice_t src) {
    SliceWrite(dest, 0, src->ptr, src->len);
}

inline bool SliceEqual(Slice_t b1, Slice_t b2) {
    assert_param(b1 && b2 && b1->len == b2->len);
    return memcmp(b1->ptr, b2->ptr, b1->len) == 0;
}

inline Slice_t SliceFirstAligned(Slice_t s, size_t blockSize, size_t n) {
    assert_param(s && blockSize && s->len % blockSize == 0);
    assert_test((n > 0 ? n - 1 : n) * blockSize <= s->len);
    return SliceNewView(s, n * blockSize, blockSize);
}

inline Slice_t SliceLastAligned(Slice_t s, size_t blockSize, size_t n) {
    assert_param(s && s->len % blockSize == 0);
    size_t lastBlock = s->len / blockSize;
    lastBlock = (lastBlock > 0 ? lastBlock - 1 : lastBlock);
    return SliceFirstAligned(s, blockSize, lastBlock - n);
}