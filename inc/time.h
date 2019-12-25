/**
 * @file TIME.H
 * @brief  TIME CONVERSION.
 *         
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
#ifndef __TIME_H
#define __TIME_H


#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif

#ifndef  _TIME_T
#define  _TIME_T
typedef long time_t;
#endif

#ifndef  _CLOCK_T
#define  _CLOCK_T
typedef long clock_t;


#define CLOCKS_PER_SEC 18.2
#define CLK_TCK        18.2

#endif  /* _TIME_T */


struct  time    {
    unsigned char   ti_min;     ///< Minutes
    unsigned char   ti_hour;    ///< Hours 
    unsigned char   ti_hund;    ///< Hundredths of seconds 
    unsigned char   ti_sec;     ///< Seconds
};

struct  date    {
    int         da_year;    ///< Year - 1980 
    char        da_day;     ///< Day of the month
    char        da_mon;     ///< Month (1 = Jan) 
};


struct tm
{
  int   tm_sec;
  int   tm_min;
  int   tm_hour;
  int   tm_mday;
  int   tm_mon;
  int   tm_year;
  int   tm_wday;
  int   tm_yday;
  int   tm_isdst;
};

#ifdef _USEDAYLIGHT
extern int    daylight;
#endif

extern bank1 long      timezone;
extern volatile bank1 time_t TIME;

long dostounix(struct date *d, struct time *t);
void  unixtodos(long time, struct date *d, struct time *t);


#endif
/**************************************************[END]*********/

