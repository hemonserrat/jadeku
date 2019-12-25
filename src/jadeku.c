/**
 * @file JADEKU.C
 * @brief JADEKU Portable ADQ device for Routines Control.
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
 * 
 * @note    Platform ADQ128 V2 Embedded Board. HT-PIC V8.05PL2   
 */
#include <app.h>
/* 
  - File system resources
   REGS 500 pages 1 to 500 = 21500 regs.
   UNIT  11 pages 501 to 511 = 957 units.
  - Com
   download regs at 500 regs per min.
*/
/**
*                      
*  configuration bits: 
*      OSC: XT         
*     WDTE: DISABLE     
*    PWRTE: DISABLE     
*    BODEN: DISABLE     
*      LVP: HV DISABLE         
*      CPD: PROTECTED  
*      WRT: NO EECON   
*    DEBUG: DISABLE    
*  CP1|CP0:  OFF       
*                      
*/
// __CONFIG(UNPROTECT|FOSC0);
// __CONFIG(PROTECT|BKBUG|PWRTE|FOSC0);
//__CONFIG(PROTECT&XT&DEBUGDIS&WRTDIS&DPROT&BORDIS&PWRTDIS&WDTDIS);
// EDC - Added DP and LVPDIS. Removed PROTECT bit.
//__CONFIG(DP&XT&DEBUGDIS&WRTDIS&DPROT&BORDIS&PWRTDIS&WDTDIS&LVPDIS); // NORMAL

#define  _USESLEEP 1

/**
*       Local Function Prototypes         
*/
static BYTE SearchbyID(WORD id);
static BYTE Save(WORD id);
static void Process(void);
static BYTE CRO_Crc(volatile BYTE *data, BYTE size);
static void SendRegis(void);
static VOID xitoa(WORD n, BYTE* s, BYTE pad);
static VOID xreverse(BYTE* s);
static void  InitialState(void);
//static void  beep(BYTE time);
static void  DoOK(void);
static void  DoError(void);
static BYTE ccp(BYTE idx);
static void InitPacket(BYTE cmd);
static void Add(BYTE num);
static void Init0(void);
static void Init1(void);
static void SendRegis(void);
static void SendInfo(void);
static void SendFlash(void);
static void SetTime(BYTE dir);
static void CheckUnits(WORD id);
static void Process(void);

/**
*         Variables               
*/
volatile BYTE   gFlags=0x00;

static bank1 WORD gIbut;             ///< Current touch read
static bank1 WORD gTbut;             ///< antiping touch read
static bank1 BYTE gBuf[10];          ///< general buffer
static bank2 BYTE gIdx;              ///< index to build packets

static volatile bank1 time_t gTimeout=0;  ///< Protocol Timeout
static volatile bank2 BYTE gRetry=0;      ///< Retry count

volatile bank1 time_t TIME=0;       ///< Timer in unix format
#define OUTBUFSIZE  20
volatile bank1 BYTE  gOut[20];      ///< Output buffer
#define INBUFSIZE   20           
volatile bank1 BYTE  gIn[20];       ///< Input buffer
volatile bank2 BYTE  gTXCount=0;    ///< Output buffer index
volatile bank2 BYTE  gRXCount=0;    ///< Input buffer index
volatile bank2 BYTE  gByte='A';     ///< Input byte
volatile bank2 BYTE  gError=0x00;   ///< Error flag state

#ifdef _USESLEEP
volatile bank2 BYTE gSleepTime;  ///< Count to sleep
volatile BYTE gSleep=0;          ///< Sleep mode enable
#endif

volatile bank2 time_t gTampTime;      ///< Tamper loop time
volatile bank2 BYTE   gState=FM_IDLE; ///< Protocol Receiver state machine
volatile bank2 BYTE   gCrc=0;         ///< Protocol Checksum


#define WLONG   100                   ///< Wait long time
#define WSHORT  25                   ///<  Wait short time
#define BLONG   2000                   ///< Beep long time
#define BSHORT  1000                   ///< Beep short time


/**
*  Sets ports for low power sleep mode.
*/
void setports(void)
{
   // set all ports as outputs (low current drain)
   ADCON1=0x00;
   PORTA=0x00; TRISA=0x00;  
   RA0=1;
   DF_CS_HI();  // chip select HI of FLASH

   RB1=0; TRISB1=0;  
   RB2=0; TRISB2=0;  
   RB3=1; TRISB3=0;  
} // setports

