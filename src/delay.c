/**
 * @file DELAY.C
 * @brief  Delay routines.
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
#include	<app.h>

/**
*  Delays ejecution in mult. of 10us.
*  @param   x - number of 10us units to delay.
*/
void delay_10us(unsigned char x)	
{
 unsigned char _dcnt; 
 _dcnt = x; 
   while(--_dcnt != 0) DelayUs(10); 
}// delay_10us

/**
*  Delays ejecution in mult. of millisecs
*  @param   t - number of millisecs to delay.
*/
void delay_ms(long t)
{
   do
   {
     delay_10us(100);
   } while(--t);
}// delay_ms

#ifdef _DELAYMS
/**
*  Delays ejecution in millisecs based on crystal frec.
*  @param   cnt - number of millisecs to delay.
*/
void DelayMs(unsigned char cnt)
{
#if	XTAL_FREQ <= 2MHZ
	do {
		DelayUs(996);
	} while(--cnt);
#endif

#if    XTAL_FREQ > 2MHZ	
	unsigned char	i;
	do {
		i = 4;
		do {
			DelayUs(250);
		} while(--i);
	} while(--cnt);
#endif
}// DelayMs
#endif
/* ***************************************************************[ENDL]**** */


