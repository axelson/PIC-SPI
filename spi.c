#include <p18f4620.h>
#include "spi.h"

void spiMasterSetup(void)
{
	spiMakeGeneral();

	SSPCON1bits.SSPM3 = 0;
	SSPCON1bits.SSPM2 = 0;
	// FOSC/64
	SSPCON1bits.SSPM1 = 1;
	SSPCON1bits.SSPM0 = 0;

	TRISCbits.TRISC3 = 0;		// Master mode

	SSPCON1bits.SSPEN = 1;		// MSSP enable bit
}

void spiSlaveSelect(int slave)
{
//	if(slave == 1)
//	else
}

void spiSendByte(char byte)
{
	/*	When a byte is moved to the SSPBUF, the buffer full detect
		bit (BF) is set.  When the SSPBUF is read, the BF bit is
		cleared.
	*/
	while(SSPSTATbits.BF);

PORTA = 0b00000100;

	SSPBUF = byte;
}

void spiClearSSPBUF(void)
{
	char dummy;
	dummy = SSPBUF;
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
	while(!(PIR1bits.SSPIF & SSPSTATbits.BF));
	byte = SSPBUF;

	PIR1bits.SSPIF = 0;		// Clear flag
	return byte;
}

void spiSlaveSetup(void)
{
	spiMakeGeneral();

	SSPCON1bits.SSPM3 = 0;
	SSPCON1bits.SSPM2 = 1;
	SSPCON1bits.SSPM1 = 0;
	SSPCON1bits.SSPM0 = 1;		// SS disabled (1)
								// SS enabled (0)

	TRISCbits.TRISC3 = 1;		// Slave mode

	SSPCON1bits.SSPEN = 1;		// MSSP enable bit
}



void spiMakeGeneral(void)
{
	/*	CONTROL REGISTERS

		The MSSP module has three associated registers.  These
		include a status register (SSPSTAT) and two control
		registers (SSPCON1 and SSPCON2).
	*/

	/*	SPI MODE

		The SPI mode allows 8 bits of data to be synchronously
		transmitted and received simultaneously.  All four SPI
		modes are supported.  To accomplish communication,
		typically three pins are used:
		-	Serial Data Out (SDO) - RC5/SDO
		-	Serial Data In (SDI) - RC4/SDI/SDA
		-	Serial Clock (SCK) - RC3/SCK/SCL

		Additionally, a fourth pin may be used when in Slave
		mode of operation:
		-	Slave Select (SS') - RA5/AN/SS'/HLVDIN/C2OUT
	*/

	/*	The MSSP module has four registers for SPI mode operation.
		These are:
		-	MSSP Control Register 1 (SSPCON1)
		-	MSSP Status Register (SSPSTAT)
		-	Serial Receive/Transmit Buffer Registers (SSPBUF)
		-	MSSP Shift Register (SSPSR) - Not directly accessible

		SSPCON1 and SSPSTAT are the control and status registers in
		SPI mode operation.  The SSPCON1 register is readable and
		writable.  The lower 6 bits of the SSPSTAT are read-only.
		The upper 2 bits of the SSPSTAT are read/write.

		SSPSR is the shift register used for shifting data in or
		out. SSPBUF is the buffer register to which data bytes
		are written to or read from.

		In receive operations, SSPSR and SSPBUF together create a
		double buffered receiver.  When SSPSR receives a complete
		byte, it is transferred to SSPBUF and the SSPIF interrupt
		is set.

		During transmission, the SSPBUF is not double-buffered.
		A write to SSPBUF will write to both SSPBUF and SSPSR.

	*/

	/*	OPERATION

		When initializing the SPI, several operations need to be
		specified.  This is done by programming the appropriate
		control bits (SSPCON1<5:0> and SSPSTAT<7:6>).  These
		control bits allow for the following to be specified:
		-	Master mode (SCK is the clock output)
		-	Slave mode (SCK is the clock input)
		-	Clock Polarity (Idle state of SCK)
		-	Data Input Sample Phase (middle or end of data output
			time)
		-	Clock Rate (Master mode only)
		-	Slave Select mode (Slave mode only)

		The MSSP consists of a transmit/receive shift register
		(SSPSR) and a buffer register (SSPBUF).  The SSPSR shifts
		the data in and out of the device, MSb first.  The SSPBUF
		holds the data that was written to the SSPSR until the
		received data is ready.  Once the 8 bits of data have been
		received, that byte is moved to the SSPBUF register.  Then,
		the buffer Full detect bit, BF (SSPSTAT<0>) and the
		interrupt flag bit, SSPIF, are set.  This double buffering
		of the received data (SSPBUF) allows the next byte to start
		reception before reading the data that was just received.
		Any write to the SSPBUF register during transmission/reception
		of data will be ignored and the write collision detect bit
		WCOL (SSPCON1<7>), will be set.  User software must clear
		the WCOL bit so that it can be determined if the following
		write(s) to the SSPBUF register completed successfully.

		When the application software is expecting to receive valid
		data, the SSPBUF should be read before the next byte of
		data transfer is written to the SSPBUF.  The Buffer Full
		bit, BF (SSPSTAT<0>), indicates when SSPBUF has been loaded
		with the receive data (transmission complete).  When the
		SSPBUF is read, the BF bit is cleared.  This data may be
		irrelevant if the SPI is only a transmitter.  Generally,
		the MSSP interrupt is used to determine when the
		transmisson/reception has completed.  The SSPBUF must be
		read and/or written.  If the interrupt method is not going
		to be used, then software polling can be done to ensure
		that a write collision does not occur.
	*/

	SSPCON1bits.SSPEN = 0;		// Turn off SPI mode until reconfig
								// is complete

	SSPCON1bits.CKP = 0;		// Both processors should be
								// programmed to same CKP

	SSPSTATbits.SMP = 0;		// Input data sampled at middle
								// of data output time
	SSPSTATbits.CKE = 1;

/*	ENABLING SPI I/O

		To enable the serial port, MSSP Enable bit, SSPEN
		(SSPCON1<5>), must be set.  To reset or reconfigure SPI
		mode, clear the SSPEN bit, reinitialize the SSPCON
		registers and then set the SSPEN bit.  This configures
		the SDI, SDO, SCK, and SS' pins as serial port pins.
		For the pins to behave as the serial port function, some
		must have their data direction bits (in the TRIS register)
		appopriately programmed as follows:
		-	SDI is automatically controlled by the SPI module
		-	SDO must have TRISC<5> bit cleared
		-	SCK (Master mode) must have TRISC<3> bit cleared
		-	SCK (Slave mode) must have the TRISC<3> bit set
		-	SS' must have TRISA<5> bit set
	*/

	SSPCON1bits.SSPEN = 1;
	TRISCbits.TRISC5 = 0;

//	TRISCbits.TRISC3 = 1;		// Slave mode
//	TRISCbits.TRISC3 = 0;		// Master mode

	TRISAbits.TRISA5 = 1;		// SS

	/*
		Any serial port function that is not desired may be
		overridden by programming the corresponding data direction
		(TRIS) register to the opposite value.
	*/

}