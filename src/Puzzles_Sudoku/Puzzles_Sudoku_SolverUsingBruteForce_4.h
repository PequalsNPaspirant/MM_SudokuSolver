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
using namespace std;

#include "Puzzles_Sudoku/Puzzles_Sudoku_BigInteger.h"

namespace mm {

	//Forward declaraton
	struct SudokuPuzzleBasicStats;

	class SudokuMatrix4
	{
		class Cell
		{
		public:
			Cell(const int size);
			Cell(const Cell& obj);
			const Cell& operator=(const Cell& obj);
			int getOnBitPosition();

			bool isBitON(int index) const;
			void setBitOFF(int index);

			int m_value;
			BigIntegerBitCollection m_isPossible;
			int m_isPossibleCount;
		};

	public:
		static bool solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats);

		SudokuMatrix4(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats);
		SudokuMatrix4(const SudokuMatrix4& copy);
		bool assign(const int& row, const int& column, const int& value);
		bool eliminate(const int& row, const int& column, const int& valueToEliminate);
		bool solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions);

		vector< vector<Cell> > m_data;
		bool m_isValid;
		SudokuPuzzleBasicStats& m_stats;
	};

}