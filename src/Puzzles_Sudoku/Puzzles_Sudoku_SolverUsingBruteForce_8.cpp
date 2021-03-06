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
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_8.h"
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
	  9		9 * 9 / (3 * 9 - 4 - 2 * 3) = 81 / (27 - 4 - 6) = 81 / 17 = 4
	  16	16 * 16 / (3 * 16 - 4 - 2 * 4) = 256 / (48 - 4 - 8) = 256 / 36 = 7
- Find hidden single
  (total row, column and box = 3 * n
   total 3n * (2*n + 1) BigInt bitwise operations ~ 6*n*n)
	- Loop through all cells in each row, column and box, 2 BigInt & and | operations to find atLeatOnes and moreThanOnce masks for each cell
	  (for each row, column and box = 2 * n BigInt bitwise operations)
	- onlyOnes = atLeatOnes & ~moreThanOnce to find hidden singles
- Find locked candidate



*/

namespace mm {

	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_2::m_initializationFiltersRow(MAX_DIM, BigIntegerBitCollection(0));
	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_2::m_initializationFiltersColumn(MAX_DIM, BigIntegerBitCollection(0));
	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_2::m_initializationFiltersBox(MAX_DIM, BigIntegerBitCollection(0));
	//vector< vector<int> > FastestBruteForceSudokuMatrix_2::m_solution(MAX_DIM, vector<int>(MAX_DIM, 0));

	//vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_2::m_initializationFiltersRowHS(MAX_DIM, BigIntegerBitCollection(0));
	//vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_2::m_initializationFiltersColumnHS(MAX_DIM, BigIntegerBitCollection(0));
	//vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_2::m_initializationFiltersBoxHS(MAX_DIM, BigIntegerBitCollection(0));

	BigIntegerBitCollection FastestBruteForceSudokuMatrix_2::m_bitsON(0);
	BigIntegerBitCollection FastestBruteForceSudokuMatrix_2::m_valuesToEliminate(0);

	vector< vector<BigIntegerBitCollection> > FastestBruteForceSudokuMatrix_2::m_lockedCandidatesHorizontalStrips(MAX_DIM, vector<BigIntegerBitCollection>(MAX_BOX_DIM, BigIntegerBitCollection(0)));
	vector< vector<BigIntegerBitCollection> > FastestBruteForceSudokuMatrix_2::m_lockedCandidatesVerticalStrips(MAX_BOX_DIM, vector<BigIntegerBitCollection>(MAX_DIM, BigIntegerBitCollection(0)));

	//PendingAssignmentsCircularQueue FastestBruteForceSudokuMatrix_2::m_pendingAssignmentsQueue(MAX_DIM * MAX_DIM);
	map<std::pair<unsigned int, unsigned int>, unsigned int> FastestBruteForceSudokuMatrix_2::m_pendingAssignmentsHashMap;

