// Copyright 2017 Johannes Heßling. All rights reserved.

#pragma once

#include <stdio.h>
#include <inttypes.h>

#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args)

typedef bool (*ForeachFunc_t)(void *data, void *OptArgs);

typedef bool (*LessFunc_t)(void *dataA, void *dataB);

typedef void (*FreeFunc_t)(void *data);


typedef struct sliceData {
    size_t len;
    uint8_t *ptr;
} *Slice_t;

Slice_t SliceNewAllocate(size_t capacity);

Slice_t SliceNewView(Slice_t s, size_t offset, size_t len);

void SliceFree(Slice_t *s);

void SliceClear(Slice_t s);

void SliceSet(Slice_t s, uint8_t c);

bool SliceEqual(Slice_t s1, Slice_t s2);

void SliceCopy(Slice_t dest, Slice_t src);

void SliceAppend(Slice_t s, uint8_t *ptr, size_t len);

void SliceWrite(Slice_t s, size_t offset, uint8_t *ptr, size_t len);

Slice_t SliceLastAligned(Slice_t s, size_t blockSize, size_t n);

Slice_t SliceFirstAligned(Slice_t s, size_t blockSize, size_t n);


typedef struct Stack {
    Slice_t stackMem;
} *Stack_t;

Stack_t StackNew();

void StackFree(Stack_t *s);

void StackPush(Stack_t s, void *data);

void *StackPop(Stack_t s);

size_t StackLen(Stack_t s);

void StackForEach(Stack_t s, ForeachFunc_t func, void *OptArgs);

bool StackIsEmpty(Stack_t s);


typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
    void *data;
} *ListNode_t;

typedef struct List {
    ListNode_t head;
    ListNode_t tail;
    FreeFunc_t freeFunc;
    size_t len;
} *List_t;

List_t ListNew(FreeFunc_t f);

void ListFree(List_t *l);

size_t ListLen(List_t l);

bool ListIsEmpty(List_t l);

void ListAppend(List_t l, void *data);

void *ListGet(List_t l, size_t index);

void ListInsert(List_t l, size_t index, void *data);

void ListClear(List_t l);

void ListDelete(List_t l, size_t index);

void ListOverride(List_t l, size_t index, void *data);

void ListForeachDelete(List_t l, ForeachFunc_t f, void *OptArgs);

void *ListForeachSearch(List_t l, ForeachFunc_t f, void *OptArgs);

void ListSort(List_t l, LessFunc_t f);

typedef struct Map {
    List_t l;
    FreeFunc_t freeFunc;
} *Map_t;

Map_t MapNew(FreeFunc_t freeFunc);

void MapFree(Map_t *m);

void MapSet(Map_t m, const char *label, void *data);

void *MapGet(Map_t m, const char *label);

void MapDelete(Map_t m, const char *label);

typedef void (*ForeachFuncMap_t)(const char *label, void *data, void *OptArgs);

void MapForeach(Map_t m, ForeachFuncMap_t f, void *OptArgs);

size_t MapLen(Map_t m);