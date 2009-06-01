#include <p18f4620.h>
#include "spi.h"

void main(void)
{
	char data;
	int i, j;

	TRISA &= 0b11110001;
	PORTA |= 0b00000010;

	for(i = 0; i < 1000; i++)
		for(j = 0; j < 100; j++);

	spiSlaveSetup();
	
	spiClearSSPBUF();
	data = spiReadByte();

	if(data == 'j')
		PORTA |= 0b00001000;
	else
		PORTA |= 0b00000100;

	while(1);
}