/**
*  Sets initilization status and variables for the CPU and systems.
*/
static void  InitialState(void)
{
   di();           // disable all interrupts
      
   OPTION_REG=0b00111111;    // 0011 1 111

   // set all ports as outputs (low current drain)
   PORTA=0x00; TRISA=0x00;  
   PORTB=0x00; TRISB=0x00;  
   PORTC=0x00; TRISC=0x00;  
   RA0=1;
   RB3=1;
   DF_CS_HI();  // chip select HI of FLASH
   
   TRISC7=DIR_IN;    // Set RX as input
#ifdef _USESLEEP
   TRISB4=DIR_IN;    // Set WAKE as input
#endif
   TRISB5=DIR_IN;    // Set TAMP as input
   TRISB0=DIR_IN;    // Set TOUCH as input

   // RTC 
   TMR1L=0x00;
   TMR1H=0x00;     // 2 sec at 32Khz
   TMR1ON=OFF;     // Stops timer1
   T1CON=0x3E;     // 0011 1110
    
    SCI_Init(9600,SCI_EIGHT);
    
    PIR1 = CLEAR;	// clear any pending interrupts 
    PEIE = ENABLE;	// enable perhipheral interrupts
    // perform other setup 
    RCIE = ENABLE;	// unmask receiver interrupts... 
    // an interrupt could now come from the SCI receiver  at any time. 
    TXIE = ENABLE;   
    // WAKE and TAMP could now generate an interrupt
    RBIF=CLEAR;     // clear
    RBIE=ENABLE; 
    // Enable Timer1 Interrupt
    TMR1IE=ENABLE;   
    TMR1ON=ON;   // TURN ON TIMER
    // Enable RB0 Interrupt
    INTEDG=CLEAR;  // falling edge  
    INTF=CLEAR;    // clear
    INTE=ENABLE; 
    
    SPI_Init(); // SPI bus init
    
    ei();	    // global interrupts enabled 
    
} // InitialState 

/**
*  Activates the beeper and green led for a lapsed time.
*  @param   time - number of milliseconds to sound.
*/
#define  beep(time)  piezo(18,time)

static void piezo(BYTE t, WORD d)
{
	d=(WORD)(d/3);
    bitclr(TRISB,1);
    bitclr(TRISB,2);
	LB2=ON;
	LB1=ON;
    while(--d){
//	    LB1=OFF;
//	    LB2=ON;
            delay_10us(t);  
//	    LB1=ON;
//	    LB2=OFF;
            delay_10us(t);  
   }    
//#define CONSUMO
#ifndef CONSUMO
	LB1=OFF;
	LB2=OFF;
#endif
}


/**
*  If all is ok beep and show green status on the LED.
*  if battery low tilt.
*/
static void  DoOK(void)
{
 BYTE tmp;
  if( bittst(gFlags, FN_VSENS) ) // low batery
  {
     tmp = 4;
     do{
       beep(BSHORT);
       delay_ms(WSHORT); // t2
     }while(--tmp);
     beep(BLONG);
   return;  
  }
  if( bittst(gFlags, FN_SPACE) ) // low space
  {
     beep(BSHORT);
    return; 
  }
  // All OK
  beep(BLONG);
} // DoOK

/**
*  Sets the error indicator in the LED (red)
*/
static void  DoError(void)
{
BYTE f;
  f = 3;
 do { 
   bitclr(TRISB,1);
   bitclr(TRISB,2);
   LB1=ON;
   delay_ms(25);
   LB2=OFF;
   delay_ms(25);  // t1 = 100
   LB1=OFF;
   LB1=OFF;
   delay_ms(25);  // t2 = 50
 }while(--f);  
} /* DoError */

