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


#include <iostream>
//#include <chrono> //C++11
//#include <ctime>
#include <sstream>
//#include <string>
#include <iomanip> //for std::put_time
#include <windows.h> //For Sleep(int milliseconds)

#include "Timer/Timer.h"

namespace mm {

	//=============== Test ====================

	void Timer_sanityTest()
	{
		Timer t;
		Sleep(1234);
		cout << "\nStart Time: " << t.getStartTimeInNanoSeconds();
		cout << "\nDuration till now: " << t.getDurationStringTillNowInNanoSeconds();

		cout << "\n\nPrint current time in various different formats:";
		Timer::printCurrentLocalTimeInDifferentFormats();
	}

	void Timer_performanceTest()
	{
		Timer t;
		string currentTime, duration;
		const int repeatitions = 25;

		//-------------- Test performance difference between functions getDurationTillNowInNanoSeconds and getDurationStringTillNowInNanoSeconds for some sample floating point arithmatic --------------

		cout << "\n\nTest 1: Test performance difference between functions getDurationTillNowInNanoSeconds and getDurationStringTillNowInNanoSeconds for some sample floating point arithmatic";
		cout << "\n\nCase 1: When we use getDurationTillNowInNanoSeconds() to capture duration. Takes on an average 150,000 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			Timer tLocal;
			for (int i = 0; i < 100000; i++)
			{
				double n = 342642422.42353535;
				n /= 352525235.3467352532;
			}
			long long durationNanoSeconds = tLocal.getDurationTillNowInNanoSeconds();
			cout << "\nTime (captured by case 1:getDurationTillNowInNanoSeconds()) required for some sample floating point arithmatic = " << durationNanoSeconds << " nanoseconds";
		}

