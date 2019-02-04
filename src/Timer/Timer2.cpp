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


//Goal
//Timer utilities

#include <iostream>
//#include <chrono> //C++11
//#include <ctime>
#include <sstream>
//#include <string>
#include <iomanip> //for std::put_time
#include <windows.h> //For Sleep(int milliseconds)
#include <typeinfo>
using namespace std;

#include "Timer2.h"

namespace mm {

	//============= More generic templetized Clock Utility class ============================

	void systemClockUtils()
	{
		std::cout << "\n\nSystem Clock:\n";
		ClockUtilities<SystemClock> tm;
		tm.captureStartTime();
		std::cout << "\ngetNanoSecondsSinceEpoch = " << tm.getNanoSecondsSinceEpoch(tm.getNow());
		std::cout << "\nStart time = ";
		tm.print1(tm.getStartTime());
		tm.captureEndTime();
		std::cout << "\nEnd time = ";
		tm.print2(tm.getEndTime());
		double diffSec = tm.getDiffInSeconds();
		unsigned long long diff1 = tm.getDiffInNanoSeconds();
		unsigned long long diff2 = tm.getDiffInNanoSeconds2();
		std::cout << "\nDiff in sec = " << diffSec;
		std::cout << "\nDiff in nano sec = " << diffSec * 1000000000;
		std::cout << "\nDiff in nano sec 1 = " << diff1;
		std::cout << "\nDiff in nano sec 2 = " << diff2;

		chrono::system_clock::time_point tp = chrono::system_clock::now();
		std::cout << "\nTime since epoch = " << tp.time_since_epoch().count();
		const std::string &str = tm.time_point_to_string(tp);
		std::cout << "\n Time: " << str;
		tp = tm.string_to_time_point(str);
		std::cout << "\nTime since epoch = " << tp.time_since_epoch().count();
	}

	void HRClockUtils()
	{
		std::cout << "\n\nHigh Resolution Clock:\n";
		ClockUtilities<HRClock> tm;
		tm.captureStartTime();
		std::cout << "\ngetNanoSecondsSinceEpoch = " << tm.getNanoSecondsSinceEpoch(tm.getNow());
		std::cout << "\nStart time = ";
		tm.print(tm.getStartTime());
		tm.captureEndTime();
		std::cout << "\nEnd time = ";
		tm.print(tm.getEndTime());
		double diffSec = tm.getDiffInSeconds();
		unsigned long long diff1 = tm.getDiffInNanoSeconds();
		unsigned long long diff2 = tm.getDiffInNanoSeconds2();
		std::cout << "\nDiff in sec = " << diffSec;
		std::cout << "\nDiff in nano sec = " << diffSec * 1000000000;
		std::cout << "\nDiff in nano sec 1 = " << diff1;
		std::cout << "\nDiff in nano sec 2 = " << diff2;
	}

	void myFunction()
	{
		std::cout << "\nCalled void myFunction(), sleeping for 1000 ms...";
		//std::this_thread::sleep_for(1s);
		Sleep(1000);
	}

