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
//Timer utility interface

//#include <iostream>
#include <stdio.h>
#include <chrono> //C++11
#include <ctime>
//#include <sstream>
#include <string>
//#include <iomanip> //for std::put_time

/*
Clocks:

A clock consists of a starting point (or epoch) and a tick rate. For example, a clock may have an epoch of January 1, 1970
and tick every second. C++ defines three clock types:
Defined in header <chrono>
Defined in namespace std::chrono

system_clock:
wall clock time from the system-wide realtime clock

steady_clock:
monotonic clock that will never be adjusted

high_resolution_clock:
the clock with the shortest tick period available

Generally high_resolution_clock is defined as:
typedef std::chrono::steady_clock std::chrono::high_resolution_clock
*/

using namespace std;
using HRClock = std::chrono::high_resolution_clock;
using SystemClock = std::chrono::system_clock;

namespace mm {

#ifdef __GNUC__

#define localtime_s(type_tm, type_time_t) localtime_r(type_time_t, type_tm)
#define ctime_s(type_char, count, type_time_t) ctime_r(type_time_t, type_char)
#define asctime_s(type_char, type_tm) asctime_r(type_tm, type_char)
//#define sscanf_s sscanf_r
#define sscanf_s sscanf
#define gmtime_s(type_tm, type_time_t) gmtime_r(type_time_t, type_tm)
//#define ctime_s ctime_r

#endif

	//============= More generic templetized Clock Utility class ============================

	template <typename clockType>
	class BasicClockUtilities
	{
	public:
		void captureStartTime();
		void captureEndTime();
		typename clockType::time_point getStartTime();
		typename clockType::time_point getEndTime();

		typename clockType::time_point getNow();
		unsigned long long getNanoSecondsSinceEpoch(typename clockType::time_point now);

		double getDiffInSeconds();
		unsigned long long getDiffInNanoSeconds();
		unsigned long long getDiffInNanoSeconds2();

	protected:
		typename clockType::time_point start_time;
		typename clockType::time_point end_time;
	};

	template <typename clockType>
	class ClockUtilities : public BasicClockUtilities<clockType>
	{
	};

	template <>
	class ClockUtilities<SystemClock> : public BasicClockUtilities<SystemClock>
	{
	public:
		void print(SystemClock::time_point now) { print1(now);  }
		void print1(SystemClock::time_point now);
		void print2(SystemClock::time_point now);
		SystemClock::time_point string_to_time_point(const std::string &str);
		std::string time_point_to_string(SystemClock::time_point &tp);
	};

#if _MSC_VER == 1900 || _MSC_VER > 1900

	template <>
	class ClockUtilities<HRClock> : public BasicClockUtilities<HRClock>
	{
	public:
		SystemClock::time_point convertToSystemClock1(HRClock::time_point htp);
		SystemClock::time_point convertToSystemClock2(HRClock::time_point htp);
		void print(HRClock::time_point now);
	};

#endif

	//============= class BasicClockUtilities - function definitions ============================

	template <typename clockType>
	void BasicClockUtilities<clockType>::captureStartTime()
	{
		start_time = clockType::now();
	}

	template <typename clockType>
	typename clockType::time_point BasicClockUtilities<clockType>::getStartTime()
	{
		return start_time;
	}

	template <typename clockType>
	void BasicClockUtilities<clockType>::captureEndTime()
	{
		end_time = clockType::now();
	}

	template <typename clockType>
	typename clockType::time_point BasicClockUtilities<clockType>::getEndTime()
	{
		return end_time;
	}

	template <typename clockType>
	typename clockType::time_point BasicClockUtilities<clockType>::getNow()
	{
		return clockType::now();
	}

	template <typename clockType>
	unsigned long long BasicClockUtilities<clockType>::getNanoSecondsSinceEpoch(typename clockType::time_point now)
	{
		unsigned long long nanosecondsSinceEpoch = now.time_since_epoch().count();
		return nanosecondsSinceEpoch;
	}

	template <typename clockType>
	double BasicClockUtilities<clockType>::getDiffInSeconds()
	{
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
		return time_span.count();
	}

	template <typename clockType>
	unsigned long long BasicClockUtilities<clockType>::getDiffInNanoSeconds()
	{
		return getNanoSecondsSinceEpoch(end_time) - getNanoSecondsSinceEpoch(start_time);
	}

	template <typename clockType>
	unsigned long long BasicClockUtilities<clockType>::getDiffInNanoSeconds2()
	{
		std::chrono::nanoseconds time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		return time_span.count();
	}

