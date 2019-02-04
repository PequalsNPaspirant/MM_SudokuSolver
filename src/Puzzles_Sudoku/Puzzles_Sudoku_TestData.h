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
using namespace std;

namespace mm {

	class SudokuPuzzleData
	{
	public:
		SudokuPuzzleData(const vector< vector<int> >& dataIn, const vector< vector<int> >& solution = vector< vector<int> >(0, vector<int>(0)), const string& category = "");
		SudokuPuzzleData(const string& str, const string& solution = "", const size_t dim = 9);
		const vector< vector<int> >& getPuzzleData() const;
		const vector< vector<int> >& getSolution() const;
		const string& getCategory() const;

	private:
		vector< vector<int> > m_sudokuPuzzleData;
		vector< vector<int> > m_solution;
		string m_category;
	};

	class SudokuPuzzleDataFactory
	{
	public:
		static const vector<SudokuPuzzleData>& getSimpleSudokuPuzzleDataSets();
		static const vector<SudokuPuzzleData>& getHardSudokuPuzzleDataSets();
		static const vector<SudokuPuzzleData>& getHardestSudokuPuzzleDataSets();
		static const vector<SudokuPuzzleData>& getSudokuPuzzleDataSetsFromSource1_easy();
		static const vector<SudokuPuzzleData>& getSudokuPuzzleDataSetsFromSource1_simple();
		static const vector<SudokuPuzzleData>& getSudokuPuzzleDataSetsFromSource1_intermediate();
		static const vector<SudokuPuzzleData>& getSudokuPuzzleDataSetsFromSource1_expert();

	private:
		SudokuPuzzleDataFactory() = delete;
	};
}

