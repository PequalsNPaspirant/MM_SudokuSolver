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

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_0.h"
#include "Assert/MyAssert.h"

namespace mm {

	Comparator::Comparator(const vector< vector<mm::Cell_0> >& dataIn)
		: m_refData(dataIn)
	{

	}

	bool Comparator::operator()(const std::pair<int, int>& left, const std::pair<int, int>& right) const
	{
		return m_refData[left.first][left.second].m_isPossibleCount < m_refData[right.first][right.second].m_isPossibleCount;
	}



	Cell_0::Cell_0()
		: m_value(0), isEmpty(true), m_isPossible(0), m_isPossibleCount(0), originalClue(false)
	{
	}

	Cell_0::Cell_0(const Cell_0& obj)
		: m_value(obj.m_value), isEmpty(obj.isEmpty), originalClue(obj.originalClue)
	{
		if (isEmpty)
		{
			m_isPossible = obj.m_isPossible;
			m_isPossibleCount = obj.m_isPossibleCount;
		}
	}

	bool Cell_0::setFalse(int index)
	{
		bool needToPropagateContraint = true;
		if (isEmpty && m_isPossible[index])
		{
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
						
				//isEmpty = false;
				//needToPropagateContraint = true;
			}

			if (m_isPossibleCount == 0)
				needToPropagateContraint = false;
		}

