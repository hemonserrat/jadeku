/**
 * @file ONEWIRE.H
 * @brief  1 wire protocol functions.
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
#ifndef _ONEWIRE_H
#define _ONEWIRE_H 1

#define W1_OK      0x00  ///< Function call success
#define W1_CRC     0x01  ///< CRC verification fail
#define W1_FOUND   0x02  ///< No device found

/**
* 1-wire prototypes
*/
BYTE OneWire_GetID(BYTE *buff);
BYTE OneWire_Init(void);
BYTE OneWire_Read(void);
void OneWire_Write(BYTE d);
BYTE OneWire_Crc8(BYTE Data, BYTE Accum);

#endif
/* ***************************************************************[ENDL]**** */