//#pragma interrupt_level 1
/**
*  Interrupt service routine:
*  services: SCI, SPI, RTC, I/Os
*/
static void interrupt isr(void)
{
    // RTC must be always active
    if( TMR1IF ) // RTC Overflow
    {
      TMR1IF=CLEAR;
      TMR1H=0x00; // reload value;
      T1CON=0x3F;     // 0011 1110
      TIME+=16;
      CLRWDT();   // if TMR1 is turned off die.
      return;
    }// TMR1IF
    // Serial Receive always active
	if(RCIF)
	{
#ifdef _USESLEEP      
          gSleep = 0;
          gSleepTime=0;
#endif       
	  gByte = SCI_Read();
        if( gState == FM_END )
        {
          if( gCrc == gByte ){
              SCI_Write(CR_ACK);                                  
              bitset(gFlags, FN_PCMD);
          }          
          gState=FM_IDLE;
        } else {
      
           switch(gByte)
           {
            case CR_NAK:
                    if( bittst(gFlags, FN_DOWN) ) 
                             TXIE=ENABLE; // send again                                        
            break;
            case CR_ACK:
                 if( bittst(gFlags, FN_WAITACK) ) // waiting for an ACK ?
                 {
                    bitclr(gFlags, FN_WAITACK);
                    if( bittst(gFlags, FN_DOWN) ) 
                         bitset(gFlags, FN_NEXT);
                 }
           break;
           case CR_STX: 
                if( bittst(gFlags, FN_PCMD) )  // if waiting to finish a command
               		SCI_Write(CR_NAK);
                else {    
                    gRXCount=0;
                    gState=FM_INIT;
                    gCrc=CR_STX;
                }    
           break;
           case CR_ETX: 
                  if( gState==FM_INIT)
                  {
                    gState=FM_END;
                    gCrc^=CR_ETX;
                  }  
           break;
           default:
                    if( gRXCount > INBUFSIZE )  
                            gRXCount=0;
                    gIn[gRXCount++]=gByte;   
                    gCrc^=gByte;
        }// switch
       } // if-else
      }// RCIF
	if(TXIF && TXIE)
	{
  	 SCI_Write(gOut[gTXCount++]);
	 if(gTXCount >= gIdx)
         {
      	    TXIE = DISABLE;	// finished 
            gTXCount=0;
            gTimeout = TIME;
         }            
	}// TXIF
    if( RBIF ) // TAMP, VSENS, WAKEUP
    {
#ifdef _USESLEEP      
       gSleep = 0;
       gSleepTime=0;
#endif              
       if( !TAMP )
       {          
	 if( bittst(gFlags, FN_TAMP) )
	     bitclr(gFlags, FN_TAMP);      // reset flag
	 else 
             bitset(gFlags, FN_TAMP);
       }
      PORTB=PORTB;  // end mismatch condition
      RBIF=0; // clear flag      
    }// RBIF
    if( INTF ) // TOUCH
    {
      INTE=DISABLE; // disable ints
#ifdef _USESLEEP      
          gSleep = 0;
          gSleepTime=0;
#endif       
          bitset(gFlags, FN_INTR);      // mark
      INTF=CLEAR; // clear flag
    } // INTF
    
   if( SSPIF )
   {
    SSPIF=CLEAR;    
    if( gSpiTx )
           gSpiTx=CLEAR;
    else       
           gSpiRx=SET;
   }// SSPIF

}// isr 

/**
* Battery Level Monitor
*/
void TestBat(void)
{
  RA0=1;
  TRISA=0x0F;
  ADCON1=0x8C;
  ADCON0=0x41;
  RB3=0;
  RA5=1; // ON
  delay_ms(1); 
  //ADGO=1;  // charge
  ADCON0bits.GO =1;
  while(ADCON0bits.GO); 
  delay_ms(1); // WAIT 2.0TAD
//  ADGO=1;  // sample
  ADCON0bits.GO =1;
  while(ADCON0bits.GO);
  RA5=0; // OFF
  RB3=1;
  if( ADRESH < 3 ){   // 32D.  813*1.7=1.38*2=2,76V  
    if( ADRESL < 0x2D )        
      bitset(gFlags, FN_VSENS);
     else
      bitclr(gFlags, FN_VSENS);      // reset flag
  } else
      bitclr(gFlags, FN_VSENS);      // reset flag
  ADCON0=0x00;
//  ADCON1=0x07;
  ADCON1=0x00;
  TRISA=0x00;  
  RA0=1;
 
}


