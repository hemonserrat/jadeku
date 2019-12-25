/**
 * @file PGRL.H
 * @brief  Microchip PIC C Generic, General defines.
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
#ifndef _GRL_H_
#define _GRL_H_ 1

/**
*  General Purpose Defines                                                 
*/
#ifndef NULL
#define NULL  0
#endif

#define NEAR                near
#define LONG                long
#define VOID                void
#define PASCAL              pascal

typedef unsigned char       BYTE;
typedef unsigned int        UINT;
typedef short int	        SINT;
typedef short int	        SHORT;
typedef unsigned short int	USHORT;
typedef int                 BOOL;

typedef unsigned int        WORD;
typedef unsigned long       DWORD;

typedef const char far      *LPCSTR;
typedef const char          *PCSTR;
typedef char                *PSTR;
typedef char near           *NPSTR;
typedef char far            *LPSTR;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;
typedef long near           *PLONG;
typedef long far            *LPLONG;
typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;
typedef void far            *LPVOID;

typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;

#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | (((DWORD)((WORD)(b))) << 16)))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)((WORD)(w) >> 8))
#define MAKELPARAM(low, high)   ((LPARAM)MAKELONG(low, high))
#define MAKELRESULT(low, high)  ((LRESULT)MAKELONG(low, high))
#define MAKELP(sel, off)    ((void FAR*)MAKELONG((off), (sel)))
#define SELECTOROF(lp)      HIWORD(lp)
#define OFFSETOF(lp)        LOWORD(lp)
#define FIELDOFFSET(type, field)    ((int)(&((type NEAR*)1)->field)-1)

#define bitset(var,bitno) ((var) |= 1 << (bitno))
#define bitclr(var,bitno) ((var) &= ~(1 << (bitno)))
#define bittst(var,bitno) ((var)&(1<<(bitno)))

#define ENABLE    1
#define DISABLE   0
#define HIGH      1
#define LOW       0
#define HENABLE   1
#define LDISABLE  0
#define LENABLE   0
#define HDISABLE  1
#define ON        1
#define OFF       0

#ifndef TRUE
    #define TRUE      1
    #define FALSE     0
#endif

#define HPULSE    1
#define LPULSE    0
#define CLEAR     0
#define SET       1
#define DIR_OUT   0
#define DIR_IN    1

#ifndef SLEEP
    #define SLEEP() asm("   sleep")
#endif    


#endif // _GRL_H_
/* ***************************************************************[ENDL]**** */

