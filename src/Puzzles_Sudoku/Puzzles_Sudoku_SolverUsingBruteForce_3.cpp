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

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_3.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"

namespace mm {

	Cell3::Cell3(const int size)
		: m_value(0), 
		m_isPossible(),  // 0b111111111 = 0x1FF = 511 = 2^9 - 1 = (1 << 9) - 1; // int having only righmost 9 bits ON, rest bits OFF
		m_isPossibleCount(size)
	{
		m_isPossible.resize(size / BigIntegerBitCollection::BITS_IN_ONE_DIGIT + 1, 0);
		m_isPossible.setAllRightmostBitsON(size);
	}

	Cell3::Cell3(const Cell3& obj)
		: m_value(obj.m_value),
		m_isPossible(obj.m_isPossible),
		m_isPossibleCount(obj.m_isPossibleCount)
	{
	}

	int Cell3::getOnBitPosition()
	{
		//Ideally there should be only one bit ON
		//MyAssert::myRunTimeAssert((m_isPossible & (m_isPossible - 1)) == 0);
		//MyAssert::myRunTimeAssert(m_isPossible.isOnlyOneBitON());
		if (m_isPossible.isOnlyOneBitON())
			return m_isPossible.getOnBitPosition();
		else
			return 0;
	}

	bool Cell3::isBitON(int index) const
	{
		return m_isPossible.isnthBitON(index);
	}

	void Cell3::setBitOFF(int index)
	{
		m_isPossible.setnthBitOFF(index);
	}

	bool SudokuMatrix3::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		SudokuMatrix3 obj(dataIn, stats);
		if (obj.m_isValid && obj.solve(solutionSetsOut, numSolutions))
			return true;
		else
			return false;
	}

	SudokuMatrix3::SudokuMatrix3(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
	: m_data(dataIn.size(), vector<Cell3>(dataIn.size(), Cell3(dataIn.size()))), m_isValid(true),
		m_stats(stats)
	{
		const int size = dataIn.size();
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				if (dataIn[i][j] != 0)
				{
					m_data[i][j].m_value = dataIn[i][j];
					//Set only m_value'th bit ON
					m_data[i][j].m_isPossible.resetAllDigits();
					m_data[i][j].m_isPossible.setnthBitON(dataIn[i][j]);
					m_data[i][j].m_isPossibleCount = 1;
				}
			}
		}

		// Initial reduction
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				if (m_data[i][j].m_value == 0)
					continue;
				bool propagationResult = propagateConstraints(i, j, m_data);
				if (!propagationResult)
				{
					m_isValid = false;
					return;
				}
			}
		}
	}

	void SudokuMatrix3::copyDataFromTo(const vector< vector<Cell3> >& from, vector< vector<Cell3> >& to)
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

	bool SudokuMatrix3::propagateConstraintsHelper(const int& row, const int& column, const int& currentValue, vector< vector<Cell3> >& dataIn)
	{
		if (dataIn[row][column].isBitON(currentValue))
		{
			dataIn[row][column].setBitOFF(currentValue);
			dataIn[row][column].m_isPossibleCount -= 1;

			if (dataIn[row][column].m_isPossibleCount == 0)
				return false;
			else if (dataIn[row][column].m_isPossibleCount == 1)
			{
				dataIn[row][column].m_value = dataIn[row][column].getOnBitPosition();
				if (!propagateConstraints(row, column, dataIn))
					return false;
			}
		}

		return true;
	}

	bool SudokuMatrix3::propagateConstraints(const int& row, const int& column, vector< vector<Cell3> >& dataIn)
	{
		int size = dataIn.size();
		int currentValue = dataIn[row][column].m_value;
		// Check row
		for (int k = 0; k < size; ++k)
			if (k != column && !propagateConstraintsHelper(row, k, currentValue, dataIn))
				return false;

		// Check column
		for (int k = 0; k < size; ++k)
			if (k != row && !propagateConstraintsHelper(k, column, currentValue, dataIn))
				return false;
		
		// Check box
		//int a = row - row % 3;
		//int a = (row / 3) * 3;
		int boxSize = sqrt(size);
		//int boxRow = row < 3 ? 0 : row < 6 ? 3 : 6;
		//int boxColumnStart = column < 3 ? 0 : column < 6 ? 3 : 6;
		int boxRow = row - row % boxSize;
		int boxColumnStart = column - column % boxSize;
		for (int i = 0; i < boxSize; ++i, ++boxRow)
		{
			int boxColumn = boxColumnStart;
			for (int j = 0; j < boxSize; ++j, ++boxColumn)
				if (boxRow != row && boxColumn != column && !propagateConstraintsHelper(boxRow, boxColumn, currentValue, dataIn))
					return false;
		}

		return true;
	}

	bool SudokuMatrix3::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions)
	{
		return executeStep(m_data, solutionSetsOut, numSolutions);
		//if (success)
		//	SudokuPuzzleUtils::getLogger() << "\nSuccess!\n";
		//else
		//	SudokuPuzzleUtils::getLogger() << "\nFailed!\n";
	}

	bool SudokuMatrix3::executeStep(const vector< vector<Cell3> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions)
	{
		int size = dataIn.size();
		int minPossibleCount = size + 1;
		std::pair<int, int> obj(-1, -1);
		for (int i = 0; i < size && minPossibleCount != 2; ++i)
		{
			for (int j = 0; j < size && minPossibleCount != 2; ++j)
			{
				if (dataIn[i][j].m_isPossibleCount > 1 && dataIn[i][j].m_isPossibleCount < minPossibleCount)
				{
					obj = std::pair<int, int>(i, j);
					minPossibleCount = dataIn[i][j].m_isPossibleCount;
				}
			}
		}

		if (obj.first == -1 || m_stats.m_valuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
		{
			vector< vector<int> > solution(size, vector<int>(size, 0));
			for (int i = 0; i < size; ++i)
				for (int j = 0; j < size; ++j)
					solution[i][j] = dataIn[i][j].m_value;

			solutionSetsOut.push_back(std::move(solution));
			return true;
		}

		bool success = false;
		int row = obj.first;
		int column = obj.second;
		const Cell3& cell = dataIn[row][column];

		++m_stats.m_cellsTriedOrLinkUpdates;

		//int currentPossibleValues = Cell3.m_isPossible.size();
		//vector< vector<Cell3> > copy(dataIn);
		for (int i = 0; i < size; ++i)
		{
			if (cell.isBitON(i + 1))
			{
				++m_stats.m_valuesTried;
				//if (m_countValuesTried > SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
				//	return false;

				//copyDataFromTo(dataIn, copy);
				vector< vector<Cell3> > copy(dataIn);
				copy[row][column].m_value = i + 1;
				copy[row][column].m_isPossible.resetAllDigits();
				copy[row][column].m_isPossible.setnthBitON(i + 1);
				copy[row][column].m_isPossibleCount = 1;

				bool validationSuccessful = propagateConstraints(row, column, copy);
				if (!validationSuccessful)
				{
					++m_stats.m_wrongGuesses;
					continue;
				}

				success = executeStep(copy, solutionSetsOut, numSolutions);
				if (success && solutionSetsOut.size() == numSolutions)
					break;

				++m_stats.m_wrongGuesses;
			}
		}

		return (solutionSetsOut.size() != 0);
	}

}
