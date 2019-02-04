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
#include <vector>
#include <array>
#include <set>
#include <string>
#include <cmath>
using namespace std;

#include <windows.h>

#include "Puzzles_Sudoku/Puzzles_Sudoku_GlobalLookupTables.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_10.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"
#include "Timer/Timer.h"

/*
Algorithm in detail:

Dimension of grid = n
Dimension of box = sqrt(n)

A1 A2 A3 | A4 A5 A6 | A7 A8 A9
B1 B2 B3 | B4 B5 B6 | B7 B8 B9
C1 C2 C3 | C4 C5 C6 | C7 C8 C9
---------+----------+---------
D1 D2 D3 | D4 D5 D6 | D7 D8 D9
E1 E2 E3 | E4 E5 E6 | E7 E8 E9
F1 F2 F3 | F4 F5 F6 | F7 F8 F9
---------+----------+---------
G1 G2 G3 | G4 G5 G6 | G7 G8 G9
H1 H2 H3 | H4 H5 H6 | H7 H8 H9
I1 I2 I3 | I4 I5 I6 | I7 I8 I9

- Initialising the grid with given hints:
	- Initializing one by one - Assign a value val to any cell c:
	  (If d initial clues are provided then
	   d * ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1)) BigInt & operations)
		- Update the bits for naked singles
		  ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1) BigInt & operations)
			- Change the value of current cell/Remove the the possibilities of other values in current cell than the choosen value val 
			  (1 BigInt = operation)
			- Propagate the constraint to the row, column and box to which this cell belongs to i.e. remove the possibility of value val 
			  from other cells in the same row, column and box
			  ((n - 1) BigInt & operations for row to check if bit is ON, if yes, then BigInt & operation to make bit OFF, 
			   (n - 1) BigInt & operations for column to check if bit is ON, if yes, then BigInt & operation to make bit OFF, 
			   (n - 2 * sqrt(n) + 1) BigInt & operations for box to check if bit is ON, if yes, then BigInt & operation to make bit OFF)
			  Check if only one bit is ON here to find naked single.
			  ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1) operations to count the ON bits and same number of (int == 1) operations)
		- Update the counters for hidden singles
		  (3 * (1 + p + ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1)) int - operations for each of d clues)
			- Set counter = 1 at the position where value val is assigned to. Do it for each row, column and box counters. (3 int = operations)
				Reduce the counters by 1 for the positions = all ON bits in the current cell. Say we found p such positions. Do it for each row, column and box counters. 
				(loop of n iterations => n BigInt & operations to check if bit is ON, if yes, then 3 int - operations
				i.e. 3 * p int - operations)
			- While propagating constraint (refer line 40 in this file), if the bit at val is ON, then reduce the counter by 1
			  ((n - 1) BigInt & operations for row to check if bit is ON, if yes, then int - operation to reduce counter for column and box, 
			   (n - 1) BigInt & operations for column to check if bit is ON, if yes, then int - operation to reduce counter for row and box, 
			   (n - 2 * sqrt(n) + 1) BigInt & operations for box to check if bit is ON, if yes, then int - operation to reduce counter for row and column)
	- Initializing all in one go (this can increase efficiency of Propagation of the constraint above at line 40 in this file):
		- Update the bits for naked singles
		  (total 3 * (d + n*n) BigInt bitwise operations)
			- Loop through all n*n cells and Prepare the initializer mask
			  (If d initial clues are provided then d * (one BigInt | operation each for row, column and box)
			  i.e. 3 * d BigInt | operations)
			- Loop through all n*n cells and Apply masks for initial structure of naked singles for entire grid
			  (2 BigInt | operations to combine masks for row, column and box,
			   1 BigInt & operation to remove bits from naked single bits
			   i.e. 3 * n * n BigInt bitwise operations)
			  Check if only one bit is ON here to find naked single.
			  (n*n operations to count the ON bits and same number of (int == 1) operations)
		- Update the counters for hidden singles
		  (d * 3*p + 1 + n*n * 3*p)
			- For all d clues, while removing all ON bits other than val, update counters for all such ON bits (let's say we found p such ON positions) 
			  for row, column and box in which this cell falls
			  (for each row, column and box, p int - operations
			   i.e. total d * 3*p int - operations)
			- Before removing bits at line 68, keep track of all ON bits. Lets say it is bitsToBeRemoved
			  (requires one BigInt & operation)
			- Reduce the counters by 1 for the positions = all ON bits in the above bitsToBeRemoved. Say we found p such positions. 
			  Do it for each row, column and box counters.
			  (total operations for n*n cells = n*n * 3*p)
	- Which is better? Initializing d initial clues one by one OR Initializing all clues in one go
	  d * ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1)) = 3 * (d + n*n)
	  d * (3n - 1 - 2 * sqrt(n))                      = 3 * (d + n*n)
	  d * (3n - 1 - 2 * sqrt(n)) - 3d                 = n*n
	  d * (3n - 4 - 2 * sqrt(n))                      = n*n
	  d                                               = n*n / (3n - 4 - 2 * sqrt(n))
	  d                                               ~ n*n / 3n
	  d                                               ~ n / 3
	  Thus when d ~ n / 3, the number of operations required in both cases are same
	  OR we can say, when d ~< n / 3, the one by one assignment would be preferable
	  Actual values of d below which one by one assignment would be preferable
	  n		d = n*n / (3n - 4 - 2 * sqrt(n)) 
	  --------------------------------------
	  4		4 * 4 / (3 * 4 - 4 - 2 * 2) = 16 / (12 - 4 - 4) = 16 / 4 = 4
	  9		9 * 9 / (3 * 9 - 4 - 2 * 3) = 81 / (27 - 4 - 6) = 81 / 17 = 4.76
	  16	16 * 16 / (3 * 16 - 4 - 2 * 4) = 256 / (48 - 4 - 8) = 256 / 36 = 7.11
- Find hidden single
  (total row, column and box = 3 * n
   total 3n * (2*n + 1) BigInt bitwise operations ~ 6*n*n)
	- Loop through all cells in each row, column and box, 2 BigInt & and | operations to find atLeatOnes and moreThanOnce masks for each cell
	  (for each row, column and box = 2 * n BigInt bitwise operations)
	- onlyOnes = atLeatOnes & ~moreThanOnce to find hidden singles
- Find locked candidate



*/

namespace mm {

	unsigned int FastestBruteForceSudokuMatrix_4::m_size;
	unsigned int FastestBruteForceSudokuMatrix_4::m_boxSize;

	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_4::m_initializationFiltersRow(MAX_DIM, BigIntegerBitCollection(0));
	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_4::m_initializationFiltersColumn(MAX_DIM, BigIntegerBitCollection(0));
	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_4::m_initializationFiltersBox(MAX_DIM, BigIntegerBitCollection(0));
	//vector< vector<int> > FastestBruteForceSudokuMatrix_4::m_solution(MAX_DIM, vector<int>(MAX_DIM, 0));

