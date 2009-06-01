#ifndef SPI_H
#define SPI_H

#include <p18f4620.h>

void spiMasterSetup(void);
void spiSlaveSelect(int slave);
void spiClearSSPBUF(void);
void spiSendByte(char byte);
char spiReadByte(void);
void spiSlaveSetup(void);
void spiMakeGeneral(void);

#endif