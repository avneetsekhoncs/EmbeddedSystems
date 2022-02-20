#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2cFileDesc;
}

void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}

void adjust_pattern(int currentDips)
{
	int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	if(currentDips == 0)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xA1); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x86); //Top Side
		close(i2cFileDesc);
	}

	if(currentDips == 1)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0x80); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x12); //Top Side
		close(i2cFileDesc);
	}

	if(currentDips == 2)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0x31); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x0E); //Top Side
		close(i2cFileDesc);
	}
	
	if(currentDips == 3)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xB0); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x0E); //Top Side
		close(i2cFileDesc);
	}
	
	if(currentDips == 4)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0x90); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8A); //Top Side
		close(i2cFileDesc);
	}

	if(currentDips == 5)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xB0); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8C); //Top Side
		close(i2cFileDesc);
	}

	if(currentDips == 6)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xB1); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8C); //Top Side
		close(i2cFileDesc);
	}

	if(currentDips == 7)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0x04); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x14); //Top Side
		close(i2cFileDesc);
	}

	if(currentDips == 8)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xB1); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8F); //Top Side
		close(i2cFileDesc);
	}

	if(currentDips >= 9)
	{
		writeI2cReg(i2cFileDesc, REG_OUTA, 0x90); //Bottom Side
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8F); //Top Side
		close(i2cFileDesc);
	}
	
	
}

void turn_off(char *fileName)
{
	//Write into gpio files and turn off
	FILE *gpioFile = fopen(fileName, "w");

	if (gpioFile == NULL) 
	{
		printf("ERROR OPENING %s.\n", fileName);
		exit(1);
	}

	int charWritten = fprintf(gpioFile, "0");
	if (charWritten <= 0) { 
		printf("ERROR WRITING DATA\n");
		exit(1);
	}

	fclose(gpioFile);
}

void turn_on(char *fileName)
{
	//Write into gpio files and turn off
	FILE *gpioFile = fopen(fileName, "w");

	if (gpioFile == NULL) 
	{
		printf("ERROR OPENING %s.\n", fileName);
		exit(1);
	}

	int charWritten = fprintf(gpioFile, "1");
	if (charWritten <= 0) { 
		printf("ERROR WRITING DATA\n");
		exit(1);
	}

	fclose(gpioFile);
}