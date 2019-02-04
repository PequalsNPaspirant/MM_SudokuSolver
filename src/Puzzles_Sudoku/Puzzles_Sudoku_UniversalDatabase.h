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


#pragma once

#include <vector>
#include <initializer_list>
#include <string>
#include <map>
#include <unordered_set>
using namespace std;

namespace mm {

	class comparator
	{
	public:
		bool operator()(std::pair<unsigned int, string> lhs, std::pair<unsigned int, string> rhs)
		{
			return (lhs.first < rhs.first) || (lhs.first == rhs.first && lhs.second < rhs.second);
		}
	};

	class SudokuPuzzleUniversalDatabase
	{
	public:
		static const map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>& readExistngLibrary(const string& libraryPath, const bool SkipStatistics);
		static bool writeToLibrary();

		static bool addToLibrary(const unsigned int dimension, const string& newPuzzle);
		static map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>::iterator& getCurrentEntry()
		{
			return m_currentEntry;
		}

		static void setCurrentCategory(const string& category);
		static const string& getCurrentCategory();

	private:
		static string m_sudokuPuzzleLibraryFilePath;
		static map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator> m_sudokuPuzzleLibrary;
		static map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>::iterator m_currentEntry;

		static int m_initialCount;
		static int m_duplicatePuzzlesCount;
		static string m_puzzleCategory;
	};
}

