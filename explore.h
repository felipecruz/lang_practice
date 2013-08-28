#include <stdlib.h>
#include <stdio.h>

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

int *resolve (char *key) {
    static int first = 0;
    static int *a, *b, *c;
    switch (first) {
        case 0: goto L0;
        case 1: goto L1;
    }

L0:
    a = malloc (sizeof (int));
    b = malloc (sizeof (int));
    c = malloc (sizeof (int));
    first = 1;

L1:
    if (strcmp(key, "a") == 0)
        return a;
    if (strcmp(key, "b") == 0)
        return b;
    if (strcmp(key, "c") == 0)
        return c;


    return 0;
}
