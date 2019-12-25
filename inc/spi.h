/** @file SPI.H
 @brief  Serial peripheral Interface.
         Target platform dependent.

 @attention Copyright (c) 2000 JANUS.                                
  This computer program includes confidential, proprietary 
  information and is a trade secret of JANUS.  All         
  use, disclosure, and/or reproduction is prohibited unless
  expressly authorized in writing.                    
  All rights reserved.                                     
 
 @author  hgm
 @version $Header: /home/cvsroot/jadeku/spi.h,v 1.1.1.1 2006-09-12 16:57:15 hernan Exp $
*/
//******************************************************************************
#ifndef _SPI_H
#define _SPI_H

extern volatile bit gSpiTx;
extern volatile bit gSpiRx;

/**
* Function prototypes
*/
void SPI_Init( void );
BYTE SPI_Exchange(BYTE data);
void SPI_Write(BYTE data);
BYTE SPI_Read(void);

#endif // _SPI_H
//*****************************************************************************[ENDL]***************



