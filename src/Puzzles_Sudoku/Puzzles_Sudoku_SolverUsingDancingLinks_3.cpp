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

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingDancingLinks_3.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"

namespace mm {
	SudokuMatrixDancingLinks3::DancingLinksNode* SudokuMatrixDancingLinks3::m_memoryPool = nullptr;
	vector< vector<bool> > SudokuMatrixDancingLinks3::m_grid(0, vector<bool>(0));

	int SudokuMatrixDancingLinks3::m_memoryPoolIndex = -1;
	unsigned long long SudokuMatrixDancingLinks3::m_countLinkUpdates = 0;
	unsigned long long SudokuMatrixDancingLinks3::m_countValuesTried = 0;
	int SudokuMatrixDancingLinks3::m_nodesLeaked = 0;

	void SudokuMatrixDancingLinks3::initialize(int size)
	{
		SudokuMatrixDancingLinks3::m_memoryPoolIndex = -1;
		size_t gridRows = size * size * size;
		if (gridRows == m_grid.size())
			return;

		delete[] m_memoryPool;
		int numMasterHeader = 1;
		int numColumnHeaders = size * size * 4;
		int numPossibleNodes = size * size * size * 4; // 4 columns have true in each row of matrix
		m_memoryPool = new DancingLinksNode[numMasterHeader + numColumnHeaders + numPossibleNodes];

		int cells = size * size;
		int boxSize = sqrt(size);
		int gridColumns = size * size * 4;
		if (gridRows != m_grid.size())
		{
			m_grid.clear();
			m_grid.resize(gridRows, vector<bool>(gridColumns, false)); // 729 rows x 324 columns
		}
		
		int row = 0;
		int column = 0;
		int value = 0;
		for (size_t i = 0; i < m_grid.size(); ++i)
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
			m_grid[i][row * size + column] = true;

			//Row constraints // 82 to 162 columns in grid[i]. Every column represents possible constraint (row, value)
			m_grid[i][cells + (row * size + value - 1)] = true;

			//Column constraints // 163 to 243 columns in grid[i]. Every column represents possible constraint (column, value)
			m_grid[i][2 * cells + (column * size + value - 1)] = true;

			//Region constraints // 244 to 324 columns in grid[i]. Every column represents possible constraint (region, value)
			int regionIndex = (row / boxSize) * boxSize + column / boxSize; // FROM 0 TO 8
			m_grid[i][3 * cells + (regionIndex * size + value - 1)] = true;
		}
	}

	SudokuMatrixDancingLinks3::DancingLinksNode* SudokuMatrixDancingLinks3::getNewNode(DancingLinksNode* columnHeader /*= nullptr*/, int indexIn /*= 0*/)
	{
		m_memoryPool[++m_memoryPoolIndex].C = columnHeader;
		m_memoryPool[m_memoryPoolIndex].index = indexIn;
		return &m_memoryPool[m_memoryPoolIndex];
	}

	bool SudokuMatrixDancingLinks3::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		SudokuMatrixDancingLinks3 obj(dataIn);
		if (obj.m_isValid && obj.solve(solutionSetsOut, numSolutions))
		{
			stats.m_cellsTriedOrLinkUpdates = obj.getPerformanceParameter();
			stats.m_valuesTried = obj.getValuesTried();
			return true;
		}
		else
			return false;
	}

	SudokuMatrixDancingLinks3::SudokuMatrixDancingLinks3(const vector< vector<int> >& dataIn, bool findAllSolutions /*= false*/)
		: m_size(dataIn.size()), m_isValid(true)
	{
		//Timer t;
		SudokuMatrixDancingLinks3::initialize(m_size);
		m_countLinkUpdates = 0;
		m_countValuesTried = 0;
		m_nodesLeaked = 0;

		//SudokuPuzzleUtils::getLogger() << "\nStep 1: " << t.getDurationStringTillNowInNanoSeconds();
		//t.resetTimer();
		vector< vector<bool> > grid = SudokuMatrixDancingLinks3::m_grid;
		//SudokuPuzzleUtils::getLogger() << "\nStep 2: " << t.getDurationStringTillNowInNanoSeconds();
		//t.resetTimer();
		int boxSize = sqrt(m_size);
		int cells = m_size * m_size;

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

		//SudokuPuzzleUtils::getLogger() << "\nStep 3: " << t.getDurationStringTillNowInNanoSeconds();
		//t.resetTimer();

		//Prepare the dancing links structure from constraint matrix
		m_masterHeader = SudokuMatrixDancingLinks3::getNewNode(nullptr, -1);
		++m_nodesLeaked;
		DancingLinksNode* prev = m_masterHeader;
		int gridRows = grid.size();
		int gridColumns = grid[0].size();
		for (int iColumn = 0; iColumn < gridColumns; ++iColumn)
		{
			DancingLinksNode* newNode = SudokuMatrixDancingLinks3::getNewNode(nullptr, iColumn);
			++m_nodesLeaked;
			prev->hookRight(newNode);
			prev = newNode;
		}

		//SudokuPuzzleUtils::getLogger() << "\nStep 4: " << t.getDurationStringTillNowInNanoSeconds();
		//t.resetTimer();

		for (int iRow = 0; iRow < gridRows; ++iRow)
		{
			DancingLinksNode* columnHeader = m_masterHeader;
			DancingLinksNode* prev = nullptr;
			for (int iColumn = 0; iColumn < gridColumns; ++iColumn)
			{
				columnHeader = static_cast<DancingLinksNode*>(columnHeader->R);
				if (grid[iRow][iColumn] == true)
				{
					DancingLinksNode* newNode = SudokuMatrixDancingLinks3::getNewNode(columnHeader);
					++m_nodesLeaked;
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
		}

		//SudokuPuzzleUtils::getLogger() << "\nStep 5: " << t.getDurationStringTillNowInNanoSeconds();
		//t.resetTimer();

		//SudokuPuzzleUtils::getLogger() << "\nNo. of total nodes created: " << m_nodesLeaked;
	}

	SudokuMatrixDancingLinks3::~SudokuMatrixDancingLinks3()
	{
		/*
		DancingLinksNode* columnHeader = m_masterHeader->R;
		do
		{
			DancingLinksNode* row = columnHeader->D;
			do
			{
				row = row->D;
				delete row->U;
				--m_nodesLeaked;
			} while (row != columnHeader);
			
			columnHeader = columnHeader->R;
			delete columnHeader->L;
			--m_nodesLeaked;

		} while (columnHeader != m_masterHeader);

		delete m_masterHeader;
		--m_nodesLeaked;

		SudokuPuzzleUtils::getLogger() << "\nNo. of nodes leaked: " << m_nodesLeaked;
		*/

		//Reset all links
		for (int i = 0; i <= m_memoryPoolIndex; ++i)
		{
			m_memoryPool[i].L = m_memoryPool[i].R = m_memoryPool[i].U = m_memoryPool[i].D = &m_memoryPool[i];
			m_memoryPool[i].C = nullptr;
			m_memoryPool[i].index = 0;
			m_memoryPool[i].size = 0;
		}
	}

	bool SudokuMatrixDancingLinks3::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions)
	{
		if (search(0, solutionSetsOut, numSolutions))
			return true;

		return false;
	}

	bool SudokuMatrixDancingLinks3::search(int k, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions)
	{
		if (m_masterHeader->R == m_masterHeader || m_countValuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_DANCING_LINKS)
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
			if (((DancingLinksNode*)columnHeader)->size < minCount)
			{
				minCount = ((DancingLinksNode*)columnHeader)->size;
				columnHeaderSelected = (DancingLinksNode*)columnHeader;
			}
		}

		++m_countValuesTried;
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

	void SudokuMatrixDancingLinks3::cover(DancingLinksNode* columnNode)
	{
		columnNode->unlinkLR();
		for (DancingLinksNode* row = columnNode->D; row != columnNode; row = row->D)
			for (DancingLinksNode* column = row->R; column != row; column = column->R)
				column->unlinkUD();
	}

	void SudokuMatrixDancingLinks3::uncover(DancingLinksNode* columnNode)
	{
		for (DancingLinksNode* row = columnNode->U; row != columnNode; row = row->U)
			for (DancingLinksNode* column = row->L; column != row; column = column->L)
				column->relinkUD();

		columnNode->relinkLR();
	}

}
