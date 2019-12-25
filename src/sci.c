/**
 * @file SCI.C
 * @brief  Serial Communication Interface.
 *         Target: PIC16F876
 * 
 * This file is part of JADEKU Portable ADQ device for Routines Control.
 *
 * Copyright (C) 2006-2009,  Hernan Monserrat hemonserrat<at>gmail<dot>com
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
#include "app.h"

/**
*  Initialize SCI 
*  @param   baud     - Baud rate for serial com.
*  @param   ninebits - set nine bits mode
*  @return  TRUE, FALSE.
*/
BYTE SCI_Init(DWORD baud, BYTE ninebits)
{
	int X;
	DWORD tmp;
	
	/* calculate and set baud rate register */
	/* for asynchronous mode */
	tmp = 16UL * baud;
	X = (int)(FOSC/tmp) - 1;
	if((X>255) || (X<0))
	{
		tmp = 64UL * baud;
		X = (int)(FOSC/tmp) - 1;
		if((X>255) || (X<0))
		{
			return TRUE;	/* panic - baud rate unobtainable */
		}
		else
			BRGH = CLEAR;	/* low baud rate */
	}
	else
		BRGH = SET;	/* high baud rate */
	SPBRG = X;	/* set the baud rate */

	SYNC = CLEAR;	/* asynchronous */
	SPEN = ENABLE;	/* enable serial port pins */
	CREN = ENABLE;	/* enable reception */
	SREN = CLEAR;	/* no effect */
	TXIE = DISABLE;	/* disable tx interrupts */
	RCIE = DISABLE;	/* disable rx interrupts */
	TX9  = ninebits?SET:CLEAR;	/* 8- or 9-bit transmission */
	RX9  = ninebits?SET:CLEAR;	/* 8- or 9-bit reception */
	TXEN = ENABLE;	/* enable the transmitter */

	return FALSE;
} 

//#pragma interrupt_level 1
/**
*  Put a byte in the outgoing buffer
*  @param   byte - byte to send.
*/
void SCI_Write(BYTE byte)
{
	while(!TXIF)	/* set when register is empty */
		continue;
	TXREG = byte;

	return;
}

/**
*  Get byte from the input buffer
*  @return  read byte from input buffer.
*/
BYTE SCI_Read(void)
{
	while(!RCIF)	/* set when register is not empty */
		continue;

	return RCREG;	/* RXD9 and FERR are gone now */
} 

/**
*  Get Overrun error
*  @return  TRUE, FALSE.
*/
BYTE SCI_CheckOverrun(void)
{
	if(OERR)	// re-enable after overrun error 
	{
		CREN = DISABLE;
		CREN = ENABLE;
		return TRUE;
	}
	
	return FALSE;
} 

//*****************************************************************************[ENDL]***************
