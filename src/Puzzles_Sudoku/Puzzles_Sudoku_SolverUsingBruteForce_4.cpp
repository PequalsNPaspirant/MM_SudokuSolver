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

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_4.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"

namespace mm {

	SudokuMatrix4::Cell::Cell(const int size)
		: m_value(0), 
		m_isPossible(),  // 0b111111111 = 0x1FF = 511 = 2^9 - 1 = (1 << 9) - 1; // int having only righmost 9 bits ON, rest bits OFF
		m_isPossibleCount(size)
	{
		m_isPossible.resize(size / BigIntegerBitCollection::BITS_IN_ONE_DIGIT + 1, 0);
		m_isPossible.setAllRightmostBitsON(size);
	}

	SudokuMatrix4::Cell::Cell(const Cell& obj)
		: m_value(obj.m_value),
		m_isPossible(obj.m_isPossible),
		m_isPossibleCount(obj.m_isPossibleCount)
	{
	}

	const SudokuMatrix4::Cell& SudokuMatrix4::Cell::operator=(const Cell& obj)
	{
		m_value = obj.m_value;
		m_isPossible = obj.m_isPossible;
		m_isPossibleCount = obj.m_isPossibleCount;

		return *this;
	}

	int SudokuMatrix4::Cell::getOnBitPosition()
	{
		//Ideally there should be only one bit ON
		//MyAssert::myRunTimeAssert((m_isPossible & (m_isPossible - 1)) == 0);
		//MyAssert::myRunTimeAssert(m_isPossible.isOnlyOneBitON());

		if (m_isPossible.isOnlyOneBitON())
			return m_isPossible.getOnBitPosition();
		else
			return 0;
	}

	bool SudokuMatrix4::Cell::isBitON(int index) const
	{
		return m_isPossible.isnthBitON(index);
	}

	void SudokuMatrix4::Cell::setBitOFF(int index)
	{
		m_isPossible.setnthBitOFF(index);
	}

