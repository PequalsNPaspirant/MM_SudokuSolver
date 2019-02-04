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
#include <set>
#include <string>
#include <cmath>
using namespace std;

#include <windows.h>

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_6.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"

namespace mm {

	SudokuMatrix6::Cell::Cell(const unsigned int& size)
		: m_isPossible(),  // 0b111111111 = 0x1FF = 511 = 2^9 - 1 = (1 << 9) - 1; // int having only righmost 9 bits ON, rest bits OFF1
		m_isPossibleCount(size)
	{
		m_isPossible.resize(size / BigIntegerBitCollection::BITS_IN_ONE_DIGIT + 1, 0);
		m_isPossible.setAllRightmostBitsON(size);
	}

	SudokuMatrix6::Cell::Cell(const Cell& obj)
		: m_isPossible(obj.m_isPossible),
		m_isPossibleCount(obj.m_isPossibleCount)
	{
	}

	const unsigned int SudokuMatrix6::Cell::getOnBitPosition()
	{
		//Ideally there should be only one bit ON
		//MyAssert::myRunTimeAssert((m_isPossible & (m_isPossible - 1)) == 0);
		//MyAssert::myRunTimeAssert(m_isPossible.isOnlyOneBitON());
		if (m_isPossible.isOnlyOneBitON())
			return m_isPossible.getOnBitPosition();
		else
			return 0;
	}

	bool SudokuMatrix6::Cell::isBitON(const unsigned int& index) const
	{
		return m_isPossible.isnthBitON(index);
	}

	void SudokuMatrix6::Cell::setBitOFF(const unsigned int& index)
	{
		m_isPossible.setnthBitOFF(index);
	}

