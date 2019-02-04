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

	class SudokuMatrix6
	{
	public:
		static bool solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats);

		class Cell
		{
		public:
			Cell(const unsigned int& size);
			Cell(const Cell& obj);
			const unsigned int getOnBitPosition();

			bool isBitON(const unsigned int& index) const;
			void setBitOFF(const unsigned int& index);

			//int m_value;
			BigIntegerBitCollection m_isPossible;
			unsigned int m_isPossibleCount;
		};

		SudokuMatrix6(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats);
		bool solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions);
		bool assign(const unsigned int& row, const unsigned int& column, const unsigned int& value);

		bool m_isValid;		
		unsigned int m_cluesCount;
		SudokuPuzzleBasicStats& m_stats;
		vector< vector<Cell> > m_data;

	private:
		bool propagateConstraint(const unsigned int& row, const unsigned int& column, const unsigned int& value);
		bool checkIfCellHasNakedSingle(const unsigned int& row, const unsigned int& column);
		bool checkIfRowHasHiddenSingle(const unsigned int& row, const unsigned int& value);
		bool checkIfColumnHasHiddenSingle(const unsigned int& column, const unsigned int& value);
		bool checkIfBoxHasHiddenSingle(const unsigned int& boxRowStart, const unsigned int& boxColumnStart, const unsigned int& value);

		bool removeFromPossibleValuesOfPeer(const unsigned int& row, const unsigned int& column, const unsigned int& value);

		void copyDataFromTo(const vector< vector<Cell> >& from, vector< vector<Cell> >& to);

		const unsigned int m_size;
		const unsigned int m_boxSize;
	};

}
