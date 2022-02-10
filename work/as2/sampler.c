#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#include "potDriver.h"
#include "circbuff.h"

#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

static pthread_t tid; // Thread ID
static long long totalRuns = 0;

void Sampler_setHistorySize(int newSize)
{
    //might have to build struct in circbuff.c and write into it
}


long long Sampler_getNumSamplesTaken(void)
{
    totalRuns+=1; 
    return totalRuns;
}


// Thread function to generate sum of 0 to N
void *sum_runner(void *arg)
{
    long long *limit_ptr = (long long *)arg;
    long long limit = *limit_ptr;
    long seconds = 0;
    long nanoseconds = 1000000;
    long long i = 0;

    free(arg);

    struct timespec reqDelay = {seconds, nanoseconds};

    long long sum = 0;

    while (i < limit)
    {
        int reading = getVoltage0Reading();
        double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        printf("Value %5d ==> %5.2fV\n", reading, voltage);

        nanosleep(&reqDelay, (struct timespec *)NULL);

        sum = Sampler_getNumSamplesTaken();
        i++;
    }

    // Pass back data in dynamically allocated memory
    long long *answer = malloc(sizeof(*answer));
    *answer = sum;

    pthread_exit(answer);
}

//Stop the sampling thread
void Sampler_stopSampling(void)
{
    long long *result;
    pthread_join(tid, (void **)&result);
    printf("Sum is %lld\n", *result);
    free(result);
}

// Start the sampling thread
void Sampler_startSampling(void)
{
    long long *limit = malloc(sizeof(*limit));
    *limit = 5;

    // Create attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, sum_runner, limit);
}