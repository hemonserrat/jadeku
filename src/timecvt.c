/**
 * @file TIMECVT.C
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
#include "pgrl.h"
#include "time.h"

#define DefaultTimeZone -3L

bank1 long  timezone = DefaultTimeZone * 60L * 60L; /* Set for EST */

static const char Days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/**
*  converts date and time to UNIX time format
*  @param   d    - date struct to convert
*           t    - time struct to convert
*  @return  UNIX version of the given date and time.
*  @remarks Converts a date and time (as returned from getdate and
*          gettime) into UNIX-format time.
*/
long dostounix(struct date *d, struct time *t)
{
        long            x;
        int             i;
        int             days;
        int             hours;

        x = 24L * 60L * 60L * 3652L + timezone; /* Convert from 1980 to
                                                   1970 base date GMT */
        i = d->da_year - 1980;
        x += (i >> 2) * (1461L * 24L * 60L * 60L);
        x += (i & 3) * (24L * 60L * 60L * 365L);
        if (i & 3)
                x += 24L * 3600L;
        days = 0;
        i = d->da_mon - 1;                      /* Add in months */
        while (i > 0)
        {
                i--;
                days += (int)Days[i];
        }
        days += d->da_day - 1;
        if (d->da_mon > 2 && (d->da_year & 3) == 0)
                days++;                         /* Currently in leap year */
        hours = days * 24 + t->ti_hour;         /* Find hours */
#ifdef _USEDAYLIGHT
        if (daylight && __isDST( t->ti_hour, days, 0, d->da_year-1970))
                hours--;
#endif                
        x += hours * 3600L;
        x += 60L * t->ti_min + t->ti_sec;
        return (x);
}


/**
*  converts from UNIX-format time
*  @param   time - time in Unix format.
*           d    - date struct to store conversion
*           t    - time struct to store conversion
*  @remarks Converts a UNIX-format time into date and time.
*/
void  unixtodos(long time, struct date *d, struct time *t)
{

        time -= 24L * 60L * 60L * 3652L + timezone;
        t->ti_hund = 0;
        t->ti_sec = time % 60;
        time /= 60;                             /* Time in minutes */
        t->ti_min = time % 60;
        time /= 60;                             /* Time in hours */
        d->da_year = 1980 + (int)((time / (1461L * 24L)) << 2);
        time %= 1461L * 24L;
        if (time >= 366 * 24)
        {
                time -= 366 * 24;
                d->da_year++;
                d->da_year += (int)(time / (365 * 24));
                time %= 365 * 24;
        }
#ifdef _USEDAYLIGHT
        if (daylight && __isDST( (int)(time % 24), (int)(time / 24), 0, d->da_year-1970 ))
                time++;
#endif                
        t->ti_hour = time % 24;
        time /= 24;                             /* Time in days */
        time++;
        if ((d->da_year & 3) == 0)
        {
                if (time > 60)
                        time--;
                else
                if (time == 60)
                {
                        d->da_mon = 2;
                        d->da_day = 29;
                        return;
                }
        }
        for (d->da_mon = 0; Days[d->da_mon] < time; d->da_mon++)
                time -= Days[d->da_mon];
        d->da_mon++;
        d->da_day = (char)time;
} 


//*****************************************************************************[ENDL]***************
