/**
 * @file FS.C
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
#include "app.h"

bank3 WORD gRcp;
bank3 WORD gRnor;
bank3 WORD gRcb;
bank3 WORD gUcp;
bank3 WORD gUnor;
bank3 WORD gUcb;

void FS_FlashTest(void);

// Algorithm 2
bank3 WORD    gPA=0x0000; ///< Stored value in flash
bank3 WORD    gPC=0x0000; ///< temporal

/**
*  POLL Flash status device to see if ready
*  @remarks Need to implement a timeout, now if fails, blocks for ever.
*/
void FS_WaitReady(void)
{
 // may be more efficient to have a timeout of 1s
  while( !IS_DF_READY(DF_ReadStatus()) )
                               continue; // wait complete
}


/**
*  Periodic function to check and health the file system structure.
*  @remarks Use algorithm 2 from Serial data flash devices.
*/
void FS_Check( void )
{
  if( gPC==0 )
        return; // nothing to do
      DF_AutoPageRewrite(gPA);
      FS_WaitReady();
      gPA++;
      if( gPA > (MAX_PAGE_1MBIT_DENSITY-1) )
                          gPA = 0; // set to the first
  --gPC;
} 

/**
*  Initialize file system
*  @return  FSOK
*  @return  FSNOCHIP
*  @remarks if no valid boot sector found then formats
*/
BYTE FS_Init( void )
{
 WORD st;
  
     st=DF_ReadStatus();
     st&=DF_STATUS_DENSITY_MASK;
     if(st&DF_1MBIT_DENSITY_CODE)
     {
//       flashtest();
        // Read page 0 BOOT Record
        FS_GetBoot(FSFM_OFF, (BYTE*)&st, sizeof(WORD));
        if( st!=FSMARK ) // not formatted
        {
           FS_Format();
           gRnor=0;
           gUnor=0;
	 return FSOK; 
        }  
        FS_Length(DBREGIS, &st);
        gRnor=st;
        FS_Length(DBUNIT, &st);      
        gUnor=st;
        FS_GetBoot(FSTZ_OFF, (BYTE*)&timezone, sizeof(long));
       return FSOK; 
     }
return FSNOCHIP; // ERROR
}

/**
*  Sets boot sector information
*  @param   Offset - from buffer
*  @param   data   - buffer to extract data
*  @param   size   - number of bytes to write
*  @remarks Use buffer 1 to mirror.
*/
void FS_SetBoot(WORD Offset, BYTE* data, BYTE size)
{
BYTE r;
  DF_Page2Buffer1(BOOTSECTOR);
  FS_WaitReady();
  DF_WriteBuffer1(Offset, data, size);
  r=3;
  do {
   DF_Buffer12MainMemoryE(BOOTSECTOR);            
   gPC++;
   FS_WaitReady();         
   DF_Compare(BOOTSECTOR);
   FS_WaitReady();         
   if(IS_DF_COMP_OK(DF_ReadStatus()) )
                              return;
  } while(r--);
}
         
/**
*  Get boot sector information
*  @param   Offset - from buffer
*  @param   data   - buffer to store data
*  @param   size   - number of bytes to read
*  @remarks Use Buffer 1 to mirror.
*/
void FS_GetBoot(WORD Offset, BYTE* data, BYTE size)
{
  DF_Page2Buffer1(BOOTSECTOR);
  FS_WaitReady();
  DF_ReadBuffer1(Offset, data, size);
}

/**
*  Initialize and formats the storage device
*  @param   nops - Number of pages for files.
*/
void FS_Format( void )
{
 WORD data;
 WORD x;
 REGIS rg;
 UNIT  ut;
//********************** Init each page
     // clear buffer
     data=0xFFFF;
//     data=0x0101;
     for( x=0; x<PAGESIZE; x+=2 )
      DF_WriteBuffer1(x, (BYTE*)&data, sizeof(WORD));     
     // REGIS DB 1 to 500
     rg.ttime=0;
//     rg.ttime=43;
     rg.id =0;
        DF_WriteBuffer1(REGCRL_OFF,(BYTE*)&rg, sizeof(REGIS));
     for( x=REGSPA; x<DEFMAXREGIS; x++ )
     {
        DF_Buffer12MainMemoryE(x);            
        gPC++;
        FS_WaitReady();
     }
//********************** clear buffer
     data=0xFFFF;
     for( x=0; x<PAGESIZE; x+=2 )
      DF_WriteBuffer1(x, (BYTE*)&data, sizeof(WORD));     
     // UNIT DB 501 to 512
     ut.id=0;
     ut.state=0;
//     ut.state=87;
        DF_WriteBuffer1(UNICRL_OFF,(BYTE*)&ut, sizeof(UNIT));
     for( x=UNITSPA; x<DEFMAXUNIT; x++ )
     {
        DF_Buffer12MainMemoryE(x);            
        gPC++;
        FS_WaitReady();
     }
//*************************** clear buffer
     data=0xFFFF;
     for( x=0; x<PAGESIZE; x+=2 )
      DF_WriteBuffer1(x, (BYTE*)&data, sizeof(WORD));     
     // Format mark
     data = FSMARK;
     DF_WriteBuffer1(FSFM_OFF, (BYTE*)&data, sizeof(WORD));     
     DF_WriteBuffer1(FSDT_OFF, (BYTE*)&TIME, sizeof(time_t));   // date time
     DF_WriteBuffer1(FSTZ_OFF, (BYTE*)&timezone, sizeof(long)); // Time zone
     data = '1';
     DF_WriteBuffer1(FSDN_OFF, (BYTE*)&data, sizeof(BYTE));        // Device number
     // Write page
     DF_Buffer12MainMemoryE(BOOTSECTOR);      
     gPC++;
     FS_WaitReady();
}


