//=======================================================================================================//
//   Copyright (c) 2017 Maruti Mhetre                                                                    //
//   All rights reserved.                                                                                //
//=======================================================================================================//
//   Redistribution and use of this software in source and binary forms, with or without modification,   //
//   are permitted for personal, educational or non-commercial purposes provided that the following      //
//   conditions are met:                                                                                 //
//   1. Redistributions of source code must retain the above copyright notice, this list of conditions   //
//      and the following disclaimer.                                                                    //
//   2. Redistributions in binary form must reproduce the above copyright notice, this list of           //
//      conditions and the following disclaimer in the documentation and/or other materials provided     //
//      with the distribution.                                                                           //
//   3. Neither the name of the copyright holder nor the names of its contributors may be used to        //
//      endorse or promote products derived from this software without specific prior written            //
//      permission.                                                                                      //
//=======================================================================================================//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR      //
//   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND    //
//   FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR          //
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   //
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   //
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER  //
//   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT   //
//   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     //
//=======================================================================================================//


//Goal:
//Test MSDN time example

// crt_times.c
// compile with: /W3
// This program demonstrates these time and date functions:
//      time         _ftime    ctime_s     asctime_s
//      _localtime64_s    _gmtime64_s    mktime    _tzset
//      _strtime_s     _strdate_s  strftime
//
// Also the global variable:
//      _tzname
//
// Turn off deprecated unsafe CRT function warnings
#define _CRT_SECURE_NO_WARNINGS 1

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>

namespace mm {

	void MSDNTimeExample()
	{
		char tmpbuf[128], timebuf[26], ampm[] = "AM";
		time_t ltime;
		struct _timeb tstruct;
		struct tm today, gmt, xmas = { 0, 0, 12, 25, 11, 93 };
		errno_t err;

		// Set time zone from TZ environment variable. If TZ is not set,
		// the operating system is queried to obtain the default value 
		// for the variable. 
		//
		_tzset();

		// Display operating system-style date and time. 
		_strtime_s(tmpbuf, 128);
		printf("OS time:\t\t\t\t%s\n", tmpbuf);
		_strdate_s(tmpbuf, 128);
		printf("OS date:\t\t\t\t%s\n", tmpbuf);

		// Get UNIX-style time and display as number and string. 
		time(&ltime);
		printf("Time in seconds since UTC 1/1/70:\t%lld\n", (long long)ltime);
		err = ctime_s(timebuf, 26, &ltime);
		if (err)
		{
			printf("ctime_s failed due to an invalid argument.");
			exit(1);
		}
		printf("UNIX time and date:\t\t\t%s", timebuf);

		// Display UTC. 
		err = _gmtime64_s(&gmt, &ltime);
		if (err)
		{
			printf("_gmtime64_s failed due to an invalid argument.");
		}
		err = asctime_s(timebuf, 26, &gmt);
		if (err)
		{
			printf("asctime_s failed due to an invalid argument.");
			exit(1);
		}
		printf("Coordinated universal time:\t\t%s", timebuf);

		// Convert to time structure and adjust for PM if necessary. 
		err = _localtime64_s(&today, &ltime);
		if (err)
		{
			printf("_localtime64_s failed due to an invalid argument.");
			exit(1);
		}
		if (today.tm_hour >= 12)
		{
			strcpy_s(ampm, sizeof(ampm), "PM");
			today.tm_hour -= 12;
		}
		if (today.tm_hour == 0)  // Adjust if midnight hour.
			today.tm_hour = 12;

		// Convert today into an ASCII string 
		err = asctime_s(timebuf, 26, &today);
		if (err)
		{
			printf("asctime_s failed due to an invalid argument.");
			exit(1);
		}

		// Note how pointer addition is used to skip the first 11 
		// characters and printf is used to trim off terminating 
		// characters.
		//
		printf("12-hour time:\t\t\t\t%.8s %s\n",
			timebuf + 11, ampm);

		// Print additional time information. 
		_ftime(&tstruct); // C4996
						  // Note: _ftime is deprecated; consider using _ftime_s instead
		printf("Plus milliseconds:\t\t\t%u\n", tstruct.millitm);
		printf("Zone difference in hours from UTC:\t%u\n",
			tstruct.timezone / 60);
		printf("Time zone name:\t\t\t\t%s\n", _tzname[0]); //C4996
														   // Note: _tzname is deprecated; consider using _get_tzname
		printf("Daylight savings:\t\t\t%s\n",
			tstruct.dstflag ? "YES" : "NO");

		// Make time for noon on Christmas, 1993. 
		if (mktime(&xmas) != (time_t)-1)
		{
			err = asctime_s(timebuf, 26, &xmas);
			if (err)
			{
				printf("asctime_s failed due to an invalid argument.");
				exit(1);
			}
			printf("Christmas\t\t\t\t%s\n", timebuf);
		}

		// Use time structure to build a customized time string. 
		err = _localtime64_s(&today, &ltime);
		if (err)
		{
			printf(" _localtime64_s failed due to invalid arguments.");
			exit(1);
		}

		// Use strftime to build a customized time string. 
		strftime(tmpbuf, 128,
			"Today is %A, day %d of %B in the year %Y.\n", &today);
		printf(tmpbuf);
	}

}