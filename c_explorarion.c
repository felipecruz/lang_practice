#include "explore.h"

/*run(($(a) = 1,
     $(b) = square ( $(a) ),
     sum (($(c) = sum (4, 5),
           $(a) = square (sum (1, 2) )), 3));
    printf ("a: %d\n", $(a));
    printf ("b: %d\n", $(b));
    printf ("c: %d\n", $(c));
)*/

int main (int a, char **b)
{
    init_heap();

    return (($(a) = 2, printf ("%d\n", $(a)),
     $(b) = square ( $(a) ? 3:1 ),
     sum (($(c) = sum (4, 6),
           $(a) = square (sum (1, 2) )), 3)),
    _("a: %d\n", $(a)),
    _("b: %d\n", $(b)),
    _("c: %d\n", $(c)),
    1);

}