	//vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_4::m_initializationFiltersRowHS(MAX_DIM, BigIntegerBitCollection(0));
	//vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_4::m_initializationFiltersColumnHS(MAX_DIM, BigIntegerBitCollection(0));
	//vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_4::m_initializationFiltersBoxHS(MAX_DIM, BigIntegerBitCollection(0));

	BigIntegerBitCollection FastestBruteForceSudokuMatrix_4::m_bitsON(0);
	BigIntegerBitCollection FastestBruteForceSudokuMatrix_4::m_valuesToEliminate(0);

	vector< vector<BigIntegerBitCollection> > FastestBruteForceSudokuMatrix_4::m_lockedCandidatesHorizontalStrips(MAX_DIM, vector<BigIntegerBitCollection>(MAX_BOX_DIM, BigIntegerBitCollection(0)));
	vector< vector<BigIntegerBitCollection> > FastestBruteForceSudokuMatrix_4::m_lockedCandidatesVerticalStrips(MAX_BOX_DIM, vector<BigIntegerBitCollection>(MAX_DIM, BigIntegerBitCollection(0)));

	PendingAssignmentsCircularQueue FastestBruteForceSudokuMatrix_4::m_pendingAssignmentsQueue(MAX_DIM * MAX_DIM);
	//map<std::pair<unsigned int, unsigned int>, unsigned int> FastestBruteForceSudokuMatrix_4::m_pendingAssignmentsHashMap;

	BigIntegerBitCollection FastestBruteForceSudokuMatrix_4::onceOrMoreThanOnce(0);
	BigIntegerBitCollection& FastestBruteForceSudokuMatrix_4::onlyOnce = FastestBruteForceSudokuMatrix_4::onceOrMoreThanOnce;
	BigIntegerBitCollection FastestBruteForceSudokuMatrix_4::moreThanOnce(0);
	BigIntegerBitCollection FastestBruteForceSudokuMatrix_4::tempCurrentNSMask(0);

