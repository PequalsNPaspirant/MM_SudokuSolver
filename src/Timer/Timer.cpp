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

#include <stdio.h>
#include <iostream>
//#include <chrono> //C++11
//#include <ctime>
#include <sstream>
//#include <string>
#include <iomanip> //for std::put_time
#include <windows.h> //For Sleep(int milliseconds)
#include <vector>
#include <time.h>
#include <cmath>
using namespace std;

#include "Timer/Timer.h"
#include "Assert/MyAssert.h"

namespace mm {

#ifdef __GNUC__

#define localtime_s(type_tm, type_time_t) localtime_r(type_time_t, type_tm)
#define ctime_s(type_char, count, type_time_t) ctime_r(type_time_t, type_char)
#define asctime_s(type_char, type_tm) asctime_r(type_tm, type_char)
	//#define ssscanf_s sscanf_r
#define sscanf_s sscanf
#define gmtime_s(type_tm, type_time_t) gmtime_r(type_time_t, type_tm)
	//#define ctime_s ctime_r

#endif

	//========== Public functions ============

	Timer::Timer()
		: m_startTime(HRClock::now()),
		m_endTime(HRClock::now())
	{
	}

	Timer::~Timer()
	{
		//cout << "\nTime Duration: " << getDurationStringTillNow() << " nanoseconds";
	}

	void Timer::resetTimer()
	{
		m_startTime = HRClock::now();
	}

	void Timer::stopTimer()
	{
		m_endTime = HRClock::now();
	}

	// Section 1: duration. It uses only HRClock
	//-----------------------------

	long long Timer::getDurationTillNowInNanoSeconds()
	{
		HRClock::time_point end_time = HRClock::now();
		return getDurationInNanoSeconds(m_startTime, end_time);
	}

	string Timer::getDurationStringTillNowInNanoSeconds()
	{
		HRClock::time_point end_time = HRClock::now();
		return getDurationStringInNanoSeconds(m_startTime, end_time);
	}

	string Timer::getDurationStringTillNowInNanoSeconds2()
	{
		HRClock::time_point end_time = HRClock::now();
		return getDurationStringInNanoSeconds2(m_startTime, end_time);
	}

	long long Timer::getDurationInNanoSeconds(const HRClock::time_point& end_time)
	{
		return getDurationInNanoSeconds(m_startTime, end_time);
	}

	string Timer::getDurationStringInNanoSeconds(const HRClock::time_point& end_time)
	{
		return getDurationStringInNanoSeconds(m_startTime, end_time);
	}

	long long Timer::getDurationInNanoSeconds(const HRClock::time_point& startTime, const HRClock::time_point& end_time)
	{
		chrono::nanoseconds time_span = chrono::duration_cast<chrono::nanoseconds>(end_time - startTime);
		return time_span.count(); //return nanoseconds
	}

	long long Timer::getDurationInMicroSeconds(const HRClock::time_point& startTime, const HRClock::time_point& end_time)
	{
		chrono::microseconds time_span = chrono::duration_cast<chrono::microseconds>(end_time - startTime);
		return time_span.count() / 1000; //return microseconds
	}

	string Timer::getDurationStringInNanoSeconds(const HRClock::time_point& startTime, const HRClock::time_point& end_time)
	{
		return getCommaSeparatedDurationString(getDurationInNanoSeconds(startTime, end_time)) + " nanoseconds";
	}

	string Timer::getCommaSeparatedDurationString(long long duration)
	{
		int length = log10(duration + 1) + 1; //Avoid log(0)
		//int lenNanosecondsAtEnd = 12;
		int lenNanosecondsAtEnd = 0;
		length = length + (length - 1) / 3 + lenNanosecondsAtEnd;
		//static const string pattern("000,000,000,000,000,000,000,000,000,000,000,000,000 nanoseconds");
		static const string pattern("000,000,000,000,000,000,000,000,000,000,000,000,000");
		string durationString(pattern.end() - length, pattern.end());
		int commaPosition = 0;
		for (int index = length - 1 - lenNanosecondsAtEnd; duration > 0; --index, ++commaPosition)
		{
			if (durationString[index] != ',')
			{
				durationString[index] = '0' + (duration % 10);
				duration /= 10;
			}
		}

		return durationString;
	}

