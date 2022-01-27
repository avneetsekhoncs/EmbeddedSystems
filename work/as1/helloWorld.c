#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/epoll.h>

#define DA_TRIGGER_FILE_NAME_HERE "/sys/class/leds/beaglebone:green:usr0/trigger"
#define LED1_TRIGGER_FILE "/sys/class/leds/beaglebone:green:usr1/trigger"
#define LED2_TRIGGER_FILE "/sys/class/leds/beaglebone:green:usr2/trigger"
#define SECOND_TRIGGER_FILE_NAME_HERE "/sys/class/leds/beaglebone:green:usr3/trigger"

#define FIRST_LED_BRIGHTNESS_FILE "/sys/class/leds/beaglebone:green:usr0"
#define SECOND_LED_BRIGHTNESS_FILE "/sys/class/leds/beaglebone:green:usr1"
#define THIRD_LED_BRIGHTNESS_FILE "/sys/class/leds/beaglebone:green:usr2"
#define FOURTH_LED_BRIGHTNESS_FILE "/sys/class/leds/beaglebone:green:usr3"

#define JOYSTICK_PRESSED_UP "/sys/class/gpio/gpio26/value"
#define JOYSTICK_PRESSED_DOWN "/sys/class/gpio/gpio46/value"
#define JOYSTICK_PRESSED_LEFT "/sys/class/gpio/gpio65/value"
#define JOYSTICK_PRESSED_RIGHT "/sys/class/gpio/gpio47/value"


// Brightness off
void brightnessClose(char *fileName)
{	
	char brightnessPath[1024];
	strcpy(brightnessPath, fileName);
	strcat(brightnessPath, "/brightness");
	
	FILE *pLedBrightnessFile = fopen(brightnessPath, "w");
	
	int charWritten = fprintf(pLedBrightnessFile, "0");
	if (charWritten <= 0) { 
		printf("ERROR WRITING DATA");
		exit(1);
	}
	fclose(pLedBrightnessFile);
}


// Brightness on
void brightnessOpen(char *fileName)
{	
	char brightnessPath[1024];
	strcpy(brightnessPath, fileName);
	strcat(brightnessPath, "/brightness");
	
	// Write into led files
	FILE *pLedBrightnessFile = fopen(brightnessPath, "w");
		
	if (pLedBrightnessFile == NULL) 
	{
		printf("ERROR OPENING %s.", brightnessPath);
		exit(1);
	}
	int charWritten = fprintf(pLedBrightnessFile, "100");
	if (charWritten <= 0) { 
		printf("ERROR WRITING DATA");
		exit(1);
	}
	fclose(pLedBrightnessFile);
}

void flashLED(int numFlash, long sec, long nsec)
{
	long seconds = sec;
   	long nanoseconds = nsec;
   	struct timespec reqDelay = {seconds, nanoseconds};
   	
   	for (int i = 0; i < numFlash; i++)
   	{
		brightnessOpen(FIRST_LED_BRIGHTNESS_FILE);
		brightnessOpen(SECOND_LED_BRIGHTNESS_FILE);
		brightnessOpen(THIRD_LED_BRIGHTNESS_FILE);
		brightnessOpen(FOURTH_LED_BRIGHTNESS_FILE);
		
		// Sleep
   		nanosleep(&reqDelay, (struct timespec *) NULL);
   		
		brightnessClose(FIRST_LED_BRIGHTNESS_FILE);
		brightnessClose(SECOND_LED_BRIGHTNESS_FILE);
		brightnessClose(THIRD_LED_BRIGHTNESS_FILE);
		brightnessClose(FOURTH_LED_BRIGHTNESS_FILE);
		
		// Sleep
   		nanosleep(&reqDelay, (struct timespec *) NULL);	
   	}
}

//Turn on LED0 "UP"
void turnOnUpLed(char *fileName, char *secondFileName)
{
	//Write into led files and turn off LED3
	FILE *pLedTriggerFile2 = fopen(fileName, "w");

	if (pLedTriggerFile2 == NULL) 
	{
		printf("ERROR OPENING %s.", fileName);
		exit(1);
	}

	int charWritten2 = fprintf(pLedTriggerFile2, "none");
	if (charWritten2 <= 0) { 
		printf("ERROR WRITING DATA");
		exit(1);
	}

	fclose(pLedTriggerFile2);
	
	//Write into led files turn on LED0
	FILE *pLedTriggerFile = fopen(secondFileName, "w");

	if (pLedTriggerFile == NULL) 
	{
		printf("ERROR OPENING %s.", secondFileName);
		exit(1);
	}

	int charWritten = fprintf(pLedTriggerFile, "default-on");
	if (charWritten <= 0) { 
		printf("ERROR WRITING DATA");
		exit(1);
	}
	
	fclose(pLedTriggerFile);
}

//Turn on LED3 "DOWN"
void turnOnDownLed(char *fileName, char *secondFileName)
{
	//Write into led files and turn off LED0
	FILE *pLedTriggerFile = fopen(fileName, "w");

	if (pLedTriggerFile == NULL) 
	{
		printf("ERROR OPENING %s.", fileName);
		exit(1);
	}

	int charWritten = fprintf(pLedTriggerFile, "none");
	if (charWritten <= 0) { 
		printf("ERROR WRITING DATA");
		exit(1);
	}

	fclose(pLedTriggerFile);
	
	//Write into led files and turn on LED3
	FILE *pLedTriggerFile2 = fopen(secondFileName, "w");

	if (pLedTriggerFile2 == NULL) 
	{
		printf("ERROR OPENING %s.", secondFileName);
		exit(1);
	}

	int charWritten2 = fprintf(pLedTriggerFile2, "default-on");
	if (charWritten2 <= 0) { 
		printf("ERROR WRITING DATA");
		exit(1);
	}

	fclose(pLedTriggerFile2);
}

//Read from GPIO file
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
   
   // Sleep
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
   	flashLED(1, 0, 100000000);
   	updateScore+=1;
   } else {
   	printf("Incorrect :(\n");
   	flashLED(5, 0, 100000000);
   }
   
   // Close
   fclose(pFile);

   do
   {
	  FILE *pFile = fopen(fileName, "r");
	  fgets(buff, MAX_LENGTH, pFile);
	  result = strcmp(buff, str1);
	  if(result == 1)
	  {
		  fclose(pFile);
		  break;
	  }
	  fclose(pFile);
   } while(1);

   //printf("Read: '%s'\n", buff);
   
   return updateScore; 
}

//Read from GPIO file to EXIT
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
   
   // Sleep
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
   	printf("Thank you for playing!\n");
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
   
   while (1)
   {   
	   int randNum = rand() % 2;
	   
	   if (randNum % 2 == 0) //Turn on LED0
	   {
		turnOnUpLed(SECOND_TRIGGER_FILE_NAME_HERE, DA_TRIGGER_FILE_NAME_HERE);
		score = readFromFileToScreen(JOYSTICK_PRESSED_UP, score);
	   }

	   
	   if (randNum % 2 == 1) //Turn on LED3
	   {
		turnOnDownLed(DA_TRIGGER_FILE_NAME_HERE, SECOND_TRIGGER_FILE_NAME_HERE);
		score = readFromFileToScreen(JOYSTICK_PRESSED_DOWN, score);
	   }
	    
	   rounds++;
	   printf("Press joystick; Score: (%d / %d)\n", score, rounds);
	   readFromFileToExit(JOYSTICK_PRESSED_LEFT, score, rounds);
	   readFromFileToExit(JOYSTICK_PRESSED_RIGHT, score, rounds);
   }
   return 0;
}

