/**
 * @file SPI.C
 * @brief  Serial Peripheral Interface module
 *         Target: PIC16F876
 * 
 * This file is part of JADEKU Portable ADQ device for Routines Control.
 *
 * Copyright (C) 2002-2009,  Hernan Monserrat hemonserrat<at>gmail<dot>com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <app.h>

/**
* SPI interrupt flags
*/
volatile bit gSpiTx=CLEAR; 
volatile bit gSpiRx=CLEAR;
/**
 *  Initialize SPI subsystem
 */
void SPI_Init(void) 
{
    SSPEN = DISABLE; // Reset

    TRISC4 = DIR_IN; // SPI Slave In
    TRISC5 = DIR_OUT; // SPI Slave Out
    TRISC3 = DIR_OUT; // SPI Slave CLK Out
    //  TRISA5 = DIR_IN;   // SPI SS Enabled In, used for other purpose then SPI

    SMP = CLEAR; // Input data at middle (compat=0)

    SSPM0 = CLEAR; // Clock /4 
    SSPM1 = CLEAR;
    SSPM2 = CLEAR;
    SSPM3 = CLEAR;

    CKP = SET; // Clock idle is high
    CKE = CLEAR; // Data on rising edge (compat=1)

    SSPEN = ENABLE; // SPI port pins enabled
    SSPIE = ENABLE; // Enable interrupt on SPI receiving
    PEIE = ENABLE; // Enable peripherial interrupt(For SPI)
}
/**
 *  Poll the shift register.
 *  @param   data - outgoing byte.
 *  @return  a byte read (input)
 *  @remarks do the input/output polling the register
 */
BYTE SPI_Exchange(BYTE data) 
{
    BYTE rxdata;
    rxdata = SSPBUF; // lose any data in buffer, clear BF
    SSPBUF = data;
    while (!BF) continue;
    CLRWDT();
    return SSPBUF;
}
/**
 *  Send a byte to the SPI network.
 *  @param   data - Put a data byte in the shift register.
 */
void SPI_Write(BYTE data) 
{
    gSpiTx = 1;
    SPI_Exchange(data);
    while (gSpiTx);
}
/**
 *  Get a byte from the SPI subsystem
 *  RMC packet is received.
 *  @return  a byte from the shift register
 */
BYTE SPI_Read(void) 
{
    BYTE in;
    gSpiRx = 0;
    in = SPI_Exchange(0x00);
    while (!gSpiRx);
    return in; // get received data, clear BF 
}

//*********************************************************[ENDL]***************




