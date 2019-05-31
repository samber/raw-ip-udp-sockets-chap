
#include <stdio.h>
#include "mychap.h"

void show_mem(void *ptr, size_t len)
{
    printf("Memory:");
    for (size_t i = 0; i < len; i++)
    {
        if (i % 4 == 0)
            printf("\n%d\t%p\t", (int)i, ptr + i);
        printf(" %.2x", *((unsigned char *)ptr + i));
    }
    printf("\n\n");
}
