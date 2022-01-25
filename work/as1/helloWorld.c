#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/epoll.h>

#define DA_TRIGGER_FILE_NAME_HERE "/sys/class/leds/beaglebone:green:usr0/trigger"
#define SECOND_TRIGGER_FILE_NAME_HERE "/sys/class/leds/beaglebone:green:usr3/trigger"
#define JOYSTICK_PRESSED_UP "/sys/class/gpio/gpio26/value"
#define JOYSTICK_PRESSED_DOWN "/sys/class/gpio/gpio46/value"
#define JOYSTICK_PRESSED_LEFT "/sys/class/gpio/gpio65/value"
#define JOYSTICK_PRESSED_RIGHT "/sys/class/gpio/gpio47/value"

//read from GPIO file
int readFromFileToScreen(char *fileName, int updateScore)
{
   int result = 1;
   char str1[] = "0";
   long seconds = 1;
   long nanoseconds = 100000000;
   struct timespec reqDelay = {seconds, nanoseconds};
   
   FILE *pFile = fopen(fileName, "r");
   if (pFile == NULL) {
	printf("ERROR: Unable to open file (%s) for read\n", fileName);
	exit(-1);
   }
   
   // Sleep 0.3 seconds
   nanosleep(&reqDelay, (struct timespec *) NULL);
   
   // Read string (line)
   const int MAX_LENGTH = 1024;
   char buff[MAX_LENGTH];
   fgets(buff, MAX_LENGTH, pFile);
   
   result = strcmp(buff, str1);
   //printf("result: %d\n", result);
   
   if (result == 10)
   {
   	printf("Correct!\n");
   	updateScore+=1;
   } else {
   	printf("Incorrect ;(\n");
   }
   
   // Close
   fclose(pFile);

   //printf("Read: '%s'\n", buff);
   
   return updateScore; 
}

//read from GPIO file to EXIT
void readFromFileToExit(char *fileName, int finalScore, int finalRounds)
{
   int result = 1;
   char str1[] = "0";   
   long seconds = 1;
   long nanoseconds = 100000000;
   struct timespec reqDelay = {seconds, nanoseconds};
   
   FILE *pFile = fopen(fileName, "r");
   if (pFile == NULL) {
	printf("ERROR: Unable to open file (%s) for read\n", fileName);
	exit(-1);
   }
   
   // Sleep 0.3 seconds
   nanosleep(&reqDelay, (struct timespec *) NULL);
   
   // Read string (line)
   const int MAX_LENGTH = 1024;
   char buff[MAX_LENGTH];
   fgets(buff, MAX_LENGTH, pFile);
   
   result = strcmp(buff, str1);
   //printf("result: %d\n", result);
   
   if (result == 10)
   {
   	printf("Your final score was (%d / %d)\n", finalScore, finalRounds);
   	printf("Thank you for playing\n");
   	exit(-1);
   } 
   
   // Close
   fclose(pFile);

   //printf("Read: '%s'\n", buff);
}


int main(int argc, char* args[]) {
 
   int score = 0;
   int rounds = 0;

   printf("Hello embedded world, from Avneet!\n");
   printf("Press the Zen cape's Joystick in the direction of the LED.\n");
   printf("UP for LED0 (top)\n");
   printf("DOWN for LED3 (bottom)\n");
   printf("LEFT/RIGHT for exit app\n");
   
   printf("Press joystick; Score: (%d / %d)\n", score, rounds);
   
   long seconds = 1;
   long nanoseconds = 100000000;
   struct timespec reqDelay = {seconds, nanoseconds};
   
   int n = 1;
   while (n < 10)
   {   
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
		
		score = readFromFileToScreen(JOYSTICK_PRESSED_UP, score);
		readFromFileToExit(JOYSTICK_PRESSED_LEFT, score, rounds);
		readFromFileToExit(JOYSTICK_PRESSED_RIGHT, score, rounds);
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
		
		score = readFromFileToScreen(JOYSTICK_PRESSED_DOWN, score);
		readFromFileToExit(JOYSTICK_PRESSED_LEFT, score, rounds);
		readFromFileToExit(JOYSTICK_PRESSED_RIGHT, score, rounds);
	   }
	    
	   // Sleep 0.3 seconds
	   nanosleep(&reqDelay, (struct timespec *) NULL);
	   //printf("running: %d\n", n); 
	   n++;
	   rounds++;
	   printf("Press joystick; Score: (%d / %d)\n", score, rounds);
   }
   return 0;
}

