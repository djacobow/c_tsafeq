#pragma once

#define INCLUDE_TSAFE (1)

/* 
   Author: Dave Jacobowitz
   Date  : Spring 2021

   Purpose:
       Implement queue data structures with the following properties:
            0. fixed, compile-time allocation
            1. they work, basic C
            2. thread-safe and non-thread safe version
            3. non-blocking, except for the thread safe version, which can optionally
               block on mutex acquisition

       There is a function interface that will not check the types of your
       sources and destination structures. There is also a macro interface
       that also doesn't check the types, but at least checks the sizes.


   Copyright 2021, Dave Jacobowtiz
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Generic, non-thread-safe queue, using fixed storage, not blocking
// but always returning success or failure. 

typedef enum qq_rv_t {
    QQ_OK = 0,
    QQ_WAS_FULL,
    QQ_WAS_EMPTY,
    QQ_NO_LOCK,
} qq_rv_t;

typedef struct qq_t {
    uint8_t *contents;
    uint32_t front;
    uint32_t rear;
    uint32_t max_elems;
    size_t   elem_size;
} qq_t;

void     qq_init(qq_t *q, uint8_t *data, uint32_t max_elems, size_t elem_size);
qq_rv_t  qq_push(qq_t *q, void *src);
qq_rv_t  qq_pop(qq_t *q, void *dst, bool peek);
qq_rv_t  qq_full(qq_t *q);
qq_rv_t  qq_empty(qq_t *q);
uint32_t qq_size(qq_t *q);

#define _QQ_ASSERT(condition) ((void)sizeof(char[1 - 2*!(condition)]))

#define QQ_ALLOC(_NAME, _TYPE, _MAX_ELEMS) \
    static uint8_t data_##_NAME[_MAX_ELEMS * sizeof(_TYPE)]; \
    static const size_t elem_size_##_NAME = sizeof(_TYPE); \
    static const uint32_t max_elems_##_NAME = _MAX_ELEMS; \
    qq_t _NAME

#define QQ_INIT(_NAME) \
    qq_init(&_NAME, data_##_NAME, max_elems_##_NAME, elem_size_##_NAME)

#define QQ_PUSH(_NAME, _ITEM) \
    qq_push(&_NAME, &_ITEM); \
    _QQ_ASSERT(sizeof(_ITEM) == elem_size_##_NAME)

#define QQ_POP(_NAME, _DEST) \
    qq_pop(&_NAME, &_DEST, false); \
    _QQ_ASSERT(sizeof(_DEST) == elem_size_##_NAME)

#define QQ_FULL(_NAME)  qq_full(&_NAME)
#define QQ_EMPTY(_NAME) qq_empty(&_NAME)
#define QQ_SIZE(_NAME)  qq_size(&_NAME)



#ifdef INCLUDE_TSAFE




#include <pthread.h>

typedef struct tsqq_t {
    qq_t q;
    pthread_mutex_t mtx;
    pthread_cond_t  cnd;    
} tsqq_t;

void     tsqq_init(tsqq_t *q, uint8_t *data, uint32_t max_elems, size_t elem_size);
qq_rv_t  tsqq_push(tsqq_t *q, void *src);
qq_rv_t  tsqq_push_noblock(tsqq_t *q, void *src);
qq_rv_t  tsqq_pop(tsqq_t *q, void *dst, bool peek);         // waits on mutex
qq_rv_t  tsqq_pop_noblock(tsqq_t *q, void *dst, bool peek); // waits on nothin
qq_rv_t  tsqq_pop_block(tsqq_t *q, void *dst, bool peek);   // waits on mutex and non-emptiness
qq_rv_t  tsqq_full(tsqq_t *q);
qq_rv_t  tsqq_empty(tsqq_t *q);
uint32_t tsqq_size(tsqq_t *q);

#define TSQQ_ALLOC(_NAME, _TYPE, _MAX_ELEMS) \
    static uint8_t        data_##_NAME[_MAX_ELEMS * sizeof(_TYPE)] __attribute__((aligned(16))); \
    static const size_t   elem_size_##_NAME = sizeof(_TYPE); \
    static const uint32_t max_elems_##_NAME = _MAX_ELEMS; \
    tsqq_t _NAME

#define TSQQ_INIT(_NAME) \
    tsqq_init(&_NAME, data_##_NAME, max_elems_##_NAME, elem_size_##_NAME)

#define TSQQ_PUSH(_NAME, _ITEM) \
    tsqq_push(&_NAME, &_ITEM); \
    _QQ_ASSERT(sizeof(_ITEM) == elem_size_##_NAME)

#define TSQQ_PUSH_NOBLOCK(_NAME, _ITEM) \
    tsqq_push_noblock(&_NAME, &_ITEM); \
    _QQ_ASSERT(sizeof(_ITEM) == elem_size_##_NAME)

#define TSQQ_POP(_NAME, _DEST) \
    tsqq_pop(&_NAME, &_DEST, false); \
    _QQ_ASSERT(sizeof(_DEST) == elem_size_##_NAME)

#define TSQQ_POP_NOBLOCK(_NAME, _DEST) \
    tsqq_pop_noblock(&_NAME, &_DEST, false); \
    _QQ_ASSERT(sizeof(_DEST) == elem_size_##_NAME)

#define TSQQ_POP_BLOCK(_NAME, _DEST) \
    tsqq_pop_block(&_NAME, &_DEST, false); \
    _QQ_ASSERT(sizeof(_DEST) == elem_size_##_NAME)

#define TSQQ_FULL(_NAME)  tsqq_full(&_NAME)
#define TSQQ_EMPTY(_NAME) tsqq_empty(&_NAME)
#define TSQQ_SIZE(_NAME)  tsqq_size(&_NAME)

#endif // INCLUDE_TSAFE
