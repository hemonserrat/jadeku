/**
 * @file ONEWIRE.C
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
#include <app.h>

#define PWR_ON_PIN RA1	    ///< RA.1 - power to DS2401
#define PWR_ON_DIR TRISA1

#define ID_DQ_PIN RB0		///< RB.0 - DQ for DS2401
#define ID_DQ_DIR TRISB0

#define OneWire_PinHI()   TRISB0 = DIR_IN
#define OneWire_PinLO()  { RB0 = 0x00; TRISB0 = DIR_OUT; }
#define W_CELL 4
#define W_INIT 50
#define W_WAIT 2000

/**
*  Test for the 1 wire device presence, read ROM code and calculates crc.
*  @param   buff - buffer to store read data.
*  @return  TRUE on packet received, FALSE timeout.
*  @remarks  note that 8-byte ID is returned in array buff.
*/
BYTE OneWire_GetID(BYTE *buff)
{
   BYTE count, crc;
   ID_DQ_DIR = DIR_IN;

   if(OneWire_Init()==0) 
       return W1_FOUND;

   OneWire_Write(0x33);	// read ROM

   for (count=0; count<8; count++)
   {
      buff[count]=OneWire_Read();
   }
    ID_DQ_DIR = DIR_IN;
    
   crc=0; 
   for (count = 0; count < 8; count++) crc = OneWire_Crc8(buff[count], crc);  
   if( crc ) return W1_CRC;
   return W1_OK;
}

/**
*  Send initialization flags to the 1 wire network.
*  @return  TRUE on device presence.
*/
BYTE OneWire_Init(void)
{
 WORD w; 
 BYTE is;
   OneWire_PinHI();
   OneWire_PinLO();
   delay_10us(W_INIT);
   OneWire_PinHI();
   w = W_WAIT;
   is=0;
   while(w) 
   {
     if( bittst(gFlags,FN_PCMD)  ) 
                 return 0; // pending command or in process
     --w;
     if( !ID_DQ_PIN ) is=1;
   }
 return is;
}

/**
*  Read a byte from the 1 wire network.
*  @return  byte read.
*/
BYTE OneWire_Read(void)
{
   BYTE n, i_byte;
   for (n=0; n<8; n++)
   {
      RB0=0x00;
      TRISB0=DIR_OUT;
      TRISB0=DIR_IN;
   #asm
      clrwdt
      nop
      nop
   #endasm
      if (RB0)
      {
        i_byte=(i_byte>>1) | 0x80;	// least sig bit first
      }
      else
      {
        i_byte=i_byte >> 1;
      }
      delay_10us(W_CELL);
   }
   return(i_byte);
}

/**
*  Send a byte to the 1 wire network.
*  @param   d - data byte to send.
*  @return  none.
*/
void OneWire_Write(BYTE d)
{
   BYTE n;
   for(n=0; n<8; n++)
   {
      if (d&0x01)
      {
         RB0=0;
         TRISB0=DIR_OUT;		// momentary low
         TRISB0=DIR_IN;
         delay_10us(W_CELL);
      }

      else
      {
          RB0=0;
          TRISB0=DIR_OUT;
	       delay_10us(W_CELL);
          TRISB0=DIR_IN;
      }
      d=d>>1;
   }
}

/**
*  Computes CRC8.
*  @param   Data  - data byte to compute.
*  @param   Accum - accumulated result.
*  @return  the CRC8 accumulated result based on the params.
*/
BYTE OneWire_Crc8(BYTE Data, BYTE Accum)
{
BYTE i, f;

   for (i = 0; i < 8; i++)
   {
      f    = 1 & (Data ^ Accum);
      Accum       >>= 1;
      Data        >>= 1;
      if (f) Accum ^= 0x8c;  // b10001100 es la palabra del CRC (x8 + x5 + x4 + 1)
                             //  7..43..0  junto con el 1 aplicado a f.
   }
   return Accum;
}
//*****************************************************************************[ENDL]***************