/**
* Start entry point. Polling loop.
*/
void main(void)
{
  // NOTE that Vsens (Low battery indicator) is reset only
  // when a Master reset is done.
  // gError is cleared when is TX to the host and on a reset

  
   InitialState();   
   memset((char*)gOut,0, OUTBUFSIZE);
   delay_ms(50); 

   
  // TO=1 and PD=1 ==> must init vars
  // TO=0 a WDT reset occurs
	if((STATUS & 0x10)) {		// power on-reset?
          SetTime(0); // default time
          DoOK();
          FS_Init();
	} else {			        // watchdog reset
          DoError();
	}

  DoOK();
  delay_ms(10); // t2
  gTampTime=TIME;
#ifdef _USESLEEP
  gSleepTime=0;
#endif
  // Main loop    
  do {
    if( bittst(gFlags, FN_NEXT) )
    { 
           SendRegis(); // send next packet
    }
    
     if(bittst(gFlags,FN_WAITACK)) // pending ACK
     {
       if( !TXIE ){ // not transmitting ?
         if( (TIME - gTimeout) > 5 ) // timeout waiting ACK
         {
          gTimeout = TIME; // actualize
            if( --gRetry )
                       TXIE=ENABLE; // send again
            else {  // sorry retries end
               bitset(gError, GE_LTA); // (Last TX not ACK)
               bitclr(gFlags, FN_WAITACK);
               bitclr(gFlags, FN_DOWN);
            }//retry           
          }//TIME
        }// TXIE
      bitclr(gFlags, FN_INTR); // disable any require
     }// ACK   

   SCI_CheckOverrun(); // clear serial OVERRUN
   if( bittst(gFlags,FN_PCMD)  ) // pending command or in process
   {
           Process();         // process command
     bitclr(gFlags, FN_PCMD); // finish
     bitclr(gFlags, FN_INTR); // disable any require
   }  else {  // CMD  
     // Sensor process
    switch( OneWire_GetID(gBuf) )
    { 
       case W1_OK:
          gIbut = gBuf[2];
          gIbut<<=8;
          gIbut |= (WORD)(gBuf[1] & 0x00FF);          
          if( gIbut == 0 || gIbut == 0xFFFF ) break;
	  if( gIbut==gTbut ) break; // antiping
	  gTbut = gIbut;	
          if( SearchbyID(gIbut) )
          {
             TestBat(); // charge
              if(Save(gIbut))              
                         DoOK();                          
	   delay_ms(10); // t2
          } else
            DoError();   // Access denied       
        break;
        case W1_FOUND:
               gTbut=0xFFFF;
        break;
   }
  }

   // Alarm flags
   if( bittst(gFlags, FN_TAMP) )
   {
    if( (TIME-gTampTime) > 60 )
    {
     Save(0x0000); // case open
     gTampTime=TIME;
    }
     bitclr(gFlags, FN_TAMP);      // reset flag
   }   
  
   // File system 
   FS_Check(); // do clean up

   // Put all to low power
   setports();    

#ifdef _USESLEEP
    gSleepTime++;
    if( gSleepTime > 200 )
    {
     gSleep=1;
     gSleepTime=0;
    }
   // Now with all done go to sleep
   if( gSleep )
   {
    // set RB0 as input
     bitclr(gFlags , FN_INTR); // clear mark    
     TRISB0=DIR_IN; // INPUT
      INTF=CLEAR;    // Clear flag
      INTE=ENABLE;   // enable ints INT0 wakeup
      SLEEP();
      CLRWDT();
      INTE=DISABLE; // disable ints
   }   
#endif   
  } while(1);

} // main 

/**
*  Converts an ASCII to its byte representation
*  from the input buffer SCI.
*  @param   idx - offset into the input buffer
*  @return  byte conversion of the ASCII value in the buffer.
*/
static BYTE ccp(BYTE idx)
{
  memcpy(gBuf, &gIn[idx], 2); // YY
  gBuf[2]=0;
  return (BYTE)atoi(gBuf);
}// ccp


/**
*  Initilize the outgoing buffer with the command to be send
*  @param   cmd - command to be send
*/
static void InitPacket(BYTE cmd)
{
  gOut[0]=CR_STX;
  gOut[1]=cmd;
  gIdx=2;
}// Initpacket

/**
*  Converts a byte number into its ASCII representation
*  and store it in the outgoing buffer.
*  @param   num - byte to convert and store.
*/
static void Add(BYTE num)
{
 xitoa(num, gBuf, 2); // MM
 gOut[gIdx++]=gBuf[0];
 gOut[gIdx++]=gBuf[1]; 
}// Add

