#include <stdlib.h>
#include <stdio.h>

#include "hash.c"

#define _(a, b) \
    printf (a, b)

#define $(name) \
    *((int*)resolve(#name))

#define run(exp) \
    int main (int argc, char **argv) \
    { do { ##exp } while (0); \
        return 1; }

int square (int a)
{
    return a * a;
}

int sum (int a, int b)
{
    return a + b;
}

static hash *HEAP;

void init_heap ()
{
    HEAP = hash_init (1024);
}

int *resolve (uint8_t *key) {
    int *pt;
    helem *el;

    el = hash_get (HEAP, key);

    if (!el) {
        pt = malloc (sizeof (int));
        hash_put (HEAP, key, pt, sizeof (int*));
    } else {
        return el->value;
    }

    return pt;
}
