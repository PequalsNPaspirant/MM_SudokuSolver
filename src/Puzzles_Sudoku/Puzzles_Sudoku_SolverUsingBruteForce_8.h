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
#include <cmath>
#include <map>
using namespace std;

#include "Puzzles_Sudoku/Puzzles_Sudoku_BigInteger.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"

namespace mm {

	//Forward declaraton
	struct SudokuPuzzleBasicStats;

	class FastestBruteForceSudokuMatrix_2
	{
	public:
		static bool solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats);

		const unsigned int getCountOfONBits(const BigIntegerBitCollection& bitSet);
	private:
		const unsigned int getCountOfONBits_v1(const BigIntegerBitCollection& bitSet);
		const unsigned int getCountOfONBits_v2(const BigIntegerBitCollection& bitSet);
		const unsigned int getCountOfONBits_v3(const BigIntegerBitCollection& bitSet);
		const unsigned int getCountOfONBits_v4(const BigIntegerBitCollection& bitSet);

	public:
		const unsigned int getOnBitPosition(const BigIntegerBitCollection& bitSet);
	private:
		const unsigned int getOnBitPosition_v1(const BigIntegerBitCollection& bitSet);
		const unsigned int getOnBitPosition_v2(const BigIntegerBitCollection& bitSet);

	public:
		bool isBitON(const BigIntegerBitCollection& bitSet, const unsigned int& index) const;
		bool isBitON(const BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask) const;
		void setBitOFF(BigIntegerBitCollection& bitSet, const unsigned int& index);
		void setBitOFF(BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask);
		void setBitON(BigIntegerBitCollection& bitSet, const unsigned int& index);
		void setBitON(BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask);
		void copyValue(const BigIntegerBitCollection& from, BigIntegerBitCollection& to);

		FastestBruteForceSudokuMatrix_2(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats);
		bool solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions);
		//bool assign(const unsigned int& row, const unsigned int& column, const unsigned int& value);

	private:
		//bool propagateConstraint(const unsigned int& row, const unsigned int& column, const unsigned int& removeThisBit);
		bool updateNSCounters(const unsigned int& row, const unsigned int& column, const unsigned int& removeThisBit);
		bool updateHSCountersRow(const unsigned int& row, const unsigned int& thisBitRemoved);
		bool updateHSCountersColumn(const unsigned int& column, const unsigned int& thisBitRemoved);
		bool updateHSCountersBox(const unsigned int& boxIndex, const unsigned int& thisBitRemoved);
		//bool updateHSPossiblePositionsRow(const unsigned int& row, const unsigned int& column, const unsigned int& thisBitRemoved);
		//bool updateHSPossiblePositionsColumn(const unsigned int& row, const unsigned int& column, const unsigned int& thisBitRemoved);
		//bool updateHSPossiblePositionsBox(const unsigned int& boxIndex, const unsigned int& cellIndexInBox, const unsigned int& thisBitRemoved);

		bool checkIfNakedSingle(const unsigned int& row, const unsigned int& column);
		bool searchForHiddenSingles();
		bool searchForLockedCandidate();
		bool processPendingAssignments();

	private:
		const unsigned int m_size;
		const unsigned int m_boxSize;
		SudokuPuzzleBasicStats& m_stats;

	public:
		vector< vector<BigIntegerBitCollection> > m_NSPossibleValues;
		//vector< vector<unsigned int> > m_NSPossibleValuesCount;

		//vector< vector<BigIntegerBitCollection> > m_HSPossiblePositionsRow;
		vector< vector<unsigned int> > m_HSPossiblePositionsCountRow;
		//vector< vector<BigIntegerBitCollection> > m_HSPossiblePositionsColumn;
		vector< vector<unsigned int> > m_HSPossiblePositionsCountColumn;
		//vector< vector<BigIntegerBitCollection> > m_HSPossiblePositionsBox;
		vector< vector<unsigned int> > m_HSPossiblePositionsCountBox;

	public:
		unsigned int m_cluesToFind;

		//static vector< vector<int> > m_solution;
		static vector<BigIntegerBitCollection> m_initializationFiltersRow;
		static vector<BigIntegerBitCollection> m_initializationFiltersColumn;
		static vector<BigIntegerBitCollection> m_initializationFiltersBox;

		//static vector<BigIntegerBitCollection> m_initializationFiltersRowHS;
		//static vector<BigIntegerBitCollection> m_initializationFiltersColumnHS;
		//static vector<BigIntegerBitCollection> m_initializationFiltersBoxHS;

		static BigIntegerBitCollection m_bitsON;
		static BigIntegerBitCollection m_valuesToEliminate;

		//For locked candidates
		//static PendingAssignmentsCircularQueue m_pendingAssignmentsQueue;
		static map<std::pair<unsigned int, unsigned int>, unsigned int> m_pendingAssignmentsHashMap;
		static vector< vector<BigIntegerBitCollection> > m_lockedCandidatesHorizontalStrips;
		static vector< vector<BigIntegerBitCollection> > m_lockedCandidatesVerticalStrips;		
	};
}