/**
*  Send initialization info 2
*/
static void Init0(void)
{
BYTE tmp;
struct date d;
struct time t;
       // no wrap the buffer wait
       while( TXIE ) continue;        
       unixtodos(TIME, &d, &t);
       InitPacket(CR_INIT0);
       gOut[gIdx++]='0'; //  to 3
       Add(d.da_mon);  // MM
       Add(d.da_day);  // DD
       Add(t.ti_hour); // HH
       Add(t.ti_min);  // MM                  
       FS_GetBoot(FSDN_OFF, (BYTE*)&tmp, sizeof(BYTE));
       memset(&gOut[11],'0', 4);
       gOut[15]= tmp; 
       gOut[16]=CR_ETX;
       gOut[17]=CRO_Crc(gOut, 17);
       gIdx=18;
       bitset(gFlags, FN_WAITACK);  // Wait ACK
       gRetry=3;
       TXIE=ENABLE;      // Send
       while( TXIE ) continue;        
}// Init0

/**
*  Send Initialization info 1
*  @param   regis - number of available registrations to download.
*/
static void Init1(void)
{
struct date d;
struct time t;

      // no wrap the buffer wait
     while( TXIE ) continue;        
     
     FS_GetBoot(FSDT_OFF,(BYTE*)&gTimeout,sizeof(DWORD)); 
     unixtodos(gTimeout, &d, &t);                  
     
     InitPacket(CR_INIT1);
     gOut[gIdx++]='0'; // to 3
      Add(d.da_mon); // MM
      Add(d.da_day); // DD
      Add(t.ti_hour); // HH
     Add(t.ti_min); // MM
     xitoa(gRnor, gBuf, 5);  // regis
     memcpy(&gOut[11],gBuf, 5);
     
    gOut[16]=CR_ETX;
    gOut[17]=CRO_Crc(gOut, 17);
    gIdx=18;
    gRetry=3;
    bitset(gFlags, FN_WAITACK);  // Wait ACK
    TXIE=ENABLE;      // Send
    while( TXIE ) continue;        
}// Init1

/**
*  Retrieve next record from REGIS DB and send it to host.
*/
static void SendRegis(void)
{
REGIS r;
struct date d;
struct time t;
   // no wrap the buffer wait
   while( TXIE ) continue;        

   bitclr(gFlags, FN_NEXT);     // just do it

 if( FS_ReadRegis(&r)==FSOK)
 {
   InitPacket('2');
   gOut[gIdx++]='0';
   unixtodos(r.ttime, &d, &t);                  
   Add(d.da_mon);  // MM
   Add(d.da_day);  // DD
   Add(t.ti_hour); // HH
   Add(t.ti_min);  // MM
   xitoa(r.id, gBuf, 5);  // regis
   memcpy(&gOut[11],gBuf, 5);
   gOut[16]=CR_ETX;
   gOut[17]=CRO_Crc(gOut, 17);
   gIdx=18;
   gRetry=3;
   bitset(gFlags, FN_WAITACK);  // Wait ACK
   TXIE=ENABLE;      // send
   while( TXIE ) continue;        
 } else   // finish
   bitclr(gFlags, FN_DOWN); 
  
   
}// SendRegis


/**
*  Retrieve Flash sectors and send it to host.
*/
static void SendFlash(void)
{
WORD x;
WORD y;
BYTE n;
   // no wrap the buffer wait
   while( TXIE ) continue;          
   InitPacket(CR_FF);
   gIdx=2;
   TXIE=ENABLE;      // send
   while( TXIE ) continue;        
   
       for( x=0; x<MAX_PAGE_1MBIT_DENSITY; x++)
       {
          DF_Page2Buffer1(x);
          FS_WaitReady();
          for(y=0; y<PAGESIZE; y++){
             DF_ReadBuffer1(y, (BYTE*)&n, sizeof(BYTE));
             FS_WaitReady();
         	 gOut[0]=n;                                  
             gIdx=1;
             TXIE=ENABLE;      // send
             while( TXIE ) continue;        
          }    
       }   
   gOut[0]=CR_ETX;
   gOut[1]=0xFF;
   gIdx=2;
   TXIE=ENABLE;      // send
   while( TXIE ) continue;        
   
}// SendFlash

/**
*  Send information to the host
*/
static void SendInfo(void)
{
BYTE tmp;
   // no wrap the buffer wait
    while( TXIE ) continue;        
   InitPacket(CR_INFO);
   // Device number
   FS_GetBoot(FSDN_OFF, (BYTE*)&tmp, sizeof(BYTE));
   gOut[gIdx++]=tmp;
   // Error flag
   xitoa(gError, gBuf, 3);  
   memcpy(&gOut[gIdx],gBuf, 3);
   gOut[6]=CR_ETX;
   gOut[7]=CRO_Crc(gOut, 7);
   gIdx=8;
   bitset(gFlags, FN_WAITACK);  // Wait ACK
   gRetry=3;
   TXIE=ENABLE;      // send
   while( TXIE ) continue;        
   
}// SendInfo

