#include <p18f4620.h>
#include "spi.h"

void main(void)
{
	char data;
	int i, j;

	TRISA = 0b11110000;
	PORTA = 0b00001001;

	for(i = 0; i < 1000; i++)
		for(j = 0; j < 1000; j++);

	spiMasterSetup();
	data = 'j';

	PORTA = 0b00001010;
//	for(i = 0; i < 1000; i++)
//		for(j = 0; j < 1000; j++);

	while(!(PORTA & 0b00010000));

	spiClearSSPBUF();
	spiSendByte(data);

//	while(SSPSTATbits.BF);
	for(i = 0; i < 1; i++)
		for(j = 0; j < 4; j++);
	PORTA = 0b00001100;

	while(1);

}