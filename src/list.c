// Copyright 2017 Johannes Heßling. All rights reserved.

#include "libUtil_Internal.h"

inline static ListNode_t __new_node(List_t l, void *data) {
    const ListNode_t newn = calloc(1, sizeof(struct ListNode));
    assert_test(newn);
    l->len++;
    newn->data = data;
    return newn;
}

inline static void __remove_node(List_t l, ListNode_t n) {
    l->len--;
    if (n->prev && n->next) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    } else if (!n->prev && n->next) {
        n->next->prev = NULL;
        l->head = n->next;
    } else if (n->prev && !n->next) {
        n->prev->next = NULL;
        l->tail = n->prev;
    } else if (!n->prev && !n->next) {
        l->head = NULL, l->tail = NULL;
    }
    free(n);
    return;
}

inline static void __swap(void **a, void **b) {
    void *temp = *a;
    *a = *b;
    *b = temp;
}

static ListNode_t __partition(ListNode_t first, ListNode_t last, LessFunc_t f) {
    ListNode_t pivot = first->prev;
    for (ListNode_t n = first; n != last; n = n->next) {
        if (f(n->data, last->data)) {
            pivot = pivot ? pivot->next : first;
            __swap(&pivot->data, &n->data);
        }
    }
    pivot = pivot ? pivot->next : first;
    __swap(&pivot->data, &last->data);
    return pivot;
}

/* A recursive quicksort */
static void __quickSort(ListNode_t first, ListNode_t last, LessFunc_t f) {
    if (last && first != last && first != last->next) {
        const ListNode_t pivot = __partition(first, last, f);
        __quickSort(first, pivot->prev, f);
        __quickSort(pivot->next, last, f);
    }
}

inline static bool __by_index(void *data, void *index) {
    return !(*(size_t *) index)--;
}

inline static ListNode_t __Search(List_t l, ForeachFunc_t f, void *OptArgs) {
    for (ListNode_t n = LIST_ITERATE(l))
        if (f(n->data, OptArgs))
            return n;
    return NULL;
}


List_t ListNew(FreeFunc_t f) {
    const List_t l = calloc(1, sizeof(struct List));
    assert_test(l);
    l->freeFunc = f;
    return l;
}

inline void ListClear(List_t l) {
    assert_param(l);
    if (!l->head)
        return;
    ListNode_t to_free;
    while (l->head) {
        to_free = l->head;
        l->head = l->head->next;
        if (l->freeFunc)
            l->freeFunc(to_free->data);
        free(to_free);
    }
    l->head = NULL, l->tail = NULL, l->len = 0;
}

inline void ListFree(List_t *l) {
    assert_param(l && *l);
    ListClear(*l);
    free(*l);
}

inline size_t ListLen(List_t l) { return l->len; }

inline bool ListIsEmpty(List_t l) { return l->len == 0; }

inline void ListAppend(List_t l, void *data) {
    ListInsert(l, ListLen(l), data);
}

void ListInsert(List_t l, size_t index, void *data) {
    assert_param(l && data && index <= l->len);
    if (!l->head) {
        l->head = __new_node(l, data);
        l->tail = l->head;
        return;
    }
    ListNode_t n = NULL;
    if (index == l->len) {  // Push at end of List...
        n = l->tail;
        n->next = __new_node(l, data);
        n->next->prev = n;
        l->tail = n->next;
        return;
    }
    if (index == 0) {   // Push at front of List...
        n = l->head;
        l->head = __new_node(l, data);
        l->head->next = n;
        n->prev = l->head;
        return;
    }
    // Push at the indexed position of List...
    const ListNode_t newn = __new_node(l, data);
    index--; // to get the element before the indexed one
    n = __Search(l, __by_index, (void *) &index);
    newn->next = n->next;
    newn->prev = n;
    newn->next->prev = newn;
    n->next = newn;
}

void *ListGet(List_t l, size_t index) {
    assert_param(l && l->head && index < ListLen(l));
    return __Search(l, __by_index, (void *) &index)->data;
}

void ListOverride(List_t l, size_t index, void *data) {
    assert_param(l && data && index < ListLen(l));
    const ListNode_t n = __Search(l, __by_index, (void *) &index);
    if (l->freeFunc) // If the user has set a FreeFunc_t, call it on the user data
        l->freeFunc(n->data);
    n->data = data;
}

void ListDelete(List_t l, size_t index) {
    assert_param(l && l->head && index < ListLen(l));
    const ListNode_t n = __Search(l, __by_index, (void *) &index);
    if (l->freeFunc) // If the user has set a FreeFunc_t, call it on the user data
        l->freeFunc(n->data);
    __remove_node(l, n);
}

void ListForeachDelete(List_t l, ForeachFunc_t f, void *OptArgs) {
    assert_param(l && f);
    ListNode_t to_free, ent = l->head;
    while (ent) {
        to_free = ent;
        ent = ent->next;
        if (f(to_free->data, OptArgs)) {
            if (l->freeFunc) // If the user has set a FreeFunc_t, call it on the user data
                l->freeFunc(to_free->data);
            __remove_node(l, to_free);
        }
    }
}

inline void *ListForeachSearch(List_t l, ForeachFunc_t f, void *OptArgs) {
    assert_param(l && f);
    const ListNode_t n = __Search(l, f, OptArgs);
    return n ? n->data : NULL;
}

inline void ListSort(List_t l, LessFunc_t f) {
    assert_param(l && f);
    __quickSort(l->head, l->tail, f);
}
