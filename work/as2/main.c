#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "sampler.h"
#include "potDriver.h"

#define GET_HISTORY 10


int main(int argc, char **argv)
{
    long seconds = 10;
    long nanoseconds = 1000000;
    struct timespec reqDelay = {seconds, nanoseconds};
    int volt_0_reading = getVoltage0Reading();
    int length_size = GET_HISTORY;

    Sampler_setHistorySize(volt_0_reading);
    Sampler_getHistorySize();
    Sampler_startSampling();

    nanosleep(&reqDelay, (struct timespec *)NULL);

    Sampler_getHistory(length_size);
    Sampler_getNumSamplesInHistory();
    Sampler_stopSampling();
    Sampler_getNumSamplesTaken();
}