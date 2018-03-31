// Copyright 2017 Johannes Heßling. All rights reserved.

#include <time.h>

#include "libUtil_Internal.h"

static char *testData[5] =
        {"Test1Block1Test1Block2Test1Block3Test1Block4Test1Block5",
         "Test2Block1Test2Block2Test2Block3Test2Block4Test2Block5",
         "Test3Block1Test3Block2Test3Block3Test3Block4Test3Block5",
         "Test4Block1Test4Block2Test4Block3Test4Block4Test4Block5",
         "Test5Block1Test5Block2Test5Block3Test5Block4Test5Block5"};

bool ForEachFree(void *data, void *u) {
    free(data);
    return true;
}

bool PrintListStr(void *d, void *userArgs) {
    printf("%s\n", (char *) d);
    return false;
}

bool sortFuncAsc(void *da, void *db);

bool sortFuncDesc(void *da, void *db);

inline bool sortFuncAsc(void *da, void *db) {
    return *((int *) da) < *((int *) db);
}

inline bool sortFuncDesc(void *da, void *db) {
    return *((int *) da) > *((int *) db);
}

bool PrintListInt(void *d, void *userArgs) {
    printf("%d\n", *(int *) d);
    return false;
}

static size_t calcListLen(List_t l) {
    assert(l);
    if (!l->head)
        return 0;
    size_t i = 0;
    for (ListNode_t ent = l->head; ent; ent = ent->next, i++);
    assert(l->len == i);
    return i;
}

static ListNode_t calcTail(ListNode_t head) {
    ListNode_t ent = head;
    while (ent->next)
        ent = ent->next;
    return ent;
}


static void ListBasicTest() {
    List_t l = ListNew(NULL);
    for (size_t i = 0; i < 5; i++) {
        ListInsert(l, i, testData[i]);
        assert(calcTail(l->head) == l->tail);
        assert(calcListLen(l) == l->len && calcListLen(l) == i + 1);
        assert(ListGet(l, i) == testData[i] && ListLen(l) == i + 1);
    }
    assert(ListLen(l) == 5 && 5 == l->len);
    assert(calcTail(l->head) == l->tail);
    assert(calcListLen(l) == l->len);

    ListClear(l);
    for (size_t i = 0; i < 5; i++) {
        ListInsert(l, i, testData[i]);
        assert(calcTail(l->head) == l->tail);
        assert(calcListLen(l) == l->len && calcListLen(l) == i + 1);
        assert(ListGet(l, i) == testData[i] && ListLen(l) == i + 1 && i + 1 == l->len);
    }
    assert(ListLen(l) == 5 && l->len == 5);
    for (size_t i = 0; i < 5; i++)
        assert(ListGet(l, i) == testData[i]);

    ListOverride(l, ListLen(l) - 1, strdup(testData[4]));
    l->freeFunc = free;
    ListOverride(l, ListLen(l) - 1, testData[4]);
    l->freeFunc = NULL;

    assert(ListLen(l) == 5 && 5 == l->len);
    assert(calcTail(l->head) == l->tail);
    assert(calcListLen(l) == l->len);

    for (size_t i = 0; i < 5; i++) {
        assert(ListGet(l, 4 - i) == testData[4 - i] && ListLen(l) == 5 - i && ListLen(l) == l->len);
        ListDelete(l, 4 - i);
    }
    assert(ListLen(l) == 0);
    for (size_t i = 0; i < 5; i++) {
        ListInsert(l, i, testData[i]);
        assert(ListGet(l, i) == testData[i] && ListLen(l) == i + 1);
    }
    for (size_t i = 0; i < 5; i++) {
        assert(ListGet(l, 0) == testData[i]);
        assert(calcTail(l->head) == l->tail);
        assert(calcListLen(l) == l->len);
        ListDelete(l, 0);
    }
    ListClear(l);
    assert(ListLen(l) == 0);
    for (size_t i = 0; i < 5; i++)
        ListAppend(l, strdup(testData[i]));

    assert(ListLen(l) == 5);
    ListForeachDelete(l, ForEachFree, NULL);
    assert(ListLen(l) == 0);
    ListFree(&l);
    assert(l == NULL);
}

static void ListTestIndex() {
    List_t l = ListNew(free);
    for (size_t i = 0; i < 5; i++)
        ListAppend(l, strdup(testData[i]));
    assert(ListLen(l) == 5);
    ListInsert(l, 0, strdup("index Null"));
    ListInsert(l, 1, strdup("index Eins"));
    ListInsert(l, 6, strdup("index Sechs"));
    ListInsert(l, ListLen(l), strdup("index End"));
    assert(!strcmp(ListGet(l, 0), "index Null"));
    assert(!strcmp(ListGet(l, 1), "index Eins"));
    assert(!strcmp(ListGet(l, 6), "index Sechs"));
    assert(!strcmp(ListGet(l, ListLen(l) - 1), "index End"));

    ListDelete(l, ListLen(l) - 1);
    ListDelete(l, 6);
    ListDelete(l, 1);
    ListDelete(l, 0);
    assert(ListLen(l) == 5 && l->len == 5);
    size_t len = 0;
    ListNode_t ent = l->head;
    for (; ent->next; ent = ent->next, len++)
        assert(!strcmp((char *) ent->data, testData[len]));
    assert(ent == l->tail);
    assert(ListLen(l) == 5 && l->len == 5);
    for (; ent->prev; ent = ent->prev, len--)
        assert(!strcmp((char *) ent->data, testData[len]));
    assert(ent == l->head);
    ListClear(l);
    assert(l->head == NULL && l->tail == NULL);
    assert(ListLen(l) == 0 && l->len == 0);
    ListFree(&l);
}

void TestListSort(size_t len) {
    List_t l = ListNew(NULL);
    time_t start, stop;
    time(&start);
    size_t randomNumbers[len];
    srand(time(NULL));
    for (size_t i = 0; i < len; i++) {
        randomNumbers[i] = rand() % len;
        ListAppend(l, &randomNumbers[i]);
    }
    ListSort(l, sortFuncAsc);
    for (size_t i = 0; i < len - 1; i++)
        assert(*(size_t *) ListGet(l, i) <= *(size_t *) ListGet(l, i + 1));
    ListSort(l, sortFuncDesc);
    for (size_t i = 0; i < len - 1; i++)
        assert(*(size_t *) ListGet(l, i) >= *(size_t *) ListGet(l, i + 1));
    ListFree(&l);
    time(&stop);
    printf("Finished  sort test of 30000 random ints in about %.0f seconds. \n", difftime(stop, start));
}


void ListTestAll() {
    ListBasicTest();
    TestListSort(30000);
    ListTestIndex();
    ListBasicTest();
}