	string Timer::getDurationStringInNanoSeconds2(const HRClock::time_point& startTime, const HRClock::time_point& end_time)
	{
		long long duration = getDurationInNanoSeconds(startTime, end_time);
		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << duration;
		return ss.str() + " nanoseconds";
	}

	// Section 2: absolute time. It uses HRClock & SystemClock both.
	//-----------------------------

	const vector<int>& getDaysInYear()
	{
		static int* init = nullptr;
		static vector<int> daysInYear(200);
		if (init == nullptr)
		{
			init = new int;
			daysInYear[99] = 0; //year 1970 is at 99th index in array. The array contains years in range [1871, 2070]
			auto isLeapYear = [](const int& year) -> bool { return (year % 100 == 0 && year % 400 == 0) || (year % 4 == 0); };
			for (int i = 100; i < 200; ++i)
				daysInYear[i] = daysInYear[i - 1] + (isLeapYear(1871 + i) ? 366 : 365);
			for (int i = 98; i > 0; --i)
				daysInYear[i] = daysInYear[i + 1] - (isLeapYear(1871 + i) ? 366 : 365);
		}

		return daysInYear;
	}

	// This function uses HRClock
	string Timer::getCurrentLocalTimeInNanoSeconds()
	{
		// Reference: https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/tzset

		int daylight = 0;
		long timezone = 0;

#ifdef _MSC_VER

		_tzset();
		_get_daylight(&daylight); // Nonzero value if a daylight-saving-time zone is specified in TZ setting; otherwise, 0
		//printf("_daylight = %d\n", daylight);
		_get_timezone(&timezone); // Difference in seconds between local time and UTC
		//printf("_timezone = %ld\n", timezone);
		//size_t s;
		//char tzname[100];
		//_get_tzname(&s, tzname, sizeof(tzname), 0); 
		//printf("_tzname[0] = %s\n", tzname);

		// Output:
		// _daylight = 1
		// _timezone = 28800
		// _tzname[0] = Pacific Standard Time

#endif

		//This function takes aorund 50 to 70 microseconds to execute
		HRClock::time_point now = HRClock::now();
		long long totalDurationInNanosconds = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch()).count();
		long long totalDurationInSeconds = totalDurationInNanosconds / (long long)1000000000 - timezone + (daylight * 60 * 60);
		long long totalDurationInMins = totalDurationInSeconds / 60;
		long long totalDurationInHrs = totalDurationInMins / 60;
		long long totalDurationInDays = totalDurationInHrs / 24;
		long long nanoseconds = totalDurationInNanosconds % (long long)1000000000;
		long long seconds = totalDurationInSeconds % 60;
		long long min = totalDurationInMins % 60;
		long long hrs = totalDurationInHrs % 24;

		auto binarySearch = [](const long long& days, const vector<int>& daysVector) -> int {
			int start = 0;
			int end = daysVector.size() - 1;
			MyAssert::myRunTimeAssert(days >= 0 && days <= daysVector[end], "Year out of range");

			while (end - start > 1)
			{
				int mid = (start + end) / 2;
				if (daysVector[mid] < days)
					start = mid;
				else
					end = mid;
			}

			return end;
		};

		int year0 = 1870;
		const vector<int>& daysInYear = getDaysInYear();
		int year = year0 + binarySearch(totalDurationInDays, daysInYear);

		long long remainingDaysInYear = totalDurationInDays - daysInYear[(year - 1) - year0];
		static const vector<int> daysInMonthNonLeapYear({
			0,
			31,
			31 + 28,
			31 + 28 + 31,
			31 + 28 + 31 + 30,
			31 + 28 + 31 + 30 + 31,
			31 + 28 + 31 + 30 + 31 + 30,
			31 + 28 + 31 + 30 + 31 + 30 + 31,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
			31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
		});
		static const vector<int> daysInMonthLeapYear({
			0,
			31,
			31 + 29,
			31 + 29 + 31,
			31 + 29 + 31 + 30,
			31 + 29 + 31 + 30 + 31,
			31 + 29 + 31 + 30 + 31 + 30,
			31 + 29 + 31 + 30 + 31 + 30 + 31,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
			31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
		});
		auto isLeapYear = [](const int& year) -> bool { return (year % 100 == 0 && year % 400 == 0) || (year % 4 == 0); };
		int month = 0;
		int date = 0;
		if (isLeapYear(year))
		{
			month = binarySearch(remainingDaysInYear, daysInMonthLeapYear);
			date = remainingDaysInYear - daysInMonthLeapYear[month - 1] + 1;
		}
		else
		{
			month = binarySearch(remainingDaysInYear, daysInMonthNonLeapYear);
			date = remainingDaysInYear - daysInMonthNonLeapYear[month - 1] + 1;
		}

