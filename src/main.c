#include <stdio.h>
#include "allocator.h"


int main()
{
    int *arr = malloc(5 * sizeof(int));

    for (int i = 0; i < 5; i++)
        arr[i] = i * 10;

    for (int i = 0; i < 5; i++)
        printf("%d ", arr[i]);

    printf("\n");

    free(arr);
    return 0;
}