	bool FastestBruteForceSudokuMatrix_4::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		m_size = dataIn.size();
		m_boxSize = sqrt(m_size);
		FastestBruteForceSudokuMatrix_4 obj(dataIn, stats);
		return obj.solve(solutionSetsOut, numSolutions);
	}

	const unsigned int FastestBruteForceSudokuMatrix_4::getCountOfONBits(const BigIntegerBitCollection& bitSet)
	{
		return getCountOfONBits_v4(bitSet);
	}

	const unsigned int FastestBruteForceSudokuMatrix_4::getCountOfONBits_v1(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int value = 1; value <= m_size; ++value)
			if (isBitON(bitSet, value))
				++count;

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_4::getCountOfONBits_v2(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < bitSet.getDigits().size(); ++i)
			count += (globalLookupTables.countONbits8[bitSet.getDigits()[i] & 0b11111111]
				+ globalLookupTables.countONbits8[(bitSet.getDigits()[i] >> 8) & 0b11111111]
				+ globalLookupTables.countONbits8[(bitSet.getDigits()[i] >> 16) & 0b11111111]
				+ globalLookupTables.countONbits8[(bitSet.getDigits()[i] >> 24) & 0b11111111]);

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_4::getCountOfONBits_v3(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < bitSet.getDigits().size(); ++i)
			count += (globalLookupTables.countONbits16[bitSet.getDigits()[i] & 0b1111111111111111]
				+ globalLookupTables.countONbits16[(bitSet.getDigits()[i] >> 16) & 0b1111111111111111]);

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_4::getCountOfONBits_v4(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < bitSet.getDigits().size(); ++i)
		{
			unsigned int temp = bitSet.getDigits()[i];
			temp = temp - ((temp >> 1) & 0x55555555);
			temp = (temp & 0x33333333) + ((temp >> 2) & 0x33333333);
			count += (((temp + (temp >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
		}

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_4::getOnBitPosition(const BigIntegerBitCollection& bitSet)
	{
		return getOnBitPosition_v4(bitSet);
	}

	const unsigned int FastestBruteForceSudokuMatrix_4::getOnBitPosition_v1(const BigIntegerBitCollection& bitSet)
	{
		//Ideally there should be only one bit ON
		//MyAssert::myRunTimeAssert((m_isPossible & (m_isPossible - 1)) == 0);
		//MyAssert::myRunTimeAssert(m_isPossible.isOnlyOneBitON());
		//if (m_isPossible.isOnlyOneBitON())
		return bitSet.getOnBitPosition();
		//else
		//	return 0;
	}

	const unsigned int FastestBruteForceSudokuMatrix_4::getOnBitPosition_v2(const BigIntegerBitCollection& bitSet)
	{
		unsigned int size = bitSet.getDigits().size();
		unsigned int carryPositions = 0;
		unsigned int digitIndex = 0;
		for (; digitIndex < size && bitSet.getDigits()[digitIndex] == 0; ++carryPositions, ++digitIndex);

		unsigned int temp = globalLookupTables.whichBitIsON16[bitSet.getDigits()[digitIndex] & 0b1111111111111111];
		unsigned int count = temp;
		if (temp == 0)
		{
			temp = globalLookupTables.whichBitIsON16[(bitSet.getDigits()[digitIndex] >> 16) & 0b1111111111111111];
			count = 16 + temp;
		}
		return carryPositions * BigIntegerBitCollection::BITS_IN_ONE_DIGIT + count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_4::getOnBitPosition_v3(const BigIntegerBitCollection& bitSet)
	{
		unsigned int i = bitSet.getDigits().size();
		unsigned int digitPosition = 0;
		for (; i > 0 && bitSet.getDigits()[--i] == 0; ++digitPosition);
		unsigned int currentDigit = bitSet.getDigits()[i];
		unsigned int localBytePosition = 0;
		for (; (currentDigit & 0b11111111) == 0; ++localBytePosition, currentDigit = currentDigit >> 8);

		return digitPosition * BigIntegerBitCollection::BITS_IN_ONE_DIGIT
			+ localBytePosition * 8
			+ globalLookupTables.whichBitIsON8[currentDigit & 0b11111111];
	}

	//The above implementation can be rewritten as following to avoid branching
	const unsigned int FastestBruteForceSudokuMatrix_4::getOnBitPosition_v4(const BigIntegerBitCollection& bitSet)
	{
		unsigned int i = 0;
		unsigned int carryPositions = 0;
		for (; i < bitSet.getDigits().size() && bitSet.getDigits()[i] == 0; ++carryPositions, ++i);

		unsigned int currentDigit = bitSet.getDigits()[i];
		unsigned int temp1 = globalLookupTables.whichBitIsON8[currentDigit & 0b11111111];
		unsigned int temp2 = globalLookupTables.whichBitIsON8[(currentDigit >> 8) & 0b11111111];
		unsigned int temp3 = globalLookupTables.whichBitIsON8[(currentDigit >> 16) & 0b11111111];
		unsigned int temp4 = globalLookupTables.whichBitIsON8[(currentDigit >> 24) & 0b11111111];

		static unsigned int multiplier[9] = {0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u};
		return carryPositions * BigIntegerBitCollection::BITS_IN_ONE_DIGIT 
			+ temp1
			+ (8 * multiplier[temp2] + temp2)
			+ (16 * multiplier[temp3] + temp3)
			+ (24 * multiplier[temp4] + temp4);
	}

	// Index range [1, infinity)
	bool FastestBruteForceSudokuMatrix_4::isBitON(const BigIntegerBitCollection& bitSet, const unsigned int& index) const
	{
		//return m_isPossible.isnthBitON(index);
		return bitSet.getDigits()[globalLookupTables.bitPositionToDigitIndexHash[index]] & globalLookupTables.bitMask32[BigIntegerBitCollection::getBitPositionInCurrentDigit(index)];
	}

	bool FastestBruteForceSudokuMatrix_4::isBitON(const BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask) const
	{
		for (unsigned int i = 0; i < mask.getDigits().size(); ++i)
			if (mask.getDigits()[i] != 0 && (bitSet.getDigits()[i] & mask.getDigits()[i]) == 0)
				return false;

		return true;
	}

	bool FastestBruteForceSudokuMatrix_4::isBitSubset(const BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& subSet) const
	{
		for (unsigned int i = 0; i < subSet.getDigits().size(); ++i)
			if ((bitSet.getDigits()[i] & subSet.getDigits()[i]) != subSet.getDigits()[i])
				return false;

		return true;
	}

	// Index range [1, infinity)
	void FastestBruteForceSudokuMatrix_4::setBitOFF(BigIntegerBitCollection& bitSet, const unsigned int& index)
	{
		//m_isPossible.setnthBitOFF(index);
		bitSet.getDigits()[ globalLookupTables.bitPositionToDigitIndexHash[index] ] &= globalLookupTables.bitMask32Inv[BigIntegerBitCollection::getBitPositionInCurrentDigit(index)];
	}

	void FastestBruteForceSudokuMatrix_4::setBitOFF(BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask)
	{		
		for (unsigned int i = 0; i < mask.getDigits().size(); ++i)
			bitSet.getDigits()[i] &= ~mask.getDigits()[i];
	}

	// Index range [1, infinity)
	void FastestBruteForceSudokuMatrix_4::setBitON(BigIntegerBitCollection& bitSet, const unsigned int& index)
	{
		//m_isPossible.setnthBitOFF(index);
		bitSet.getDigits()[globalLookupTables.bitPositionToDigitIndexHash[index]] |= globalLookupTables.bitMask32[BigIntegerBitCollection::getBitPositionInCurrentDigit(index)];
	}

	void FastestBruteForceSudokuMatrix_4::setBitON(BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask)
	{
		for (unsigned int i = 0; i < mask.getDigits().size(); ++i)
			bitSet.getDigits()[i] |= mask.getDigits()[i];
	}

	void FastestBruteForceSudokuMatrix_4::copyValue(const BigIntegerBitCollection& from, BigIntegerBitCollection& to)
	{
		//to.getDigits().assign(from.getDigits().begin(), from.getDigits().end());
		unsigned int index = 0;
		for (; index < from.getDigits().size(); ++index)
			to.getDigits()[index] = from.getDigits()[index];

		for (; index < to.getDigits().size(); ++index)
			to.getDigits()[index] = 0;
	}

	FastestBruteForceSudokuMatrix_4::FastestBruteForceSudokuMatrix_4(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
		: m_stats(stats),
		m_cluesToFind(m_size * m_size),
		m_NSPossibleValues(m_size * m_size, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])
		//m_NSPossibleValuesCount(m_size, vector<unsigned int>(m_size, m_size)),
		//m_HSPossiblePositionsRow(m_size, vector<BigIntegerBitCollection>(m_size + 1, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])),
		//m_HSPossiblePositionsCountRow(m_size, vector<unsigned int>(m_size + 1, m_size)),
		//m_HSPossiblePositionsColumn(m_size, vector<BigIntegerBitCollection>(m_size + 1, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])),
		//m_HSPossiblePositionsCountColumn(m_size, vector<unsigned int>(m_size + 1, m_size)),
		//m_HSPossiblePositionsBox(m_size, vector<BigIntegerBitCollection>(m_size + 1, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]]))
		//m_HSPossiblePositionsCountBox(m_size, vector<unsigned int>(m_size + 1, m_size))
		, m_cellToBoxIndex(globalLookupTables.cellToBoxIndex[m_boxSize])
		, m_boxIndexToBoxStart(globalLookupTables.boxIndexToBoxStart[m_boxSize])
	{
		unsigned int numDigits = m_NSPossibleValues[0].getDigits().size();
		if (m_bitsON.getDigits().size() < numDigits)
		{
			m_bitsON.getDigits().resize(numDigits);
			m_valuesToEliminate.getDigits().resize(numDigits);
			for (unsigned int i = 0; i < m_size; ++i)
			{
				m_initializationFiltersRow[i].resize(numDigits);
				m_initializationFiltersColumn[i].resize(numDigits);
				m_initializationFiltersBox[i].resize(numDigits);
			}
			onceOrMoreThanOnce.resize(numDigits);
			moreThanOnce.resize(numDigits);
			tempCurrentNSMask.resize(numDigits);
			for (unsigned int row = 0; row < m_lockedCandidatesHorizontalStrips.size(); ++row)
				for (unsigned int column = 0; column < m_lockedCandidatesHorizontalStrips[row].size(); ++column)
					m_lockedCandidatesHorizontalStrips[row][column].resize(numDigits);
			for (unsigned int row = 0; row < m_lockedCandidatesVerticalStrips.size(); ++row)
				for (unsigned int column = 0; column < m_lockedCandidatesVerticalStrips[row].size(); ++column)
					m_lockedCandidatesVerticalStrips[row][column].resize(numDigits);
		}

		//Reset all mask to be used to update NS data structure
		//for (unsigned int i = 0; i < m_size; ++i)
		//{
		//	m_initializationFiltersRow[i].resetAllDigits();
		//	m_initializationFiltersColumn[i].resetAllDigits();
		//	m_initializationFiltersBox[i].resetAllDigits();

		//	//m_initializationFiltersRowHS[i].resetAllDigits();
		//	//m_initializationFiltersColumnHS[i].resetAllDigits();
		//	//m_initializationFiltersBoxHS[i].resetAllDigits();
		//}

		for (unsigned int row = 0; row < m_size; ++row)
			for (unsigned int column = 0; column < m_size; ++column)
				if (dataIn[row][column] != 0)
				{
					--m_cluesToFind;
					copyValue(globalLookupTables.bitMask[dataIn[row][column]], m_NSPossibleValues[row * m_size + column]);
					//setBitON(m_NSPossibleValues[row * m_size + column], dataIn[row][column]);
					m_pendingAssignmentsQueue.push(row, column, dataIn[row][column]);
					//m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = dataIn[row][column];
				}
	}

	bool FastestBruteForceSudokuMatrix_4::processPendingAssignments()
	{
		//If there are less pending assignements, process them one by one. 
		//In fact process only first in it and return so that the parent function solve() calls it again since the map is still not empty.
		if (m_pendingAssignmentsQueue.count() < 5)
		//if (m_pendingAssignmentsHashMap.size() < 5)
			return processPendingAssignmentsOneByOne();
		else
			return processPendingAssignmentsAllAtOnce();
	}
	
	bool FastestBruteForceSudokuMatrix_4::processPendingAssignmentsOneByOne()
	{
		unsigned int row = m_pendingAssignmentsQueue.top().row;
		unsigned int column = m_pendingAssignmentsQueue.top().column;
		unsigned int value = m_pendingAssignmentsQueue.top().value;
		m_pendingAssignmentsQueue.pop();
		//auto iterator = m_pendingAssignmentsHashMap.begin();
		//unsigned int row = iterator->first.first;
		//unsigned int column = iterator->first.second;
		//unsigned int value = iterator->second;
		//m_pendingAssignmentsHashMap.erase(iterator);

		if (!isBitON(m_NSPossibleValues[row * m_size + column], value))
			return false;

		//If already assigned, continue
		//if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) == 1)
		//	return true;

		//Update NS Data structure: Set all other bits OFF as this cell is solved
		//copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[row * m_size + column]);

		//Propagate constraint			
		// Check row
		for (unsigned int k = 0; k < m_size; ++k)
		{
			if (k != column && isBitON(m_NSPossibleValues[row * m_size + k], value))
			{
				setBitOFF(m_NSPossibleValues[row * m_size + k], value);
				if (!checkIfNakedSingle(row, k))
					return false;
			}
		}

		// Check column
		for (unsigned int k = 0; k < m_size; ++k)
		{
			if (k != row && isBitON(m_NSPossibleValues[k * m_size + column], value))
			{
				setBitOFF(m_NSPossibleValues[k * m_size + column], value);
				if (!checkIfNakedSingle(k, column))
					return false;
			}
		}

		// Check box
		//Get box index from cell
		//unsigned int boxIndex = m_boxSize * (row / m_boxSize) + (column / m_boxSize);

		//Get box start from box index
		//const int boxRowStart = m_boxSize * (boxIndex / m_boxSize);
		//const int boxColumnStart = m_boxSize * (boxIndex % m_boxSize);

		//Get box start from cell
		//const int boxRowStart = row - row % m_boxSize;
		//const int boxColumnStart = column - column % m_boxSize;
		unsigned int boxIndex = m_cellToBoxIndex[row][column];
		const int boxRowStart = m_boxIndexToBoxStart[boxIndex].first;
		const int boxColumnStart = m_boxIndexToBoxStart[boxIndex].second;

		unsigned int boxRow = boxRowStart;
		for (unsigned int i = 0; i < m_boxSize; ++i, ++boxRow)
		{
			if (boxRow == row)
				continue;
			unsigned int boxColumn = boxColumnStart;
			for (unsigned int j = 0; j < m_boxSize; ++j, ++boxColumn)
				if (boxColumn != column && isBitON(m_NSPossibleValues[boxRow * m_size + boxColumn], value))
				{
					setBitOFF(m_NSPossibleValues[boxRow * m_size + boxColumn], value);
					if (!checkIfNakedSingle(boxRow, boxColumn))
						return false;
				}
		}

		//--m_cluesToFind;

		//return so that the parent function solve() calls it again since the map is still not empty.
		return true;
	}

	bool FastestBruteForceSudokuMatrix_4::processPendingAssignmentsAllAtOnce()
	{
		//If there are more pending assignements, process them all at once
		for (unsigned int i = 0; i < m_size; ++i)
		{
			m_initializationFiltersRow[i].resetAllDigits();
			m_initializationFiltersColumn[i].resetAllDigits();
			m_initializationFiltersBox[i].resetAllDigits();
		}

		//Prepare all masks to be used to update NS and HS data structures
		//unsigned int pendingAssignements = m_pendingAssignmentsQueue.count();
		while(!m_pendingAssignmentsQueue.isEmpty())
		//for (auto iterator = m_pendingAssignmentsHashMap.begin(); iterator != m_pendingAssignmentsHashMap.end(); ++iterator)
		{
			unsigned int row = m_pendingAssignmentsQueue.top().row;
			unsigned int column = m_pendingAssignmentsQueue.top().column;
			unsigned int value = m_pendingAssignmentsQueue.top().value;
			m_pendingAssignmentsQueue.pop();

			//unsigned int row = iterator->first.first;
			//unsigned int column = iterator->first.second;
			//unsigned int value = iterator->second;

			if (!isBitON(m_NSPossibleValues[row * m_size + column], value))
				return false;

			//If m_pendingAssignmentsQueue already had this entry duplicated before, ignore it and continue
			unsigned int boxIndex = m_cellToBoxIndex[row][column];
			//if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) == 1
			//	&& isBitON(m_initializationFiltersRow[row], value)
			//	&& isBitON(m_initializationFiltersColumn[column], value)
			//	&& isBitON(m_initializationFiltersBox[boxIndex], value)
			//	)
			//{
			//	//--pendingAssignements;
			//	continue;
			//}

			//Update NS data structure: Set all other bits OFF as this cell is solved
			//copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[row * m_size + column]);

			//Prepare all masks to be used to update NS data structure
			//If the bit is already ON, there is already a value in pending assignments in this row, column or box
			if (isBitON(m_initializationFiltersRow[row], value))
				return false;
			setBitON(m_initializationFiltersRow[row], value);

			if (isBitON(m_initializationFiltersColumn[column], value))
				return false;
			setBitON(m_initializationFiltersColumn[column], value);

			if (isBitON(m_initializationFiltersBox[boxIndex], value))
				return false;
			setBitON(m_initializationFiltersBox[boxIndex], value);
		}

		//static map<std::pair<unsigned int, unsigned int>, unsigned int> tempHashMap;
		//tempHashMap.clear();
		//tempHashMap.swap(m_pendingAssignmentsHashMap);

		//Apply all mask to NS data structure
		for (unsigned int row = 0; row < m_size; ++row)
		{
			for (unsigned int column = 0; column < m_size; ++column)
			{
				//auto iterator = tempHashMap.find(std::pair<unsigned int, unsigned int>(row, column));
				//if (iterator != tempHashMap.end())
				//	continue;

				unsigned int boxIndex = m_cellToBoxIndex[row][column];
				if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) == 1
					&& isBitON(m_initializationFiltersRow[row], m_NSPossibleValues[row * m_size + column])
					&& isBitON(m_initializationFiltersColumn[column], m_NSPossibleValues[row * m_size + column])
					&& isBitON(m_initializationFiltersBox[boxIndex], m_NSPossibleValues[row * m_size + column])
					)
					continue;

				// TODO: Simplify following statement. Avoid temporary objects: --DONE
				//copyValue((m_NSPossibleValues[row * m_size + column] & (m_initializationFiltersRow[row] | m_initializationFiltersColumn[column] | m_initializationFiltersBox[boxIndex])),
				//	m_bitsON);
				copyValue(m_initializationFiltersRow[row], m_bitsON);
				m_bitsON |= m_initializationFiltersColumn[column];
				m_bitsON |= m_initializationFiltersBox[boxIndex];

				m_bitsON &= m_NSPossibleValues[row * m_size + column];
				if (!m_bitsON.isZero())
				{
					setBitOFF(m_NSPossibleValues[row * m_size + column], m_bitsON);
					if (!checkIfNakedSingle(row, column))
						return false;
				}
			}
		}

		//m_cluesToFind -= tempHashMap.size();
		//m_cluesToFind -= pendingAssignements;

		return true;
	}

	bool FastestBruteForceSudokuMatrix_4::checkIfNakedSingle(const unsigned int& row, const unsigned int& column)
	{
		if (m_NSPossibleValues[row * m_size + column].isZero())
			return false;
		if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) == 1)
		{
			++m_stats.m_nakedSinglesFound;
			--m_cluesToFind;
			unsigned int currentValue = getOnBitPosition(m_NSPossibleValues[row * m_size + column]);
			m_pendingAssignmentsQueue.push(row, column, currentValue);
			//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
			//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = currentValue;
			//else
			//{
			//	unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
			//	if(oldVal != currentValue)
			//		return false;
			//}
		}

		return true;
	}
	
	bool FastestBruteForceSudokuMatrix_4::searchForHiddenSingles()
	{
		unsigned int value = 0;

		//Check rows
		for (unsigned int row = 0; row < m_size; ++row)
		{
			onceOrMoreThanOnce.resetAllDigits();
			moreThanOnce.resetAllDigits();

			for (unsigned int column = 0; column < m_size; ++column)
			{
				copyValue(m_NSPossibleValues[row * m_size + column], tempCurrentNSMask);
				tempCurrentNSMask &= onceOrMoreThanOnce;
				moreThanOnce |= tempCurrentNSMask;
				onceOrMoreThanOnce |= m_NSPossibleValues[row * m_size + column];
			}

			// onceOrMoreThanOnce should have all bits ON
			if (onceOrMoreThanOnce != globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])
				return false;
			//copyValue(onceOrMoreThanOnce, onlyOnce); // To avoid this copying, onlyOnce is made reference to onceOrMoreThanOnce
			setBitOFF(onceOrMoreThanOnce, moreThanOnce);
			if (onceOrMoreThanOnce.isZero())
				continue;

			for (unsigned int column = 0; column < m_size; ++column)
			{
				if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) == 1)
					continue;

				copyValue(m_NSPossibleValues[row * m_size + column], tempCurrentNSMask);
				tempCurrentNSMask &= onceOrMoreThanOnce;

				if (!tempCurrentNSMask.isZero())
				{
					//MyAssert::myRunTimeAssert(getCountOfONBits(tempCurrentNSMask) == 1);
					if (getCountOfONBits(tempCurrentNSMask) != 1)
						return false;

					value = getOnBitPosition(tempCurrentNSMask);
					++m_stats.m_hiddenSinglesFound;
					copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[row * m_size + column]);
					--m_cluesToFind;
					m_pendingAssignmentsQueue.push(row, column, value);
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	value = getOnBitPosition(tempCurrentNSMask);
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = value;
					//}
					//else
					//{
					//	value = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
					//	if (value != getOnBitPosition(tempCurrentNSMask))
					//		return false;
					//}
				}
			}
		}

		//Check columns
		for (unsigned int column = 0; column < m_size; ++column)
		{
			onceOrMoreThanOnce.resetAllDigits();
			moreThanOnce.resetAllDigits();

			for (unsigned int row = 0; row < m_size; ++row)
			{
				copyValue(m_NSPossibleValues[row * m_size + column], tempCurrentNSMask);
				tempCurrentNSMask &= onceOrMoreThanOnce;
				moreThanOnce |= tempCurrentNSMask;
				onceOrMoreThanOnce |= m_NSPossibleValues[row * m_size + column];
			}

			// onceOrMoreThanOnce should have all bits ON
			if (onceOrMoreThanOnce != globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])
				return false;

			setBitOFF(onceOrMoreThanOnce, moreThanOnce);
			if (onceOrMoreThanOnce.isZero())
				continue;

			for (unsigned int row = 0; row < m_size; ++row)
			{
				if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) == 1)
					continue;

				copyValue(m_NSPossibleValues[row * m_size + column], tempCurrentNSMask);
				tempCurrentNSMask &= onceOrMoreThanOnce;

				if (!tempCurrentNSMask.isZero())
				{
					//MyAssert::myRunTimeAssert(getCountOfONBits(tempCurrentNSMask) == 1);
					if (getCountOfONBits(tempCurrentNSMask) != 1)
						return false;

					value = getOnBitPosition(tempCurrentNSMask);
					copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[row * m_size + column]);
					--m_cluesToFind;
					m_pendingAssignmentsQueue.push(row, column, value);
					++m_stats.m_hiddenSinglesFound;
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	value = getOnBitPosition(tempCurrentNSMask);
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = value;
					//}
					//else
					//{
					//	value = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
					//	if (value != getOnBitPosition(tempCurrentNSMask))
					//		return false;
					//}
				}
			}
		}

		//Check boxes
		for (unsigned int boxIndex = 0; boxIndex < m_size; ++boxIndex)
		{
			onceOrMoreThanOnce.resetAllDigits();
			moreThanOnce.resetAllDigits();

			unsigned int rowStart = m_boxIndexToBoxStart[boxIndex].first;
			unsigned int columnStart = m_boxIndexToBoxStart[boxIndex].second;
			for (unsigned int ri = 0, row = rowStart; ri < m_boxSize; ++ri, ++row)
			{
				for (unsigned int ci = 0, column = columnStart; ci < m_boxSize; ++ci, ++column)
				{
					copyValue(m_NSPossibleValues[row * m_size + column], tempCurrentNSMask);
					tempCurrentNSMask &= onceOrMoreThanOnce;
					moreThanOnce |= tempCurrentNSMask;
					onceOrMoreThanOnce |= m_NSPossibleValues[row * m_size + column];
				}
			}

			// onceOrMoreThanOnce should have all bits ON
			if (onceOrMoreThanOnce != globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])
				return false;

			setBitOFF(onceOrMoreThanOnce, moreThanOnce);
			if (onceOrMoreThanOnce.isZero())
				continue;

			for (unsigned int ri = 0, row = rowStart; ri < m_boxSize; ++ri, ++row)
			{
				for (unsigned int ci = 0, column = columnStart; ci < m_boxSize; ++ci, ++column)
				{
					if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) == 1)
						continue;

					copyValue(m_NSPossibleValues[row * m_size + column], tempCurrentNSMask);
					tempCurrentNSMask &= onceOrMoreThanOnce;

					if (!tempCurrentNSMask.isZero())
					{
						//MyAssert::myRunTimeAssert(getCountOfONBits(tempCurrentNSMask) == 1);
						if (getCountOfONBits(tempCurrentNSMask) != 1)
							return false;

						value = getOnBitPosition(tempCurrentNSMask);
						m_pendingAssignmentsQueue.push(row, column, value);
						copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[row * m_size + column]);
						--m_cluesToFind;
						++m_stats.m_hiddenSinglesFound;
						//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
						//{
						//	++m_stats.m_hiddenSinglesFound;
						//	value = getOnBitPosition(tempCurrentNSMask);
						//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = value;
						//}
						//else
						//{
						//	value = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
						//	if (value != getOnBitPosition(tempCurrentNSMask))
						//		return false;
						//}
					}
				}
			}
		}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_4::searchForHiddenSingles2()
	{
		unsigned int count = 0, targetRow = 0, targetColumn = 0, oldValue = 0;

		//Check rows
		for (unsigned int row = 0; row < m_size; ++row)
		{
			for (unsigned int value = 1; value <= m_size; ++value)
			{
				count = 0;
				targetColumn = 0;
				for (unsigned int column = 0; column < m_size && count < 2; ++column)
				{
					if (isBitON(m_NSPossibleValues[row * m_size + column], value))
					{
						++count;
						targetColumn = column;
					}
				}

				if (count == 0)
					return false;
				if (count == 1 && getCountOfONBits(m_NSPossibleValues[row * m_size + targetColumn]) != 1)
				{
					m_pendingAssignmentsQueue.push(row, targetColumn, value);
					++m_stats.m_hiddenSinglesFound;
					copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[row * m_size + targetColumn]);
					--m_cluesToFind;
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, targetColumn)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, targetColumn)] = value;
					//}
					//else
					//{
					//	oldValue = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, targetColumn)];
					//	if (value != oldValue)
					//		return false;
					//}
				}
			}

		}

		//Check columns
		for (unsigned int column = 0; column < m_size; ++column)
		{
			for (unsigned int value = 1; value <= m_size; ++value)
			{
				count = 0;
				targetRow = 0;
				for (unsigned int row = 0; row < m_size && count < 2; ++row)
				{
					if (isBitON(m_NSPossibleValues[row * m_size + column], value))
					{
						++count;
						targetRow = row;
					}
				}

				if (count == 0)
					return false;
				if (count == 1 && getCountOfONBits(m_NSPossibleValues[targetRow * m_size + column]) != 1)
				{
					m_pendingAssignmentsQueue.push(targetRow, column, value);
					++m_stats.m_hiddenSinglesFound;
					copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[targetRow * m_size + column]);
					--m_cluesToFind;
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(targetRow, column)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(targetRow, column)] = value;
					//}
					//else
					//{
					//	oldValue = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(targetRow, column)];
					//	if (value != oldValue)
					//		return false;
					//}
				}
			}

		}

		//Check boxes
		for (unsigned int boxIndex = 0; boxIndex < m_size; ++boxIndex)
		{
			for (unsigned int value = 1; value <= m_size; ++value)
			{
				count = 0;
				targetRow = 0;
				targetColumn = 0;

				unsigned int rowStart = m_boxIndexToBoxStart[boxIndex].first;
				unsigned int columnStart = m_boxIndexToBoxStart[boxIndex].second;
				for (unsigned int ri = 0, row = rowStart; ri < m_boxSize && count < 2; ++ri, ++row)
				{
					for (unsigned int ci = 0, column = columnStart; ci < m_boxSize && count < 2; ++ci, ++column)
					{
						if (isBitON(m_NSPossibleValues[row * m_size + column], value))
						{
							++count;
							targetRow = row;
							targetColumn = column;
						}
					}
				}

				if (count == 0)
					return false;
				if (count == 1 && getCountOfONBits(m_NSPossibleValues[targetRow * m_size + targetColumn]) != 1)
				{
					m_pendingAssignmentsQueue.push(targetRow, targetColumn, value);
					++m_stats.m_hiddenSinglesFound;
					copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[targetRow * m_size + targetColumn]);
					--m_cluesToFind;
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(targetRow, targetColumn)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(targetRow, targetColumn)] = value;
					//}
					//else
					//{
					//	oldValue = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(targetRow, targetColumn)];
					//	if (value != oldValue)
					//		return false;
					//}
				}
			}
		}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_4::searchForLockedCandidate()
	{
		static BigIntegerBitCollection temp1(BigIntegerBitCollection::getNumDigits(m_size), 0);
		static BigIntegerBitCollection temp2(BigIntegerBitCollection::getNumDigits(m_size), 0);

		//Clear locked candidates
		//Consider horizontal strips
		for (unsigned int stripRowIndex = 0; stripRowIndex < m_size; ++stripRowIndex)
		{
			for (unsigned int stripColumnIndex = 0; stripColumnIndex < m_boxSize; ++stripColumnIndex)
			{
				m_lockedCandidatesHorizontalStrips[stripRowIndex][stripColumnIndex].resetAllDigits();
				for (unsigned int column = stripColumnIndex * m_boxSize, i = 0; i < m_boxSize; ++column, ++i)
				{
					m_lockedCandidatesHorizontalStrips[stripRowIndex][stripColumnIndex] |=
						m_NSPossibleValues[stripRowIndex * m_size + column];
				}
			}
		}

		for (unsigned int stripRowIndex = 0; stripRowIndex < m_size; ++stripRowIndex)
		{
			for (unsigned int stripColumnIndex = 0; stripColumnIndex < m_boxSize; ++stripColumnIndex)
			{				
				temp1.resetAllDigits();
				for (unsigned int iRow = stripRowIndex - stripRowIndex % m_boxSize, i = 0; i < m_boxSize; ++iRow, ++i)
				{
					if (iRow == stripRowIndex)
						continue;
					temp1 |= m_lockedCandidatesHorizontalStrips[iRow][stripColumnIndex];
				}
				
				temp2.resetAllDigits();
				for (unsigned int iColumn = 0; iColumn < m_boxSize; ++iColumn)
				{
					if (iColumn == stripColumnIndex)
						continue;
					temp2 |= m_lockedCandidatesHorizontalStrips[stripRowIndex][iColumn];
				}
				temp1 ^= temp2;
				if (!temp1.isZero()) // Found locked candidate
				{
					m_stats.m_lockedCandidatesFound += getCountOfONBits(temp1);
					for (unsigned int iRow = stripRowIndex - stripRowIndex % m_boxSize, i = 0; i < m_boxSize; ++iRow, ++i)
					{
						if (iRow == stripRowIndex)
							continue;
						for (unsigned int column = stripColumnIndex * m_boxSize, i = 0; i < m_boxSize; ++column, ++i)
						{
							if (getCountOfONBits(m_NSPossibleValues[iRow * m_size + column]) == 1)
							{
								setBitOFF(m_NSPossibleValues[iRow * m_size + column], temp1);
								if (m_NSPossibleValues[iRow * m_size + column].isZero())
									return false;
							}
							else
							{
								setBitOFF(m_NSPossibleValues[iRow * m_size + column], temp1);
								if (!checkIfNakedSingle(iRow, column))
									return false;
							}
						}
					}
					for (unsigned int column = 0; column < m_size; ++column)
					{
						if (stripColumnIndex == (column / m_boxSize))
							continue;

						if (getCountOfONBits(m_NSPossibleValues[stripRowIndex * m_size + column]) == 1)
						{
							setBitOFF(m_NSPossibleValues[stripRowIndex * m_size + column], temp1);
							if (m_NSPossibleValues[stripRowIndex * m_size + column].isZero())
								return false;
						}
						else
						{
							setBitOFF(m_NSPossibleValues[stripRowIndex * m_size + column], temp1);
							if (!checkIfNakedSingle(stripRowIndex, column))
								return false;
						}
					}
				}
			}
		}

		//Consider vertical strips
		for (unsigned int stripRowIndex = 0; stripRowIndex < m_boxSize; ++stripRowIndex)
		{
			for (unsigned int stripColumnIndex = 0; stripColumnIndex < m_size; ++stripColumnIndex)
			{
				m_lockedCandidatesVerticalStrips[stripRowIndex][stripColumnIndex].resetAllDigits();
				for (unsigned int row = stripRowIndex * m_boxSize, i = 0; i < m_boxSize; ++row, ++i)
				{
					m_lockedCandidatesVerticalStrips[stripRowIndex][stripColumnIndex] |=
						m_NSPossibleValues[row * m_size + stripColumnIndex];
				}
			}
		}

		for (unsigned int stripRowIndex = 0; stripRowIndex < m_boxSize; ++stripRowIndex)
		{
			for (unsigned int stripColumnIndex = 0; stripColumnIndex < m_size; ++stripColumnIndex)
			{
				temp1.resetAllDigits();
				for (unsigned int iColumn = stripColumnIndex - stripColumnIndex % m_boxSize, i = 0; i < m_boxSize; ++iColumn, ++i)
				{
					if (iColumn == stripColumnIndex)
						continue;
					temp1 |= m_lockedCandidatesVerticalStrips[stripRowIndex][iColumn];
				}

				temp2.resetAllDigits();
				for (unsigned int iRow = 0; iRow < m_boxSize; ++iRow)
				{
					if (iRow == stripRowIndex)
						continue;
					temp2 |= m_lockedCandidatesVerticalStrips[iRow][stripColumnIndex];
				}
				temp1 ^= temp2;
				if (!temp1.isZero()) // Found locked candidate
				{
					m_stats.m_lockedCandidatesFound += getCountOfONBits(temp1);
					for (unsigned int iColumn = stripColumnIndex - stripColumnIndex % m_boxSize, i = 0; i < m_boxSize; ++iColumn, ++i)
					{
						if (iColumn == stripColumnIndex)
							continue;
						for (unsigned int row = stripRowIndex * m_boxSize, i = 0; i < m_boxSize; ++row, ++i)
						{
							if (getCountOfONBits(m_NSPossibleValues[row * m_size + iColumn]) == 1)
							{
								setBitOFF(m_NSPossibleValues[row * m_size + iColumn], temp1);
								if (m_NSPossibleValues[row * m_size + iColumn].isZero())
									return false;
							}
							else
							{
								setBitOFF(m_NSPossibleValues[row * m_size + iColumn], temp1);
								if (!checkIfNakedSingle(row, iColumn))
									return false;
							}
						}
					}
					for (unsigned int row = 0; row < m_size; ++row)
					{
						if (stripRowIndex == (row / m_boxSize))
							continue;

						if (getCountOfONBits(m_NSPossibleValues[row * m_size + stripColumnIndex]) == 1)
						{
							setBitOFF(m_NSPossibleValues[row * m_size + stripColumnIndex], temp1);
							if (m_NSPossibleValues[row * m_size + stripColumnIndex].isZero())
								return false;
						}
						else
						{
							setBitOFF(m_NSPossibleValues[row * m_size + stripColumnIndex], temp1);
							if (!checkIfNakedSingle(row, stripColumnIndex))
								return false;
						}
					}
				}
			}
		}
		return true;
	}

	bool FastestBruteForceSudokuMatrix_4::searchForNakedPair()
	{
		bool nakedPairExist = false;
		static BigIntegerBitCollection buffer(BigIntegerBitCollection::getNumDigits(m_size), 0);

		//Check rows
		for (unsigned int row = 0; row < m_size; ++row)
		{
			buffer.resetAllDigits();
			for (unsigned int column = 0; column < m_size; ++column)
			{
				if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) != 2)
					continue;
				
				if (buffer.isZero())
					copyValue(m_NSPossibleValues[row * m_size + column], buffer);
				else if (buffer == m_NSPossibleValues[row * m_size + column])
					nakedPairExist = true;
				else if (isBitSubset(buffer, m_NSPossibleValues[row * m_size + column]))
				{
					//There has been possibility that naked pair of m_NSPossibleValues[row * m_size + column] before, confirm it before procceeding
					for (unsigned int column2 = 0; column2 < column && !nakedPairExist; ++column2)
						if (m_NSPossibleValues[row * m_size + column2] == m_NSPossibleValues[row * m_size + column])
							nakedPairExist = true;
				}

				if (nakedPairExist)
				{
					//Naked Pair found
					++m_stats.m_nakedPairsFound;
					for (unsigned int column2 = 0; column2 < m_size; ++column2)
						if (m_NSPossibleValues[row * m_size + column2] != m_NSPossibleValues[row * m_size + column])
						{
							if (isBitON(m_NSPossibleValues[row * m_size + column2], m_NSPossibleValues[row * m_size + column]))
							{
								setBitOFF(m_NSPossibleValues[row * m_size + column2], m_NSPossibleValues[row * m_size + column]);
								if (!checkIfNakedSingle(row, column2))
									return false;
							}
						}

					setBitOFF(buffer, m_NSPossibleValues[row * m_size + column]);
					nakedPairExist = false;
				}
				else
					buffer |= m_NSPossibleValues[row * m_size + column];
			}
		}

		//Check columns
		for (unsigned int column = 0; column < m_size; ++column)
		{
			buffer.resetAllDigits();
			for (unsigned int row = 0; row < m_size; ++row)
			{
				if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) != 2)
					continue;

				if (buffer.isZero())
					copyValue(m_NSPossibleValues[row * m_size + column], buffer);
				else if (buffer == m_NSPossibleValues[row * m_size + column])
					nakedPairExist = true;
				else if (isBitSubset(buffer, m_NSPossibleValues[row * m_size + column]))
				{
					//There has been possibility that naked pair of m_NSPossibleValues[row * m_size + column] before, confirm it before procceeding
					for (unsigned int row2 = 0; row2 < row; ++row2)
						if (m_NSPossibleValues[row2 * m_size + column] == m_NSPossibleValues[row * m_size + column])
							nakedPairExist = true;
				}

				if (nakedPairExist)
				{
					//Naked Pair found
					++m_stats.m_nakedPairsFound;
					for (unsigned int row2 = 0; row2 < m_size; ++row2)
						if (m_NSPossibleValues[row2 * m_size + column] != m_NSPossibleValues[row * m_size + column])
						{
							if (isBitON(m_NSPossibleValues[row2 * m_size + column], m_NSPossibleValues[row * m_size + column]))
							{
								setBitOFF(m_NSPossibleValues[row2 * m_size + column], m_NSPossibleValues[row * m_size + column]);
								if (!checkIfNakedSingle(row2, column))
									return false;
							}
						}

					setBitOFF(buffer, m_NSPossibleValues[row * m_size + column]);
					nakedPairExist = false;
				}
				else
					buffer |= m_NSPossibleValues[row * m_size + column];
			}
		}

		//Check boxes
		for (unsigned int boxIndex = 0; boxIndex < m_size; ++boxIndex)
		{
			buffer.resetAllDigits();
			unsigned int rowStart = m_boxIndexToBoxStart[boxIndex].first;
			unsigned int columnStart = m_boxIndexToBoxStart[boxIndex].second;
			for (unsigned int ri = 0, row = rowStart; ri < m_boxSize; ++ri, ++row)
			{
				for (unsigned int ci = 0, column = columnStart; ci < m_boxSize; ++ci, ++column)
				{
					if (getCountOfONBits(m_NSPossibleValues[row * m_size + column]) != 2)
						continue;
					
					if (buffer.isZero())
						copyValue(m_NSPossibleValues[row * m_size + column], buffer);
					else if (buffer == m_NSPossibleValues[row * m_size + column])
						nakedPairExist = true;
					else if (isBitSubset(buffer, m_NSPossibleValues[row * m_size + column]))
					{
						//There has been possibility that naked pair of m_NSPossibleValues[row * m_size + column] before, confirm it before procceeding
						for (unsigned int ri2 = 0, row2 = rowStart; ri2 < ri && !nakedPairExist; ++ri2, ++row2)
							for (unsigned int ci2 = 0, column2 = columnStart; ci2 < ci && !nakedPairExist; ++ci2, ++column2)
								if (m_NSPossibleValues[row2 * m_size + column2] == m_NSPossibleValues[row * m_size + column])
									nakedPairExist = true;
					}

					if (nakedPairExist)
					{
						//Naked Pair found
						++m_stats.m_nakedPairsFound;
						for (unsigned int ri2 = 0, row2 = rowStart; ri2 < m_boxSize; ++ri2, ++row2)
							for (unsigned int ci2 = 0, column2 = columnStart; ci2 < m_boxSize; ++ci2, ++column2)
								if (m_NSPossibleValues[row2 * m_size + column2] != m_NSPossibleValues[row * m_size + column])
								{
									if (isBitON(m_NSPossibleValues[row2 * m_size + column2], m_NSPossibleValues[row * m_size + column]))
									{
										setBitOFF(m_NSPossibleValues[row2 * m_size + column2], m_NSPossibleValues[row * m_size + column]);
										if (!checkIfNakedSingle(row2, column2))
											return false;
									}
								}

						setBitOFF(buffer, m_NSPossibleValues[row * m_size + column]);
						nakedPairExist = false;
					}
					else
						buffer |= m_NSPossibleValues[row * m_size + column];
				}
			}
		}
		return true;
	}

	bool FastestBruteForceSudokuMatrix_4::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions)
	{
		while (m_cluesToFind > 0)
		{
			if (!processPendingAssignments())
				return false;			
			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;
			//if (!m_pendingAssignmentsHashMap.empty())
			//	continue;

			if (m_cluesToFind > 0 && !searchForHiddenSingles())
				return false;		
			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;
			//if (!m_pendingAssignmentsHashMap.empty())
			//	continue;

			if (m_cluesToFind > 0 && !searchForLockedCandidate())
				return false;			
			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;
			//if (!m_pendingAssignmentsHashMap.empty())
			//	continue;

			if (m_cluesToFind > 0 && !searchForNakedPair())
				return false;
			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;
			//if (!m_pendingAssignmentsHashMap.empty())
			//	continue;

			break;
		}
		
		//bool solved = true;
		if (m_cluesToFind == 0 || m_stats.m_valuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
		{
			m_pendingAssignmentsQueue.makeEmpty();
			vector< vector<int> > curentSolution(m_size, vector<int>(m_size, 0));
			for (unsigned int i = 0; i < m_size /*&& solved*/; ++i)
				for (unsigned int j = 0; j < m_size /*&& solved*/; ++j)
				{
					//unsigned int count = getCountOfONBits(m_NSPossibleValues[i * m_size + j]);
					//if (count == 1)
						curentSolution[i][j] = getOnBitPosition(m_NSPossibleValues[i * m_size + j]);
					//else
					//	solved = false;
				}

			//if (solved)
			//{
				solutionSetsOut.push_back(std::move(curentSolution));
				return true; // Found the solution
			//}
		}

		//Make a guess
		unsigned int minPossibleCount = m_size + 1;
		unsigned int row = minPossibleCount;
		unsigned int column = minPossibleCount;

		//TODO: Instead of finding min every time, keep min and second_min ini memory and whenever the count is updated, update these values
		for (unsigned int i = 0; i < m_size && minPossibleCount != 2; ++i)
		{
			for (unsigned int j = 0; j < m_size && minPossibleCount != 2; ++j)
			{
				unsigned int count = getCountOfONBits(m_NSPossibleValues[i * m_size + j]);
				if (count > 1u && count < minPossibleCount)
				{
					row = i;
					column = j;
					minPossibleCount = count;
				}
			}
		}

		// VIMP TODO: Remove this assert failure: --DONE
		//MyAssert::myRunTimeAssert(minPossibleCount < (m_size + 1));
		if (minPossibleCount == (m_size + 1))
		{
			vector< vector<int> > curentSolution(m_size, vector<int>(m_size, 0));
			for (unsigned int i = 0; i < m_size; ++i)
				for (unsigned int j = 0; j < m_size; ++j)
					curentSolution[i][j] = m_NSPossibleValues[i * m_size + j].getOnBitPosition();

			solutionSetsOut.push_back(std::move(curentSolution));
			return true;
		}

		++m_stats.m_cellsTriedOrLinkUpdates;

		for (unsigned int i = 1; i <= m_size; ++i)
		{
			if (!isBitON(m_NSPossibleValues[row * m_size + column], i))
				continue;

			++m_stats.m_valuesTried;

			MyAssert::myRunTimeAssert(m_pendingAssignmentsQueue.count() == 0);
			m_pendingAssignmentsQueue.push(row, column, i);
			//MyAssert::myRunTimeAssert(m_pendingAssignmentsHashMap.size() == 0);
			//m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = i;

			FastestBruteForceSudokuMatrix_4 s(*this);
			copyValue(globalLookupTables.bitMask[i], s.m_NSPossibleValues[row * m_size + column]);
			--s.m_cluesToFind;
			//copyValue(globalLookupTables.bitMask[i], s.m_NSPossibleValues[row * m_size + column]);
			// If current guess is wrong, prepare for next guess
			if (!s.solve(solutionSetsOut, numSolutions))
			{
				m_pendingAssignmentsQueue.makeEmpty();
				//m_pendingAssignmentsHashMap.clear();
				
				++m_stats.m_wrongGuesses;
				continue;
			}

			//If we are here, all cells are successfully filled in and we found a right solution

			//Check if we found required number of solutions
			if(solutionSetsOut.size() == numSolutions)
				return true;			
		}

		return false;
	}
	

} // namespace mm
