#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tsafeq.h"

typedef struct {
    uint32_t x;
    uint64_t y;
    uint8_t  z;
} test_type_t;

TSQQ_ALLOC(bloop, test_type_t, 50);

void *f0(void *p) {
    uint32_t i = 0;
    while (i <= 200) {
        test_type_t x0;
        x0.x = i;
        x0.y = i;
        x0.z = i;
        qq_rv_t rv = TSQQ_PUSH(bloop, x0);
        usleep(100 * (rand() & 0xff));
        if (rv == QQ_OK) {
            printf("Pushed %d\n", i);
            i++;
        }
    }
    return 0;
}

void *f1(void *p) {
    qq_rv_t rv;
    test_type_t x1;
    x1.x = 0;
    do {
        rv = TSQQ_POP(bloop, x1);
        if (rv == QQ_OK) {
            printf("Popped %d\n", x1.x);
        }
    } while (x1.x != 200);
    return 0;
}

int main(int argc, char *argv[]) {

    TSQQ_INIT(bloop);

    pthread_t t0, t1;
    pthread_create(&t0, NULL, f0, NULL);
    pthread_create(&t1, NULL, f1, NULL);

    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    return 0;
}
