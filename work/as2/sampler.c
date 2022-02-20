#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>

#include "potDriver.h"
#include "segDisplay.h"

#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define GPIO61_VAL "/sys/class/gpio/gpio61/value"
#define GPIO44_VAL "/sys/class/gpio/gpio44/value"
#define MSG_MAX_LEN 1024
#define PORT 12345

static pthread_t tid, tid2, tid3, tid4;
static double *buff_array;
static double *copy_array;
static double *adjust_array;
static int *dip_array;
static int history_size;
static int nextId = 0;
static int dipId = 0;
static bool sample = true;
static long long sum = 0;
static double average = 0;
static double volt_value;
static double weight_val;
static int dipRecorded = 0;
static int validSampleRecorded = 0;
static int dips = 0;
static int total_dips = 0;
static int get_length = 0;

bool get_status(void)
{
    return sample;
}

// Count Command
long long Sampler_getNumSamplesTaken(void)
{
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

int count_dips(void)
{
    for (int i = 0; i < history_size; i++)
    {
        if (dip_array[i] == 1)
        {
            dipRecorded += 1;
        }
    }
    return dipRecorded;
}

void detect_dip(double lightLevel)
{
    double hysteresis = 0.03;

    double average_voltage = ((double)average / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
    lightLevel = ((double)lightLevel / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;

    if (lightLevel < (average_voltage - 0.1))
    {
        while (lightLevel < (average_voltage - hysteresis))
        {
            average = Sampler_getAverageReading();
            average_voltage = ((double)average / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
            double currentReading = getVoltage1Reading();
            lightLevel = ((double)currentReading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        }
        total_dips += 1;
        dip_array[dipId] = 1;
        dipId = (dipId + 1) % (history_size);
    }
    dip_array[dipId] = 0;
    dipId = (dipId + 1) % (history_size);
}

// Length command: "Currently holding __ samples."
int Sampler_getNumSamplesInHistory()
{
    return validSampleRecorded;
}

// History Command
double *Sampler_getHistory(int length)
{
    get_length = length;
    copy_array = (double *)malloc(sizeof(double) * (length + 1));

    for (int i = 0; i < length; i++)
    {
        copy_array[i] = buff_array[i];
    }

    return copy_array;
}

// Length Command: "History can hold __ samples."
int Sampler_getHistorySize(void)
{
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
    dip_array = (int *)malloc(sizeof(int) * (newSize + 1));
    if (newSize == 0)
    {
        history_size = 1;
    }
    else
    {
        history_size = newSize;
    }

    if (buff_array == NULL)
    {
        printf("Malloc failed!\n");
        exit(1);
    }

    changeInHistorySize(newSize);
}

// Join thread
void Sampler_stopSampling(void)
{
    pthread_join(tid, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);
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

    while (sample == true)
    {
        int reading = getVoltage1Reading();
        double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        buff_array[nextId] = voltage;

        nanosleep(&reqDelay, (struct timespec *)NULL);

        nextId = (nextId + 1) % (limit - 1);

        if (i < limit)
        {
            validSampleRecorded++;
        }

        sum += 1;
        i++;

        Sampler_getAverageReading();
        detect_dip(reading);
    }
    printf("Average: %f\n", average);
    printf("Dips recorded: %i\n", total_dips);
    pthread_exit(0);
}

void *term_output(void *argvp)
{
    int record_sum = sum;
    int two_hundreth = 0;
    while (sample == true)
    {
        dips = count_dips();
        dipRecorded = 0;
        sleep(1);
        printf("# Light Samples: %i, ", record_sum);
        printf("POT Value: %d, ", history_size);
        printf("Valid samples: %d, ", validSampleRecorded);
        printf("Average Light: %f, ", average);
        printf("Dips: %d", dips);
        printf("\n");
        printf("200th: %f ", buff_array[two_hundreth]);
        printf("\n");
        record_sum = sum - record_sum;
        two_hundreth += 200;
        if (two_hundreth > history_size)
        {
            two_hundreth = 0;
        }
    }

    return NULL;
}

void *gpio_output(void *argvp)
{
    long seconds = 0;
    long nanoseconds = 5000000;
    struct timespec reqDelay = {seconds, nanoseconds};

    while (sample != false)
    {
        turn_off(GPIO61_VAL); // left
        turn_off(GPIO44_VAL); // right
        adjust_pattern(dips);
        turn_on(GPIO61_VAL); // left
        nanosleep(&reqDelay, (struct timespec *)NULL);
        turn_off(GPIO61_VAL); // left
        turn_off(GPIO44_VAL); // right
        adjust_pattern(dips);
        turn_on(GPIO44_VAL); // right
        nanosleep(&reqDelay, (struct timespec *)NULL);
    }
    return NULL;
}

void *udp_listen(void *argvp)
{
    // Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;                // Connection may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Host to Network long
    sin.sin_port = htons(PORT);              // Host to Network short

    // Create the socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind the socket to the port (PORT) that we specify
    bind(socketDescriptor, (struct sockaddr *)&sin, sizeof(sin));

    // Check for errors (-1)

    while (1)
    {
        // Get the data (blocking)
        // Will change sin (the address) to be the address of the client.
        // Note: sin passes information in and out of call!
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LEN];

        // Pass buffer size - 1 for max # bytes so room for the null (string data)
        int bytesRx = recvfrom(socketDescriptor,
                               messageRx, MSG_MAX_LEN - 1, 0,
                               (struct sockaddr *)&sinRemote, &sin_len);

        // Check for errors (-1)

        // Make it null terminated (so string functions work)
        // - recvfrom given max size - 1, so there is always room for the null
        messageRx[bytesRx] = 0;
        printf("Message received (%d bytes): \n\n'%s'\n", bytesRx, messageRx);

        if (strncmp(messageRx, "stop", strlen("stop")) == 0)
        {
            printf("Program terminating.\n");
            sample = false;
            break;
        }

        if (strncmp(messageRx, "help", strlen("help")) == 0)
        {
            printf("Accepted command examples:\n");
            printf("count -- display total number of samples taken.\n");
            printf("length -- display number of samples in history (both max, and current).\n");
            printf("history -- display the full sample history being saved.\n");
            printf("display the 10 most recent history values.\n");
            printf("display number of .\n");
            printf("stop -- cause the server program to end.\n");
            printf("<enter> -- repeat last command.\n");
        }

        if (strncmp(messageRx, "count", strlen("count")) == 0)
        {
            int numSampleTaken = Sampler_getNumSamplesTaken();
            printf("Number of samples taken = %d\n", numSampleTaken);
        }

        if (strncmp(messageRx, "length", strlen("length")) == 0)
        {
            int historyCanHold = Sampler_getHistorySize();
            int currentlyholding = Sampler_getNumSamplesInHistory();
            printf("History can hold %d samples\n", historyCanHold);
            printf("Currently holding %i samples.\n", currentlyholding);
        }

        if (strncmp(messageRx, "history", strlen("history")) == 0)
        {
            int get_length = getVoltage0Reading();
            Sampler_getHistory(get_length);
            for (int i = 0; i < get_length; i++)
            {
                printf("%5.3fV,", (double)copy_array[i]);
            }
                printf("\n");

        }

        if (strncmp(messageRx, "dips", strlen("dips")) == 0)
        {
            printf("# Dips = %d\n", total_dips);
        }
    }
    // Close
	close(socketDescriptor);
    return NULL;
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
    pthread_create(&tid2, NULL, term_output, NULL);
    pthread_create(&tid3, NULL, gpio_output, NULL);
    pthread_create(&tid4, NULL, udp_listen, NULL);
}