	bool FastestBruteForceSudokuMatrix_2::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		FastestBruteForceSudokuMatrix_2 obj(dataIn, stats);
		if (obj.solve(solutionSetsOut, numSolutions))
			return true;
		else
			return false;
	}

	const unsigned int FastestBruteForceSudokuMatrix_2::getCountOfONBits(const BigIntegerBitCollection& bitSet)
	{
		
		return getCountOfONBits_v4(bitSet);
	}

	const unsigned int FastestBruteForceSudokuMatrix_2::getCountOfONBits_v1(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int value = 1; value <= m_size; ++value)
			if (isBitON(bitSet, value))
				++count;

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_2::getCountOfONBits_v2(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < bitSet.getDigits().size(); ++i)
			count += (globalLookupTables.countONbits8[bitSet.getDigits()[i] & 0b11111111]
				+ globalLookupTables.countONbits8[(bitSet.getDigits()[i] >> 8) & 0b11111111]
				+ globalLookupTables.countONbits8[(bitSet.getDigits()[i] >> 16) & 0b11111111]
				+ globalLookupTables.countONbits8[(bitSet.getDigits()[i] >> 24) & 0b11111111]);

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_2::getCountOfONBits_v3(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < bitSet.getDigits().size(); ++i)
			count += (globalLookupTables.countONbits16[bitSet.getDigits()[i] & 0b1111111111111111]
				+ globalLookupTables.countONbits16[(bitSet.getDigits()[i] >> 16) & 0b1111111111111111]);

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_2::getCountOfONBits_v4(const BigIntegerBitCollection& bitSet)
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

	const unsigned int FastestBruteForceSudokuMatrix_2::getOnBitPosition(const BigIntegerBitCollection& bitSet)
	{
		return getOnBitPosition_v2(bitSet);
	}

	const unsigned int FastestBruteForceSudokuMatrix_2::getOnBitPosition_v1(const BigIntegerBitCollection& bitSet)
	{
		//Ideally there should be only one bit ON
		//MyAssert::myRunTimeAssert((m_isPossible & (m_isPossible - 1)) == 0);
		//MyAssert::myRunTimeAssert(m_isPossible.isOnlyOneBitON());
		//if (m_isPossible.isOnlyOneBitON())
		return bitSet.getOnBitPosition();
		//else
		//	return 0;
	}

	const unsigned int FastestBruteForceSudokuMatrix_2::getOnBitPosition_v2(const BigIntegerBitCollection& bitSet)
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

	// Index range [1, infinity)
	bool FastestBruteForceSudokuMatrix_2::isBitON(const BigIntegerBitCollection& bitSet, const unsigned int& index) const
	{
		//return m_isPossible.isnthBitON(index);
		return bitSet.getDigits()[globalLookupTables.bitPositionToDigitIndexHash[index]] & globalLookupTables.bitMask32[BigIntegerBitCollection::getBitPositionInCurrentDigit(index)];
	}

	bool FastestBruteForceSudokuMatrix_2::isBitON(const BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask) const
	{
		for (unsigned int i = 0; i < mask.getDigits().size(); ++i)
			if (mask.getDigits()[i] != 0 && (bitSet.getDigits()[i] & mask.getDigits()[i]) == 0)
				return false;

		return true;
	}

	// Index range [1, infinity)
	void FastestBruteForceSudokuMatrix_2::setBitOFF(BigIntegerBitCollection& bitSet, const unsigned int& index)
	{
		//m_isPossible.setnthBitOFF(index);
		bitSet.getDigits()[ globalLookupTables.bitPositionToDigitIndexHash[index] ] &= globalLookupTables.bitMask32Inv[BigIntegerBitCollection::getBitPositionInCurrentDigit(index)];
	}

	void FastestBruteForceSudokuMatrix_2::setBitOFF(BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask)
	{		
		for (unsigned int i = 0; i < mask.getDigits().size(); ++i)
			bitSet.getDigits()[i] &= ~mask.getDigits()[i];
	}

	// Index range [1, infinity)
	void FastestBruteForceSudokuMatrix_2::setBitON(BigIntegerBitCollection& bitSet, const unsigned int& index)
	{
		//m_isPossible.setnthBitOFF(index);
		bitSet.getDigits()[globalLookupTables.bitPositionToDigitIndexHash[index]] |= globalLookupTables.bitMask32[BigIntegerBitCollection::getBitPositionInCurrentDigit(index)];
	}

	void FastestBruteForceSudokuMatrix_2::setBitON(BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask)
	{
		for (unsigned int i = 0; i < mask.getDigits().size(); ++i)
			bitSet.getDigits()[i] |= mask.getDigits()[i];
	}

	void FastestBruteForceSudokuMatrix_2::copyValue(const BigIntegerBitCollection& from, BigIntegerBitCollection& to)
	{
		//to.getDigits().assign(from.getDigits().begin(), from.getDigits().end());
		unsigned int index = 0;
		for (; index < from.getDigits().size(); ++index)
			to.getDigits()[index] = from.getDigits()[index];

		for (; index < to.getDigits().size(); ++index)
			to.getDigits()[index] = 0;
	}

	FastestBruteForceSudokuMatrix_2::FastestBruteForceSudokuMatrix_2(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
		: m_size(dataIn.size()),
		m_boxSize(sqrt(m_size)),
		m_stats(stats),
		m_cluesToFind(m_size * m_size),
		m_NSPossibleValues(m_size, vector<BigIntegerBitCollection>(m_size, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])),
		//m_NSPossibleValuesCount(m_size, vector<unsigned int>(m_size, m_size)),
		//m_HSPossiblePositionsRow(m_size, vector<BigIntegerBitCollection>(m_size + 1, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])),
		m_HSPossiblePositionsCountRow(m_size, vector<unsigned int>(m_size + 1, m_size)),
		//m_HSPossiblePositionsColumn(m_size, vector<BigIntegerBitCollection>(m_size + 1, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])),
		m_HSPossiblePositionsCountColumn(m_size, vector<unsigned int>(m_size + 1, m_size)),
		//m_HSPossiblePositionsBox(m_size, vector<BigIntegerBitCollection>(m_size + 1, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]]))
		m_HSPossiblePositionsCountBox(m_size, vector<unsigned int>(m_size + 1, m_size))
	{
		unsigned int numDigits = m_NSPossibleValues[0][0].getDigits().size();
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

			for (unsigned int row = 0; row < m_lockedCandidatesHorizontalStrips.size(); ++row)
				for (unsigned int column = 0; column < m_lockedCandidatesHorizontalStrips[row].size(); ++column)
					m_lockedCandidatesHorizontalStrips[row][column].resize(numDigits);
			for (unsigned int row = 0; row < m_lockedCandidatesVerticalStrips.size(); ++row)
				for (unsigned int column = 0; column < m_lockedCandidatesVerticalStrips[row].size(); ++column)
					m_lockedCandidatesVerticalStrips[row][column].resize(numDigits);
		}

		//Reset all mask to be used to update NS data structure
		for (unsigned int i = 0; i < m_size; ++i)
		{
			m_initializationFiltersRow[i].resetAllDigits();
			m_initializationFiltersColumn[i].resetAllDigits();
			m_initializationFiltersBox[i].resetAllDigits();

			//m_initializationFiltersRowHS[i].resetAllDigits();
			//m_initializationFiltersColumnHS[i].resetAllDigits();
			//m_initializationFiltersBoxHS[i].resetAllDigits();
		}

		for (unsigned int row = 0; row < m_size; ++row)
			for (unsigned int column = 0; column < m_size; ++column)
				if (dataIn[row][column] != 0)
					//m_pendingAssignmentsQueue.push(row, column, dataIn[row][column]);
					if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
						m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = dataIn[row][column];
					else
					{
						unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
						MyAssert::myRunTimeAssert(oldVal == dataIn[row][column]);
					}
	}

	bool FastestBruteForceSudokuMatrix_2::processPendingAssignments()
	{
		if (m_pendingAssignmentsHashMap.empty())
			return true;

		//If there are less pending assignements, process them one by one
		if (m_pendingAssignmentsHashMap.size() <= 4)
		{
			//unsigned int row = m_pendingAssignmentsQueue.top().row;
			//unsigned int column = m_pendingAssignmentsQueue.top().column;
			//unsigned int value = m_pendingAssignmentsQueue.top().value;
			//m_pendingAssignmentsQueue.pop();
			auto iterator = m_pendingAssignmentsHashMap.begin();
			unsigned int row = iterator->first.first;
			unsigned int column = iterator->first.second;
			unsigned int value = iterator->second;
			m_pendingAssignmentsHashMap.erase(iterator);

			if (!isBitON(m_NSPossibleValues[row][column], value))
				return false;
			//else if(m_NSPossibleValuesCount[row][column] == 1) // Already assigned
			//	return true;

			//if (m_solution[row][column] != 0) //already assigned
			//	return true;
			//else
			//	m_solution[row][column] = value;

			// Set only m_value'th bit OFF
			setBitOFF(m_NSPossibleValues[row][column], value);
			BigIntegerBitCollection valuesToEliminate(m_NSPossibleValues[row][column]);

			//Update NS Data structure: Set all bits OFF as this cell is solved
			copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[row][column]);
			//m_NSPossibleValuesCount[row][column] = 1;

			--m_cluesToFind;

			//Update HS data structure
			//m_HSPossiblePositionsCountRow[row][value] = 1;
			//m_HSPossiblePositionsCountColumn[column][value] = 1;
			//m_HSPossiblePositionsCountBox[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]][value] = 1;

			for (unsigned int index = 1; index <= m_size; ++index)
				if (isBitON(valuesToEliminate, index))
				{
					if (!updateHSCountersRow(row, index)
						|| !updateHSCountersColumn(column, index)
						|| !updateHSCountersBox(globalLookupTables.cellToBoxIndex[m_boxSize][row][column], index))
					{
						return false;
					}
				}

			//Propagate constraint
			unsigned int boxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][row][column];
			// Check row
			for (unsigned int k = 0; k < m_size; ++k)
			{
				if (k != column && isBitON(m_NSPossibleValues[row][k], value))
				{
					if (!updateNSCounters(row, k, value))
						return false;
					if (!updateHSCountersColumn(k, value))
						return false;
					unsigned int currentBoxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][row][k];
					if (currentBoxIndex != boxIndex && !updateHSCountersBox(currentBoxIndex, value))
						return false;
				}
			}

			// Check column
			for (unsigned int k = 0; k < m_size; ++k)
			{
				if (k != row && isBitON(m_NSPossibleValues[k][column], value))
				{
					if (!updateNSCounters(k, column, value))
						return false;
					if (!updateHSCountersRow(k, value))
						return false;
					unsigned int currentBoxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][k][column];
					if (currentBoxIndex != boxIndex && !updateHSCountersBox(currentBoxIndex, value))
						return false;
				}
			}

			// Check box
			//int a = row - row % 3;
			//int a = (row / 3) * 3;
			//const int boxRowStart = row - row % m_boxSize;
			//const int boxColumnStart = column - column % m_boxSize;
			const int boxRowStart = globalLookupTables.boxIndexToBoxStart[m_boxSize][boxIndex].first;
			const int boxColumnStart = globalLookupTables.boxIndexToBoxStart[m_boxSize][boxIndex].second;

			unsigned int boxRow = boxRowStart;
			for (unsigned int i = 0; i < m_boxSize; ++i, ++boxRow)
			{
				if (boxRow == row)
					continue;
				unsigned int boxColumn = boxColumnStart;
				for (unsigned int j = 0; j < m_boxSize; ++j, ++boxColumn)
					if (boxColumn != column && isBitON(m_NSPossibleValues[boxRow][boxColumn], value))
					{
						if (!updateNSCounters(boxRow, boxColumn, value))
							return false;
						if (!updateHSCountersRow(boxRow, value))
							return false;
						if (!updateHSCountersColumn(boxColumn, value))
							return false;
					}
			}

			if (!processPendingAssignments())
				return false;

			return true;
		}

		//If there are more pending assignements, process them all at once

		for (unsigned int i = 0; i < m_size; ++i)
		{
			m_initializationFiltersRow[i].resetAllDigits();
			m_initializationFiltersColumn[i].resetAllDigits();
			m_initializationFiltersBox[i].resetAllDigits();
		}

		//Prepare all masks to be used to update NS and HS data structures
		//int oldCountClues = m_cluesToFind;
		while(!m_pendingAssignmentsHashMap.empty())
		{
			//unsigned int row = m_pendingAssignmentsQueue.top().row;
			//unsigned int column = m_pendingAssignmentsQueue.top().column;
			//unsigned int value = m_pendingAssignmentsQueue.top().value;
			//m_pendingAssignmentsQueue.pop();

			auto iterator = m_pendingAssignmentsHashMap.begin();
			unsigned int row = iterator->first.first;
			unsigned int column = iterator->first.second;
			unsigned int value = iterator->second;
			m_pendingAssignmentsHashMap.erase(iterator);

			if (!isBitON(m_NSPossibleValues[row][column], value))
				return false;

			--m_cluesToFind;
			//m_solution[row][column] = dataIn[row][column];

			//Prepare all masks to be used to update NS data structure
			setBitON(m_initializationFiltersRow[row], value);
			setBitON(m_initializationFiltersColumn[column], value);
			setBitON(m_initializationFiltersBox[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]], value);

			//Prepare all masks to be used to update HS data structure
			//setBitON(m_initializationFiltersRowHS[row], column + 1);
			//setBitON(m_initializationFiltersColumnHS[column], row + 1);
			//setBitON(m_initializationFiltersBoxHS[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]], 
			//	m_boxSize * (row % m_boxSize) + (column % m_boxSize) + 1);

			//Update NS data structure
			setBitOFF(m_NSPossibleValues[row][column], value);
			BigIntegerBitCollection valuesToEliminate(m_NSPossibleValues[row][column]);
			copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[row][column]);
			//m_NSPossibleValuesCount[row][column] = 1;

			//Update HS data structure
			//copyValue(globalLookupTables.bitMask[column + 1], m_HSPossiblePositionsRow[row][dataIn[row][column]]);
			//copyValue(globalLookupTables.bitMask[row + 1], m_HSPossiblePositionsColumn[column][dataIn[row][column]]);
			//copyValue(globalLookupTables.bitMask[m_boxSize * (row % m_boxSize) + (column % m_boxSize) + 1], m_HSPossiblePositionsBox[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]][dataIn[row][column]]);

			// A value is assigned to a cell. So the possible HS count of all other values is reduced by 1.
			// We will update every cell for all possible values at once later. 
			// Just to avoid conditional statements later, increase the HS count by one now, we will reduce it later along with other cells.
			//m_HSPossiblePositionsCountRow[row][value] = 1;
			//m_HSPossiblePositionsCountColumn[column][value] = 1;
			//m_HSPossiblePositionsCountBox[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]][value] = 1;
			for (unsigned int index = 1; index <= m_size; ++index)
				if (isBitON(valuesToEliminate, index))
				{
					//if (!updateHSCountersRow(row, index)
					//	|| !updateHSCountersColumn(column, index)
					//	|| !updateHSCountersBox(globalLookupTables.cellToBoxIndex[m_boxSize][row][column], index))
					//{
					//	//m_solution[row][column] = 0;
					//	return false;
					//}

					unsigned int boxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][row][column];
					if ((m_HSPossiblePositionsCountRow[row][index] != 1 && !updateHSCountersRow(row, index))
						|| (m_HSPossiblePositionsCountColumn[column][index] != 1 && !updateHSCountersColumn(column, index))
						|| (m_HSPossiblePositionsCountBox[boxIndex][index] != 1 && !updateHSCountersBox(boxIndex, index)))
					{
						return false;
					}
				}
		}
		/*
		for (unsigned int index = 0; index < m_size; ++index)
		{
			unsigned int rowCount = getCountOfONBits(m_initializationFiltersRow[index]);
			unsigned int columnCount = getCountOfONBits(m_initializationFiltersColumn[index]);
			unsigned int boxCount = getCountOfONBits(m_initializationFiltersBox[index]);

			for (unsigned int value = 1; value <= m_size; ++value)
			{
				m_HSPossiblePositionsCountRow[index][value] -= rowCount;
				m_HSPossiblePositionsCountColumn[index][value] -= columnCount;
				m_HSPossiblePositionsCountBox[index][value] -= boxCount;
			}
		}*/

		//Avoid memory allocation by defining static member at start
		//BigIntegerBitCollection bitsON(0);
		//Apply all mask to NS data structure TODO: Combine following 3 loops
		for (unsigned int row = 0; row < m_size; ++row)
			for (unsigned int column = 0; column < m_size; ++column)
			{
				//if (m_NSPossibleValuesCount[row][column] == 1)
				//	continue;

				if (getCountOfONBits(m_NSPossibleValues[row][column]) == 1)
					continue;

				//if (dataIn[row][column] != 0)
				//	continue;

				unsigned int boxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][row][column];
				copyValue((m_NSPossibleValues[row][column] & (m_initializationFiltersRow[row] | m_initializationFiltersColumn[column] | m_initializationFiltersBox[boxIndex])),
					m_bitsON);

				setBitOFF(m_NSPossibleValues[row][column], m_bitsON);
				unsigned int count = getCountOfONBits(m_NSPossibleValues[row][column]);
				//m_NSPossibleValuesCount[row][column] = count;
					
				if (count == 0) // || count == 1 && !assign(row, column, getOnBitPosition(m_NSPossibleValues[row][column])))
				{
					return false;
				}
				else if (count == 1)
				{
					++m_stats.m_nakedSinglesFound;
					//m_pendingAssignmentsQueue.push(row, column, getOnBitPosition(m_NSPossibleValues[row][column]));
					if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
						m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = getOnBitPosition(m_NSPossibleValues[row][column]);
					else
					{
						unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
						if(oldVal != getOnBitPosition(m_NSPossibleValues[row][column]))
							return false;
					}
				}

				// Reduce the possible HS count due to all removed bits from other cells than assigned values
				for (unsigned int value = 1; value <= m_size; ++value)
				{
					if (isBitON(m_bitsON, value))
					{
						unsigned int boxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][row][column];
						if ((m_HSPossiblePositionsCountRow[row][value] != 1 && !updateHSCountersRow(row, value))
							|| (m_HSPossiblePositionsCountColumn[column][value] != 1 && !updateHSCountersColumn(column, value))
							|| (m_HSPossiblePositionsCountBox[boxIndex][value] != 1 && !updateHSCountersBox(boxIndex, value)))
						{
							return false;
						}
					}
				}
			}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_2::updateNSCounters(const unsigned int& row, const unsigned int& column, const unsigned int& removeThisBit)
	{
		setBitOFF(m_NSPossibleValues[row][column], removeThisBit);
		//m_NSPossibleValuesCount[row][column] -= 1;
		if (m_NSPossibleValues[row][column].isZero())
			return false;
		if (getCountOfONBits(m_NSPossibleValues[row][column]) == 1)
		{
			++m_stats.m_nakedSinglesFound;
			int currentValue = m_NSPossibleValues[row][column].getOnBitPosition();
			//if (!assign(row, column, currentValue))
			//	return false;
			//m_pendingAssignmentsQueue.push(row, column, currentValue);
			if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
				m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = currentValue;
			else
			{
				unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
				if(oldVal != currentValue)
					return false;
			}
		}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_2::checkIfNakedSingle(const unsigned int& row, const unsigned int& column)
	{
		if (m_NSPossibleValues[row][column].isZero())
			return false;
		if (getCountOfONBits(m_NSPossibleValues[row][column]) == 1)
		{
			++m_stats.m_nakedSinglesFound;
			int currentValue = m_NSPossibleValues[row][column].getOnBitPosition();
			//if (!assign(row, column, currentValue))
			//	return false;
			//m_pendingAssignmentsQueue.push(row, column, currentValue);
			if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
				m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = currentValue;
			else
			{
				unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
				if(oldVal != currentValue)
					return false;
			}
		}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_2::updateHSCountersRow(const unsigned int& row, const unsigned int& thisBitRemoved)
	{
		//return true;

		m_HSPossiblePositionsCountRow[row][thisBitRemoved] -= 1;
		if (m_HSPossiblePositionsCountRow[row][thisBitRemoved] == 0)
			return false;
		if (m_HSPossiblePositionsCountRow[row][thisBitRemoved] == 1) // TODO: In case HS counter is zero, find the cell efficiently 
		{
			for (unsigned int i = 0; i < m_size; ++i)
				if (isBitON(m_NSPossibleValues[row][i], thisBitRemoved) && (getCountOfONBits(m_NSPossibleValues[row][i]) != 1))
				{
					++m_stats.m_hiddenSinglesFound;
					//if (!assign(row, i, thisBitRemoved))
					//	return false;
					//m_pendingAssignmentsQueue.push(row, i, thisBitRemoved);
					if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, i)) == m_pendingAssignmentsHashMap.end())
						m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, i)] = thisBitRemoved;
					else
					{
						unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, i)];
						if(oldVal != thisBitRemoved)
							return false;
					}
					break;
				}
		}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_2::updateHSCountersColumn(const unsigned int& column, const unsigned int& thisBitRemoved)
	{
		//return true;

		m_HSPossiblePositionsCountColumn[column][thisBitRemoved] -= 1;
		if (m_HSPossiblePositionsCountColumn[column][thisBitRemoved] == 0)
			return false;
		if (m_HSPossiblePositionsCountColumn[column][thisBitRemoved] == 1)
		{
			for (unsigned int i = 0; i < m_size; ++i)
				if (isBitON(m_NSPossibleValues[i][column], thisBitRemoved) && (getCountOfONBits(m_NSPossibleValues[i][column]) != 1))
				{
					++m_stats.m_hiddenSinglesFound;
					//if (!assign(i, column, thisBitRemoved))
					//	return false;
					//m_pendingAssignmentsQueue.push(i, column, thisBitRemoved);
					if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(i, column)) == m_pendingAssignmentsHashMap.end())
						m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(i, column)] = thisBitRemoved;
					else
					{
						unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(i, column)];
						if(oldVal != thisBitRemoved)
							return false;
					}
					break;
				}
		}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_2::updateHSCountersBox(const unsigned int& boxIndex, const unsigned int& thisBitRemoved)
	{
		m_HSPossiblePositionsCountBox[boxIndex][thisBitRemoved] -= 1;
		if (m_HSPossiblePositionsCountBox[boxIndex][thisBitRemoved] == 0)
			return false;
		if (m_HSPossiblePositionsCountBox[boxIndex][thisBitRemoved] == 1)
		{
			for (unsigned int ri = globalLookupTables.boxIndexToBoxStart[m_boxSize][boxIndex].first, i = 0; i < m_boxSize; ++ri, ++i)
				for (unsigned int ci = globalLookupTables.boxIndexToBoxStart[m_boxSize][boxIndex].second, j = 0; j < m_boxSize; ++ci, ++j)
					if (isBitON(m_NSPossibleValues[ri][ci], thisBitRemoved) && (getCountOfONBits(m_NSPossibleValues[ri][ci]) != 1))
					{
						++m_stats.m_hiddenSinglesFound;
						//if (!assign(ri, ci, thisBitRemoved))
						//	return false;
						//m_pendingAssignmentsQueue.push(ri, ci, thisBitRemoved);
						if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(ri, ci)) == m_pendingAssignmentsHashMap.end())
							m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(ri, ci)] = thisBitRemoved;
						else
						{
							unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(ri, ci)];
							if(oldVal != thisBitRemoved)
								return false;
						}

						break;
					}
		}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_2::searchForHiddenSingles()
	{

		return true;
	}

	bool FastestBruteForceSudokuMatrix_2::searchForLockedCandidate()
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
						m_NSPossibleValues[stripRowIndex][column];
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
					for (unsigned int iRow = stripRowIndex - stripRowIndex % m_boxSize, i = 0; i < m_boxSize; ++iRow, ++i)
					{
						if (iRow == stripRowIndex)
							continue;
						for (unsigned int column = stripColumnIndex * m_boxSize, i = 0; i < m_boxSize; ++column, ++i)
						{
							if (getCountOfONBits(m_NSPossibleValues[iRow][column]) == 1)
							{
								setBitOFF(m_NSPossibleValues[iRow][column], temp1);
								if (m_NSPossibleValues[iRow][column].isZero())
									return false;
							}
							else
							{
								setBitOFF(m_NSPossibleValues[iRow][column], temp1);
								if (!checkIfNakedSingle(iRow, column))
									return false;
							}
						}
					}
					for (unsigned int column = 0; column < m_size; ++column)
					{
						if (stripColumnIndex == (column / m_boxSize))
							continue;

						if (getCountOfONBits(m_NSPossibleValues[stripRowIndex][column]) == 1)
						{
							setBitOFF(m_NSPossibleValues[stripRowIndex][column], temp1);
							if (m_NSPossibleValues[stripRowIndex][column].isZero())
								return false;
						}
						else
						{
							setBitOFF(m_NSPossibleValues[stripRowIndex][column], temp1);
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
						m_NSPossibleValues[row][stripColumnIndex];
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
					for (unsigned int iColumn = stripColumnIndex - stripColumnIndex % m_boxSize, i = 0; i < m_boxSize; ++iColumn, ++i)
					{
						if (iColumn == stripColumnIndex)
							continue;
						for (unsigned int row = stripRowIndex * m_boxSize, i = 0; i < m_boxSize; ++row, ++i)
						{
							if (getCountOfONBits(m_NSPossibleValues[row][iColumn]) == 1)
							{
								setBitOFF(m_NSPossibleValues[row][iColumn], temp1);
								if (m_NSPossibleValues[row][iColumn].isZero())
									return false;
							}
							else
							{
								setBitOFF(m_NSPossibleValues[row][iColumn], temp1);
								if (!checkIfNakedSingle(row, iColumn))
									return false;
							}
						}
					}
					for (unsigned int row = 0; row < m_size; ++row)
					{
						if (stripRowIndex == (row / m_boxSize))
							continue;

						if (getCountOfONBits(m_NSPossibleValues[row][stripColumnIndex]) == 1)
						{
							setBitOFF(m_NSPossibleValues[row][stripColumnIndex], temp1);
							if (m_NSPossibleValues[row][stripColumnIndex].isZero())
								return false;
						}
						else
						{
							setBitOFF(m_NSPossibleValues[row][stripColumnIndex], temp1);
							if (!checkIfNakedSingle(row, stripColumnIndex))
								return false;
						}
					}
				}
			}
		}
		return true;
	}

	bool FastestBruteForceSudokuMatrix_2::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions)
	{
		while (m_cluesToFind > 0)
		{
			if (!processPendingAssignments())
				return false;
			if (!m_pendingAssignmentsHashMap.empty())
				continue;

			if (!searchForHiddenSingles())
				return false;
			if (!m_pendingAssignmentsHashMap.empty())
				continue;

			//if (!searchForLockedCandidate())
			//	return false;
			//if (!m_pendingAssignmentsQueue.isEmpty())
			//	continue;

			break;
		}

		if (m_cluesToFind == 0 || m_stats.m_valuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
		{
			vector< vector<int> > curentSolution(m_size, vector<int>(m_size, 0));
			for (unsigned int i = 0; i < m_size; ++i)
				for (unsigned int j = 0; j < m_size; ++j)
					curentSolution[i][j] = m_NSPossibleValues[i][j].getOnBitPosition();

			solutionSetsOut.push_back(std::move(curentSolution));
			return true;
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
				//if (m_solution[i][j] != 0)
				//	continue;

				unsigned int count = getCountOfONBits(m_NSPossibleValues[i][j]);
				if (count > 1u && count < minPossibleCount)
				{
					row = i;
					column = j;
					minPossibleCount = count;
				}
			}
		}

		// VIMP TODO: Remove this assert failure
		//MyAssert::myRunTimeAssert(minPossibleCount < (m_size + 1));
		if (minPossibleCount == (m_size + 1))
		{
			vector< vector<int> > curentSolution(m_size, vector<int>(m_size, 0));
			for (unsigned int i = 0; i < m_size; ++i)
				for (unsigned int j = 0; j < m_size; ++j)
					curentSolution[i][j] = m_NSPossibleValues[i][j].getOnBitPosition();

			solutionSetsOut.push_back(std::move(curentSolution));
			return true;
		}

		//bool success = false;

		++m_stats.m_cellsTriedOrLinkUpdates;
		//SudokuPuzzleUtils::getLogger() << "\nTrying cell[ " << row << ", " << column << "]";

		//int currentPossibleValues = Cell.m_isPossible.size();
		//vector< vector<Cell> > copy(dataIn);
		//FastestBruteForceSudokuMatrix_2 s(*this);
		for (unsigned int i = 1; i <= m_size; ++i)
		{
			if (isBitON(m_NSPossibleValues[row][column], i))
				//if (m_NSPossibleValues[row][column].isBitON(globalLookupTables.bitMask[i])) // TODO: make it efficient
			{
				++m_stats.m_valuesTried;

				//if (m_countValuesTried > SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
				//	return false;

				//SudokuPuzzleUtils::getLogger() << "\nTrying Value: " << i + 1;

				//copyDataFromTo(m_NSPossibleValues, s.m_NSPossibleValues);
				//vector< vector<Cell> > copy(dataIn);
				MyAssert::myRunTimeAssert(m_pendingAssignmentsHashMap.size() == 0);
				FastestBruteForceSudokuMatrix_2 s(*this);

				//m_pendingAssignmentsQueue.push(row, column, i);
				if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
					m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = i;
				else
				{
					unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
					if(oldVal != i)
						return false;
				}

				//if (s.assign(row, column, i)
				if(s.solve(solutionSetsOut, numSolutions)
					&& solutionSetsOut.size() == numSolutions)
					return true;

				m_pendingAssignmentsHashMap.clear();
				++m_stats.m_wrongGuesses;
			}
		}

		return false;
	}
	

} // namespace mm
