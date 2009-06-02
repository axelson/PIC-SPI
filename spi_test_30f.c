#include <p30f1010.h>
#include "spi.h"
#include <delay.h>

void main(void)
{
	int data=0x22;
	int i=0, j=0;

	TRISB = 0b11110001;
	PORTB = 0b00001110;
	spiMasterSetup();
	
	LATB |= 0b10;
	
	while(!_RB0);

	// Slave select enable
	LATB &= 0b1110111;
	spiSendByte(data);

	//while(SPI_STATUS.SPITBF == 1);
	// Slave select disable
	for(i=0; i<1000 ;i++);
	LATB |= 0b00001000;

	LATB |= 0b100;

	/*
	if(data == 'j')
		PORTA |= 0b00001000;
	else
		PORTA |= 0b00000100;

	while(1);
	*/
}
