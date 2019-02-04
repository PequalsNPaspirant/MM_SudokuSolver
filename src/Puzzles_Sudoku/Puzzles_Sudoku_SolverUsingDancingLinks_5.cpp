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

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingDancingLinks_5.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"

namespace mm {

	FastestDancingLinksSudokuMatrix::DancingLinksNode* FastestDancingLinksSudokuMatrix::m_masterHeader = nullptr;
	vector<FastestDancingLinksSudokuMatrix::DancingLinksNode*> FastestDancingLinksSudokuMatrix::m_rowsHeaders(0);
	FastestDancingLinksSudokuMatrix::DancingLinksNode* FastestDancingLinksSudokuMatrix::m_memoryPool = nullptr;	
	int FastestDancingLinksSudokuMatrix::m_memoryPoolIndex = -1;

	void FastestDancingLinksSudokuMatrix::initialize(int size)
	{
		FastestDancingLinksSudokuMatrix::m_memoryPoolIndex = -1;
		size_t gridRows = size * size * size;
		static vector< vector<bool> > grid(0, vector<bool>(0, false)); // 729 rows x 324 columns

		if (gridRows == grid.size())
			return;

		delete[] m_memoryPool;
		int numMasterHeader = 1;
		int numColumnHeaders = size * size * 4;
		int numPossibleNodes = size * size * size * 4; // 4 columns have true in each row of matrix
		m_memoryPool = new DancingLinksNode[numMasterHeader + numColumnHeaders + numPossibleNodes];
		
		int cells = size * size;
		int boxSize = sqrt(size);			
		int gridColumns = size * size * 4;

		m_rowsHeaders.resize(gridRows);
			
		grid.clear();
		grid.resize(gridRows, vector<bool>(gridColumns, false));

		int row = 0;
		int column = 0;
		int value = 0;
		for (size_t i = 0; i < gridRows; ++i)
		{
			//int row = i / cells;
			//int column = (i % cells) / size;
			//int value = i % size;
			++value;
			if (value > size)
			{
				value = 1;
				++column;
				if (column == size)
				{
					column = 0;
					++row;
				}
			}

			//Position constraints: 0 t0 81 columns in grid[i]. Every column represents position row * size + column
			grid[i][row * size + column] = true;

			//Row constraints // 82 to 162 columns in grid[i]. Every column represents possible constraint (row, value)
			grid[i][cells + (row * size + value - 1)] = true;

			//Column constraints // 163 to 243 columns in grid[i]. Every column represents possible constraint (column, value)
			grid[i][2 * cells + (column * size + value - 1)] = true;

			//Region constraints // 244 to 324 columns in grid[i]. Every column represents possible constraint (region, value)
			int regionIndex = (row / boxSize) * boxSize + column / boxSize; // FROM 0 TO 8
			grid[i][3 * cells + (regionIndex * size + value - 1)] = true;
		}

		//Prepare the dancing links structure from constraint matrix
		m_masterHeader = FastestDancingLinksSudokuMatrix::getNewNode(nullptr, -1);
		DancingLinksNode* prev = m_masterHeader;
		for (int iColumn = 0; iColumn < gridColumns; ++iColumn)
		{
			DancingLinksNode* newNode = FastestDancingLinksSudokuMatrix::getNewNode(nullptr, iColumn);
			prev->hookRight(newNode);
			prev = newNode;
		}

		//SudokuPuzzleUtils::getLogger() << "\nStep 4: " << t.getDurationStringTillNowInNanoSeconds();
		//t.resetTimer();

		for (size_t iRow = 0; iRow < gridRows; ++iRow)
		{
			DancingLinksNode* columnHeader = m_masterHeader;
			DancingLinksNode* prev = nullptr;
			for (int iColumn = 0; iColumn < gridColumns; ++iColumn)
			{
				columnHeader = columnHeader->R;
				if (grid[iRow][iColumn] == true)
				{
					DancingLinksNode* newNode = FastestDancingLinksSudokuMatrix::getNewNode(columnHeader);
					columnHeader->U->hookDown(newNode);
					if (prev == nullptr)
					{
						prev = newNode;
						m_rowsHeaders[iRow] = newNode;
					}
					else
					{
						prev->hookRight(newNode);
						prev = newNode;
					}
				}
			}
		}
	}

	FastestDancingLinksSudokuMatrix::DancingLinksNode* FastestDancingLinksSudokuMatrix::getNewNode(DancingLinksNode* columnHeader /*= nullptr*/, int indexIn /*= 0*/)
	{
		m_memoryPool[++m_memoryPoolIndex].C = columnHeader;
		m_memoryPool[m_memoryPoolIndex].index = indexIn;
		return &m_memoryPool[m_memoryPoolIndex];
	}

