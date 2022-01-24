// Edge trigger the joystic left
//
// Based on code by Derek Malloy: http://exploringbeaglebone.com/chapter6/
// Originally based on code in epoll man page
// Modified by Brian Fraser

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>		// Errors
#include <string.h>

#include <sys/epoll.h>  // for epoll()
#include <fcntl.h>      // for open()
#include <unistd.h>     // for close()

#define JOYSTICK_DIRECTION "/sys/class/gpio/gpio65/direction"
#define JOYSTICK_EDGE      "/sys/class/gpio/gpio65/edge"
#define JOYSTICK_IN        "/sys/class/gpio/gpio65/value"

static void writeToFile(const char* fileName, const char* value);
static int readLineFromFile(char* fileName, char* buff, unsigned int maxLength);


static int waitForGpioEdge(const char* fileNameForGpioValue) 
{
	// If you want to wait for input on multiple file, you could change this function
	// to take an array of names, and then loop throug each, adding it to the 
	// epoll instance (accessed via epollfd) by calling epoll_ctl() numerous times.

	// create an epoll instance
	// .. kernel ignores size argument; must be > 0
	int epollfd = epoll_create(1);
	if (epollfd == -1) {
		fprintf(stderr, "ERROR: epoll_create() returned (%d) = %s\n", epollfd, strerror(errno));
		return -1;
	}

	// open GPIO value file:
	int fdLeft = open(fileNameForGpioValue, O_RDONLY | O_NONBLOCK);
	if (fdLeft == -1) {
		fprintf(stderr, "ERROR: unable to open() GPIO value file (%d) = %s\n", fdLeft, strerror(errno));
		return -1;
	}

	// configure epoll to wait for events: read operation | edge triggered | urgent data
	struct epoll_event epollStruct;
	epollStruct.events = EPOLLIN | EPOLLET | EPOLLPRI;
	epollStruct.data.fd = fdLeft;

	// register file descriptor on the epoll instance, see: man epoll_ctl
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fdLeft, &epollStruct) == -1) {
		fprintf(stderr, "ERROR: epoll_ctl() failed on add control interface (%d) = %s\n", fdLeft, strerror(errno));
		return -1;
	}

	// ignore first trigger
	for (int i = 0; i < 2; i++) {
		int waitRet = epoll_wait(
				epollfd, 
				&epollStruct, 
				1,                // maximum # events
				-1);              // timeout in ms, -1 = wait indefinite; 0 = returne immediately

		if (waitRet == -1){
			fprintf(stderr, "ERROR: epoll_wait() failed (%d) = %s\n", waitRet, strerror(errno));
			close(fdLeft);
			close(epollfd);
			return -1;
		}
	}

	// cleanup epoll instance:
	close(fdLeft);
	close(epollfd);
	return 0;
}

int main() 
{
	// Assume joystick pin already exported as GPIO
	// .. set direction of GPIO pin to input
	writeToFile(JOYSTICK_DIRECTION, "in");
	// .. set edge trigger; options are "none", "rising", "falling", "both"
	writeToFile(JOYSTICK_EDGE, "both");

	// Block and wait for edge triggered change on GPIO pin
	printf("Now waiting on input for file: %s\n", JOYSTICK_IN);
	while (1) {

		// Wait for an edge trigger:
		int ret = waitForGpioEdge(JOYSTICK_IN);
		if (ret == -1) {
			exit(EXIT_FAILURE);
		}
	
		// Current state:
		char buff[1024];
		int bytesRead = readLineFromFile(JOYSTICK_IN, buff, 1024);
		if (bytesRead > 0) {
			printf("GPIO pin reads: %c\n", buff[0]);
		} else {
			fprintf(stderr, "ERROR: Read 0 bytes from GPIO input: %s\n", strerror(errno));
		}
	}
	return 0;
}

static void writeToFile(const char* fileName, const char* value)
{
	FILE *pFile = fopen(fileName, "w");
	fprintf(pFile, "%s", value);
	fclose(pFile);
}

static int readLineFromFile(char* fileName, char* buff, unsigned int maxLength)
{
	FILE *file = fopen(fileName, "r");
	int bytes_read = getline(&buff, &maxLength, file);
	fclose(file);
	return bytes_read;
}
