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
#include <cmath>
using namespace std;

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingDancingLinks_1.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"

namespace mm {
	int SudokuMatrixDancingLinks1::m_countLinkUpdates = 0;
	int SudokuMatrixDancingLinks1::m_countValuesTried = 0;

	bool SudokuMatrixDancingLinks1::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		SudokuMatrixDancingLinks1 obj(dataIn);
		return obj.execute(solutionSetsOut[0]);
	}

	SudokuMatrixDancingLinks1::SudokuMatrixDancingLinks1(const vector< vector<int> >& dataIn, bool findAllSolutions /*= false*/)
		: m_findAllSolutions(findAllSolutions), m_size(dataIn.size())
	{
		m_countLinkUpdates = 0;
		m_countValuesTried = 0;

		int cells = m_size * m_size;
		int boxSize = sqrt(m_size);
		int gridRows = m_size * m_size * m_size;
		int gridColumns = m_size * m_size * 4;
		vector< vector<bool> > grid(gridRows, vector<bool>(gridColumns, false)); // 729 rows x 324 columns
		int row = 0;
		int column = 0;
		int value = 0;
		for (size_t i = 0; i < grid.size(); ++i)
		{
			//int row = i / cells;
			//int column = (i % cells) / size;
			//int value = i % size;
			++value;
			if (value > m_size)
			{
				value = 1;
				++column;
				if (column == m_size)
				{
					column = 0;
					++row;
				}
			}

			//Position constraints: 0 t0 81 columns in grid[i]. Every column represents position row * size + column
			grid[i][row * m_size + column] = true;

			//Row constraints // 82 to 162 columns in grid[i]. Every column represents possible constraint (row, value)
			grid[i][cells + (row * m_size + value - 1)] = true;

			//Column constraints // 163 to 243 columns in grid[i]. Every column represents possible constraint (column, value)
			grid[i][2 * cells + (column * m_size + value - 1)] = true;

			//Region constraints // 244 to 324 columns in grid[i]. Every column represents possible constraint (region, value)
			int regionIndex = (row / boxSize) * boxSize + column / boxSize; // FROM 0 TO 8
			grid[i][3 * cells + (regionIndex * m_size + value - 1)] = true;
		}

		//Apply Sudoku clues to above constraint matrix
		for (int row = 0; row < m_size; ++row)
		{
			for (int column = 0; column < m_size; ++column)
			{
				if (dataIn[row][column] == 0)
					continue;

				int value = dataIn[row][column];
				int gridIndexStart = (row * m_size + column) * m_size;
				int regionIndex = (row / boxSize) * boxSize + column / boxSize; // FROM 0 TO 8

				for (int i = 0; i < m_size; i++)
				{
					if (i == (value - 1))
						continue;

					//Modify Position constraints
					grid[gridIndexStart + i][row * m_size + column] = false;

					//Modify Row constraints
					grid[gridIndexStart + i][cells + (row * m_size + i)] = false;

					//Modify Column constraints
					grid[gridIndexStart + i][2 * cells + (column * m_size + i)] = false;

					//Modify Region constraints
					grid[gridIndexStart + i][3 * cells + (regionIndex * m_size + i)] = false;
				}
			}
		}

		//Prepare the dancing links structure from constraint matrix
		m_masterHeader = new DancingLinksColumnNode(0);
		DancingLinksNode* prev = m_masterHeader;
		for (int iColumn = 0; iColumn < gridColumns; ++iColumn)
		{
			DancingLinksColumnNode* newNode = new DancingLinksColumnNode(iColumn);
			prev->hookRight(newNode);
			prev = newNode;
		}
		//complete the circular link
		//prev->hookRight(masterHeader);

		for (int iRow = 0; iRow < gridRows; ++iRow)
		{
			DancingLinksColumnNode* columnHeader = m_masterHeader;
			DancingLinksNode* prev = nullptr;
			for (int iColumn = 0; iColumn < gridColumns; ++iColumn)
			{
				columnHeader = static_cast<DancingLinksColumnNode*>(columnHeader->R);
				if (grid[iRow][iColumn] == true)
				{
					DancingLinksNode* newNode = new DancingLinksNode(columnHeader);
					columnHeader->U->hookDown(newNode);
					if (prev == nullptr)
						prev = newNode;
					else
					{
						prev->hookRight(newNode);
						prev = newNode;
					}					
				}
			}
			//complete the circular link
			//prev->hookRight(masterHeader);
		}
	}

	SudokuMatrixDancingLinks1::~SudokuMatrixDancingLinks1()
	{

	}

	bool SudokuMatrixDancingLinks1::execute(vector< vector<int> >& solutionOut)
	{
		if (search(0))
		{
			int cells = m_size * m_size;
			for (size_t i = 0; i < m_solution.size(); i++)
			{
				DancingLinksNode* currentRow = m_solution[i];
				int minIndex = currentRow->C->index;
				DancingLinksNode* targetRow = currentRow;
				for (DancingLinksNode* row = currentRow->R; row != currentRow; row = row->R)
				{
					if (minIndex > row->C->index)
					{
						minIndex = row->C->index;
						targetRow = row;
					}
				}

				int index = targetRow->C->index;
				int rowIndex = index / m_size;
				int columnIndex = index % m_size;
				int value = targetRow->R->C->index - cells - (rowIndex * m_size) + 1;
				solutionOut[rowIndex][columnIndex] = value;
			}
			return true;
		}

		return false;
	}

	bool SudokuMatrixDancingLinks1::search(int k)
	{
		if (m_masterHeader->R == m_masterHeader)
			return true;

		bool result = false;
		int minCount = m_size * m_size * m_size;
		DancingLinksColumnNode* columnHeaderSelected = nullptr;
		for (DancingLinksNode* columnHeader = m_masterHeader->R; columnHeader != m_masterHeader; columnHeader = columnHeader->R)
		{
			if (((DancingLinksColumnNode*)columnHeader)->size < minCount)
			{
				minCount = ((DancingLinksColumnNode*)columnHeader)->size;
				columnHeaderSelected = (DancingLinksColumnNode*)columnHeader;
			}
		}

		++m_countValuesTried;

		cover(columnHeaderSelected);

		for (DancingLinksNode* row = columnHeaderSelected->D; row != columnHeaderSelected; row = row->D)
		{
			m_solution.push_back(row);
			for (DancingLinksNode* column = row->R; column != row; column = column->R)
				cover(column->C);

			if ((result = search(k + 1)))
				return true;

			vector<DancingLinksNode*>::iterator it = m_solution.erase(m_solution.end() - 1);
			//row = (*it);
			//columnHeaderSelected = row->C;

			for (DancingLinksNode* column = row->L; column != row; column = column->L)
				uncover(column->C);

			if (result && !m_findAllSolutions)
				break;
		}
		uncover(columnHeaderSelected);

		return result;
	}

	void SudokuMatrixDancingLinks1::cover(DancingLinksColumnNode* columnNode)
	{
		columnNode->unlinkLR();
		for (DancingLinksNode* row = columnNode->D; row != columnNode; row = row->D)
			for (DancingLinksNode* column = row->R; column != row; column = column->R)
				column->unlinkUD();
	}

	void SudokuMatrixDancingLinks1::uncover(DancingLinksColumnNode* columnNode)
	{
		for (DancingLinksNode* row = columnNode->U; row != columnNode; row = row->U)
			for (DancingLinksNode* column = row->L; column != row; column = column->L)
				column->relinkUD();

		columnNode->relinkLR();
	}

}
