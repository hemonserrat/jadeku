/**
 * @file AT45xx.H
 * @brief  Serial Data Flash Driver.
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
#include <app.h>


/**
*  Read the status byte from the device
*  @return  the status byte.
*/
BYTE DF_ReadStatus(void)
{
   BYTE Data;

   DF_CS_LO();

   //command
   SPI_Write(DF_STATUS);

   Data = SPI_Read();

   DF_CS_HI();

   return (Data);
}

/**
*  Store a buffer to the internal Buffer 1.
*  @param   Address - offset from the internal buffer.
*           Data    - buffer to store data
*           size    - number of bytes to read
*/
void DF_WriteBuffer1(WORD Address, BYTE* Data, WORD size)
{
   DF_CS_LO();

   //command
   SPI_Write(BUFFER1_WRITE);
   //don't care 8 bits
   SPI_Write(0);
   //2 high address bits
   SPI_Write(*((BYTE*)&Address +1));
   //8 low address bits
   SPI_Write((BYTE)Address);
   //write data byte
  do { 
   SPI_Write(*Data);
   Data++;
  } while(--size);
  
   DF_CS_HI();
} 

/**
*  Reads contents from the buffer 1.
*  @param   Address - offset from the internal buffer.
*           data    - buffer to store data
*           size    - number of bytes to read
*/
void DF_ReadBuffer1(WORD Address, BYTE* data, WORD size)
{
   DF_CS_LO();

   //command
   SPI_Write(BUFFER1_READ);
   //don't care 8 bits
   SPI_Write(0);
   //2 high address bits
   SPI_Write(*((BYTE*)&Address +1));
   //8 low address bits
   SPI_Write((BYTE)Address);
   //don't care 8 bits
   SPI_Write(0);
   do {
      *data = SPI_Read();
      data++;
   }while(--size);
   DF_CS_HI();
}

/**
*  Erase, sets to 1's, the selected page.
*  @param   PageNum - Page number to erase.
*/
#ifdef _ERASEPAGE
void DF_ErasePage(WORD PageNum)
{
   DF_CS_LO();

   PageNum <<= 1;

   //command
   SPI_Write(PAGE_ERASE);
   // high address bits
   SPI_Write(*((BYTE*)&PageNum + 1));
   // low address bits
   SPI_Write((BYTE)PageNum);
   //don't care 8 bits
   SPI_Write(0);

   DF_CS_HI();
}
#endif
/**
*  Copy Buffer 1 to Main Memory, erase first.
*  @param   PageNum - Page number to copy buffer contents.
*/
void DF_Buffer12MainMemoryE(WORD PageNum)
{
   PageNum <<= 1;

   DF_CS_LO();

   //command
   SPI_Write(BUFFER1_PROGRAM_WITH_ERASE);
   // high address bits
   SPI_Write(*((BYTE*)&PageNum + 1));
   // low address bits
   SPI_Write((BYTE)PageNum);
   //don't care 8 bits
   SPI_Write(0);

   DF_CS_HI();
} 

/**
*  Get Page to Buffer 1
*  @param   Pagenum - Page numbre to get.
*/
void DF_Page2Buffer1(WORD Pagenum)
{
   Pagenum <<= 1;
   DF_CS_LO();
   
   //command
   SPI_Write(MAIN_MEMORY_PAGE_TO_BUFFER1_COPY);
   // high address bits
   SPI_Write(*((BYTE*)&Pagenum + 1));
   // low address bits
   SPI_Write((BYTE)Pagenum);
   //don't care 8 bits
   SPI_Write(0);
   
   DF_CS_HI();
} 

/**
*  Auto page rewrite command
*  @param   PageNum - Page to execute command.
*/
void DF_AutoPageRewrite(WORD PageNum)
{
   DF_CS_LO();

   PageNum <<= 1;

   //command
   SPI_Write(AUTO_PAGE_THROUGH_BUFFER1_REWRITE);
   // high address bits
   SPI_Write(*((BYTE*)&PageNum + 1));
   // low address bits
   SPI_Write((BYTE)PageNum);
   //don't care 8 bits
   SPI_Write(0);

   DF_CS_HI();
} 

/**
*  Compare buffer and page. Read Status value to see result.
*  @param   PageNum - Page to execute command.
*/
void DF_Compare(WORD Pagenum)
{
   Pagenum <<= 1;
   DF_CS_LO();
   
   //command
   SPI_Write(MAIN_MEMORY_PAGE_TO_BUFFER1_COMPARE);
   // high address bits
   SPI_Write(*((BYTE*)&Pagenum + 1));
   // low address bits
   SPI_Write((BYTE)Pagenum);
   //don't care 8 bits
   SPI_Write(0);
   
   DF_CS_HI();
} 
//*****************************************************************************[ENDL]***************
