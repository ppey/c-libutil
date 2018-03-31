// Copyright 2017 Johannes Heßling. All rights reserved.


#include "libUtil_Internal.h"

static char *testData[5] =
        {"Test1Block1Test1Block2Test1Block3Test1Block4Test1Block5",
         "Test2Block1Test2Block2Test2Block3Test2Block4Test2Block5",
         "Test3Block1Test3Block2Test3Block3Test3Block4Test3Block5",
         "Test4Block1Test4Block2Test4Block3Test4Block4Test4Block5",
         "Test5Block1Test5Block2Test5Block3Test5Block4Test5Block5"};

void TestSlice() {
    size_t len = strlen(testData[1]);
    Slice_t slice = NULL, sliceCopy = NULL, blockView = NULL, view = NULL;
    char *result = calloc(1, 5 * len + 1);
    slice = SliceNewAllocate(1);

    for (int i = 0; i < 5; i++) {
        SliceAppend(slice, (uint8_t *) testData[i], len);
        assert(slice->len == (i + 1) * len);
        strcat(result, testData[i]);
    }
    assert(!strcmp((char *) slice->ptr, result));

    view = SliceNewView(slice, 0, slice->len);
    assert(!strcmp((char *) view->ptr, result));
    assert(view->len == slice->len);
    assert(view->ptr == slice->ptr);
    SliceFree(&slice);
    assert(!strcmp((char *) view->ptr, result));

    for (int i = 0; i < 5; i++) {
        blockView = SliceFirstAligned(view, len, i);
        assert(strncmp((char *) blockView->ptr, testData[i], len) == 0);
        SliceFree(&blockView);
    }

    sliceCopy = SliceNewAllocate(0);
    SliceCopy(sliceCopy, view);

    assert(strcmp((char *) sliceCopy->ptr, result) == 0);
    assert(SliceEqual(sliceCopy, view));

    SliceWrite(sliceCopy, 0, (uint8_t *) "aw", 2);
    assert(!SliceEqual(sliceCopy, view));

    free(result);
    SliceFree(&view);
    SliceFree(&sliceCopy);
    assert(sliceCopy == NULL);
}

void TestSliceAppend() {
    size_t len = strlen(testData[1]);
    Slice_t slice = SliceNewAllocate(1), view = SliceNewView(slice, 0, 0);

    char *result = calloc(1, 5 * len + 1);
    for (int i = 0; i < 5; i++) {
        SliceAppend(slice, (uint8_t *) testData[i], len);
        assert(slice->len == (i + 1) * len);
        strcat(result, testData[i]);
    }
    assert(view->len == 0);
    assert(!strcmp((char *) slice->ptr, result));
    assert(!strcmp((char *) view->ptr, result));

    free(result);
    result = calloc(1, 5 * len + 1);

    for (int i = 4; i >= 0; i--) {
        SliceAppend(view, (uint8_t *) testData[i], len);
        assert(view->len == ((4 - i) + 1) * len);
        strcat(result, testData[i]);
    }
    assert(!strcmp((char *) slice->ptr, result));
    assert(!strcmp((char *) view->ptr, result));

    Slice_t block = SliceFirstAligned(view, len, 2);/**/
    assert(!strncmp((char *) block->ptr, testData[2], len));

    for (int i = 4; i >= 0; i--)
        SliceAppend(view, (uint8_t *) testData[i], len);
    assert(!strncmp((char *) block->ptr, testData[2], len));

    free(result);
    SliceFree(&view);
    SliceFree(&block);
    SliceFree(&slice);
}

bool testForEarchStak(void *data, void *arg) {
    size_t *index = (size_t *) arg;
    assert(data == testData[*index]);
    (*index)++;
    return false;
}

void TestStack() {
    Stack_t stack = StackNew();
    for (int i = 0; i < 5; i++)
        StackPush(stack, (uint8_t *) testData[i]);
    assert(StackLen(stack) == 5);

    for (int i = 4; i >= 0; i--)
        assert ((uint8_t *) testData[i] == StackPop(stack));
    assert(StackPop(stack) == NULL && StackLen(stack) == 0);

    for (int i = 0; i < 5; i++)
        StackPush(stack, (uint8_t *) testData[i]);

    size_t index = 0;
    StackForEach(stack, testForEarchStak, (void *) &index);

    StackFree(&stack);
    assert(stack == NULL);
}

void TestMap() {
    Map_t map = MapNew(free);
    assert(MapGet(map, "key") == NULL);
    char *keys[5] = {"key1", "key2", "key3", "key4", "key5"};
    for (size_t i = 0; i < 5; i++) {
        MapSet(map, keys[i], strdup(testData[i]));
        assert(!strcmp(MapGet(map, keys[i]), testData[i]) && MapLen(map) == i + 1);
        MapSet(map, keys[i], testData[i]);
    }
    map->freeFunc = NULL;
    MapDelete(map, "key1");
    assert(MapGet(map, "key1") == NULL);
    MapFree(&map);
    assert(map == NULL);
}


int main() {
    printf("Running tests...\n");
    TestSlice();
    TestSliceAppend();
    ListTestAll();
    TestStack();
    TestMap();
    TestThread1();
    printf("Test done... \n");
    TestSlice();
    return 0;
}
