#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DA_TRIGGER_FILE_NAME_HERE "/sys/class/leds/beaglebone:green:usr0/trigger"
#define SECOND_TRIGGER_FILE_NAME_HERE "/sys/class/leds/beaglebone:green:usr3/trigger"


//read from GPIO file
/*void readFromFileToScreen(char *fileName)
{
   FILE *pFile = fopen(fileName, "r");
   if (pFile == NULL) {
	printf("ERROR: Unable to open file (%s) for read\n", fileName);
	exit(-1);
   }
   
   // Read string (line)
   const int MAX_LENGTH = 1024;
   char buff[MAX_LENGTH];
   fgets(buff, MAX_LENGTH, pFile);
   
   // Close
   fclose(pFile);
   
   printf("Read: '%s'\n", buff);
}*/


int main(int argc, char* args[]) {
   int score = 0;
   int rounds = 0;
   
   
   printf("Hello embedded world, from Avneet!\n");
   printf("Press the Zen cape's Joystick in the direction of the LED.\n");
   printf("UP for LED0 (top)\n");
   printf("DOWN for LED3 (bottom)\n");
   printf("LEFT/RIGHT for exit app\n");
   
   
   printf("Press joystick; Score: (%d / %d)\n", score, rounds);
   
   
   int n = 1;
   while (n < 10)
   {
	   
	   /*int numOptions = 2;
   	   char options[2][10] = {
   	                         "UP",
   	                         "DOWN"
                            };
 
   	   srand(time(0));
   	   int randNum = rand();
   	   char choice[] = options[randNum % numOptions];*/
	   
	   int randNum = rand() % 2;
	   
	   if (randNum % 2 == 0) //Turn on LED0
	   {
		//Write into led files and turn off LED3
		FILE *pLedTriggerFile2 = fopen(SECOND_TRIGGER_FILE_NAME_HERE, "w");

		if (pLedTriggerFile2 == NULL) 
		{
			printf("ERROR OPENING %s.", SECOND_TRIGGER_FILE_NAME_HERE);
			exit(1);
		}
		//printf("File opened!\n");

		int charWritten2 = fprintf(pLedTriggerFile2, "none");
		if (charWritten2 <= 0) { 
			printf("ERROR WRITING DATA");
			exit(1);
		}

		fclose(pLedTriggerFile2);
		
		//Write into led files turn on LED0
		FILE *pLedTriggerFile = fopen(DA_TRIGGER_FILE_NAME_HERE, "w");

		if (pLedTriggerFile == NULL) 
		{
			printf("ERROR OPENING %s.", DA_TRIGGER_FILE_NAME_HERE);
			exit(1);
		}
		//printf("File opened!\n");

		int charWritten = fprintf(pLedTriggerFile, "default-on");
		if (charWritten <= 0) { 
			printf("ERROR WRITING DATA");
			exit(1);
		}
		
		fclose(pLedTriggerFile);
	   }

	   
	   if (randNum % 2 == 1) //Turn on LED3
	   {
		//Write into led files and turn off LED0
		FILE *pLedTriggerFile = fopen(DA_TRIGGER_FILE_NAME_HERE, "w");

		if (pLedTriggerFile == NULL) 
		{
			printf("ERROR OPENING %s.", DA_TRIGGER_FILE_NAME_HERE);
			exit(1);
		}
		//printf("File opened!\n");

		int charWritten = fprintf(pLedTriggerFile, "none");
		if (charWritten <= 0) { 
			printf("ERROR WRITING DATA");
			exit(1);
		}

		fclose(pLedTriggerFile);
		
		//Write into led files and turn on LED3
		FILE *pLedTriggerFile2 = fopen(SECOND_TRIGGER_FILE_NAME_HERE, "w");

		if (pLedTriggerFile2 == NULL) 
		{
			printf("ERROR OPENING %s.", SECOND_TRIGGER_FILE_NAME_HERE);
			exit(1);
		}
		//printf("File opened!\n");

		int charWritten2 = fprintf(pLedTriggerFile2, "default-on");
		if (charWritten2 <= 0) { 
			printf("ERROR WRITING DATA");
			exit(1);
		}

		fclose(pLedTriggerFile2);
	   }
	   
	   // Sleep 0.3 seconds
	   long seconds = 0;
	   long nanoseconds = 300000000;
	   struct timespec reqDelay = {seconds, nanoseconds};
	   nanosleep(&reqDelay, (struct timespec *) NULL);
	   printf("running: %d\n", n); 
	   n++;
   }
   return 0;
}

