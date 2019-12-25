/**
 * @file SCI.H
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
#ifndef _SCI_H
#define _SCI_H 1

// #define FOSC	(4000000L)
#define FOSC	(3686400L)
#define SCI_EIGHT	(0)
#define SCI_NINE	(1)

BYTE	SCI_Init(DWORD, BYTE);
void	SCI_Write(BYTE);
BYTE	SCI_Read(void);
void	sci_PutNinth(BYTE);
BYTE    sci_GetNinth(void);
BYTE  	sci_GetFERR(void);
BYTE  	SCI_CheckOverrun(void);

#endif
//********************************************************[ENDL]***************