	bool FastestDancingLinksSudokuMatrix::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		FastestDancingLinksSudokuMatrix obj(dataIn, stats);
		if (obj.m_isValid && obj.solve(solutionSetsOut, numSolutions))
			return true;
		else
			return false;
	}

	FastestDancingLinksSudokuMatrix::FastestDancingLinksSudokuMatrix(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats, bool findAllSolutions /*= false*/)
		: m_size(dataIn.size()), m_isValid(true), m_stats(stats)
	{
		//Timer t;
		FastestDancingLinksSudokuMatrix::initialize(m_size);

		int row = 0;
		int column = 0;
		int value = 0;
		size_t gridRows = m_size * m_size * m_size;
		for (size_t i = 0; i < gridRows; ++i)
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

			if (dataIn[row][column] == 0 || dataIn[row][column] == value)
				continue;

			DancingLinksNode* currentRowHeader = m_rowsHeaders[i];
			m_initiallyRemovedRows.push_back(currentRowHeader);
			unlinkUD(currentRowHeader);
			for (DancingLinksNode* node = currentRowHeader->R; node != currentRowHeader; node = node->R)
				unlinkUD(node);
		}

		//SudokuPuzzleUtils::getLogger() << "\nStep 3: " << t.getDurationStringTillNowInNanoSeconds();
	}

	FastestDancingLinksSudokuMatrix::~FastestDancingLinksSudokuMatrix()
	{
		for (size_t i = 0; i < m_initiallyRemovedRows.size(); ++i)
		{
			DancingLinksNode* currentRowHeader = m_initiallyRemovedRows[i];
			relinkUD(currentRowHeader);
			for (DancingLinksNode* node = currentRowHeader->R; node != currentRowHeader; node = node->R)
				relinkUD(node);
		}
	}

	bool FastestDancingLinksSudokuMatrix::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions)
	{
		if (search(0, solutionSetsOut, numSolutions))
			return true;

		return false;
	}

	bool FastestDancingLinksSudokuMatrix::search(int k, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions)
	{
		if (m_masterHeader->R == m_masterHeader || m_stats.m_valuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_DANCING_LINKS)
		{
			//Store the solution
			int cells = m_size * m_size;
			vector< vector<int> > solution(m_size, vector<int>(m_size, 0));
			for (size_t i = 0; i < m_solutionNodes.size(); i++)
			{
				DancingLinksNode* currentRow = m_solutionNodes[i];
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
				solution[rowIndex][columnIndex] = value;
			}
			solutionSetsOut.push_back(std::move(solution));
			return true;
		}

		bool result = false;
		int minCount = m_size * m_size * m_size;
		DancingLinksNode* columnHeaderSelected = nullptr;
		for (DancingLinksNode* columnHeader = m_masterHeader->R; columnHeader != m_masterHeader; columnHeader = columnHeader->R)
		{
			if (columnHeader->size < minCount)
			{
				minCount = columnHeader->size;
				columnHeaderSelected = columnHeader;
			}
		}

		++m_stats.m_valuesTried;
		//if (m_countValuesTried > SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_DANCING_LINKS)
		//	return false;

		cover(columnHeaderSelected);

		for (DancingLinksNode* row = columnHeaderSelected->D; row != columnHeaderSelected; row = row->D)
		{
			m_solutionNodes.push_back(row);
			for (DancingLinksNode* column = row->R; column != row; column = column->R)
				cover(column->C);

			result = search(k + 1, solutionSetsOut, numSolutions);

			vector<DancingLinksNode*>::iterator it = m_solutionNodes.erase(m_solutionNodes.end() - 1);
			//row = (*it);
			//columnHeaderSelected = row->C;

			for (DancingLinksNode* column = row->L; column != row; column = column->L)
				uncover(column->C);

			if (result && solutionSetsOut.size() == numSolutions)
				break;
		}

		uncover(columnHeaderSelected);

		return result;
	}

	void FastestDancingLinksSudokuMatrix::cover(DancingLinksNode* columnNode)
	{
		unlinkLR(columnNode);
		for (DancingLinksNode* row = columnNode->D; row != columnNode; row = row->D)
			for (DancingLinksNode* column = row->R; column != row; column = column->R)
				unlinkUD(column);
	}

	void FastestDancingLinksSudokuMatrix::uncover(DancingLinksNode* columnNode)
	{
		for (DancingLinksNode* row = columnNode->U; row != columnNode; row = row->U)
			for (DancingLinksNode* column = row->L; column != row; column = column->L)
				relinkUD(column);

		relinkLR(columnNode);
	}

}
