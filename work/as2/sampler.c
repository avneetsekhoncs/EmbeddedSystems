#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#include "potDriver.h"

#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

static pthread_t tid;
static double *buff_array;
static double *copy_array;
static double *adjust_array;
static int history_size;
static int nextId = 0;
static bool sample = true;
static long long sum = 0;
static double average = 0;
static double volt_value;
static double weight_val;

// Count Command
long long Sampler_getNumSamplesTaken(void)
{
    printf("Sum is %lld\n", sum);
    return sum;
}

double Exp_smoothing(double volt, double average, double weight)
{
    double calc_average;
    calc_average = (weight * volt) + ((1 - weight) * average);
    return calc_average;
}

double Sampler_getAverageReading(void)
{
    volt_value = getVoltage1Reading();
    average = Exp_smoothing(volt_value, average, weight_val);

    return average;
}

// Length command: "Currently holding __ samples."
int Sampler_getNumSamplesInHistory()
{
    int validSamples = 0;

    for (int i = 0; i < 10; i++)
    {
        if (buff_array[i] > 0)
        {
            validSamples += 1;
        }
    }
    printf("Currently holding %i samples.\n", validSamples);

    return validSamples;
}

// History Command
double *Sampler_getHistory(int length)
{

    copy_array = (double *)malloc(sizeof(double) * (length + 1));
    printf("Array Length: %i\n", length);

    for (int i = 0; i < length; i++)
    {
        copy_array[i] = buff_array[i];
        //printf("Value %5.3fV, \n", (double)copy_array[i]);
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

void changeInHistorySize(int size)
{
    int pot_Size = getVoltage0Reading();

    if (pot_Size < size)
    {
        adjust_array = (double *)malloc(sizeof(double) * (size + 1));
        for (int i = 0; i < size; i++)
        {
            adjust_array[i] = buff_array[i];
        }
        buff_array = (double *)malloc(sizeof(double) * (pot_Size + 1));
        for (int j = 0; j < pot_Size; j++)
        {
            buff_array[j] = adjust_array[j];
        }
    }

    if (pot_Size > size)
    {
        adjust_array = (double *)malloc(sizeof(double) * (size + 1));
        for (int i = 0; i < size; i++)
        {
            adjust_array[i] = buff_array[i];
        }
        buff_array = (double *)malloc(sizeof(double) * (pot_Size + 1));
        for (int j = 0; j < size; j++)
        {
            buff_array[j] = adjust_array[j];
        }
    }
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

    changeInHistorySize(newSize);
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
    average = getVoltage1Reading();

    while (sample)
    {

        int reading = getVoltage1Reading();
        double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        buff_array[nextId] = voltage;

        nanosleep(&reqDelay, (struct timespec *)NULL);

        nextId = (nextId + 1) % (limit - 1);

        sum += 1;
        i++;

        Sampler_getAverageReading();
    }

    printf("Average: %f\n", average);

    pthread_exit(0);
}

// Join thread
void Sampler_stopSampling(void)
{
    sample = false;
    pthread_join(tid, NULL);
}

// Create Thread
void Sampler_startSampling(void)
{
    sample = true;
    int max_size = getVoltage0Reading();
    int *limit = malloc(sizeof(*limit));
    *limit = max_size;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, Sampler_runner, limit);
}