	bool SudokuMatrix4::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		SudokuMatrix4 obj(dataIn, stats);
		if (obj.m_isValid && obj.solve(solutionSetsOut, numSolutions))
			return true;
		else
			return false;
	}

	SudokuMatrix4::SudokuMatrix4(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
	: m_data(dataIn.size(), vector<Cell>(dataIn.size(), Cell(dataIn.size()))), m_isValid(true),
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
	}

	SudokuMatrix4::SudokuMatrix4(const SudokuMatrix4& copy)
		: m_data(copy.m_data), m_isValid(copy.m_isValid), m_stats(copy.m_stats)
	{				
	}

	bool SudokuMatrix4::eliminate(const int& row, const int& column, const int& valueToEliminate)
	{
		if (!m_data[row][column].isBitON(valueToEliminate))
			return true;

		m_data[row][column].setBitOFF(valueToEliminate);
		m_data[row][column].m_isPossibleCount -= 1;
		int size = m_data.size();

		int boxSize = sqrt(size);
		int boxRowStart = 0;
		int boxColumnStart = 0;

		// #1: Search for naked single

		if (m_data[row][column].m_isPossibleCount == 0)
			return false;
		else if (m_data[row][column].m_isPossibleCount == 1)
		{
			int value = m_data[row][column].getOnBitPosition();
			if (m_data[row][column].m_value != 0)
				MyAssert::myRunTimeAssert(false);

			m_data[row][column].m_value = value;

			/*
			When we find a naked single in any cell i.e. m_data[row, column], we assign it to this cell and eliminate it from all its peers i.e. propagate this constraint to its peers.
			While eliminating this value from all its peers, we search for hidden single in the peers of cells from where we are eliminating the value.
			This is little overhead.
			If we find any naked single, we eliminate this value from all its peers i.e. from 3*n cells.
			For each of these 3*n cells, we eliminate this value and check for hidden single (of the same value) in all its peers 3*n. This takes 3*n * 3*n cell searches.
			In fact, we just need to search for hidden single in entire grid except the row, column and box where "value" is assigned, because we have already found it in this cell, 
			so this can be done in total
				= n rows - the row in which the value is 
				  + n columns - the column in which the value is 
				  + sqrt(n) boxes that falls on row - 1 box in which value is 
				  + sqrt(n) boxes that falls on column - 1 box in which value is 
			times
			The above optimization is implemented in class SudokuMatrix5 in file Puzzles_Sudoku_SolverUsingBruteForce_5.cpp
			*/

			// Check row
			for (int k = 0; k < size; ++k)
				if (k != column && !eliminate(row, k, value))
					return false;

			// Check column
			for (int k = 0; k < size; ++k)
				if (k != row && !eliminate(k, column, value))
					return false;

			// Check box
			//int a = row - row % 3;
			//int a = (row / 3) * 3;
			boxRowStart = row - row % boxSize;
			boxColumnStart = column - column % boxSize;
			int boxRow = boxRowStart;
			for (int i = 0; i < boxSize; ++i, ++boxRow)
			{
				int boxColumn = boxColumnStart;
				for (int j = 0; j < boxSize; ++j, ++boxColumn)
					if (boxRow != row && boxColumn != column && !eliminate(boxRow, boxColumn, value))
						return false;
			}
		}

		// #2: Search for hidden single

		int targetRowIndex = 0;
		int targetColumnIndex = 0;

		// Check row
		int counter = 0;
		for (int k = 0; k < size && counter < 2; ++k)
			//if (k != column && m_data[row][k].isBitON(valueToEliminate - 1))
			if (m_data[row][k].isBitON(valueToEliminate))
			{
				++counter;
				targetColumnIndex = k;
			}

		if (counter == 0)
			return false;

		if (counter == 1)// && m_data[row][targetColumnIndex].m_isPossibleCount > 1)
			if (!assign(row, targetColumnIndex, valueToEliminate))
				return false;

		// Check column
		counter = 0;
		for (int k = 0; k < size && counter < 2; ++k)
			//if (k != row && m_data[k][column].isBitON(valueToEliminate - 1))
			if (m_data[k][column].isBitON(valueToEliminate))
			{
				++counter;
				targetRowIndex = k;
			}

		if (counter == 0)
			return false;

		if (counter == 1)// && m_data[targetRowIndex][column].m_isPossibleCount > 1)
			if (!assign(targetRowIndex, column, valueToEliminate))
				return false;

		// Check box
		counter = 0;
		int boxRow = boxRowStart;
		for (int i = 0; i < boxSize; ++i, ++boxRow)
		{
			int boxColumn = boxColumnStart;
			for (int j = 0; j < boxSize; ++j, ++boxColumn)
				//if (!(a == row && b == column) && m_data[a][b].isBitON(valueToEliminate - 1))
				if (m_data[boxRow][boxColumn].isBitON(valueToEliminate))
				{
					++counter;
					targetRowIndex = boxRow;
					targetColumnIndex = boxColumn;
				}
		}

		if (counter == 0)
			return false;
		if (counter == 1)// && m_data[targetRowIndex][targetColumnIndex].m_isPossibleCount > 1)
			if (!assign(targetRowIndex, targetColumnIndex, valueToEliminate))
				return false;

		return true;
	}

	bool SudokuMatrix4::assign(const int& row, const int& column, const int& value)
	{
		if (!m_data[row][column].isBitON(value))
			return false;

		//m_data[row][column].m_value = value;

		//SudokuPuzzleUtils::getLogger() << "\nassign: cell[ " << row << ", " << column << " ] = " << value;

		int size = m_data.size();
		for (int i = 0; i < size; i++)
		{
			if (i != (value - 1) && !eliminate(row, column, i + 1))
				return false;
		}

		return true;
	}

	bool SudokuMatrix4::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions)
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
		const Cell& cell = m_data[row][column];

		++m_stats.m_cellsTriedOrLinkUpdates;
		//SudokuPuzzleUtils::getLogger() << "\nTrying cell[ " << row << ", " << column << "]";

		//int currentPossibleValues = Cell.m_isPossible.size();
		//vector< vector<Cell> > copy(dataIn);
		for (int i = 0; i < size; ++i)
		{
			if (cell.isBitON(i + 1))
			{
				++m_stats.m_valuesTried;
				//if (m_countValuesTried > SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
				//	return false;
				//SudokuPuzzleUtils::getLogger() << "\nTrying Value: " << i + 1;

				//copyDataFromTo(dataIn, copy);
				//vector< vector<Cell> > copy(dataIn);
				SudokuMatrix4 s(*this);

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