/**
*  Open a file stream 
*  @param   file - stream descriptor pointer.
*  @return  FSOK
*/
BYTE FS_Open(BYTE file)
{
  switch(file)
  {
    case DBREGIS:
         gRcb=0;
         gRcp=REGSPA;
    break;
    case DBUNIT:
         gUcb=0;
         gUcp=UNITSPA;
    break;
    default:
      return FSNOTFOUND;
  }     
 return FSOK;
}

/**
*  Close the given stream.
*  @param   file - stream descriptor pointer.
*  @return  FSOK
*           FSNOTFOUND
*/
BYTE FS_Close(BYTE file)
{
  switch(file)
  {
    case DBREGIS:
    break;
    case DBUNIT:
    break;
    default:
      return FSNOTFOUND;
  }  
 return FSOK;
}
/**
 *  Return the number of records in the stream
 *  @param   file - stream descriptor pointer.
 *  @param   p    - pointer to a word
 */
void FS_Length(BYTE file, WORD *p) 
{
    WORD x;
    WORD l;
    REGIS rg;
    UNIT ut;

    l = 0;
    *p = 0;
    switch (file) 
    {
        case DBREGIS: //APPEND
            // set last page and byte buffer
            for (x = REGSPA; x < DEFMAXREGIS; x++) 
            {
                DF_Page2Buffer1(x);
                FS_WaitReady();
                DF_ReadBuffer1(REGCRL_OFF, (BYTE*) & rg, sizeof (REGIS));
                if (rg.id != BADPAGE)
                    if ((WORD) rg.ttime == 0) {
                        *p = l;
                        return;
                    }
                l += (WORD) rg.ttime;
            }
            break;
        case DBUNIT:
            // set last page and byte buffer
            for (x = UNITSPA; x < DEFMAXUNIT; x++) 
            {
                DF_Page2Buffer1(x);
                FS_WaitReady();
                DF_ReadBuffer1(UNICRL_OFF, (BYTE*) & ut, sizeof (UNIT));
                if (ut.id != BADPAGE)
                    if (ut.state == 0) {
                        *p = l;
                        return;
                    }
                l += ut.state;
            }
            break;
        default:
            return;
    }
    *p = l;
} 
/**
 *  Deletes records on REGIS stream
 */
void FS_ZapRegis(void) 
{
    WORD x;
    REGIS rg;
    for (x = REGSPA; x < DEFMAXREGIS; x++) 
    {
        DF_Page2Buffer1(x);
        FS_WaitReady();
        DF_ReadBuffer1(REGCRL_OFF, (BYTE*) & rg, sizeof (REGIS));
        if (rg.ttime == 0) 
        {
            gRnor = 0;
            return;
        }
        rg.ttime = 0;
        rg.id = 0;
        DF_WriteBuffer1(REGCRL_OFF, (BYTE*) & rg, sizeof (REGIS));
        DF_Buffer12MainMemoryE(x);
        gPC++;
        FS_WaitReady();
    }
    gRnor = 0;
} 

/**
*  Append data to the REGIS stream.
*  @param   data - buffer to extract data.
*  @return  FSOK, FSEOF
*/
BYTE FS_AppendRegis(REGIS *data)
{
REGIS rg;
BYTE r;
     // set last page and byte buffer
     gRcp = (gRnor/43)+REGSPA;
     if( gRcp >= DEFMAXREGIS )
                    return FSEOF;
     gRcb = (gRnor%43)*sizeof(REGIS);
     if( gRcb >= (PAGESIZE-sizeof(REGIS)) )
                    return FSEOF;
  // read page
  DF_Page2Buffer1(gRcp);
  FS_WaitReady();

  DF_ReadBuffer1(REGCRL_OFF, (BYTE*)&rg, sizeof(REGIS));
  DF_WriteBuffer1(gRcb,(BYTE*)data, sizeof(REGIS));
  rg.ttime++;
  gRnor++;
  DF_WriteBuffer1(REGCRL_OFF,(BYTE*)&rg, sizeof(REGIS));
  r=3;
  do {
   DF_Buffer12MainMemoryE(gRcp);            
   gPC++;
   FS_WaitReady();         
   DF_Compare(gRcp);
   FS_WaitReady();         
   if(IS_DF_COMP_OK(DF_ReadStatus()) )
                              return FSOK;
  } while(r--);

 return FSEOF;
} 

