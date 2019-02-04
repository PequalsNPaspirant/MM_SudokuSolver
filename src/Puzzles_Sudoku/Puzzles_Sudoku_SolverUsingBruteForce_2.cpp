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
using namespace std;

#include <windows.h>

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_2.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"

namespace mm {

	Cell2::Cell2()
		: m_value(0), m_isPossible(9, true), m_isPossibleCount(9), originalClue(false)
	{
	}

	Cell2::Cell2(const Cell2& obj)
		: m_value(obj.m_value), originalClue(obj.originalClue)
	{
		m_isPossible = obj.m_isPossible;
		m_isPossibleCount = obj.m_isPossibleCount;
	}

	bool Cell2::setFalse(int index)
	{
		bool countChanged = false;
		if (m_isPossible[index])
		{
			countChanged = true;
			m_isPossible[index] = false;
			--m_isPossibleCount;

			if (m_isPossibleCount == 1)
			{
				for(size_t i = 0; i < m_isPossible.size(); i++)
					if (m_isPossible[i])
					{
						m_value = i + 1;
						break;
					}
			}
		}

		return countChanged;
	}

	bool SudokuMatrix2::solve(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut)
	{
		SudokuMatrix2 obj(dataIn);
		if (obj.m_isValid && obj.solve(solutionOut))
			return true;
		else
			return false;
	}

