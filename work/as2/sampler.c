#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#include "potDriver.h"

#define MAX_SIZE 100
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define DSP_EMA_I32_ALPHA(x) ((double)(x * 65535)) // taken from: https://www.embeddedrelated.com/showcode/304.php

static pthread_t tid;
static double *buff_array;
static int history_size;
static int nextId = 0;
static bool sample = true;
static long long sum = 0;
static double average = 0;

// Count Command
long long Sampler_getNumSamplesTaken(void)
{
    printf("Sum is %lld\n", sum);
    return sum;
}

double dsp_ema_i32(double in, double average, double alpha) // Taken from: https://www.embeddedrelated.com/showcode/304.php
{
    double tmp0;
    tmp0 = (double)in * (alpha) + (double)average * (65536 - alpha);
    return (double)((tmp0 + 32768) / 65536);
}

double Sampler_getAverageReading(void) // Taken from: https://www.embeddedrelated.com/showcode/304.php
{
    double adc_value;
    adc_value = getVoltage0Reading();
    average = dsp_ema_i32(adc_value, average, DSP_EMA_I32_ALPHA(0.1));
    return average;
}

// Length command: "Currently holding __ samples."
// int Sampler_getNumSamplesInHistory()
// {
// What is considered a valid sample???
// }

// History Command
double *Sampler_getHistory(int length) // with *length you're supposed to pass in the old array and use that to get that length
{

    double *copy_array = (double *)malloc(sizeof(double) * (length + 1));

    // if (length < MAX_SIZE) // This is wrong, comparing the same thing
    // {
    //     copy_array = (double *)malloc(sizeof(double) * (length + 1)); // adjust for most recent items
    // }
    // if (length > MAX_SIZE) // This is wrong, comparing the same thing
    // {
    //     copy_array = (double *)malloc(sizeof(double) * (length + 1)); // adjust for all N  samples + more
    // }

    for (int i = 0; i < length; i++)
    {
        copy_array[i] = buff_array[i];
    }

    for (int i = 0; i < length; i++)
    {
        printf("Value %5.3fV, ", (double)copy_array[i]);
    }

    printf("\n");

    return copy_array;
}

// Length Command: "History can hold __ samples."
int Sampler_getHistorySize(void)
{
    printf("History can hold %d samples\n", history_size);
    return history_size;
}

// Set size of history (circular buffer)
void Sampler_setHistorySize(int newSize)
{
    buff_array = (double *)malloc(sizeof(double) * (newSize + 1));
    history_size = newSize;

    if (buff_array == NULL)
    {
        printf("Malloc failed!\n");
        exit(1);
    }
}

// Run Sampler
void *Sampler_runner(void *arg)
{
    long seconds = 0;
    long nanoseconds = 1000000;
    struct timespec reqDelay = {seconds, nanoseconds};

    int *limit_ptr = (int *)arg;
    int limit = *limit_ptr;

    free(arg);

    int i = 0;

    while (sample)
    {

        int reading = getVoltage0Reading();
        double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        buff_array[nextId] = voltage;

        nanosleep(&reqDelay, (struct timespec *)NULL);

        nextId = (nextId + 1) % (limit - 1);

        sum += 1;
        i++;
        Sampler_getAverageReading();
    }

    double *answer = malloc(sizeof(*answer));
    *answer = sum;
    pthread_exit(answer);
}

// Join thread
void Sampler_stopSampling(void)
{
    sample = false;
    printf("Average: %f\n", average);
    int *result;
    pthread_join(tid, (void **)&result);
    free(result);
}

// Create Thread
void Sampler_startSampling(void)
{
    sample = true;

    int *limit = malloc(sizeof(*limit));
    *limit = MAX_SIZE;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, Sampler_runner, limit);
}