		//duration is in nanoseconds, so it can have max 9 digits, 2 commas and 1 dot
		string timeFormat("0000-00-00_00-00-00.000,000,000");
		//timeFormat = "0000-00-00_00-00-00.000,000,000";
		int index = 3;
		for (; year > 0; --index)
		{
			timeFormat[index] = '0' + (year % 10);
			year /= 10;
		}
		for (index = 6; month > 0; --index)
		{
			timeFormat[index] = '0' + (month % 10);
			month /= 10;
		}
		for (index = 9; date > 0; --index)
		{
			timeFormat[index] = '0' + (date % 10);
			date /= 10;
		}
		for (index = 12; hrs > 0; --index)
		{
			timeFormat[index] = '0' + (hrs % 10);
			hrs /= 10;
		}
		for (index = 15; min > 0; --index)
		{
			timeFormat[index] = '0' + (min % 10);
			min /= 10;
		}
		for (index = 18; seconds > 0; --index)
		{
			timeFormat[index] = '0' + (seconds % 10);
			seconds /= 10;
		}
		for (index = 30; nanoseconds > 0; --index)
		{
			if (index == 27 || index == 23)
				continue;
			timeFormat[index] = '0' + (nanoseconds % 10);
			nanoseconds /= 10;
		}

		return std::move(timeFormat);
	}

	// This function used System Clock
	string Timer::getCurrentLocalTimeInNanoSeconds2()
	{
		//This function takes aorund 50 to 70 microseconds to execute
		SystemClock::time_point now = SystemClock::now();

		std::time_t now_t = chrono::system_clock::to_time_t(now);
		std::tm nowTm;
		localtime_s(&nowTm, &now_t);
		std::stringstream bufferStream;
		bufferStream << std::put_time(&nowTm, "%Y-%m-%d_%H-%M-%S");

		long long duration = (chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch())).count() % (long long)1000000000;

		//duration is in nanoseconds, so it can have max 9 digits, 2 commas and 1 dot
		string fractional_seconds(".000,000,000");
		for (int index = 11; duration > 0; --index)
		{
			if (index == 8 || index == 4)
				continue;
			fractional_seconds[index] = '0' + (duration % 10);
			duration /= 10;
		}

		string retVal(bufferStream.str() + fractional_seconds);
		return std::move(retVal);
	}

	/*
	string Timer::getCurrentLocalTimeInNanoSeconds()
	{
		//This function takes aorund 50 to 70 microseconds to execute

		SystemClock::time_point now = SystemClock::now();
		std::time_t now_t = chrono::system_clock::to_time_t(now);
		//std::tm now_tm = *std::localtime(&now_t);
		std::tm nowTm;
		localtime_s(&nowTm, &now_t);
		std::stringstream bufferStream;
		//bufferStream.str(std::string());
		bufferStream << std::put_time(&nowTm, "%Y-%m-%d_%H-%M-%S");

		//This block needs around 600 microseconds more
		//chrono::nanoseconds ns = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch());
		//std::chrono::seconds sec(1);
		//long long nanosecondsInOneSecond = std::chrono::nanoseconds(sec).count();
		//long long duration = ns.count() % nanosecondsInOneSecond;
		long long duration = (chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch())).count() % (long long)1000000000;

		//Following block needs around 600-700 microseconds more
		//std::stringstream ss;
		//ss.imbue(std::locale(""));
		//ss << std::fixed << duration;
		//string fractional_seconds(ss.str());

		//Instead of above block, use following block, custom version to convert long long into string with comma separated digits. It takes few microseconds
		//duration is in nanoseconds, so it can have max 9 digits, 2 commas and 1 dot
		string fractional_seconds(".000,000,000");
		for (int index = 11; duration > 0; --index)
		{
			if (index == 8 || index == 4)
				continue;
			fractional_seconds[index] = '0' + (duration % 10);
			duration /= 10;
		}

		return bufferStream.str() + fractional_seconds;
	}
	*/

	string Timer::getStartTimeInNanoSeconds()
	{
		return convertHRClockTimePointToString(m_startTime);
	}

	string Timer::getEndTimeInNanoSeconds()
	{
		return convertHRClockTimePointToString(m_endTime);
	}

	string Timer::convertHRClockTimePointToString(const HRClock::time_point& timePoint)
	{
		return convertSystemClockTimePointToString(convertToSystemClock1(timePoint));
	}

	string Timer::convertSystemClockTimePointToString(const SystemClock::time_point& now)
	{
		std::time_t now_t = chrono::system_clock::to_time_t(now);
		//std::tm now_tm = *std::localtime(&now_t);
		std::tm nowTm;
		localtime_s(&nowTm, &now_t);

		std::stringstream buffer;
		buffer << std::put_time(&nowTm, "%F %T");

		chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch());
		std::size_t fractional_seconds = ms.count() % 1000000;

		return string(buffer.str() + "." + to_string(fractional_seconds));
	}


	void Timer::printCurrentLocalTimeInDifferentFormats()
	{
		//References:
		//http://en.cppreference.com/w/cpp/io/manip/put_time
		//http://www.cplusplus.com/reference/iomanip/put_time/

		int formatCounter = 1;
		string dateTimeString;
		// ======================== Common Code ========================
		SystemClock::time_point now = SystemClock::now();
		//std::time_t now_t = std::time(nullptr);
		std::time_t now_t = chrono::system_clock::to_time_t(now);
		//std::tm now_tm = *std::localtime(&now_t);
		std::tm nowTm;
		localtime_s(&nowTm, &now_t);

		std::chrono::seconds sec(1);
		//chrono::microseconds ms = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch());
		//long long microsecondsInOneSecond = std::chrono::microseconds(sec).count();
		//std::size_t duration = ms.count() % microsecondsInOneSecond;

		chrono::nanoseconds ns = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch());
		long long nanosecondsInOneSecond = std::chrono::nanoseconds(sec).count();
		long long duration = ns.count() % nanosecondsInOneSecond;

		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << duration;
		string fractional_seconds(ss.str());

		// ======================== Date Time Format 1 ========================
		std::stringstream bufferStream;
		bufferStream << std::put_time(&nowTm, "%F %T");
		dateTimeString = string(bufferStream.str() + "." + fractional_seconds);
		cout << "\nDate Time Format " << formatCounter++ << ": " << dateTimeString;

		// ======================== Date Time Format 2 ========================
		bufferStream.str(std::string());
		bufferStream << std::put_time(&nowTm, "%Y-%m-%d %H:%M:%S");
		dateTimeString = string(bufferStream.str() + "." + fractional_seconds);
		cout << "\nDate Time Format " << formatCounter++ << ": " << dateTimeString;

		// ======================== Date Time Format 3 ========================
		bufferStream.str(std::string());
		bufferStream << std::put_time(&nowTm, "%a, %d %b %y - %H:%M:%S");
		dateTimeString = string(bufferStream.str() + "." + fractional_seconds);
		cout << "\nDate Time Format " << formatCounter++ << ": " << dateTimeString;

		// ======================== Date Time Format 4 ========================
		bufferStream.str(std::string());
		bufferStream << std::put_time(&nowTm, "%A, %d %B %Y - %H:%M:%S");
		dateTimeString = string(bufferStream.str() + "." + fractional_seconds);
		cout << "\nDate Time Format " << formatCounter++ << ": " << dateTimeString;

		// ======================== Date Time Format 5 ========================
		bufferStream.str(std::string());
		bufferStream << std::put_time(&nowTm, "%A, %d %B %Y - %I:%M:%S");
		dateTimeString = string(bufferStream.str() + "." + fractional_seconds);
		cout << "\nDate Time Format " << formatCounter++ << ": " << dateTimeString;

		// ======================== Date Time Format 6 ========================
		bufferStream.str(std::string());
		bufferStream << std::put_time(&nowTm, "%A, %d %B %Y - %I:%M:%S %p %z %Z");
		dateTimeString = string(bufferStream.str()); // +"." + fractional_seconds);
		cout << "\nDate Time Format " << formatCounter++ << ": " << dateTimeString;

		// ======================== Date Time Format 7 ========================
		//std::cout << std::ctime(&time) << "." << fractional_seconds << std::endl;
		char buffer[256];
		ctime_s(buffer, 256, &now_t);
		dateTimeString = string(buffer);
		dateTimeString.erase(dateTimeString.end() - 1); //Erase newline at end
		dateTimeString += " and " + fractional_seconds;
		cout << "\nDate Time Format " << formatCounter++ << ": " << dateTimeString;

		// ======================== Date Time Format 8 ========================
		char ch[256];
		asctime_s(ch, &nowTm);
		dateTimeString = string(ch);
		cout << "\nDate Time Format " << formatCounter++ << ": " << dateTimeString;

		// ======================== Date Time Format 9 ========================

		char date_time_format[] = "%Y.%m.%d-%H.%M.%S";
		char time_str[] = "yyyy.mm.dd.HH-MM.SS.ffffff";
		strftime(time_str, strlen(time_str), date_time_format, &nowTm);

		SystemClock::time_point now_sec = SystemClock::from_time_t(now_t);
		chrono::milliseconds millisec = chrono::duration_cast<chrono::milliseconds>(now - now_sec);

		dateTimeString = string(time_str) + "." + to_string(millisec.count());
		cout << "\nDate Time Format " << formatCounter++ << ": " << dateTimeString;
	}

	//========== Private functions ============

	SystemClock::time_point Timer::convertToSystemClock1(const HRClock::time_point& htp, bool doTally /*= false*/)
	{
		//Get the current HR clock time and System time. Assume we get it at the same time
		HRClock::time_point hnow = HRClock::now();
		SystemClock::time_point snow = SystemClock::now();

		//chrono::nanoseconds hns1 = chrono::duration_cast<chrono::nanoseconds>(hnow.time_since_epoch());
		//chrono::nanoseconds sns1 = chrono::duration_cast<chrono::nanoseconds>(snow.time_since_epoch());
		//std::cout << "\nTally now: HR clock nanoseconds = " << hns1.count() << " Sys clock nanoseconds = " << sns1.count();

		chrono::milliseconds diff = chrono::duration_cast<chrono::milliseconds>(htp - hnow);
		//std::cout << "\n Diff = " << diff.count();
		snow += diff;

		//tally
		//chrono::nanoseconds hns = chrono::duration_cast<chrono::nanoseconds>(htp.time_since_epoch());
		//chrono::nanoseconds sns = chrono::duration_cast<chrono::nanoseconds>(snow.time_since_epoch());
		//std::cout << "\nTally: HR clock nanoseconds = " << hns.count() << " Sys clock nanoseconds = " << sns.count();

		return snow;
	}

	SystemClock::time_point Timer::convertToSystemClock2(const HRClock::time_point& htp, bool doTally /*= false*/)
	{
		chrono::system_clock::duration sdu = chrono::duration_cast<chrono::system_clock::duration>(htp.time_since_epoch());
		//chrono::time_point<chrono::system_clock> stp(du); //OR use following:
		chrono::system_clock::time_point stp(sdu);

		//tally
		chrono::nanoseconds hns = chrono::duration_cast<chrono::nanoseconds>(htp.time_since_epoch());
		chrono::nanoseconds sns = chrono::duration_cast<chrono::nanoseconds>(stp.time_since_epoch());
		std::cout << "\nTally: Duration: " << sdu.count() << "HR clock nanoseconds = " << hns.count() << " Sys clock nanoseconds = " << sns.count();
		if (hns != sns)
			std::cout << "\n------TALLY FAILED------. We are losing precision.";

		return stp;
	}

	SystemClock::time_point Timer::stringToSystemClock(const string& str)
	{
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

		SystemClock::time_point time_point_result = SystemClock::from_time_t(ttime_t);

		time_point_result += std::chrono::milliseconds(ffffff);
		return time_point_result;
	}

}