	SudokuMatrix2::SudokuMatrix2(const vector< vector<int> >& dataIn)
	: m_data(9, vector<Cell2>(9)), m_countCellsTried(0), m_countValuesTried(0)
	{
		int cellsPresent = 0;
		int size = dataIn.size();
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (dataIn[i][j] != 0)
				{
					m_data[i][j].m_value = dataIn[i][j];
					for (int k = 0; k < size; k++)
						if (k != (dataIn[i][j] - 1))
							m_data[i][j].m_isPossible[k] = false;
					
					m_data[i][j].m_isPossibleCount = 1;

					m_data[i][j].originalClue = true;
					++cellsPresent;
				}
			}
		}

		// Initial reduction
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (m_data[i][j].m_value == 0)
					continue;
				bool propagationResult = propagateConstraints(i, j, m_data);
				if (!propagationResult)
				{
					SudokuPuzzleUtils::getLogger() << "\nInvalid Sudoku Puzzle\n";
					return;
				}
			}
		}

		// State after initial reduction
		//printColourful(m_data);
	}

	void SudokuMatrix2::copyDataFromTo(const vector< vector<Cell2> >& from, vector< vector<Cell2> >& to)
	{
		int size = from.size();
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				to[i][j].m_value = from[i][j].m_value;
				//to[i][j].isEmpty = from[i][j].isEmpty;

				//if (from[i][j].isEmpty)
				{
					to[i][j].m_isPossibleCount = from[i][j].m_isPossibleCount;
					for (int k = 0; k < size; k++)
						to[i][j].m_isPossible[k] = from[i][j].m_isPossible[k];
				}
			}
		}
	}
	
	bool SudokuMatrix2::propagateConstraints(int row, int column, vector< vector<Cell2> >& dataIn)
	{
		//if (!validateIntermediateSteps(dataIn))
		{
			//SudokuPuzzleUtils::getLogger() << "\nSome Zol-zal.....\n";
			//printColourful(dataIn);
			//MyAssert::myRunTimeAssert(false);
		}

		int size = dataIn.size();
		int currentValue = dataIn[row][column].m_value;
		// Check row
		for (int k = 0; k < size; k++)
		{
			if (k != column)
			{
				bool countChanged = dataIn[row][k].setFalse(currentValue - 1);
				if (countChanged)
				{
					if (dataIn[row][k].m_isPossibleCount == 0)
						return false;
					else if (dataIn[row][k].m_isPossibleCount == 1 && !propagateConstraints(row, k, dataIn))
						return false;
				}
			}
		}
		// Check column
		for (int k = 0; k < size; k++)
		{
			if (k != row)
			{
				bool countChanged = dataIn[k][column].setFalse(currentValue - 1);
				if (countChanged)
				{
					if (dataIn[k][column].m_isPossibleCount == 0)
						return false;
					else if (dataIn[k][column].m_isPossibleCount == 1 && !propagateConstraints(k, column, dataIn))
						return false;
				}
				
			}
		}
		//check box
		for (int a = row - row % 3, i = 0; i < 3; a++, i++)
		{
			for (int b = column - column % 3, j = 0; j < 3; b++, j++)
			{
				if (a != row && b != column)
				{
					bool countChanged = dataIn[a][b].setFalse(currentValue - 1);
					if (countChanged)
					{
						if (dataIn[a][b].m_isPossibleCount == 0)
							return false;
						else if (dataIn[a][b].m_isPossibleCount == 1 && !propagateConstraints(a, b, dataIn))
							return false;
					}
					
				}
			}
		}

		return true;
	}

	bool SudokuMatrix2::validateIntermediateSteps(const vector< vector<Cell2> >& dataIn)
	{
		int size = dataIn.size();
		unsigned int bits;
		unsigned int initialVal = (1 << 9) - 1; // int having only righmost 9 bits ON, rest bits OFF

												// Validate each row
		for (int row = 0; row < size; row++)
		{
			bits = initialVal;
			for (int i = 0; i < size; i++)
			{
				int value = dataIn[row][i].m_value;
				if (value == 0)
					continue;
				if ((bits & (1 << (value - 1))) > 0) // Check if this bit is ON
					bits &= ~(1 << (value - 1)); // Make rightmost (i + 1)th bit OFF
				else
				{
					SudokuPuzzleUtils::getLogger() << "\nvalidateIntermediateSteps on row " << row + 1 << " failed due to duplicate value of " << value << "\n";
					return false;
				}
			}
		}

		// Validate each column
		for (int column = 0; column < size; column++)
		{
			bits = initialVal;
			for (int i = 0; i < size; i++)
			{
				int value = dataIn[i][column].m_value;
				if (value == 0)
					continue;
				if ((bits & (1 << (value - 1))) > 0) // Check if this bit is ON
					bits &= ~(1 << (value - 1)); // Make rightmost (i + 1)th bit OFF
				else
				{
					SudokuPuzzleUtils::getLogger() << "\nvalidateIntermediateSteps on column " << column + 1 << " failed due to duplicate value of " << value << "\n";
					return false;
				}
			}
		}

		// Validate each box
		for (int rowStart = 0; rowStart < size; rowStart += 3)
			for (int columnStart = 0; columnStart < size; columnStart += 3)
			{
				bits = initialVal;
				for (int row = rowStart; row < rowStart + 3; row++)
					for (int column = columnStart; column < columnStart + 3; column++)
					{
						int value = dataIn[row][column].m_value;
						if (value == 0)
							continue;
						if ((bits & (1 << (value - 1))) > 0) // Check if this bit is ON
							bits &= ~(1 << (value - 1)); // Make rightmost (i + 1)th bit OFF
						else
						{
							SudokuPuzzleUtils::getLogger() << "\nvalidateIntermediateSteps on box at (" << rowStart + 1 << ", " << columnStart + 1 << ") failed due to duplicate value of " << value << "\n";
							return false;
						}
					}
			}

		//SudokuPuzzleUtils::getLogger() << "\nvalidateIntermediateSteps successful!";
		return true;
	}

	void SudokuMatrix2::printColourful(const vector< vector<Cell2> >& dataIn)
	{
		SudokuPuzzleUtils::getLogger() << "\n";
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		//size_t dim = dataIn.size();
		const string sep(6, '-');
		for (int i = 0; i < 9; i++)
		{
			if (i == 3 || i == 6)
				SudokuPuzzleUtils::getLogger() << sep << "+-" << sep << "+" << sep << "\n";

			for (int j = 0; j < 9; j++)
			{
				if (j == 3 || j == 6)
					SudokuPuzzleUtils::getLogger() << "| ";
				if (dataIn[i][j].originalClue)
					SudokuPuzzleUtils::getLogger() << dataIn[i][j].m_value << ' ';
				else
				{
					SetConsoleTextAttribute(hConsole, 14);
					SudokuPuzzleUtils::getLogger() << dataIn[i][j].m_value << ' ';
					SetConsoleTextAttribute(hConsole, 15); // //set 15 to black background and white text
				}
			}
			SudokuPuzzleUtils::getLogger() << "\n";
		}
	}

	bool SudokuMatrix2::solve(vector< vector<int> >& solutionOut)
	{
		bool success = executeStep(m_data, solutionOut);
		//if (success)
		//	SudokuPuzzleUtils::getLogger() << "\nSuccess!\n";
		//else
		//	SudokuPuzzleUtils::getLogger() << "\nFailed!\n";
		return success;
	}

	bool SudokuMatrix2::executeStep(const vector< vector<Cell2> >& dataIn, vector< vector<int> >& solutionOut)
	{
		int size = dataIn.size();
		int minPossibleCount = size + 1;
		std::pair<int, int> obj(-1, -1);
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (dataIn[i][j].m_isPossibleCount > 1 && dataIn[i][j].m_isPossibleCount < minPossibleCount)
				{
					obj = std::pair<int, int>(i, j);
					minPossibleCount = dataIn[i][j].m_isPossibleCount;
				}
			}
		}

		if (obj.first == -1)
		{
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					solutionOut[i][j] = dataIn[i][j].m_value;

			//printColourful(dataIn);
			//validate(dataIn);
			SudokuPuzzleUtils::getLogger() << "\nNo. of total cells tried: " << m_countCellsTried;
			SudokuPuzzleUtils::getLogger() << "\nNo. of total values tried: " << m_countValuesTried;
			return true;
		}

		bool success = false;
		int row = obj.first;
		int column = obj.second;
		const Cell2& cell = dataIn[row][column];

		++m_countCellsTried;

		int currentPossibleValues = cell.m_isPossible.size();
		//vector< vector<Cell2> > copy(dataIn);
		for (int i = 0; i < currentPossibleValues; i++)
		{
			if (cell.m_isPossible[i])
			{
				++m_countValuesTried;
				//copyDataFromTo(dataIn, copy);
				vector< vector<Cell2> > copy(dataIn);
				copy[row][column].m_value = i + 1;
				for (int j = 0; j < currentPossibleValues; j++)
					if (i != j)
						copy[row][column].m_isPossible[j] = false;
				copy[row][column].m_isPossibleCount = 1;

				bool validationSuccessful = propagateConstraints(row, column, copy);
				if (!validationSuccessful)
					continue;

				success = executeStep(copy, solutionOut);
				if (success)
					break;
			}
		}

		return success;
	}

}