	void useOfCtime()
	{
		std::cout << "\n\nuseOfCtime:\n";

		/*
		typedef unspecified time_t;
		Arithmetic type capable of representing times.
			Although not defined, this is almost always an integral value holding the number of seconds(not counting leap seconds) since 00:00, Jan 1 1970 UTC, corresponding to POSIX time.
		*/
		std::time_t start = std::time(nullptr);
		/*
		Function - char* asctime( const std::tm* time_ptr );

		Converts given calendar time std::tm to a textual representation of the following fixed 25-character form: Www Mmm dd hh:mm:ss yyyy\n
		Www - three-letter English abbreviated day of the week from time_ptr->tm_wday, one of Mon, Tue, Wed, Thu, Fri, Sat, Sun.
	Mmm - three-letter English abbreviated month name from time_ptr->tm_mon, one of Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec.
	dd - 2-digit day of the month from timeptr->tm_mday as if printed by sprintf using %2d
	hh - 2-digit hour from timeptr->tm_hour as if printed by sprintf using %.2d
	mm - 2-digit minute from timeptr->tm_min as if printed by sprintf using %.2d
	ss - 2-digit second from timeptr->tm_sec as if printed by sprintf using %.2d
	yyyy - 4-digit year from timeptr->tm_year + 1900 as if printed by sprintf using %4d
	The behavior is undefined if any member of *time_ptr is outside its normal range
	The behavior is undefined if the calendar year indicated by time_ptr->tm_year has more than 4 digits or is less than the year 1000.
	The function does not support localization, and the newline character cannot be removed.
	The function modifies static storage and is not thread-safe.
		*/

		std::tm tmPtrStartLocal;
		localtime_s(&tmPtrStartLocal, &start);
		/*
		struct tm;
	Structure holding a calendar date and time broken down into its components.
	Member objects
	int tm_sec

	seconds after the minute – [0, 61](until C++11) / [0, 60] (since C++11)[note 1]
	(public member object)
	int tm_min

	minutes after the hour – [0, 59]
	(public member object)
	int tm_hour

	hours since midnight – [0, 23]
	(public member object)
	int tm_mday

	day of the month – [1, 31]
	(public member object)
	int tm_mon

	months since January – [0, 11]
	(public member object)
	int tm_year

	years since 1900
	(public member object)
	int tm_wday

	days since Sunday – [0, 6]
	(public member object)
	int tm_yday

	days since January 1 – [0, 365]
	(public member object)
	int tm_isdst

	Daylight Saving Time flag. The value is positive if DST is in effect, zero if not and negative if no information is available
	(public member object)
	Notes
	↑ Range allows for a positive leap second. Two leap seconds in the same minute are not allowed (the range 0..61 was a defect introduced in C89 and corrected in C99)
		*/

		char ch[256];
		asctime_s(ch, &tmPtrStartLocal);
		std::cout << "\nLocal Start Time: " << ch;

		std::cout << "\nSeconds since the Epoch: " << start;

		/*
		std::tm* gmtime( const std::time_t* time );
	Converts given time since epoch as std::time_t value into calendar time, expressed in Coordinated Universal Time (UTC).
		*/
		std::tm tmPtrStartUTC;
		gmtime_s(&tmPtrStartUTC, &start);
		asctime_s(ch, &tmPtrStartUTC);
		std::cout << "\nUTC Start Time: " << ch;

		/*
		char* ctime( const std::time_t* time );
	Converts given time since epoch to a calendar local time and then to a textual representation, as if by calling std::asctime(std::localtime(time)). The resulting string has the following format:
	Www Mmm dd hh:mm:ss yyyy
	Www - the day of the week (one of Mon, Tue, Wed, Thu, Fri, Sat, Sun).
	Mmm - the month (one of Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec).
	dd - the day of the month
	hh - hours
	mm - minutes
	ss - seconds
	yyyy - years
	The function does not support localization.
		*//*

		//More info at:
		//http://en.cppreference.com/w/cpp/chrono/c

		myFunction();
		std::time_t end = std::time(nullptr);
		std::tm tmPtrEnd;
		localtime_s(&tmPtrEnd, &end);
		asctime_s(ch, &tmPtrEnd);
		std::cout << "\nLocal End Time: " << ch;

		double durationInSec = difftime(end, start);
		std::cout << "\nDurationin sec: " << durationInSec;

		*/
		/*
		Output:
		??
		*/
	}



	/*
	No need to define another function for high_resolution_clock as its just a typedef for steady_clock
	//typedef chrono::steady_clock chrono::high_resolution_clock
	chrono::system_clock::time_point convertToSystemClockTimePoint(chrono::high_resolution_clock::time_point& htp)
	*/

	SystemClock::time_point convertToSystemClock1(HRClock::time_point htp)
	{
		//Get the current HR clock time and System time. Assume we get it at the same time
		HRClock::time_point hnow = HRClock::now();
		SystemClock::time_point snow = SystemClock::now();

		//std::chrono::nanoseconds hns1 = std::chrono::duration_cast<std::chrono::nanoseconds>(hnow.time_since_epoch());
		//std::chrono::nanoseconds sns1 = std::chrono::duration_cast<std::chrono::nanoseconds>(snow.time_since_epoch());
		//std::cout << "\nTally now: HR clock nanoseconds = " << hns1.count() << " Sys clock nanoseconds = " << sns1.count();

		std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(htp - hnow);
		//std::cout << "\n Diff = " << diff.count();
		snow += diff;

		//tally
		//std::chrono::nanoseconds hns = std::chrono::duration_cast<std::chrono::nanoseconds>(htp.time_since_epoch());
		//std::chrono::nanoseconds sns = std::chrono::duration_cast<std::chrono::nanoseconds>(snow.time_since_epoch());
		//std::cout << "\nTally: HR clock nanoseconds = " << hns.count() << " Sys clock nanoseconds = " << sns.count();

		return snow;
	}

