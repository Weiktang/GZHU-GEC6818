#include <stdio.h>

typedef unsigned char           uint8_t;

void memory(void* ptr, int len)
{
    printf("%p\n", ptr);
    for(int i = 0;i < len;i++)
    {
        if((i % 8 == 0) && (i != 0))
        {
            printf(" ");
        }
        if((i % 16 == 0) && (i != 0))
        {
            printf("\n");
        }
        printf("%0.2x",*((uint8_t*)ptr + i));
    }
    printf("\n");
}