	//============= class ClockUtilities<SystemClock> - function definitions ============================

	void ClockUtilities<SystemClock>::print1(SystemClock::time_point now)
	{
		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		std::size_t fractional_seconds = ms.count() % 1000000;

		//std::tm timetm = *std::localtime(&time);
		std::tm timetm;
		localtime_s(&timetm, &time);

		std::cout << std::put_time(&timetm, "%c") << "." << fractional_seconds << std::endl;
	}

	void ClockUtilities<SystemClock>::print2(SystemClock::time_point now)
	{
		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		std::size_t fractional_seconds = ms.count() % 1000000;
		//std::cout << std::ctime(&time) << "." << fractional_seconds << std::endl;
		char buffer[256];
		ctime_s(buffer, 256, &time);
		std::cout << buffer << "." << fractional_seconds << std::endl;
	}

	SystemClock::time_point ClockUtilities<SystemClock>::string_to_time_point(const std::string &str)
	{
		using namespace std;
		using namespace std::chrono;

		int yyyy, mm, dd, HH, MM, SS, ffffff;

		char scanf_format[] = "%4d.%2d.%2d-%2d.%2d.%2d.%3d";

		//sscanf(str.c_str(), scanf_format, &yyyy, &mm, &dd, &HH, &MM, &SS, &ffffff);
		sscanf_s(str.c_str(), scanf_format, &yyyy, &mm, &dd, &HH, &MM, &SS, &ffffff);

		tm ttm = tm();
		ttm.tm_year = yyyy - 1900; // Year since 1900
		ttm.tm_mon = mm - 1; // Month since January 
		ttm.tm_mday = dd; // Day of the month [1-31]
		ttm.tm_hour = HH; // Hour of the day [00-23]
		ttm.tm_min = MM;
		ttm.tm_sec = SS;

		time_t ttime_t = mktime(&ttm);

		system_clock::time_point time_point_result = std::chrono::system_clock::from_time_t(ttime_t);

		time_point_result += std::chrono::milliseconds(ffffff);
		return time_point_result;
	}

	std::string ClockUtilities<SystemClock>::time_point_to_string(SystemClock::time_point &tp)
	{
		using namespace std;
		using namespace std::chrono;

		auto ttime_t = system_clock::to_time_t(tp);
		auto tp_sec = system_clock::from_time_t(ttime_t);
		milliseconds ms = duration_cast<milliseconds>(tp - tp_sec);

		//std::tm * ttm = localtime(&ttime_t);
		std::tm ttm;
		localtime_s(&ttm, &ttime_t);

		char date_time_format[] = "%Y.%m.%d-%H.%M.%S";

		char time_str[] = "yyyy.mm.dd.HH-MM.SS.ffffff";

		strftime(time_str, strlen(time_str), date_time_format, &ttm);

		string result(time_str);
		result.append(".");
		//Use one of following methods to conver long into string
		//std::stringstream ss;
		//ss << ms.count();
		//result.append(ss.str());
		result.append(std::to_string(ms.count()));

		return result;
	}

	//============= class ClockUtilities<HRClock> - function definitions ============================

#if _MSC_VER == 1900 || _MSC_VER > 1900

	SystemClock::time_point ClockUtilities<HRClock>::convertToSystemClock1(HRClock::time_point htp)
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

	SystemClock::time_point ClockUtilities<HRClock>::convertToSystemClock2(HRClock::time_point htp)
	{
		std::chrono::system_clock::duration sdu = std::chrono::duration_cast<std::chrono::system_clock::duration>(htp.time_since_epoch());
		//std::chrono::time_point<std::chrono::system_clock> stp(du); //OR use following:
		std::chrono::system_clock::time_point stp(sdu);

		//tally
		std::chrono::nanoseconds hns = std::chrono::duration_cast<std::chrono::nanoseconds>(htp.time_since_epoch());
		std::chrono::nanoseconds sns = std::chrono::duration_cast<std::chrono::nanoseconds>(stp.time_since_epoch());
		std::cout << "\nTally: Duration: " << sdu.count() << "HR clock nanoseconds = " << hns.count() << " Sys clock nanoseconds = " << sns.count();
		if (hns != sns)
			std::cout << "\n------TALLY FAILED------. We are losing precision.";

		return stp;
	}

	void ClockUtilities<HRClock>::print(HRClock::time_point now)
	{
		ClockUtilities<SystemClock> temp;
		temp.print1(convertToSystemClock1(now));
	}

#endif

}