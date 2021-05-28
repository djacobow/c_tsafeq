/* 
   Author: Dave Jacobowitz
   Date  : Spring 2021

   Copyright 2021, Dave Jacobowtiz
*/


#include <stdint.h>
#include "tsafeq.h"

void qq_init(qq_t *q, uint8_t *data, uint32_t max_elems, size_t elem_size) {
    memset(q, 0, sizeof(qq_t));
    q->contents  = data;
    q->max_elems = max_elems;
    q->elem_size = elem_size;
}

qq_rv_t qq_push(qq_t *q, void *src) {
    int fullness = q->front - q->rear;
    if (fullness < 0) {
        fullness += q->max_elems;
    }
    if (fullness < (q->max_elems-1)) {
        memcpy(q->contents + q->front++ * q->elem_size, src, q->elem_size);
        if (q->front == q->max_elems) {
            q->front = 0;
        }
        return QQ_OK;
    }
    return QQ_WAS_FULL;
}

qq_rv_t qq_pop(qq_t *q, void *dst, bool peek) {
    if (q->rear == q->front) {
        return QQ_WAS_EMPTY;
    }
    memcpy(dst, q->contents + q->rear * q->elem_size, q->elem_size);
    if (!peek) {
        q->rear++;
        if (q->rear == q->max_elems) {
            q->rear = 0;
        }
    }
    return QQ_OK;
}


qq_rv_t qq_full(qq_t *q) {
    int fullness = q->front - q->rear;
    if (fullness < 0) {
        fullness += q->max_elems;
    }
    return (fullness == (q->max_elems-1)) ? QQ_WAS_FULL : QQ_OK;
}

qq_rv_t qq_empty(qq_t *q) {
    return (q->front == q->rear) ? QQ_WAS_EMPTY : QQ_OK;
}

uint32_t qq_size(qq_t *q) {
    int fullness = q->front - q->rear;
    if (fullness < 0) {
        fullness += q->max_elems;
    }
    return fullness;
}



#ifdef INCLUDE_TSAFE

void tsqq_init(tsqq_t *q, uint8_t *data, uint32_t max_elems, size_t elem_size) {
    memset(q, 0, sizeof(tsqq_t));
    qq_init(&q->q, data, max_elems, elem_size);
    pthread_mutex_init(&q->mtx, 0);
    pthread_cond_init(&q->cnd, 0);
}

qq_rv_t tsqq_push(tsqq_t *q, void *src) {
    pthread_mutex_lock(&q->mtx);
    qq_rv_t rv = qq_push(&q->q, src);
    pthread_mutex_unlock(&q->mtx);
    pthread_cond_signal(&q->cnd);
    return rv;
}

qq_rv_t tsqq_push_noblock(tsqq_t *q, void *src) {
    int failed = pthread_mutex_trylock(&q->mtx);
    if (failed) {
        return QQ_NO_LOCK;
    }
    qq_rv_t rv = qq_push(&q->q, src);
    pthread_mutex_unlock(&q->mtx);
    pthread_cond_signal(&q->cnd);
    return rv;
}

qq_rv_t tsqq_pop(tsqq_t *q, void *dst, bool peek) {
    pthread_mutex_lock(&q->mtx);
    qq_rv_t rv = qq_pop(&q->q, dst, peek);
    pthread_mutex_unlock(&q->mtx);
    return rv; 
}

qq_rv_t tsqq_pop_noblock(tsqq_t *q, void *dst, bool peek) {
    int failed = pthread_mutex_trylock(&q->mtx);
    if (failed) {
        return QQ_NO_LOCK;
    }
    qq_rv_t rv = qq_pop(&q->q, dst, peek);
    pthread_mutex_unlock(&q->mtx);
    return rv; 
}

qq_rv_t tsqq_full(tsqq_t *q) {
    pthread_mutex_lock(&q->mtx);
    qq_rv_t rv = qq_full(&q->q);
    pthread_mutex_unlock(&q->mtx);
    return rv;
}

qq_rv_t tsqq_empty(tsqq_t *q) {
    pthread_mutex_lock(&q->mtx);
    qq_rv_t rv = qq_empty(&q->q);
    pthread_mutex_unlock(&q->mtx);
    return rv;
}

uint32_t tsqq_size(tsqq_t *q) {
    pthread_mutex_lock(&q->mtx);
    uint32_t rv = qq_size(&q->q);
    pthread_mutex_unlock(&q->mtx);
    return rv;

}
#endif