	//Used only for steady_clock and high_resolution_clock
	//typedef chrono::steady_clock chrono::high_resolution_clock
	template<typename ClockType>
	void useOfClock()
	{
		const std::type_info& type = typeid(ClockType);  //Reference is must because it does not have copy constructor/assignment
		std::cout << "\n\nuseOfClock for type: " << type.name() << "\n";

		chrono::time_point<ClockType> start_time = ClockType::now();
		chrono::system_clock::time_point start_time_sys = convertToSystemClock1(start_time);
		std::time_t start_t = chrono::system_clock::to_time_t(start_time_sys);
		std::tm startTmPtrLocal;
		localtime_s(&startTmPtrLocal, &start_t);
		std::cout << "\nLocal Start Time: " << std::put_time(&startTmPtrLocal, "%F %T");
		//More info on std::put_time at
		//http://en.cppreference.com/w/cpp/io/manip/put_time

		std::tm startTmPtrUTC;
		gmtime_s(&startTmPtrUTC, &start_t);
		std::cout << "\nUTC Start Time: " << std::put_time(&startTmPtrUTC, "%F %T");

		std::time_t timeBefore24Hrs = chrono::system_clock::to_time_t(start_time_sys - chrono::hours(24));
		std::tm oldTmPtrUTC;
		localtime_s(&oldTmPtrUTC, &timeBefore24Hrs);
		std::cout << "\n24 hours ago, the Local time was: " << std::put_time(&oldTmPtrUTC, "%F %T");

		unsigned long long nanosecondsSinceEpochStart = start_time.time_since_epoch().count();
		std::cout << "\nNano sec since epoch (start): " << nanosecondsSinceEpochStart;

		myFunction();

		chrono::time_point<ClockType> end_time = ClockType::now();
		chrono::system_clock::time_point end_time_sys = convertToSystemClock1(end_time);
		std::time_t end_t = chrono::system_clock::to_time_t(end_time_sys);
		std::tm endTmPtrUTC;
		localtime_s(&endTmPtrUTC, &end_t);
		std::cout << "\nLocal End Time: " << std::put_time(&endTmPtrUTC, "%F %T");
		unsigned long long nanosecondsSinceEpochEnd = end_time.time_since_epoch().count();
		std::cout << "\nNano sec since epoch (end): " << nanosecondsSinceEpochEnd;

		chrono::duration<double> timeSpanInSec = chrono::duration_cast<chrono::duration<double>>(end_time - start_time);
		std::cout << "\nDuration in sec: " << timeSpanInSec.count();

		chrono::nanoseconds timeSpanInNanoSec = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time);
		std::cout << "\nDuration in nano sec: " << timeSpanInNanoSec.count();

		std::cout << "\nDuration in nano sec (calculated using diff of time since epoch): " << nanosecondsSinceEpochEnd - nanosecondsSinceEpochStart;

		bool isSteady = ClockType::is_steady;
		std::cout << "\nIs this clock steady? " << isSteady;
	}

#if _MSC_VER == 1900 || _MSC_VER > 1900

	//Best clock to measure intervals as it always and only moves forward
	void useOfSteadyClock()
	{
		std::cout << "\n\nuseOfSteadyClock:\n";

		std::cout << "\n\nuseOfSteadyClock:";
		chrono::steady_clock::time_point start = chrono::steady_clock::now();
		myFunction();
		chrono::steady_clock::time_point end = chrono::steady_clock::now();
		std::cout << "\nTime duration (in ms): " << chrono::duration_cast<chrono::microseconds>(end - start).count();

		useOfClock<chrono::steady_clock>();
	}