		cout << "\n\nCase 2: When we use getDurationStringTillNowInNanoSeconds() to capture duration. Takes on an average 150,000 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			Timer tLocal;
			for (int i = 0; i < 100000; i++)
			{
				double n = 342642422.42353535;
				n /= 352525235.3467352532;
			}
			string durationNanoSeconds = tLocal.getDurationStringTillNowInNanoSeconds();
			cout << "\nTime (captured by case 2:getDurationStringTillNowInNanoSeconds()) required for some sample floating point arithmatic = " << durationNanoSeconds;
		}

		//-------------- Test performance difference between functions getDurationTillNowInNanoSeconds and getDurationStringTillNowInNanoSeconds for no operations in between --------------

		cout << "\n\nTest 2: Test performance difference between functions getDurationTillNowInNanoSeconds and getDurationStringTillNowInNanoSeconds for no operations in between";
		cout << "\n\nCase 1: When we use getDurationTillNowInNanoSeconds() to capture duration. Takes on an average 1,058 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			Timer tLocal;
			long long durationNanoSeconds = tLocal.getDurationTillNowInNanoSeconds();
			cout << "\nTime (captured by case 1:getDurationTillNowInNanoSeconds()) required for no operations in between = " << durationNanoSeconds << " nanoseconds";
		}

		cout << "\n\nCase 2: When we use getDurationStringTillNowInNanoSeconds() to capture duration. Takes on an average 705 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			Timer tLocal;
			string durationNanoSeconds = tLocal.getDurationStringTillNowInNanoSeconds();
			cout << "\nTime (captured by case 2:getDurationStringTillNowInNanoSeconds()) required for no operations in between = " << durationNanoSeconds;
		}

		//-------------- Test performance difference between functions getDurationTillNowInNanoSeconds and getDurationStringTillNowInNanoSeconds for no operations in between when same time is reused --------------

		cout << "\n\nTest 3: Test performance difference between functions getDurationTillNowInNanoSeconds and getDurationStringTillNowInNanoSeconds for no operations in between when same time is reused";
		cout << "\n\nCase 1: When we use getDurationTillNowInNanoSeconds() to capture duration. Takes on an average 352 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			t.resetTimer();
			long long durationNanoSeconds = t.getDurationTillNowInNanoSeconds();
			cout << "\nTime (captured by case 1:getDurationTillNowInNanoSeconds()) required for no operations in between when same timer is reused = " << durationNanoSeconds << " nanoseconds";
		}

		cout << "\n\nCase 2: When we use getDurationStringTillNowInNanoSeconds() to capture duration. Takes on an average 352 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			t.resetTimer();
			string durationNanoSeconds = t.getDurationStringTillNowInNanoSeconds();
			cout << "\nTime (captured by case 2:getDurationStringTillNowInNanoSeconds()) required for no operations in between when same timer is reused = " << durationNanoSeconds;
		}

		//-------------- Test performance of function resetTimer --------------

		cout << "\n\nTest 4: Test performance of function resetTimer. Takes on an average 352 nanoseconds";
		Timer tLocal;
		for (int i = 0; i < repeatitions; i++)
		{
			tLocal.resetTimer();
			t.resetTimer();
			string durationNanoSeconds = tLocal.getDurationStringTillNowInNanoSeconds();
			cout << "\nTime required for execution of function resetTimer = " << durationNanoSeconds;
		}

		//-------------- Test performance of function getDurationTillNowInNanoSeconds --------------

		cout << "\n\nTest 5: Test performance of function getDurationTillNowInNanoSeconds Takes on an average 1,400 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			t.resetTimer();
			long long durationNanoSeconds = t.getDurationTillNowInNanoSeconds();
			duration = t.getDurationStringTillNowInNanoSeconds();
			cout << "\nTime required for execution of function getDurationInNanoSeconds = duration string - durationNanoSeconds = " << duration << " - " << durationNanoSeconds << " nanoseconds";
		}

		//-------------- Test performance of function getDurationStringTillNowInNanoSeconds --------------

		cout << "\n\nTest 6: Test performance of function getDurationStringTillNowInNanoSeconds. Takes on an average 25,000 nanoseconds";
		string duration1, duration2;
		for (int i = 0; i < repeatitions; i++)
		{
			t.resetTimer();
			duration1 = t.getDurationStringTillNowInNanoSeconds();
			duration2 = t.getDurationStringTillNowInNanoSeconds();
			cout << "\nTime required for execution of function getDurationStringTillNowInNanoSeconds = duration string 2 - duration string 1 = " << duration2 << " - " << duration1;
		}

		//-------------- Test performance of function getDurationStringTillNowInNanoSeconds2 --------------

		cout << "\n\nTest 7: Test performance of function getDurationStringTillNowInNanoSeconds2. Takes on an average 750,000 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			t.resetTimer();
			duration1 = t.getDurationStringTillNowInNanoSeconds2();
			duration2 = t.getDurationStringTillNowInNanoSeconds();
			cout << "\nTime required for execution of function getDurationStringTillNowInNanoSeconds2 = duration string 2 - duration string 1 = " << duration2 << " - " << duration1;
		}

		//-------------- Test performance of function getCurrentLocalTimeInNanoSeconds --------------

		cout << "\n\nTest 8: Test performance of function getCurrentLocalTimeInNanoSeconds. Takes on an average 50,000 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			t.resetTimer();
			currentTime = Timer::getCurrentLocalTimeInNanoSeconds();
			duration = t.getDurationStringTillNowInNanoSeconds();
			cout << "\nCurrent Time: " << currentTime << " Time required for execution of function getCurrentLocalTimeInNanoSeconds: " << duration;
		}

		//-------------- Test performance of function getCurrentLocalTimeInNanoSeconds2 --------------

		cout << "\n\nTest 9: Test performance of function getCurrentLocalTimeInNanoSeconds2. Takes on an average 70,000 nanoseconds";
		for (int i = 0; i < repeatitions; i++)
		{
			t.resetTimer();
			currentTime = Timer::getCurrentLocalTimeInNanoSeconds2();
			duration = t.getDurationStringTillNowInNanoSeconds();
			cout << "\nCurrent Time: " << currentTime << " Time required for execution of function getCurrentLocalTimeInNanoSeconds2: " << duration;
		}
	}

	void Timer_UnitTest()
	{
		cout << "\n\n~~~~~~~~~~~~~~~~~~ Timer_UnitTest() ~~~~~~~~~~~~~~~~~~~~~~~\n";

		//Timer_sanityTest();
		Timer_performanceTest();
	}

}
