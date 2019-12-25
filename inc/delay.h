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
 * 
 * @note    Based on HiTech DELAY.C examples.
 *       
 *	Note that there are range limits: x must not exceed 255 - for xtal
 *	frequencies > 12MHz the range for DelayUs is even smaller.
 *	To use DelayUs it is only necessary to include this file; to use
 *	DelayMs you must include delay.c in your project.
 *
 *	Set the crystal frequency in the CPP predefined symbols list in
 *	HPDPIC, or on the PICC commmand line, e.g.
 *	picc -DXTAL_FREQ=4MHZ
 *	
 *	or
 *	picc -DXTAL_FREQ=100KHZ
 *	
 *	Note that this is the crystal frequency, the CPU clock is
 *	divided by 4.
 *
 *	MAKE SURE this code is compiled with full optimization!!!
*/
#ifndef _DELAY_H_
#define _DELAY_H_ 1

#ifndef	XTAL_FREQ
//#define	XTAL_FREQ	4MHZ		///< Crystal frequency in MHz 
#define	XTAL_FREQ	3686		///< Crystal frequency in MHz 
#endif

#define	MHZ	*1000		///< number of kHz in a MHz 
#define	KHZ	*1			///< number of kHz in a kHz 

#if	XTAL_FREQ >= 12 MHZ

#define	DelayUs(x)	{ unsigned char _dcnt; \
			  _dcnt = (x)*((XTAL_FREQ)/12MHZ); \
			  while(--_dcnt != 0){ \
				  CLRWDT(); continue;} }
#else

#define	DelayUs(x)	{ unsigned char _dcnt; \
			  _dcnt = (x)/(12MHZ/(XTAL_FREQ))|1; \
			  while(--_dcnt != 0){ \
				  CLRWDT(); continue;} }
#endif

extern void delay_10us(unsigned char x);
extern void delay_ms(long t);
extern void DelayMs(unsigned char);

#endif
/* ***************************************************************[ENDL]**** */
