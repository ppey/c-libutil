// Copyright 2017 Johannes Heßling. All rights reserved.


#include "libUtil_Internal.h"

typedef struct MapEntry {
    char *label;
    void *data;
} *mapEntry_t;

typedef struct {
    const char *label;
    const FreeFunc_t free;
} args_t;

static bool __delete(void *me, void *_args) {
    const args_t *args = _args;
    if (args->label && strcmp(((mapEntry_t) me)->label, (char *) args->label))
        return false;
    free(((mapEntry_t) me)->label);
    if (args->free) // If the user has set a FreeFunc_t, call it on the user data
        args->free(((mapEntry_t) me)->data);
    return true;
}

static bool __search(void *me, void *label) {
    return !strcmp(((mapEntry_t) me)->label, (char *) label);
}

Map_t MapNew(FreeFunc_t freeFunc) {
    const Map_t m = calloc(1, sizeof(struct Map));
    assert_test(m);
    m->l = ListNew(free);
    m->freeFunc = freeFunc;
    return m;
}

void MapFree(Map_t *m) {
    assert_param(m && *m);
    const args_t args = {NULL, (*m)->freeFunc};
    ListForeachDelete((*m)->l, __delete, (void *) &args);
    ListFree(&((*m)->l));
    free(*m);
    *m = NULL;
}

void MapSet(Map_t m, const char *label, void *data) {
    assert_param(m && label && data);
    mapEntry_t me = (mapEntry_t) ListForeachSearch(m->l, __search, (void *) label);
    if (me) { // Override it...
        if (m->freeFunc)
            m->freeFunc(me->data);
        me->data = data;
        return;
    }
    // Insert new entry
    me = calloc(1, sizeof(struct MapEntry));
    assert_test(me);
    me->label = strdup(label);
    assert_test(me->label);
    me->data = data;
    ListAppend(m->l, me);
}

void *MapGet(Map_t m, const char *label) {
    assert_param(m && label);
    mapEntry_t me = (mapEntry_t) ListForeachSearch(m->l, __search, (void *) label);
    return me ? me->data : NULL;
}

inline void MapDelete(Map_t m, const char *label) {
    assert_param(m && label);
    const args_t args = {label, m->freeFunc};
    ListForeachDelete(m->l, __delete, (void *) &args);
}

inline size_t MapLen(Map_t m) {
    return ListLen(m->l);
}

typedef struct {
    ForeachFuncMap_t func;
    void *OptArgs;
} forEachArgs_t;

static inline bool __forEach(void *me, void *__args) {
    const forEachArgs_t *args = __args;
    args->func(((mapEntry_t) me)->label, ((mapEntry_t) me)->data, args->OptArgs);
    return false;
}

void MapForeach(Map_t m, ForeachFuncMap_t f, void *OptArgs) {
    assert_param(m && f);
    const forEachArgs_t args = {f, OptArgs};
    ListForeachSearch(m->l, __forEach, (void *) &args);
}
