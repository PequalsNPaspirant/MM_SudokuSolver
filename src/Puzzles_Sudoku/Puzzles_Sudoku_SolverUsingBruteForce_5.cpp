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

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_5.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"

namespace mm {

	SudokuMatrix5::Cell::Cell(const int size)
		: m_value(0), 
		m_isPossible(),  // 0b111111111 = 0x1FF = 511 = 2^9 - 1 = (1 << 9) - 1; // int having only righmost 9 bits ON, rest bits OFF1
		m_isPossibleCount(size)
	{
		m_isPossible.resize(size / BigIntegerBitCollection::BITS_IN_ONE_DIGIT + 1, 0);
		m_isPossible.setAllRightmostBitsON(size);
	}

	SudokuMatrix5::Cell::Cell(const Cell& obj)
		: m_value(obj.m_value),
		m_isPossible(obj.m_isPossible),
		m_isPossibleCount(obj.m_isPossibleCount)
	{
	}

	int SudokuMatrix5::Cell::getOnBitPosition()
	{
		//Ideally there should be only one bit ON
		//MyAssert::myRunTimeAssert((m_isPossible & (m_isPossible - 1)) == 0);
		//MyAssert::myRunTimeAssert(m_isPossible.isOnlyOneBitON());
		if (m_isPossible.isOnlyOneBitON())
			return m_isPossible.getOnBitPosition();
		else
			return 0;
	}

	bool SudokuMatrix5::Cell::isBitON(int index) const
	{
		return m_isPossible.isnthBitON(index);
	}

	void SudokuMatrix5::Cell::setBitOFF(int index)
	{
		m_isPossible.setnthBitOFF(index);
	}