	bool SudokuMatrix6::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		SudokuMatrix6 obj(dataIn, stats);
		if (obj.m_isValid && obj.solve(solutionSetsOut, numSolutions))
			return true;
		else
			return false;
	}

	SudokuMatrix6::SudokuMatrix6(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
	: m_data(dataIn.size(), 
		vector<Cell>(dataIn.size(), 
		Cell(dataIn.size()))), 
		m_isValid(true), 
		m_cluesCount(0),
		m_stats(stats),
		m_size(dataIn.size()), 
		m_boxSize(sqrt(m_size))
	{
		for (unsigned int i = 0; i < m_size; ++i)
		{
			for (unsigned int j = 0; j < m_size; ++j)
			{
				if (dataIn[i][j] != 0)
				{
					if(!assign(i, j, dataIn[i][j]))
					{
						m_isValid = false;
						return;
					}
				}
			}
		}

		//vector< vector<int> > solution(9, vector<int>(9));
		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		solution[i][j] = m_data[i][j].m_value;
		//cout << "\nAfter initialization is done:";
		//SudokuPuzzleUtils::printSudokuGrid(solution, dataIn);
	}

	void SudokuMatrix6::copyDataFromTo(const vector< vector<Cell> >& from, vector< vector<Cell> >& to)
	{
		for (unsigned int i = 0; i < m_size; ++i)
		{
			for (unsigned int j = 0; j < m_size; ++j)
			{
				to[i][j].m_isPossibleCount = from[i][j].m_isPossibleCount;
				to[i][j].m_isPossible = from[i][j].m_isPossible;
			}
		}
	}

	bool SudokuMatrix6::removeFromPossibleValuesOfPeer(const unsigned int& row, const unsigned int& column, const unsigned int& value)
	{
		if (m_data[row][column].isBitON(value))
		{
			m_data[row][column].setBitOFF(value);
			m_data[row][column].m_isPossibleCount -= 1;

			if (m_data[row][column].m_isPossibleCount == 0)
				return false;
			else if (checkIfCellHasNakedSingle(row, column))
			{
				++m_stats.m_nakedSinglesFound;
				int currentValue = m_data[row][column].getOnBitPosition();
				if (!assign(row, column, currentValue))
					return false;
			}
		}

		return true;
	}

	bool SudokuMatrix6::propagateConstraint(const unsigned int& row, const unsigned int& column, const unsigned int& value)
	{
		// Check row
		for (unsigned int k = 0; k < m_size; ++k)
			if (k != column && !removeFromPossibleValuesOfPeer(row, k, value))
				return false;

		// Check column
		for (unsigned int k = 0; k < m_size; ++k)
			if (k != row && !removeFromPossibleValuesOfPeer(k, column, value))
				return false;

		// Check box
		//int a = row - row % 3;
		//int a = (row / 3) * 3;
		const int boxRowStart = row - row % m_boxSize;
		const int boxColumnStart = column - column % m_boxSize;
		unsigned int boxRow = boxRowStart;
		for (unsigned int i = 0; i < m_boxSize; ++i, ++boxRow)
		{
			unsigned int boxColumn = boxColumnStart;
			for (unsigned int j = 0; j < m_boxSize; ++j, ++boxColumn)
				if (boxRow != row && boxColumn != column && !removeFromPossibleValuesOfPeer(boxRow, boxColumn, value))
					return false;
		}

		return true;
	}

	bool SudokuMatrix6::checkIfCellHasNakedSingle(const unsigned int& row, const unsigned int& column)
	{
		return (m_data[row][column].m_isPossibleCount == 1);
	}

	bool SudokuMatrix6::checkIfRowHasHiddenSingle(const unsigned int& row, const unsigned int& value)
	{
		unsigned int targetColumnIndex = 0;
		unsigned int counter = 0;
		for (unsigned int k = 0; k < m_size && counter < 2; ++k)
			//if (k != column && m_data[row][k].isBitON(bitIndex))
			if (m_data[row][k].isBitON(value))
			{
				++counter;
				targetColumnIndex = k;
			}

		if (counter == 0u)
			return false;

		if (counter == 1u && m_data[row][targetColumnIndex].m_isPossibleCount > 1u)
		{
			++m_stats.m_hiddenSinglesFound;
			if (!assign(row, targetColumnIndex, value))
				return false;
		}

		return true;
	}

	bool SudokuMatrix6::checkIfColumnHasHiddenSingle(const unsigned int& column, const unsigned int& value)
	{
		unsigned int targetRowIndex = 0;
		unsigned int counter = 0;
		for (unsigned int k = 0; k < m_size && counter < 2; ++k)
			//if (k != column && m_data[row][k].isBitON(bitIndex))
			if (m_data[k][column].isBitON(value))
			{
				++counter;
				targetRowIndex = k;
			}

		if (counter == 0u)
			return false;

		if (counter == 1u && m_data[targetRowIndex][column].m_isPossibleCount > 1u)
		{
			++m_stats.m_hiddenSinglesFound;
			if (!assign(targetRowIndex, column, value))
				return false;
		}

		return true;
	}

	bool SudokuMatrix6::checkIfBoxHasHiddenSingle(const unsigned int& boxRowStart, const unsigned int& boxColumnStart, const unsigned int& value)
	{
		// Check box
		unsigned int targetRowIndex = 0;
		unsigned int targetColumnIndex = 0;
		unsigned int counter = 0;
		unsigned int boxRow = boxRowStart;
		for (unsigned int i = 0; i < m_boxSize && counter < 2; ++i, ++boxRow)
		{
			unsigned int boxColumn = boxColumnStart;
			for (unsigned int j = 0; j < m_boxSize && counter < 2; ++j, ++boxColumn)
				//if (a != row && b != column && m_data[a][b].isBitON(bitIndex))
				if (m_data[boxRow][boxColumn].isBitON(value))
				{
					++counter;
					targetRowIndex = boxRow;
					targetColumnIndex = boxColumn;
				}
		}

		if (counter == 0u)
			return false;

		if (counter == 1u && m_data[targetRowIndex][targetColumnIndex].m_isPossibleCount > 1u)
		{
			++m_stats.m_hiddenSinglesFound;
			if (!assign(targetRowIndex, targetColumnIndex, value))
				return false;
		}

		return true;
	}

	bool SudokuMatrix6::assign(const unsigned int& row, const unsigned int& column, const unsigned int& value)
	{
		/*
		Algorithm:
		Backup m_isPossible, Remove all extra bits from m_isPossible, and keep only value'th bit ON
		Set m_isPossibleCount = 1
		PropagateConstraint i.e. remove "value" from all preers of m_data[row][column]. 
			PropagateConstraint is defined as: 
				If value'th bit is already OFF, return true. 
				If m_isPossibleCount goes to zero, return false.
				If m_isPossibleCount goes to one (naked single case), call assign()
		Find hidden single cases:
			case 1. In all rows, columns and corresponding boxes except that has m_data[row][column], search for hidden single of value
			case 2. In the row, column and box having m_data[row][column], search for hidden single of every ON bit in old m_isPossible except value
		*/

		if (!m_data[row][column].isBitON(value))
			return false;
		//else if(m_data[row][column].m_isPossibleCount == 1) // Already assigned
		//	return true;

		// Set only m_value'th bit OFF
		m_data[row][column].setBitOFF(value);
		BigIntegerBitCollection valuesToEliminate = m_data[row][column].m_isPossible;
		// Set all bits OFF as this cell is solved
		m_data[row][column].m_isPossible.resetAllDigits();
		m_data[row][column].m_isPossible.setnthBitON(value);
		m_data[row][column].m_isPossibleCount = 1;

		if (!propagateConstraint(row, column, value))
			return false;
		
		//Check all rows
		int counter = 0;
		for (unsigned int i = 0; i < m_size; ++i)
		{
			if (i == row)
				continue;
			if (!checkIfRowHasHiddenSingle(i, value))
				return false;
		}

		//Check all columns
		for (unsigned int i = 0; i < m_size; ++i)
		{
			if (i == column)
				continue;
			if (!checkIfColumnHasHiddenSingle(i, value))
				return false;
		}

		//Check only relevant boxes
		//int a = row < 3 ? 0 : row < 6 ? 3 : 6;
		//int b = column < 3 ? 0 : column < 6 ? 3 : 6;
		const int boxRowStart = row - row % m_boxSize;
		const int boxColumnStart = column - column % m_boxSize;
		for (unsigned int i = 0; i < m_size; i += m_boxSize)
		{
			for (unsigned int j = 0; j < m_size; j += m_boxSize)
			{
				if ((i == boxRowStart && j != boxColumnStart) || (i != boxRowStart && j == boxColumnStart))
				{
					if (!checkIfBoxHasHiddenSingle(i, j, value))
						return false;
				}
			}
		}

		for (unsigned int bitIndex = 1; bitIndex <= m_size; ++bitIndex)
		{
			if (!valuesToEliminate.isnthBitON(bitIndex))
				continue;

			if (!checkIfRowHasHiddenSingle(row, bitIndex)
				|| !checkIfColumnHasHiddenSingle(column, bitIndex)
				|| !checkIfBoxHasHiddenSingle(boxRowStart, boxColumnStart, bitIndex))
				return false;
		}

		++m_cluesCount;
		return true;
	}

	bool SudokuMatrix6::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions)
	{
		unsigned int minPossibleCount = m_size + 1;
		std::pair<unsigned int, unsigned int> obj(0, 0);
		for (unsigned int i = 0; i < m_size && minPossibleCount != 2; ++i)
		{
			for (unsigned int j = 0; j < m_size && minPossibleCount != 2; ++j)
			{
				if (m_data[i][j].m_isPossibleCount > 1u && m_data[i][j].m_isPossibleCount < minPossibleCount)
				{
					obj = std::pair<unsigned int, unsigned int>(i, j);
					minPossibleCount = m_data[i][j].m_isPossibleCount;
				}
			}
		}

		if (minPossibleCount == (m_size + 1) || m_stats.m_valuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
		{
			vector< vector<int> > solution(m_size, vector<int>(m_size, 0));
			for (unsigned int i = 0; i < m_size; ++i)
				for (unsigned int j = 0; j < m_size; ++j)
					solution[i][j] = m_data[i][j].getOnBitPosition();

			solutionSetsOut.push_back(std::move(solution));
			return true;
		}

		bool success = false;
		int row = obj.first;
		int column = obj.second;

		++m_stats.m_cellsTriedOrLinkUpdates;
		//SudokuPuzzleUtils::getLogger() << "\nTrying cell[ " << row << ", " << column << "]";

		//int currentPossibleValues = Cell.m_isPossible.size();
		//vector< vector<Cell> > copy(dataIn);
		//SudokuMatrix6 s(*this);
		for (unsigned int i = 0; i < m_size; ++i)
		{
			if (m_data[row][column].isBitON(i + 1))
			{
				++m_stats.m_valuesTried;

				//if (m_countValuesTried > SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
				//	return false;

				//SudokuPuzzleUtils::getLogger() << "\nTrying Value: " << i + 1;

				//copyDataFromTo(m_data, s.m_data);
				//vector< vector<Cell> > copy(dataIn);
				SudokuMatrix6 s(*this);

				if (s.assign(row, column, i + 1)
					&& (success = s.solve(solutionSetsOut, numSolutions))
					&& solutionSetsOut.size() == numSolutions)
					break;

				++m_stats.m_wrongGuesses;
			}
		}

		return success;
	}

}