/**
*  Real time clock preset routines
*  @param   dir - initial mode:
*                 1 extract from packet
*                 0 default 12:00:00 01/01/2002
*/
static void SetTime(BYTE dir)
{
struct date d;
struct time t;
        if(dir) {
            d.da_year = ccp(3)+2000; // YY 2000-1980
            d.da_mon = ccp(5); // MM
            d.da_day = ccp(7); // DD           
            t.ti_hour = ccp(10); // HH           
            t.ti_min = ccp(12); // MM           
            t.ti_sec = 0;
            t.ti_hund = 0;
        } else {
            d.da_year = 2002; // YY def = 2002
            d.da_mon = 1; // MM
            d.da_day = 1; // DD           
            t.ti_hour = 12; // HH           
            t.ti_min = 0; // MM           
            t.ti_sec = 0;
            t.ti_hund = 0;
        }    
            // set time
            TIME = dostounix(&d, &t);
}// SetTime


/**
*  Looks for ID storage hole, decides modify or append
*  @param   id - Identification to find
*/
static void CheckUnits(WORD id)
{
UNIT u;
           // search if exists
            while( FS_ReadUnit( &u )!=FSEOF )
            {
             if( id == u.id )
             {
               // setup flag
                 u.state = (BYTE)((gIn[2]==CR_ENABLE)?U_ENABLE:U_DISABLE);
               // set pos
	           gUcb-=sizeof(UNIT);
               // update
               FS_WriteUnit(&u);
               return; // found
             }    
            }// while
            // not found
           if( gIn[2]!=CR_ENABLE )
                       return;
            
            // rewind
            FS_Open(DBUNIT);
           // search for a free
            while( FS_ReadUnit(&u)!=FSEOF )
            {
             if( u.state==U_DISABLE ) // new
             {
               // setup flag
               u.state = U_ENABLE;
               u.id = id;
               // set pos
               gUcb-=sizeof(UNIT);
               // update
               FS_WriteUnit(&u);
               return; // found
             }    
            }// while            
           // not found add
            u.state = U_ENABLE;
            u.id = id;
           FS_AppendUnit(&u);
}// CheckUnits


/**
*  Process incoming packets from SCI
*/
static void Process(void)
{
WORD tmp;

     memset((char*)gOut,0, OUTBUFSIZE);  
     memset((char*)gBuf,0, 10);  
     if( gIn[0]==CR_START && gIn[1]==CR_DEV )
     {
       switch(gIn[2])
       {
        case CR_INIT0:        
               Init0();
        break;
        case CR_INIT1:
              Init1();
        break;        
        case CR_ENABLE:
        case CR_DISABLE:
               memcpy( gBuf, &gIn[3], 5);
               tmp = (WORD)atoi(gBuf);
               FS_Open(DBUNIT);
               CheckUnits(tmp);    
               FS_Close(DBUNIT);                        
        break;
        case CR_NUM: 
              tmp=(WORD)gIn[3]&0x00FF;
              FS_SetBoot(FSDN_OFF, (BYTE*)&tmp, sizeof(BYTE));
        break;
        case CR_DOWN:     
                  FS_Open(DBREGIS);
                  bitset(gFlags, FN_DOWN);
                  SendRegis();
        break;
        case CR_DELETE:   
                 FS_ZapRegis();
                 FS_SetBoot(FSDT_OFF,(BYTE*)&TIME,sizeof(DWORD)); // BUG!
                 bitclr(gFlags, FN_SPACE); // reset flag
        break;
        case CR_RESET:    
        break;
        case CR_DATETIME: 
            SetTime(1);
        break;
        case CR_INFO:
                SendInfo();
        break;
        case CR_FORMAT:
               bitclr(gFlags, FN_SPACE);
                memcpy( gBuf, &gIn[3], 3);
                tmp = (WORD)atoi(gBuf);
                if( tmp == DEFMAXREGIS )
	                FS_Format();        
                DoOK();
        break;
        case CR_FF:
             SendFlash();                
        break;
        default:
            DoError();
       }
     } // if
 
}// Process