#endif

	void useOfSystemClock()
	{
		std::cout << "\n\nuseOfSystemClock:\n";

		chrono::time_point<chrono::system_clock> start_time = chrono::system_clock::now();
		std::time_t start_t = chrono::system_clock::to_time_t(start_time);
		std::tm startTmPtrLocal;
		localtime_s(&startTmPtrLocal, &start_t);
		std::cout << "\nLocal Start Time: " << std::put_time(&startTmPtrLocal, "%F %T");
		//More info on std::put_time at
		//http://en.cppreference.com/w/cpp/io/manip/put_time

		std::tm startTmPtrUTC;
		gmtime_s(&startTmPtrUTC, &start_t);
		std::cout << "\nUTC Start Time: " << std::put_time(&startTmPtrUTC, "%F %T");

		std::time_t timeBefore24Hrs = chrono::system_clock::to_time_t(start_time - chrono::hours(24));
		std::tm oldTmPtrUTC;
		localtime_s(&oldTmPtrUTC, &timeBefore24Hrs);
		std::cout << "\n24 hours ago, the Local time was: " << std::put_time(&oldTmPtrUTC, "%F %T");

		unsigned long long nanosecondsSinceEpochStart = start_time.time_since_epoch().count();
		std::cout << "\nNano sec since epoch (start): " << nanosecondsSinceEpochStart;

		myFunction();

		chrono::time_point<chrono::system_clock> end_time = chrono::system_clock::now();
		std::time_t end_t = chrono::system_clock::to_time_t(end_time);
		std::tm endTmPtrUTC;
		localtime_s(&endTmPtrUTC, &end_t);
		std::cout << "\nLocal End Time: " << std::put_time(&endTmPtrUTC, "%F %T");
		unsigned long long nanosecondsSinceEpochEnd = start_time.time_since_epoch().count();
		std::cout << "\nNano sec since epoch (end): " << nanosecondsSinceEpochEnd;

		chrono::duration<double> timeSpanInSec = chrono::duration_cast<chrono::duration<double>>(end_time - start_time);
		std::cout << "\nDuration in sec: " << timeSpanInSec.count();

		chrono::nanoseconds timeSpanInNanoSec = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time);
		std::cout << "\nDuration in nano sec: " << timeSpanInNanoSec.count();

		std::cout << "\nDuration in nano sec (calculated using diff of time since epoch): " << nanosecondsSinceEpochEnd - nanosecondsSinceEpochStart;

		bool isSteady = chrono::system_clock::is_steady;
		std::cout << "\nIs this clock steady? " << isSteady;
	}

	void useOfHighResolutionClock()
	{
		useOfClock<chrono::high_resolution_clock>(); //typedef chrono::steady_clock chrono::high_resolution_clock
	}

	void printingTimeInVariousStyles()
	{
		std::cout << "\n\nprintingTimeInVariousStyles:\n";

		chrono::system_clock::time_point now = chrono::system_clock::now();
		std::time_t now_t = chrono::system_clock::to_time_t(now);
		std::tm nowTmPtrLocal;
		localtime_s(&nowTmPtrLocal, &now_t);
		std::cout << "\nTime format 1: " << std::put_time(&nowTmPtrLocal, "%F %T");

		std::cout << "\nTime format 2: " << std::put_time(&nowTmPtrLocal, "%c");

		chrono::nanoseconds nanos = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch());
		std::size_t fractionalNanoSeconds = nanos.count() % 1000000000;
		std::cout << "\nTime format 3: " << std::put_time(&nowTmPtrLocal, "%c") << "." << fractionalNanoSeconds;

		char buffer[256];
		ctime_s(buffer, 256, &now_t);
		std::cout << "\nTime format 4: " << buffer << "." << fractionalNanoSeconds;

		auto tp_sec = chrono::system_clock::from_time_t(now_t);
		chrono::nanoseconds nanosec = chrono::duration_cast<chrono::nanoseconds>(now - tp_sec);
		std::tm ttm;
		localtime_s(&ttm, &now_t);
		char date_time_format[] = "%Y.%m.%d-%H.%M.%S";
		char time_str[] = "yyyy.mm.dd.HH-MM.SS.ffffff";
		strftime(time_str, strlen(time_str), date_time_format, &ttm);
		std::string result(time_str);
		result.append(".");
		//Use one of following methods to conver long into string
		//std::stringstream ss;
		//ss << ms.count();
		//result.append(ss.str());
		result.append(std::to_string(nanosec.count()));
		std::cout << "\nTime format 5: " << result;
	}

	void Timer2()
	{
		cout << "\n\n~~~~~~~~~~~~~~~~~~ Timer2() ~~~~~~~~~~~~~~~~~~~~~~~\n";

		systemClockUtils();
		HRClockUtils();

		useOfCtime();

#if _MSC_VER == 1900 || _MSC_VER > 1900
		useOfSteadyClock();
#endif

		useOfSystemClock();
		useOfHighResolutionClock();

		printingTimeInVariousStyles();
	}


	//Output:

	//System Clock :
	//
	//getNanoSecondsSinceEpoch = 14623225307942792
	//Start time = 05 / 03 / 16 20 : 42 : 10.530794
	//
	//End time = Tue May  3 20 : 42 : 10 2016
	//.530810
	//
	//Diff in sec = 0.0157747
	//Diff in nano sec = 1.57747e+07
	//Diff in nano sec 1 = 157747
	//Diff in nano sec 2 = 15774700
	//Time since epoch = 14623225308286575
	//Time: 2016.05.03 - 20.42.10.828
	//	Time since epoch = 14623261308280000
	//
	//	High Resolution Clock :
	//
	//getNanoSecondsSinceEpoch = 1503687476528597
	//Start time = 05 / 03 / 16 20 : 42 : 10.530844
	//
	//End time = 05 / 03 / 16 20 : 42 : 10.530855
	//
	//Diff in sec = 0.0106214
	//Diff in nano sec = 1.06214e+07
	//Diff in nano sec 1 = 10621430
	//Diff in nano sec 2 = 10621430

	// ==============================================================================================================================

	//Looks like it works only on unix

	//#include <time.h>
	//#include <stdio.h>
	//#include <unistd.h>
	//int main()
	//{
	//	struct timeval start, end;
	//
	//	long mtime, seconds, useconds;
	//
	//	gettimeofday(&start, NULL);
	//	usleep(2000);
	//	gettimeofday(&end, NULL);
	//
	//	seconds = end.tv_sec - start.tv_sec;
	//	useconds = end.tv_usec - start.tv_usec;
	//
	//	mtime = seconds + useconds;
	//
	//	printf("Elapsed time: %ld microsecons\n", mtime);
	//
	//	return 0;
	//}

	// CLOCKS definitions from chrono - copied for quick reference

	//struct system_clock
	//
	//{	// wraps GetSystemTimePreciseAsFileTime/GetSystemTimeAsFileTime
	//	typedef long long rep;
	//
	//	typedef ratio_multiply<ratio<_XTIME_NSECS_PER_TICK, 1>, nano> period;
	//
	//	typedef chrono::duration<rep, period> duration;
	//	typedef chrono::time_point<system_clock> time_point;
	//	static _CONST_DATA bool is_steady = false;
	//
	//	static time_point now() _NOEXCEPT
	//	{	// get current time
	//		return (time_point(duration(_Xtime_get_ticks())));
	//	}
	//
	//	static __time64_t to_time_t(const time_point& _Time) _NOEXCEPT
	//	{	// convert to __time64_t
	//		return ((__time64_t)(_Time.time_since_epoch().count()
	//			/ _XTIME_TICKS_PER_TIME_T));
	//	}
	//
	//	static time_point from_time_t(__time64_t _Tm) _NOEXCEPT
	//	{	// convert from __time64_t
	//		return (time_point(duration(_Tm * _XTIME_TICKS_PER_TIME_T)));
	//	}
	//};
	//
	//struct steady_clock
	//{	// wraps QueryPerformanceCounter
	//	typedef long long rep;
	//	typedef nano period;
	//	typedef nanoseconds duration;
	//	typedef chrono::time_point<steady_clock> time_point;
	//	static _CONST_DATA bool is_steady = true;
	//
	//	static time_point now() _NOEXCEPT
	//	{	// get current time
	//		static const long long _Freq
	//			= _Query_perf_frequency();	// doesn't change after system boot
	//		const long long _Ctr = _Query_perf_counter();
	//		static_assert(period::num == 1, "This assumes period::num == 1.");
	//		const long long _Whole = (_Ctr / _Freq) * period::den;
	//		const long long _Part = (_Ctr % _Freq) * period::den / _Freq;
	//		return (time_point(duration(_Whole + _Part)));
	//	}
	//};
	//
	//typedef steady_clock high_resolution_clock;
	//}	// namespace chrono

}
