#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "sampler.h"

#define MAX_SIZE 100



int main(int argc, char **argv)
{
    long seconds = 3;
    long nanoseconds = 1000000;
    struct timespec reqDelay = {seconds, nanoseconds};

    Sampler_setHistorySize(20);
    Sampler_getHistorySize();
    Sampler_startSampling();
    Sampler_getHistory(MAX_SIZE);

    nanosleep(&reqDelay, (struct timespec *)NULL);

    Sampler_stopSampling();
    Sampler_getNumSamplesTaken();
}