/**
*  Store adquisition data to DB
*  @param   id - Identification acquired
*  @return  TRUE
*/
static BYTE Save(WORD id)
{
 BYTE tmp;
 REGIS u;
   FS_Open(DBREGIS);
   
   if( gRnor > (21400) )
   {
     bitset(gFlags, FN_SPACE); // Low disk warn
   }
   if( gRnor >= (21500) )
   {
      bitset(gError, GE_NOSPACE); // ( no more space )   
     // no more space indicator
     tmp=3;
     do {
       beep(BLONG);         // t1 = 50
       delay_ms(WLONG);     // t2 = 50
     }while(--tmp);  
     return 0; // no save no space
   }
     u.id=id;
     u.ttime = TIME;
      FS_AppendRegis(&u);
  FS_Close(DBREGIS);               
return 1;   
} // Save
 
/**
*  Search DB by ID
*  @param   id - Identification to find
*  @return  TRUE  found enable or not found but permitted
*           FALSE not found
*/
static BYTE SearchbyID(WORD id)
{
 UNIT u; 
   if( FS_Open(DBUNIT)==FSOK )
   {
    while( FS_ReadUnit( &u )!=FSEOF )
    {    
     if( id == u.id )
     {
       if( u.state==U_DISABLE )
                    return TRUE; // found and is disable
       else
                    return FALSE; // found and enable
     }    
    }// while                    
   } 
return TRUE; // not found (permit)
}// SearchbyID


/**
*  Compute CRC
*  @param   data - buffer to compute
*           size - number of bytes to compute
*  @return  CRC calculated.
*/
static BYTE CRO_Crc(volatile BYTE *data, BYTE size)
{
 BYTE crc;
  crc=0x00;
  do {
      crc^=*data;
    data++;
  }while( --size );
 return crc;   
}// CRO_Crc


/**
*  converts an integer to an ASCII string representation
*  @param    n  - number to be converted
*			 s  - Buffer to store result
*		    pad	- right alignment padding
*  @return  TRUE on packet received, FALSE timeout.
*  @remarks feed gCpos and gCtime with the received data.
*/
static VOID xitoa(WORD n, BYTE* s, BYTE pad)
{
  BYTE* ptr;
  ptr = s;
  do {
    *ptr++ = n % 10 + '0';    
	if(pad) pad--;
    } while ((n = n / 10) > 0);
  while(pad--) *ptr++='0';  // 0 padding
  *ptr = 0x00;
  xreverse(s);
}// xitoa

/**
*  reverse the contents of string s
*  @param   s - NULL terminated string
*/
static VOID xreverse(BYTE* s)
{
  BYTE *j;
  BYTE c;
  j = s + strlen(s) - 1;
  while(s < j) {
    c = *s;
    *s++ = *j;
    *j-- = c;
  }
} // xreverse

/**
*   Debug tests
*/
// #define _FLTEST 1
#ifdef _FLTEST
void FS_FlashTest(void)
{
 WORD x;
 WORD y;
 BYTE n;

/*
       for( x=0; x<2; x++)
       { 
          n=0xAA;
          for(y=0; y<PAGESIZE; y++){
	     FS_WaitReady();
	     DF_WriteBuffer1(y, (BYTE*)&n, sizeof(BYTE)); 
//             n++;
          }    
	DF_Buffer12MainMemoryE(x);      
        FS_WaitReady();
       }
*/
       for( x=0; x<2; x++)
//     for( x=REGSPA; x<DEFMAXREGIS; x++ )
       {
          DF_Page2Buffer1(x);
          FS_FS_WaitReady();
          for(y=0; y<PAGESIZE; y++){
             FS_FS_WaitReady();
             DF_ReadBuffer1(y, (BYTE*)&n, sizeof(BYTE));
	     gOut[0]=n;
             gIdx=1;
             TXIE=ENABLE;      // send
             while( TXIE ) continue;        
          }    
       }

/*     for( x=UNITSPA; x<DEFMAXUNIT; x++ )
       {
          DF_Page2Buffer1(x);
          FS_WaitReady();
          for(y=0; y<PAGESIZE; y++){
             FS_WaitReady();
             DF_ReadBuffer1(y, (BYTE*)&n, sizeof(BYTE));
	     gOut[0]=n;
             gIdx=1;
             TXIE=ENABLE;      // send
             while( TXIE ) continue;        
          }    
       }
*/
}
#endif
//*******************************************************[ENDL]***************