/**
*  Read a number of bytes from the REGIS stream
*  @param   data - buffer to store data.
*  @return  FSOK
*           FSEOF
*/
BYTE FS_ReadRegis(REGIS *data)
{                              
REGIS rg;
  if( gRcb >= (PAGESIZE-sizeof(REGIS)) )
  {
      // check for page overflow
      gRcp++;
       if( gRcp >= DEFMAXREGIS )
                     return FSEOF;
      gRcb=0;
  }
  // read page
  DF_Page2Buffer1(gRcp);
  FS_WaitReady();
  DF_ReadBuffer1(REGCRL_OFF, (BYTE*)&rg, sizeof(REGIS));
  if( rg.ttime == 0 ) return FSEOF;
  if( gRcb > ((rg.ttime-1)*sizeof(REGIS)) )
     return FSEOF;
   DF_ReadBuffer1(gRcb,(BYTE*)data, sizeof(REGIS));
   gRcb+=sizeof(REGIS);               
 return FSOK;
}

/**
*  Append data to the UNIT stream.
*  @param   data - buffer to extract data.
*  @return  FSOK, FSEOF
*/
BYTE FS_AppendUnit(UNIT *data)
{
UNIT ut;
BYTE r;
     gUcp = (gUnor/87)+UNITSPA;
     if( gUcp >= DEFMAXUNIT )
                    return FSEOF;
     gUcb = (gUnor%87)*sizeof(UNIT);
     if( gUcb >= (PAGESIZE-sizeof(UNIT)) )
                    return FSEOF; 
  // read page
  DF_Page2Buffer1(gUcp);
  FS_WaitReady();
  DF_ReadBuffer1(UNICRL_OFF, (BYTE*)&ut, sizeof(UNIT));
  DF_WriteBuffer1(gUcb,(BYTE*)data, sizeof(UNIT));
  ut.state++;
  gUnor++;
  DF_WriteBuffer1(UNICRL_OFF,(BYTE*)&ut, sizeof(UNIT));
  r=3;
  do {
   DF_Buffer12MainMemoryE(gUcp);            
   gPC++;
   FS_WaitReady();         
   DF_Compare(gUcp);
   FS_WaitReady();         
   if(IS_DF_COMP_OK(DF_ReadStatus()) )
                              return FSOK;
  } while(r--);
 return FSEOF;
} 

/**
*  Write data to the UNIT stream.
*  @param   data - buffer to extract data.
*  @return  FSOK, FSEOF
*/
BYTE FS_WriteUnit(UNIT *data)
{                                 
BYTE r;
  // read page
  DF_Page2Buffer1(gUcp);
  FS_WaitReady();
  DF_WriteBuffer1(gUcb,(BYTE*)data, sizeof(UNIT));
  r=3;
  do {
   DF_Buffer12MainMemoryE(gUcp);            
   gPC++;
   FS_WaitReady();         
   DF_Compare(gUcp);
   FS_WaitReady();         
   if(IS_DF_COMP_OK(DF_ReadStatus()) )
                              return FSOK;
  } while(r--);
 return FSEOF;
} 

/**
*  Read a number of bytes from the UNIT stream
*  @param   data - buffer to store data.
*  @return  FSOK
*           FSEOF
*/
BYTE FS_ReadUnit(UNIT *data)
{                              
UNIT ut;
  if( gUcb >= (PAGESIZE-sizeof(UNIT)) )
  {
      // check for page overflow
      gUcp++;
      if(gUcp >= DEFMAXUNIT)
               return FSEOF;
      gUcb=0;
  }
  // read page
  DF_Page2Buffer1(gUcp);
  FS_WaitReady();
  DF_ReadBuffer1(UNICRL_OFF, (BYTE*)&ut, sizeof(UNIT));
  if( ut.state == 0 ) return FSEOF;
  if( gUcb > ((ut.state-1)*sizeof(UNIT)) )
     return FSEOF;
   DF_ReadBuffer1(gUcb,(BYTE*)data, sizeof(UNIT));
   gUcb+=sizeof(UNIT);               
 return FSOK;
}
//*********************************************************[ENDL]***************