	bool SudokuMatrix5::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		SudokuMatrix5 obj(dataIn, stats);
		if (obj.m_isValid && obj.solve(solutionSetsOut, numSolutions))
			return true;
		else
			return false;
	}

	SudokuMatrix5::SudokuMatrix5(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
	: m_data(dataIn.size(), vector<Cell>(dataIn.size(), Cell(dataIn.size()))), m_isValid(true), m_cluesCount(0),
		m_stats(stats)
	{
		const int size = dataIn.size();
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
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

	void SudokuMatrix5::copyDataFromTo(const vector< vector<Cell> >& from, vector< vector<Cell> >& to)
	{
		int size = from.size();
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				to[i][j].m_value = from[i][j].m_value;
				to[i][j].m_isPossibleCount = from[i][j].m_isPossibleCount;
				to[i][j].m_isPossible = from[i][j].m_isPossible;
			}
		}
	}

	bool SudokuMatrix5::removeFromPossibleValuesOfPeer(const int& row, const int& column, const int& value)
	{
		if (m_data[row][column].isBitON(value))
		{
			m_data[row][column].setBitOFF(value);
			m_data[row][column].m_isPossibleCount -= 1;

			if (m_data[row][column].m_isPossibleCount == 0)
				return false;
			else if (m_data[row][column].m_isPossibleCount == 1)
			{
				int currentValue = m_data[row][column].getOnBitPosition();
				if (!assign(row, column, currentValue))
					return false;
			}

			//if (!checkIfOnlyOnePeerHasThisPossibleValue(row, column, value))
			//	return false;
		}

		return true;
	}

	bool SudokuMatrix5::removeFromPossibleValuesOfAllPeers(const int& row, const int& column, const int& value)
	{
		int size = m_data.size();
		// Check row
		for (int k = 0; k < size; ++k)
			if (k != column && !removeFromPossibleValuesOfPeer(row, k, value))
				return false;

		// Check column
		for (int k = 0; k < size; ++k)
			if (k != row && !removeFromPossibleValuesOfPeer(k, column, value))
				return false;

		// Check box
		int boxSize = sqrt(size);
		//int a = row - row % 3;
		//int a = (row / 3) * 3;
		const int boxRowStart = row - row % boxSize;
		const int boxColumnStart = column - column % boxSize;
		int boxRow = boxRowStart;
		for (int i = 0; i < boxSize; ++i, ++boxRow)
		{
			int boxColumn = boxColumnStart;
			for (int j = 0; j < boxSize; ++j, ++boxColumn)
				if (boxRow != row && boxColumn != column && !removeFromPossibleValuesOfPeer(boxRow, boxColumn, value))
					return false;
		}

		int targetRowIndex = 0;
		int targetColumnIndex = 0;

		//Check all rows
		int counter = 0;
		for (int i = 0; i < size; ++i)
		{
			if (i == row)
				continue;
			counter = 0;
			for (int k = 0; k < size && counter < 2; ++k)
				if (m_data[i][k].isBitON(value))
				{
					++counter;
					targetColumnIndex = k;
				}

			if (counter == 0)
				return false;

			if (counter == 1)// && m_data[row][targetColumnIndex].m_isPossibleCount > 1)
				if (!assign(i, targetColumnIndex, value))
					return false;
		}

		//Check all columns
		for (int i = 0; i < size; ++i)
		{
			if (i == column)
				continue;
			counter = 0;
			for (int k = 0; k < size && counter < 2; ++k)
				if (m_data[k][i].isBitON(value))
				{
					++counter;
					targetRowIndex = k;
				}

			if (counter == 0)
				return false;

			if (counter == 1)// && m_data[row][targetColumnIndex].m_isPossibleCount > 1)
				if (!assign(targetRowIndex, i, value))
					return false;
		}

		//Check only relevant boxes
		//int a = row < 3 ? 0 : row < 6 ? 3 : 6;
		//int b = column < 3 ? 0 : column < 6 ? 3 : 6;
		for (int i = 0; i < size; i += boxSize)
		{
			for (int j = 0; j < size; j += boxSize)
			{
				if ((i == boxRowStart && j != boxColumnStart) || (i != boxRowStart && j == boxColumnStart))
				{
					counter = 0;
					for (int p = i; p < i + boxSize && counter < 2; ++p)
						for (int q = j; q < j + boxSize && counter < 2; ++q)
							if (m_data[p][q].isBitON(value))
							{
								++counter;
								targetRowIndex = p;
								targetColumnIndex = q;
							}

					if (counter == 0)
						return false;

					if (counter == 1)// && m_data[row][targetColumnIndex].m_isPossibleCount > 1)
						if (!assign(targetRowIndex, targetColumnIndex, value))
							return false;
				}
			}
		}

		return true;
	}

	bool SudokuMatrix5::checkIfOnlyOnePeerHasThisPossibleValue(const int& row, const int& column, const int& value)
	{
		int size = m_data.size();
		int targetRowIndex = 0;
		int targetColumnIndex = 0;

		// Check row
		int counter = 0;
		for (int k = 0; k < size && counter < 2; ++k)
			//if (k != column && m_data[row][k].isBitON(bitIndex))
			if (m_data[row][k].isBitON(value))
			{
				++counter;
				targetColumnIndex = k;
			}

		if (counter == 0)
			return false;

		if (counter == 1) // && m_data[row][targetColumnIndex].m_isPossibleCount > 1)
			if (!assign(row, targetColumnIndex, value))
				return false;

		// Check column
		counter = 0;
		for (int k = 0; k < size && counter < 2; ++k)
			//if (k != row && m_data[k][column].isBitON(bitIndex))
			if (m_data[k][column].isBitON(value))
			{
				++counter;
				targetRowIndex = k;
			}

		if (counter == 0)
			return false;

		if (counter == 1) // && m_data[targetRowIndex][column].m_isPossibleCount > 1)
			if (!assign(targetRowIndex, column, value))
				return false;

		// Check box
		counter = 0;
		int boxSize = sqrt(size);
		//int a = row - row % 3;
		//int a = (row / 3) * 3;
		const int boxRowStart = row - row % boxSize;
		const int boxColumnStart = column - column % boxSize;
		int boxRow = boxRowStart;
		for (int i = 0; i < boxSize && counter < 2; ++i, ++boxRow)
		{
			int boxColumn = boxColumnStart;
			for (int j = 0; j < boxSize && counter < 2; ++j, ++boxColumn)
				//if (a != row && b != column && m_data[a][b].isBitON(bitIndex))
				if (m_data[boxRow][boxColumn].isBitON(value))
				{
					++counter;
					targetRowIndex = boxRow;
					targetColumnIndex = boxColumn;
				}
		}

		if (counter == 0)
			return false;
		if (counter == 1) // && m_data[targetRowIndex][targetColumnIndex].m_isPossibleCount > 1)
			if (!assign(targetRowIndex, targetColumnIndex, value))
				return false;

		return true;
	}

	bool SudokuMatrix5::assign(const int& row, const int& column, const int& value)
	{
		/*
		The logic behind the optimization in this class is explained in function SudokuMatrix4::eliminate() in file Puzzles_Sudoku_SolverUsingBruteForce_5.cpp
		*/

		if (m_data[row][column].m_value != 0) // Already assigned
			return true;

		//This is similar to above. We can avoid dependency on m_value
		//if (m_data[row][column].m_isPossibleCount == 1 && m_data[row][column].isBitON(value))
		//	return true;

		if (!m_data[row][column].isBitON(value))
			return false;
		//else if(m_data[row][column].m_isPossibleCount == 1)
		//	return true;

		m_data[row][column].m_value = value;
		// Set only m_value'th bit OFF
		m_data[row][column].setBitOFF(value);
		BigIntegerBitCollection valuesToEliminate = m_data[row][column].m_isPossible;
		// Set all bits OFF as this cell is solved
		m_data[row][column].m_isPossible.resetAllDigits();
		m_data[row][column].m_isPossible.setnthBitON(value);
		m_data[row][column].m_isPossibleCount = 1;

		if (!removeFromPossibleValuesOfAllPeers(row, column, value))
			return false;

		int size = m_data.size();
		for (int bitIndex = 1; bitIndex <= size; ++bitIndex)
		{
			if (/*bitIndex != value && */ valuesToEliminate.isnthBitON(bitIndex)
				&& !checkIfOnlyOnePeerHasThisPossibleValue(row, column, bitIndex))
				return false;
		}

		++m_cluesCount;
		return true;
	}

	bool SudokuMatrix5::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions)
	{
		int size = m_data.size();
		int minPossibleCount = size + 1;
		std::pair<int, int> obj(-1, -1);
		for (int i = 0; i < size && minPossibleCount != 2; ++i)
		{
			for (int j = 0; j < size && minPossibleCount != 2; ++j)
			{
				if (m_data[i][j].m_isPossibleCount > 1 && m_data[i][j].m_isPossibleCount < minPossibleCount)
				{
					obj = std::pair<int, int>(i, j);
					minPossibleCount = m_data[i][j].m_isPossibleCount;
				}
			}
		}

		if (obj.first == -1 || m_stats.m_valuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
		{
			vector< vector<int> > solution(size, vector<int>(size, 0));
			for (int i = 0; i < size; ++i)
				for (int j = 0; j < size; ++j)
					solution[i][j] = m_data[i][j].m_value;

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
		//SudokuMatrix5 s(*this);
		for (int i = 0; i < size; ++i)
		{
			if (m_data[row][column].isBitON(i + 1))
			{
				++m_stats.m_valuesTried;

				//if (m_countValuesTried > SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
				//	return false;

				//SudokuPuzzleUtils::getLogger() << "\nTrying Value: " << i + 1;

				//copyDataFromTo(m_data, s.m_data);
				//vector< vector<Cell> > copy(dataIn);
				SudokuMatrix5 s(*this);

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
