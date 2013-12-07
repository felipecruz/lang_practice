void print_int (int x)
{
    printf ("%d\n", x);
}

void swap (int *numbers, int x, int y)
{
    int c;
    c = numbers[x];
    numbers[x] = numbers[y];
    numbers[y] = c;
}

void quicksort (int *numbers, int begin, int end) {
    int original_begin;
    int pivot, size;
    int left_size, middle;

    original_begin = begin;
    size = end - begin;
    middle = begin + (size/2);

    if (size > 1) {
        pivot = numbers[middle];
        left_size = begin;
        swap (numbers, middle, end - 1);

        while (begin < end) {
            if (numbers[begin] < pivot) {
                swap (numbers, begin, left_size);
                left_size = left_size + 1;
            }
            begin = begin + 1;
        }
            
        swap (numbers, left_size, end - 1);

        quicksort (numbers, original_begin, left_size);
        quicksort (numbers, left_size + 1, end);
    }
}

void test_quicksort () {
    int max;
    int n;
    int *numbers;

    max = 50000;
    n = 0;
    numbers = malloc (sizeof (int) * 50000);
    while (max > 0) {
        numbers[n] = max;
        n = n + 1;
        max = max - 1;
    }
    
    max = 50000;

    quicksort (numbers, 0, max);

    n = 0;
    while (n < max) {
        print_int (numbers [n]);
        n = n + 1;
    }

}

int main(int argc, char **argv) {
    int x;
    
    x = 50 / 2;
/*
    array = new int[3];
    array[0] = 3;
    array[1] = 1;
    array[3] = 2;
    print_int (array[0]);
    print_int (array[1]);

    swap (array, 0, 1);

    print_int (array[0]);
    print_int (array[1]);
    quicksort (array, 0, 3);

    print_int (array[0]);
    print_int (array[1]);
    print_int (array[2]);
*/

    test_quicksort ();

    return 1;
}
