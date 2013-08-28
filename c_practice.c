#include <stdio.h>

int square (int a)
{
    return a * a;
}

int sum (int a, int b)
{
    return a + b;
}

int main (int argc, char **argv)
{

    int a = (1, -2);
    int b = (a = (1, -3)) - 3; 
    int c = sum ( (b = sum (4, 5), a = square ( sum (1, (2, 4)) )), 3);

    printf ("a: %d\n", a);
    printf ("b: %d\n", b);
    printf ("c: %d\n", c);

    return (1, -1);
}
