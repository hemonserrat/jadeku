/**
 * @file FS.H
 * @brief File System.
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
#ifndef _FS_H
#define _FS_H 1

/**
* First mark of file system
*/
#define FSMARK     0xDADA
/**
* a file mark 3Fxx where xx is the file number.
*/
#define BOOTSECTOR 0x0000

#define FSFM_OFF   0x0000  ///< Format mark
#define FSDT_OFF   0x0002  ///< date time
#define FSTZ_OFF   0x000A  ///< Time zone
#define FSDN_OFF   0x000E  ///< Device number

#define FSOK        0
#define FSNOSPACE   1
#define FSINVALID   2
#define FSNOTFOUND  3
#define FSNOCHIP    4
#define FSEOF       5

#define FSAPPEND    1
#define FSRW        0
#define FSEXIST     2

#define FSSET       0
#define FSCUR       1
#define FSEND       2

// #define REGISDIR    0x001C
// #define UNITDIR     0x002A
#define REGSPA       1
#define DEFMAXREGIS  (REGSPA+500)
#define UNITSPA      (DEFMAXREGIS)
#define DEFMAXUNIT   (UNITSPA+11)
#define BADPAGE      0xAA
#define REGCRL_OFF   258
#define UNICRL_OFF   261

/**
* File descriptor
*/
typedef struct {
WORD   nor;    ///< number of records/file size
WORD   cp;     ///< current page 
WORD   cb;     ///< current byte
} _fsFILE;

typedef _fsFILE*  fsFILE;

typedef struct {
time_t  ttime;
WORD    id;
} REGIS;  // 6b (500p=22K)

typedef struct {
BYTE    state;
WORD    id;
} UNIT; // 3b  (1p=88)

#define U_ENABLE  0x80
#define U_DISABLE 0x00

#define DBREGIS  1
#define DBUNIT   2

/**
* Function prototypes
*/
BYTE FS_Init( void );
void FS_Format( void );
BYTE FS_Open(BYTE file);
BYTE FS_Close(BYTE file);
BYTE FS_AppendRegis(REGIS *data);
BYTE FS_AppendUnit(UNIT *data);
void FS_ZapRegis(void);
BYTE FS_ReadRegis(REGIS *data);
BYTE FS_WriteUnit(UNIT *data);
BYTE FS_ReadUnit(UNIT *data);
void FS_SetBoot(WORD Offset, BYTE* data, BYTE size);
void FS_GetBoot(WORD Offset, BYTE* data, BYTE size);
void FS_Check( void );
void FS_Length(BYTE file, WORD *p);

void FS_WaitReady(void);

extern bank3 WORD gRcp;
extern bank3 WORD gRnor;
extern bank3 WORD gRcb;
extern bank3 WORD gUcp;
extern bank3 WORD gUnor;
extern bank3 WORD gUcb;


#endif // _FS_H
//********************************************************[ENDL]***************



