#ifndef _SEGDISPLAY_H_
#define _SEGDISPLAY_H_

int initI2cBus(char* bus, int address);
void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
void turn_off(char *fileName);
void turn_on(char *fileName);
void adjust_pattern(int currentDips);


#endif