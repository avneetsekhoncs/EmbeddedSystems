#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "potDriver.h"

#define N 1000


void circularBuffer()
{
    long long *buff_array = (long long *)malloc(sizeof(long long)* N + 1); // Dynamically allocate an array
    int nextId = 0;

    if (buff_array == NULL)
    {
        printf("Malloc failed!\n");
        exit(1);                                   
    }

    buff_array[nextId] = getVoltage0Reading();
    nextId = (nextId + 1) % N;

}