		return needToPropagateContraint;
	}

	void SudokuMatrix_0::solve(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut)
	{
		SudokuMatrix_0 obj(dataIn);

		obj.execute(solutionOut);
	}

	SudokuMatrix_0::SudokuMatrix_0(const vector< vector<int> >& dataIn)
	: m_data(9, vector<Cell_0>(9)), m_emptyCells(m_data)
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
					m_data[i][j].isEmpty = false;
					m_data[i][j].originalClue = true;
					++cellsPresent;
				}
				else
				{
					m_data[i][j].m_isPossible.resize(9, true);
					m_data[i][j].m_isPossibleCount = 9;
				}
			}
		}

		cout << "\nCells present: " << cellsPresent;
		cout << "\nCells to find: " << 81 - cellsPresent;

		// Initial reduction
		/*for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (!m_data[i][j].isEmpty)
				{
					bool propagationResult = propagateConstraints(i, j, m_data);
					if (!propagationResult)
					{
						cout << "\nInvalid Sudoku Puzzle\n";
						return;
					}
				}
			}
		}*/

		// Create a multiset with decreasing  m_isPossibleCount
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (m_data[i][j].isEmpty)
					m_emptyCells.insert(std::pair<int, int>(i, j));
			}
		}

		// State after initial reduction
		printColourful(m_data);
	}

	void SudokuMatrix_0::copyDataFromTo(const vector< vector<Cell_0> >& from, vector< vector<Cell_0> >& to)
	{
		int size = from.size();
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				to[i][j].m_value = from[i][j].m_value;
				to[i][j].isEmpty = from[i][j].isEmpty;

				if (from[i][j].isEmpty)
				{
					to[i][j].m_isPossibleCount = from[i][j].m_isPossibleCount;
					for (int k = 0; k < size; k++)
						to[i][j].m_isPossible[k] = from[i][j].m_isPossible[k];
				}
			}
		}
	}
	
	bool SudokuMatrix_0::propagateConstraints(int row, int column, vector< vector<Cell_0> >& dataIn)
	{
		if (!validateIntermediateSteps(dataIn))
		{
			cout << "\nSome Zol-zal.....\n";
			printColourful(dataIn);
			MyAssert::myRunTimeAssert(false);
		}

		int size = dataIn.size();
		int currentValue = dataIn[row][column].m_value;
		// Check row
		for (int k = 0; k < size; k++)
		{
			if (k != column)
			{
				bool validCell = dataIn[row][k].setFalse(currentValue - 1);
				//if (needToValidateCell && !validateSingleCell(row, k, dataIn))
				//	return false;
				if (!validCell)
					return false;
			}
		}
		// Check column
		for (int k = 0; k < size; k++)
		{
			if (k != row)
			{
				bool validCell = dataIn[k][column].setFalse(currentValue - 1);
				//if (needToValidateCell && !validateSingleCell(k, column, dataIn))
				//	return false;
				if (!validCell)
					return false;
			}
		}
		//check box
		for (int a = row - row % 3, i = 0; i < 3; a++, i++)
		{
			for (int b = column - column % 3, j = 0; j < 3; b++, j++)
			{
				if (a != row && b != column)
				{
					bool validCell = dataIn[a][b].setFalse(currentValue - 1);
					//if (needToValidateCell && !validateSingleCell(a, b, dataIn))
					//	return false;
					if (!validCell)
						return false;
				}
			}
		}

		return true;
	}

	void SudokuMatrix_0::print(const vector< vector<Cell_0> >& dataIn)
	{
		cout << "\nPrinting...\n";
		int size = dataIn.size();
		int width = 2;
		const string sep(3 * width, '-');
		for (int i = 0; i < size; i++) {
			if (i == 3 || i == 6) {
				cout << sep << "+-" << sep << "+" << sep << endl;
			}
			for (int j = 0; j < size; j++) {
				if (j == 3 || j == 6) 
					cout << "| ";
				cout << dataIn[i][j].m_value << ' ';
			}
			cout << endl;
		}
	}

	void SudokuMatrix_0::printColourful(const vector< vector<Cell_0> >& dataIn)
	{
		cout << endl;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		//size_t dim = dataIn.size();
		const string sep(6, '-');
		for (int i = 0; i < 9; i++)
		{
			if (i == 3 || i == 6)
				cout << sep << "+-" << sep << "+" << sep << endl;

			for (int j = 0; j < 9; j++)
			{
				if (j == 3 || j == 6)
					cout << "| ";
				if (dataIn[i][j].originalClue)
					cout << dataIn[i][j].m_value << ' ';
				else
				{
					SetConsoleTextAttribute(hConsole, 14);
					cout << dataIn[i][j].m_value << ' ';
					SetConsoleTextAttribute(hConsole, 15); // //set 15 to black background and white text
				}
			}
			cout << endl;
		}
	}

	void SudokuMatrix_0::execute(vector< vector<int> >& solutionOut)
	{
		bool success = executeStep(m_emptyCells.begin(), m_emptyCells.end(), m_data, solutionOut);
		if (success)
			cout << "\nSuccess!\n";
		else
			cout << "\nFailed!\n";
	}

	bool SudokuMatrix_0::executeStep(multiset<std::pair<int, int>, Comparator>::iterator itFrom, multiset<std::pair<int, int>, Comparator>::iterator itTo, const vector< vector<Cell_0> >& dataIn, vector< vector<int> >& solutionOut)
	{
		if (itFrom == itTo)
		{
			cout << "\nReached end of set...\n";
			if (validate(dataIn))
			{
				printColourful(dataIn);
				for (size_t i = 0; i < dataIn.size(); ++i)
					for (size_t j = 0; j < dataIn.size(); ++j)
						solutionOut[i][j] = dataIn[i][j].m_value;
				return true;
			}
			else
				return false;
		}

		bool success = false;
		multiset<std::pair<int, int>, Comparator>::iterator next = itFrom;
		std::pair<int, int> obj = *itFrom;
		int row = obj.first;
		int column = obj.second;
		const Cell_0& cell = dataIn[row][column];
		if (!cell.isEmpty)
		{
			cout << "\nThis is little overhead. Cell_0 at row " << row << " and column " << column << " is not eompty, but its present in set\n";
			success = executeStep(++next, itTo, dataIn, solutionOut);
		}
		else
		{
			int currentPossibleValues = cell.m_isPossible.size();
			MyAssert::myRunTimeAssert(currentPossibleValues != 0);
			//vector< vector<Cell_0> > copy(dataIn);
			for (int i = 0; i < currentPossibleValues; i++)
			{
				if (cell.m_isPossible[i])
				{
					//copyDataFromTo(dataIn, copy);
					vector< vector<Cell_0> > copy(dataIn);
					//copy[row][column].isEmpty = false;
					copy[row][column].m_value = i + 1;
					for (int j = 0; j < currentPossibleValues; j++)
						if (i != j)
							copy[row][column].m_isPossible[j] = false;
					copy[row][column].m_isPossibleCount = 1;

					//if (!validateSingleCell(row, column, copy))
					//	continue;

					bool validationSuccessful = propagateConstraints(row, column, copy);
					if (!validationSuccessful)
						continue;

					next = itFrom;
					success = executeStep(++next, itTo, copy, solutionOut);
					if (success)
						break;
				}
			}
		}

		return success;
	}

	bool SudokuMatrix_0::validate(const vector< vector<Cell_0> >& dataIn)
	{
		int size = dataIn.size();
		unsigned int bits;
		unsigned int initialVal = (1 << 9) - 1; // int having only righmost 9 bits ON, rest bits OFF

		// Validate each row
		for (int row = 0; row < size; row++)
		{
			bits = initialVal;
			for (int i = 0; i < size; i++)
				bits &= ~(1 << (dataIn[row][i].m_value - 1)); // Make rightmost (i + 1)th bit OFF
			if (bits != 0)
			{
				cout << "\nValidation on row " << row + 1 << " failed\n";
				return false;
			}
		}

		// Validate each column
		for (int column = 0; column < size; column++)
		{
			bits = initialVal;
			for (int i = 0; i < size; i++)
				bits &= ~(1 << (dataIn[i][column].m_value - 1)); // Make rightmost (i + 1)th bit OFF
			if (bits != 0)
			{
				cout << "\nValidation on column " << column + 1 << " failed\n";
				return false;
			}
		}

		// Validate each box
		for (int rowStart = 0; rowStart < size; rowStart += 3)
			for (int columnStart = 0; columnStart < size; columnStart += 3)
			{
				bits = initialVal;
				for (int row = rowStart; row < rowStart + 3; row++)
					for (int column = columnStart; column < columnStart + 3; column++)
							bits &= ~(1 << (dataIn[row][column].m_value - 1)); // Make rightmost (i + 1)th bit OFF

				if (bits != 0)
				{
					cout << "\nValidation on box at (" << rowStart + 1 << ", " << columnStart + 1 << " failed\n";
					return false;
				}
			}

		cout << "\nValidation successful!";
		return true;
	}

	bool SudokuMatrix_0::validateSingleCell(int row, int column, const vector< vector<Cell_0> >& dataIn)
	{
		int size = dataIn.size();
		int value = dataIn[row][column].m_value;

		// Validate row
		for (int k = 0; k < size; k++)
		{
			if(k != column && dataIn[row][k].m_value == value)
			{
				//cout << "\nvalidateSingleCell on row " << row + 1 << " failed due to duplicate value of " << value << "\n";
				return false;
			}
		}

		// Validate column
		for (int k = 0; k < size; k++)
		{
			if (k != row && dataIn[k][column].m_value == value)
			{
				//cout << "\nvalidateSingleCell on column " << column + 1 << " failed due to duplicate value of " << value << "\n";
				return false;
			}
		}

		// Validate box
		int rowStart = row - row % 3;
		int columnStart = column - column % 3;
		for (int i = rowStart; i < rowStart + 3; i++)
			for (int j = columnStart; j < columnStart + 3; j++)
			{
				if(i == row && j == column)
					continue;
				if(dataIn[i][j].m_value == value)
				{
					//cout << "\nvalidateSingleCell on box at (" << rowStart + 1 << ", " << columnStart + 1 << ") failed due to duplicate value of " << value << "\n";
					return false;
				}
			}

		//cout << "\nvalidateSingleCell successful!";
		return true;
	}

	bool SudokuMatrix_0::validateSingleCell2(int row, int column, const vector< vector<Cell_0> >& dataIn)
	{
		int size = dataIn.size();
		unsigned int bits;
		unsigned int initialVal = (1 << 9) - 1; // int having only righmost 9 bits ON, rest bits OFF

		// Validate row
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
				cout << "\nvalidateIntermediateSteps on row " << row + 1 << " failed due to duplicate value of " << value << "\n";
				return false;
			}
		}

		// Validate column
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
				cout << "\nvalidateIntermediateSteps on column " << column + 1 << " failed due to duplicate value of " << value << "\n";
				return false;
			}
		}

		// Validate box
		int rowStart = row - row % 3;
		int columnStart = column - column % 3;
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
					cout << "\nvalidateIntermediateSteps on box at (" << rowStart + 1 << ", " << columnStart + 1 << ") failed due to duplicate value of " << value << "\n";
					return false;
				}
			}

		cout << "\nvalidateIntermediateSteps successful!";
		return true;
	}

	bool SudokuMatrix_0::validateIntermediateSteps(const vector< vector<Cell_0> >& dataIn)
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
					cout << "\nvalidateIntermediateSteps on row " << row + 1 << " failed due to duplicate value of " << value <<"\n";
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
					cout << "\nvalidateIntermediateSteps on column " << column + 1 << " failed due to duplicate value of " << value << "\n";
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
							cout << "\nvalidateIntermediateSteps on box at (" << rowStart + 1 << ", " << columnStart + 1 << ") failed due to duplicate value of " << value << "\n";
							return false;
						}
					}
			}

		//cout << "\nvalidateIntermediateSteps successful!";
		return true;
	}

}
