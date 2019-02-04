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
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_7.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"
#include "Timer/Timer.h"

namespace mm {

	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_1::m_initializationFiltersRow(MAX_DIM, BigIntegerBitCollection(0));
	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_1::m_initializationFiltersColumn(MAX_DIM, BigIntegerBitCollection(0));
	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_1::m_initializationFiltersBox(MAX_DIM, BigIntegerBitCollection(0));

	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_1::m_initializationFiltersRowHS(MAX_DIM, BigIntegerBitCollection(0));
	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_1::m_initializationFiltersColumnHS(MAX_DIM, BigIntegerBitCollection(0));
	vector<BigIntegerBitCollection> FastestBruteForceSudokuMatrix_1::m_initializationFiltersBoxHS(MAX_DIM, BigIntegerBitCollection(0));

	BigIntegerBitCollection FastestBruteForceSudokuMatrix_1::m_bitsON(0);
	BigIntegerBitCollection FastestBruteForceSudokuMatrix_1::m_valuesToEliminate(0);

	bool FastestBruteForceSudokuMatrix_1::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		FastestBruteForceSudokuMatrix_1 obj(dataIn, stats);
		if (obj.m_isValid && obj.solve(solutionSetsOut, numSolutions))
			return true;
		else
			return false;
	}

	const unsigned int FastestBruteForceSudokuMatrix_1::getCountOfONBits(const BigIntegerBitCollection& bitSet)
	{
		
		return getCountOfONBits_v4(bitSet);
	}

	const unsigned int FastestBruteForceSudokuMatrix_1::getCountOfONBits_v1(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int value = 1; value <= m_size; ++value)
			if (isBitON(bitSet, value))
				++count;

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_1::getCountOfONBits_v2(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < bitSet.getDigits().size(); ++i)
			count += (globalLookupTables.countONbits8[bitSet.getDigits()[i] & 0b11111111]
				+ globalLookupTables.countONbits8[(bitSet.getDigits()[i] >> 8) & 0b11111111]
				+ globalLookupTables.countONbits8[(bitSet.getDigits()[i] >> 16) & 0b11111111]
				+ globalLookupTables.countONbits8[(bitSet.getDigits()[i] >> 24) & 0b11111111]);

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_1::getCountOfONBits_v3(const BigIntegerBitCollection& bitSet)
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < bitSet.getDigits().size(); ++i)
			count += (globalLookupTables.countONbits16[bitSet.getDigits()[i] & 0b1111111111111111]
				+ globalLookupTables.countONbits16[(bitSet.getDigits()[i] >> 16) & 0b1111111111111111]);

		return count;
	}

	const unsigned int FastestBruteForceSudokuMatrix_1::getCountOfONBits_v4(const BigIntegerBitCollection& bitSet)
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

	const unsigned int FastestBruteForceSudokuMatrix_1::getOnBitPosition(const BigIntegerBitCollection& bitSet)
	{
		return getOnBitPosition_v2(bitSet);
	}

	const unsigned int FastestBruteForceSudokuMatrix_1::getOnBitPosition_v1(const BigIntegerBitCollection& bitSet)
	{
		//Ideally there should be only one bit ON
		//MyAssert::myRunTimeAssert((m_isPossible & (m_isPossible - 1)) == 0);
		//MyAssert::myRunTimeAssert(m_isPossible.isOnlyOneBitON());
		//if (m_isPossible.isOnlyOneBitON())
		return bitSet.getOnBitPosition();
		//else
		//	return 0;
	}

	const unsigned int FastestBruteForceSudokuMatrix_1::getOnBitPosition_v2(const BigIntegerBitCollection& bitSet)
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
	bool FastestBruteForceSudokuMatrix_1::isBitON(const BigIntegerBitCollection& bitSet, const unsigned int& index) const
	{
		//return m_isPossible.isnthBitON(index);
		return bitSet.getDigits()[globalLookupTables.bitPositionToDigitIndexHash[index]] & globalLookupTables.bitMask32[BigIntegerBitCollection::getBitPositionInCurrentDigit(index)];
	}

	bool FastestBruteForceSudokuMatrix_1::isBitON(const BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask) const
	{
		for (unsigned int i = 0; i < mask.getDigits().size(); ++i)
			if (mask.getDigits()[i] != 0 && (bitSet.getDigits()[i] & mask.getDigits()[i]) == 0)
				return false;

		return true;
	}

	// Index range [1, infinity)
	void FastestBruteForceSudokuMatrix_1::setBitOFF(BigIntegerBitCollection& bitSet, const unsigned int& index)
	{
		//m_isPossible.setnthBitOFF(index);
		bitSet.getDigits()[ globalLookupTables.bitPositionToDigitIndexHash[index] ] &= globalLookupTables.bitMask32Inv[BigIntegerBitCollection::getBitPositionInCurrentDigit(index)];
	}

	void FastestBruteForceSudokuMatrix_1::setBitOFF(BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask)
	{		
		for (unsigned int i = 0; i < mask.getDigits().size(); ++i)
			bitSet.getDigits()[i] &= ~mask.getDigits()[i];
	}

	// Index range [1, infinity)
	void FastestBruteForceSudokuMatrix_1::setBitON(BigIntegerBitCollection& bitSet, const unsigned int& index)
	{
		//m_isPossible.setnthBitOFF(index);
		bitSet.getDigits()[globalLookupTables.bitPositionToDigitIndexHash[index]] |= globalLookupTables.bitMask32[BigIntegerBitCollection::getBitPositionInCurrentDigit(index)];
	}

	void FastestBruteForceSudokuMatrix_1::setBitON(BigIntegerBitCollection& bitSet, const BigIntegerBitCollection& mask)
	{
		for (unsigned int i = 0; i < mask.getDigits().size(); ++i)
			bitSet.getDigits()[i] |= mask.getDigits()[i];
	}

	void FastestBruteForceSudokuMatrix_1::copyValue(const BigIntegerBitCollection& from, BigIntegerBitCollection& to)
	{
		//to.getDigits().assign(from.getDigits().begin(), from.getDigits().end());
		unsigned int index = 0;
		for (; index < from.getDigits().size(); ++index)
			to.getDigits()[index] = from.getDigits()[index];

		for (; index < to.getDigits().size(); ++index)
			to.getDigits()[index] = 0;
	}

	FastestBruteForceSudokuMatrix_1::FastestBruteForceSudokuMatrix_1(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
	: m_size(dataIn.size()),
		m_boxSize(sqrt(m_size)),
		m_stats(stats),
		m_isValid(true),
		m_cluesCount(0),
		m_NSPossibleValues(m_size, vector<BigIntegerBitCollection>(m_size, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])),
		//m_NSPossibleValuesCount(m_size, vector<unsigned int>(m_size, m_size)),
		m_HSPossiblePositionsRow(m_size, vector<BigIntegerBitCollection>(m_size + 1, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])),
		//m_HSPossiblePositionsCountRow(m_size, vector<unsigned int>(m_size + 1, m_size)),
		m_HSPossiblePositionsColumn(m_size, vector<BigIntegerBitCollection>(m_size + 1, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])),
		//m_HSPossiblePositionsCountColumn(m_size, vector<unsigned int>(m_size + 1, m_size)),
		m_HSPossiblePositionsBox(m_size, vector<BigIntegerBitCollection>(m_size + 1, globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]]))
		//m_HSPossiblePositionsCountBox(m_size, vector<unsigned int>(m_size + 1, m_size))
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

				m_initializationFiltersRowHS[i].resize(numDigits);
				m_initializationFiltersColumnHS[i].resize(numDigits);
				m_initializationFiltersBoxHS[i].resize(numDigits);
			}
		}

		//Reset all mask to be used to update NS data structure
		for (unsigned int i = 0; i < m_size; ++i)
		{
			m_initializationFiltersRow[i].resetAllDigits();
			m_initializationFiltersColumn[i].resetAllDigits();
			m_initializationFiltersBox[i].resetAllDigits();

			m_initializationFiltersRowHS[i].resetAllDigits();
			m_initializationFiltersColumnHS[i].resetAllDigits();
			m_initializationFiltersBoxHS[i].resetAllDigits();
		}

		//Prepare all masks to be used to update NS and HS data structures
		for (unsigned int row = 0; row < m_size; ++row)
		{
			for (unsigned int column = 0; column < m_size; ++column)
			{
				if (dataIn[row][column] != 0)
				{
					//Prepare all masks to be used to update NS data structure
					setBitON(m_initializationFiltersRow[row], dataIn[row][column]);
					setBitON(m_initializationFiltersColumn[column], dataIn[row][column]);
					setBitON(m_initializationFiltersBox[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]], dataIn[row][column]);

					//Prepare all masks to be used to update HS data structure
					setBitON(m_initializationFiltersRowHS[row], column + 1);
					setBitON(m_initializationFiltersColumnHS[column], row + 1);
					setBitON(m_initializationFiltersBoxHS[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]], 
						m_boxSize * (row % m_boxSize) + (column % m_boxSize) + 1);

					//Update NS data structure
					copyValue(globalLookupTables.bitMask[dataIn[row][column]], m_NSPossibleValues[row][column]);
					//m_NSPossibleValuesCount[row][column] = 1;

					//Update HS data structure
					copyValue(globalLookupTables.bitMask[column + 1], m_HSPossiblePositionsRow[row][dataIn[row][column]]);
					copyValue(globalLookupTables.bitMask[row + 1], m_HSPossiblePositionsColumn[column][dataIn[row][column]]);
					copyValue(globalLookupTables.bitMask[m_boxSize * (row % m_boxSize) + (column % m_boxSize) + 1], m_HSPossiblePositionsBox[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]][dataIn[row][column]]);

					// A value is assigned to a cell. So the possible HS count of all other values is reduced by 1.
					// We will update every cell for all possible values at once later. 
					// Just to avoid conditional statements later, increase the HS count by one now, we will reduce it later along with other cells.
					//m_HSPossiblePositionsCountRow[row][dataIn[row][column]] += 1;
					//m_HSPossiblePositionsCountColumn[column][dataIn[row][column]] += 1;
					//m_HSPossiblePositionsCountBox[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]][dataIn[row][column]] += 1;
				}
			}
		}

		for (unsigned int index = 0; index < m_size; ++index)
		{
			for (unsigned int value = 1; value <= m_size; ++value)
			{
				if(m_HSPossiblePositionsRow[index][value] == globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])
					setBitOFF(m_HSPossiblePositionsRow[index][value], m_initializationFiltersRowHS[index]);
				if (m_HSPossiblePositionsColumn[index][value] == globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])
					setBitOFF(m_HSPossiblePositionsColumn[index][value], m_initializationFiltersColumnHS[index]);
				if (m_HSPossiblePositionsBox[index][value] == globalLookupTables.initialbits[globalLookupTables.sizeToIndexHash[m_size]])
					setBitOFF(m_HSPossiblePositionsBox[index][value], m_initializationFiltersBoxHS[index]);
			}
		}

		//Avoid memory allocation by defining static member at start
		//BigIntegerBitCollection bitsON(0);
		//Apply all mask to NS data structure TODO: Combine following 3 loops
		for (unsigned int row = 0; row < m_size; ++row)
			for (unsigned int column = 0; column < m_size; ++column)
			{
				//if (m_NSPossibleValuesCount[row][column] == 1)
				//	continue;

				//if (getCountOfONBits(m_NSPossibleValues[row][column]) == 1)
				//	continue;

				if (dataIn[row][column] != 0)
					continue;

				unsigned int boxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][row][column];
				copyValue((m_NSPossibleValues[row][column] & (m_initializationFiltersRow[row] | m_initializationFiltersColumn[column] | m_initializationFiltersBox[boxIndex])),
					m_bitsON);

				setBitOFF(m_NSPossibleValues[row][column], m_bitsON);
				unsigned int count = getCountOfONBits(m_NSPossibleValues[row][column]);
				//m_NSPossibleValuesCount[row][column] = count;
				if(count == 1)
					++m_stats.m_nakedSinglesFound;
				if (count == 0 || count == 1 && !assign(row, column, getOnBitPosition(m_NSPossibleValues[row][column])))
				{
					m_isValid = false;
					return;
				}

				// Reduce the possible HS count due to all removed bits from other cells than assigned values
				const unsigned int cellIndexInBox = m_boxSize * (row % m_boxSize) + (column % m_boxSize) + 1;
				for (unsigned int value = 1; value <= m_size; ++value)
				{
					if (isBitON(m_bitsON, value))
					{						
						if ((getCountOfONBits(m_HSPossiblePositionsRow[row][value]) != 1 && !updateHSPossiblePositionsRow(row, column, value))
							|| (getCountOfONBits(m_HSPossiblePositionsColumn[column][value]) != 1 && !updateHSPossiblePositionsColumn(row, column, value))
							|| (getCountOfONBits(m_HSPossiblePositionsBox[boxIndex][value]) != 1 && !updateHSPossiblePositionsBox(boxIndex, cellIndexInBox, value)))
						{
							m_isValid = false;
							return;
						}
					}
				}
			}
	}

	bool FastestBruteForceSudokuMatrix_1::updateNSCounters(const unsigned int& row, const unsigned int& column, const unsigned int& removeThisBit)
	{
		setBitOFF(m_NSPossibleValues[row][column], removeThisBit);
		//m_NSPossibleValuesCount[row][column] -= 1;
		if (m_NSPossibleValues[row][column].isZero())
			return false;
		if (getCountOfONBits(m_NSPossibleValues[row][column]) == 1)
		{
			++m_stats.m_nakedSinglesFound;
			int currentValue = m_NSPossibleValues[row][column].getOnBitPosition();
			if (!assign(row, column, currentValue))
				return false;
		}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_1::updateHSPossiblePositionsRow(const unsigned int& row, const unsigned int& column, const unsigned int& thisBitRemoved)
	{
		if (isBitON(m_HSPossiblePositionsRow[row][thisBitRemoved], column + 1))
		{
			setBitOFF(m_HSPossiblePositionsRow[row][thisBitRemoved], column + 1);
			if (m_HSPossiblePositionsRow[row][thisBitRemoved].isZero())
				return false;
			if (getCountOfONBits(m_HSPossiblePositionsRow[row][thisBitRemoved]) == 1)
			{
				int targetColumn = m_HSPossiblePositionsRow[row][thisBitRemoved].getOnBitPosition() - 1;
				if (getCountOfONBits(m_NSPossibleValues[row][targetColumn]) != 1)
				{
					++m_stats.m_hiddenSinglesFound;
					if (!assign(row, targetColumn, thisBitRemoved))
						return false;
				}
			}
		}
		//else
		//	MyAssert::myRunTimeAssert(false);

		return true;
	}

	bool FastestBruteForceSudokuMatrix_1::updateHSPossiblePositionsColumn(const unsigned int& row, const unsigned int& column, const unsigned int& thisBitRemoved)
	{
		if (isBitON(m_HSPossiblePositionsColumn[column][thisBitRemoved], row + 1))
		{
			setBitOFF(m_HSPossiblePositionsColumn[column][thisBitRemoved], row + 1);
			if (m_HSPossiblePositionsColumn[column][thisBitRemoved].isZero())
				return false;
			if (getCountOfONBits(m_HSPossiblePositionsColumn[column][thisBitRemoved]) == 1)
			{
				int targetRow = m_HSPossiblePositionsColumn[column][thisBitRemoved].getOnBitPosition() - 1;
				if (getCountOfONBits(m_NSPossibleValues[targetRow][column]) != 1)
				{
					++m_stats.m_hiddenSinglesFound;
					if (!assign(targetRow, column, thisBitRemoved))
						return false;
				}
			}
		}
		//else
		//	MyAssert::myRunTimeAssert(false);

		return true;
	}

	bool FastestBruteForceSudokuMatrix_1::updateHSPossiblePositionsBox(const unsigned int& boxIndex, const unsigned int& cellIndexInBox, const unsigned int& thisBitRemoved)
	{
		if (isBitON(m_HSPossiblePositionsBox[boxIndex][thisBitRemoved], cellIndexInBox))
		{
			setBitOFF(m_HSPossiblePositionsBox[boxIndex][thisBitRemoved], cellIndexInBox);
			if (m_HSPossiblePositionsBox[boxIndex][thisBitRemoved].isZero())
				return false;
			if (getCountOfONBits(m_HSPossiblePositionsBox[boxIndex][thisBitRemoved]) == 1)
			{
				const unsigned int targetCellIndex = m_HSPossiblePositionsBox[boxIndex][thisBitRemoved].getOnBitPosition() - 1;
				const unsigned int targetRow = globalLookupTables.boxIndexToBoxStart[m_boxSize][boxIndex].first + (targetCellIndex / m_boxSize);
				const unsigned int targetColumn = globalLookupTables.boxIndexToBoxStart[m_boxSize][boxIndex].second + (targetCellIndex % m_boxSize);
				if (getCountOfONBits(m_NSPossibleValues[targetRow][targetColumn]) != 1)
				{
					++m_stats.m_hiddenSinglesFound;
					if (!assign(targetRow, targetColumn, thisBitRemoved))
						return false;
				}
			}
		}
		//else
		//	MyAssert::myRunTimeAssert(false);

		return true;
	}

	bool FastestBruteForceSudokuMatrix_1::propagateConstraint(const unsigned int& row, const unsigned int& column, const unsigned int& removeThisBit)
	{
		unsigned int boxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][row][column];
		//Update Naked Single data structure
		// Check row
		for (unsigned int k = 0; k < m_size; ++k)
		{
			if (k != column && isBitON(m_NSPossibleValues[row][k], removeThisBit))
			{
				if (!updateNSCounters(row, k, removeThisBit))
					return false;
				if (!updateHSPossiblePositionsColumn(row, k, removeThisBit))
					return false;
				unsigned int currentBoxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][row][k];
				if (currentBoxIndex != boxIndex)
				{
					const unsigned int cellIndexInBox = m_boxSize * (row % m_boxSize) + (k % m_boxSize) + 1;
					if (!updateHSPossiblePositionsBox(currentBoxIndex, cellIndexInBox, removeThisBit))
						return false;
				}
			}
		}

		// Check column
		for (unsigned int k = 0; k < m_size; ++k)
		{
			if (k != row && isBitON(m_NSPossibleValues[k][column], removeThisBit))
			{
				if (!updateNSCounters(k, column, removeThisBit))
					return false;
				if (!updateHSPossiblePositionsRow(k, column, removeThisBit))
					return false;
				unsigned int currentBoxIndex = globalLookupTables.cellToBoxIndex[m_boxSize][k][column];
				if (currentBoxIndex != boxIndex)
				{
					const unsigned int cellIndexInBox = m_boxSize * (k % m_boxSize) + (column % m_boxSize) + 1;
					if (!updateHSPossiblePositionsBox(currentBoxIndex, cellIndexInBox, removeThisBit))
						return false;
				}
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
				if (boxColumn != column && isBitON(m_NSPossibleValues[boxRow][boxColumn], removeThisBit))
				{
					if (!updateNSCounters(boxRow, boxColumn, removeThisBit))
						return false;
					if (!updateHSPossiblePositionsRow(boxRow, boxColumn, removeThisBit))
						return false;
					if (!updateHSPossiblePositionsColumn(boxRow, boxColumn, removeThisBit))
						return false;
				}
		}

		return true;
	}

	bool FastestBruteForceSudokuMatrix_1::assign(const unsigned int& row, const unsigned int& column, const unsigned int& value)
	{
		if (!isBitON(m_NSPossibleValues[row][column], value))
			return false;
		//else if(m_NSPossibleValuesCount[row][column] == 1) // Already assigned
		//	return true;

		// Set only m_value'th bit OFF
		setBitOFF(m_NSPossibleValues[row][column], value);
		BigIntegerBitCollection valuesToEliminate(m_NSPossibleValues[row][column]);

		//Update NS Data structure: Set all bits OFF as this cell is solved
		copyValue(globalLookupTables.bitMask[value], m_NSPossibleValues[row][column]);
		//m_NSPossibleValuesCount[row][column] = 1;

		//Update HS data structure
		copyValue(globalLookupTables.bitMask[column + 1], m_HSPossiblePositionsRow[row][value]);
		copyValue(globalLookupTables.bitMask[row + 1], m_HSPossiblePositionsColumn[column][value]);
		copyValue(globalLookupTables.bitMask[m_boxSize * (row % m_boxSize) + (column % m_boxSize) + 1], m_HSPossiblePositionsBox[globalLookupTables.cellToBoxIndex[m_boxSize][row][column]][value]);

		for (unsigned int index = 1; index <= m_size; ++index)
			if (isBitON(valuesToEliminate, index))
			{
				const unsigned int cellIndexInBox = m_boxSize * (row % m_boxSize) + (column % m_boxSize) + 1;
				if (!updateHSPossiblePositionsRow(row, column, index)
					|| !updateHSPossiblePositionsColumn(row, column, index)
					|| !updateHSPossiblePositionsBox(globalLookupTables.cellToBoxIndex[m_boxSize][row][column], cellIndexInBox, index))
					return false;
			}

		if (!propagateConstraint(row, column, value))
			return false;

		return true;
	}

	bool FastestBruteForceSudokuMatrix_1::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions)
	{
		unsigned int minPossibleCount = m_size + 1;
		unsigned int row = minPossibleCount;
		unsigned int column = minPossibleCount;

		//TODO: Instead of finding min every time, keep min and second_min ini memory and whenever the count is updated, update these values
		for (unsigned int i = 0; i < m_size && minPossibleCount != 2; ++i)
		{
			for (unsigned int j = 0; j < m_size && minPossibleCount != 2; ++j)
			{
				unsigned int count = getCountOfONBits(m_NSPossibleValues[i][j]);
				if (count > 1u && count < minPossibleCount)
				{
					row = i;
					column = j;
					minPossibleCount = count;
				}
			}
		}

		if (minPossibleCount == (m_size + 1) || m_stats.m_valuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
		{
			vector< vector<int> > solution(m_size, vector<int>(m_size, 0));
			for (unsigned int i = 0; i < m_size; ++i)
				for (unsigned int j = 0; j < m_size; ++j)
					solution[i][j] = m_NSPossibleValues[i][j].getOnBitPosition();

			solutionSetsOut.push_back(std::move(solution));
			return true;
		}

		bool success = false;

		++m_stats.m_cellsTriedOrLinkUpdates;
		//SudokuPuzzleUtils::getLogger() << "\nTrying cell[ " << row << ", " << column << "]";

		//int currentPossibleValues = Cell.m_isPossible.size();
		//vector< vector<Cell> > copy(dataIn);
		//FastestBruteForceSudokuMatrix_1 s(*this);
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
				FastestBruteForceSudokuMatrix_1 s(*this);

				if (s.assign(row, column, i)
					&& (success = s.solve(solutionSetsOut, numSolutions))
					&& solutionSetsOut.size() == numSolutions)
					break;

				++m_stats.m_wrongGuesses;
			}
		}

		return success;
	}

}
