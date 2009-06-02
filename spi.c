#include <p30f1010.h>
#include "spi.h"

void spiMasterSetup(void)
{
	/* 
	 * 2. Write the desired settings to the SPIxCON1
	 * register with MSTEN (SPIxCON1<5>) = 1.
	 */
	// Enable master mode
	SPI_CONTROL.MSTEN = 1;
	/*
	 * 1 = Enables module and configures SCKx, SDOx, SDIx and SSx as serial port pins
	 * 0 = Disables module
	 */
	spiCommonSetup();

	/*
	 * 3. Clear the SPIROV bit (SPIxSTAT<6>).
	 */
	SPI_STATUS.SPIROV = 0;
	/*
	 * 1 = A new byte/word is completely received and discarded. The user software has not read the
	 * previous data in the SPIxBUF register.
	 * 0 = No overflow has occurred
	 */

	/*
	 * 4. Enable SPI operation by setting the SPIEN bit
	 * (SPIxSTAT<15>).
	 */
	SPI_STATUS.SPIEN = 1;
	/*
	 * 1 = Enables module and configures SCKx, SDOx, SDIx and SSx as serial port pins
	 * 0 = Disables module
	 */


	// Other setup that is specific to master mode

	// Set Primary Prescale Bits
	SPI_CONTROL.PPRE = 0x00;
	/* 11 = Primary prescale 1:1
	 * 10 = Primary prescale 4:1
	 * 01 = Primary prescale 16:1
	 * 00 = Primary prescale 64:1
	 */

	// Set Secondary Prescale Bits (finer resolution than primary)
	SPI_CONTROL.SPRE = 0b000;
	/* 111 = Secondary prescale 1:1
	 * 110 = Secondary prescale 2:1
	 * ...
	 * 000 = Secondary prescale 8:1
	 */


	// Select when to sample data
	SPI_CONTROL.SMP = 0;
	/*
	 * 1 = Input data sampled at end of data output time
	 * 0 = Input data sampled at middle of data output time
	 */
}


void spiSlaveSetup(void)
{
	/*
	 * 1. Clear the SPIxBUF register.
	 */
	SPI_BUFFER = 0;


	/*
	 * 3. Write the desired settings to the SPIxCON1 and
	 * SPIxCON2 registers with MSTEN
	 * (SPIxCON1<5>) = 0.
	 */
	SPI_CONTROL.MSTEN = 0;
	/*
	 * 1 = Enables module and configures SCKx, SDOx, SDIx and SSx as serial port pins
	 * 0 = Disables module
	 */
	spiCommonSetup();


	/*
	 * 4. Clear the SMP bit (SPIxCON1<9>).
	 */ 
	SPI_CONTROL.SMP = 0;
	/*
	 * SMP must be cleared when SPIx is used in Slave mode
	 */


	/*
	 * 5. If the CKE (SPIxCON1<8>) bit is set, then the
	 * SSEN bit (SPIxCON1<7>) must be set to enable
	 * the SSx pin.
	 */
	if(SPI_CONTROL.CKE == 1) {
		SPI_CONTROL.SSEN = 1;
	}


	/*
	 * 6. Clear the SPIROV bit (SPIxSTAT<6>).
	 */ 
	SPI_STATUS.SPIROV = 0;
	/*
	 * 1 = A new byte/word is completely received and discarded. The user software has not read the
	 * previous data in the SPIxBUF register.
	 * 0 = No overflow has occurred
	 */


	/*
	 * 7. Enable SPI operation by setting the SPIEN bit
	 * (SPIxSTAT<15>).
	 */
	SPI_STATUS.SPIEN = 1;


}

void spiSendByte(int data)
{
	/*	When a byte is moved to the SSPBUF, the buffer full detect
		bit (BF) is set.  When the SSPBUF is read, the BF bit is
		cleared.
	*/

	while(SPI_STATUS.SPITBF);

	SPI_BUFFER = data;
}

void spiClearSSPBUF(void)
{
//	char dummy;
//	dummy = SSPBUF;
}

char spiReadByte(void)
{
	char byte;

	/*	READ FROM THE BUFFER (page 164) 
		SSPIF and BF are both set as true when a byte has been
		fully received and moved from SSPSR and SSPBUF. SSPIF
		must be cleared in software (page 114).
    */

	// Wait until byte is full received
	while(!(SPI_STATUS.SPIRBF));

	return byte;
}


void spiCommonSetup(void)
{
	// Set Clock Polarity (same for both modes)
	SPI_CONTROL.CKP = 0;
	/*
	 * 1 = Idle state for clock is a high level; active state is a low level
	 * 0 = Idle state for clock is a low level; active state is a high l
	 */

	// Set Clock Edge Select
	SPI_CONTROL.CKE = 1;
	/*
	 * 1 = Serial output data changes on transition from active clock state to Idle clock state (see bit 6)
	 * 0 = Serial output data changes on transition from Idle clock state to active clock state (see bit 6)
	 */


	// Select word or byte communication
	SPI_CONTROL.MODE16 = 0;
	/*
	 * 1 = Communication is word-wide (16 bits)
	 * 0 = Communication is byte-wide (8 bits)
	 */
}
