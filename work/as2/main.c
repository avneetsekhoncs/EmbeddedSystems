#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "sampler.h"


int main(int argc, char **argv)
{
    Sampler_startSampling();
    Sampler_stopSampling();
}