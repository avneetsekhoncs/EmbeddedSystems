#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "sampler.h"
#include "potDriver.h"


int main(int argc, char **argv)
{
    if (system("./i2c_commands.sh") != 0)
    {
        printf("Error executing command.\n");
    }

    int volt_0_reading = getVoltage0Reading();
    bool status_check = get_status();

    Sampler_setHistorySize(volt_0_reading);
    Sampler_startSampling();

    while(1)
    {
        if(status_check == false)
        {
            break;
        }
        status_check = get_status();
    }

    Sampler_stopSampling();
}