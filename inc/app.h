/**
 * @file APP.H
 * @brief  JADEKU project defines and dependency headers
 *         
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
#ifndef _APP_H_
#define _APP_H_ 1

#define VERSION    0x05

#include	<pic.h>
#include	<stdio.h>
#include	<stdlib.h>
#include    <string.h>
#include    <math.h>    ///< required for the ffs
#include    "pgrl.h"    ///< macros and types
#include    "sci.h"     ///< Serial Comm Interface
#include    "delay.h"   ///< delay routines
#include    "onewire.h"   ///< iButton Interface
#include    "spi.h"     ///< SPI Interface
#include    "at45xx.h"  ///< FLASH Interface
#include    "time.h"    ///< Time functions
#include    "fs.h"      ///< File System

/** 
* Flash control
*/
//#define FCS0 RC2         ///< OUTPUT

/**
* BUZZER and LED
*/
#define LB1   RB1        ///< O
#define LB2   RB2        ///< O

/**
* iButton 
*/
#define TOUCH RB0        ///< I

/**
* Comparator
*/
//#define VSENS RB4        ///< I
#define TAMP  RB5        ///< I
#define WAKE  RB4        ///< I

#define  F_TAMP    0x01         ///< Tamper (case open)
#define  F_VSENS   0x02         ///< Low Battery 
#define  F_INTR    0x04         ///< Touch present
#define  F_PCMD    0x08         ///< Command to process
#define  F_WAITACK 0x10         ///< Waiting ACK
#define  F_DOWN    0x20         ///< Download REGIS Db
#define  F_SPACE   0x40         ///< Available space warn
#define  F_NEXT    0x80         ///< Send next reg


#define  FN_TAMP    0
#define  FN_VSENS   1       
#define  FN_INTR    2      
#define  FN_PCMD    3      
#define  FN_WAITACK 4      
#define  FN_DOWN    5      
#define  FN_SPACE   6      
#define  FN_NEXT    7      

#define  GE_FILE      0  ///< Error on file operation
#define  GE_LTA       1  ///< Error on comm
#define  GE_NOSPACE   2  ///< No enogh disk space

/**
* CRONOS Protocol defines
*/
#define CR_STX     0x02
#define CR_ETX     0x03
#define CR_ACK     0x06
#define CR_NAK     0x15

#define FM_IDLE   0x00
#define FM_INIT   0x01
#define FM_END    0x02

#define CR_START    'E'
#define CR_DEV      '1'

#define CR_INIT0    'A'
#define CR_INIT1    'B'
#define CR_ENABLE   '4'
#define CR_DISABLE  '5'
#define CR_NUM      'R'
#define CR_DOWN     '0'
#define CR_DELETE   '2'
#define CR_RESET    137
#define CR_DATETIME '1'

/**
* new commands
*/
#define CR_FORMAT   'F' ///< -> STX|E|1|F|x|x|x|ETX|XOR    <- ACK  
#define CR_INFO     'I' ///< -> STX|E|1|I|ETX|XOR   <- ACK > STX|I|n|x|x|x|e|e|e|ETX|XOR
#define CR_FF       'D' ///< -> STX|E|1|D|ETX|XOR   <- ACK > STX|nnnnnnn|ETX|XOR

extern volatile BYTE   gFlags;

#endif // _APP_H_
/* ***************************************************************[ENDL]**** */

