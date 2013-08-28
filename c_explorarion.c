#include <stdlib.h>
#include <stdio.h>

#define $(name) \
    *((int*)resolve(#name))

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

int main (int argc, char **argv)
{
    ($(a) = 1,
     $(b) = square ( $(a) ),
     sum (($(c) = sum (4, 5),
           $(a) = square (sum (1, 2) )), 3));

    printf ("a: %d\n", $(a));
    printf ("b: %d\n", $(b));
    printf ("c: %d\n", $(c));
}
