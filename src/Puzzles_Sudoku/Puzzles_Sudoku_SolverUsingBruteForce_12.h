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


#pragma once

#include <vector>
#include <array>
#include <cmath>
#include <map>
#include <unordered_map>
using namespace std;

#include "Puzzles_Sudoku/Puzzles_Sudoku_GlobalLookupTablesTemplate.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_StaticBitSet_2.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"

namespace mm {

	//Forward declaraton
	struct SudokuPuzzleBasicStats;

	class FastestBruteForceSudokuMatrix_6
	{
	public:
		static bool solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats);
	};

	template<unsigned int size>
	class FastestBruteForceSudokuMatrixTemplate_6
	{
	public:
		FastestBruteForceSudokuMatrixTemplate_6(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats);
		bool solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions);

	private:
		bool checkIfNakedSingle(const StaticBitSet_2<size>& mask, const unsigned int& row, const unsigned int& column);
		void markRowColBoxOnCellUpdate(const unsigned int& row, const unsigned int& column);
		bool searchForHiddenSingles();
		bool searchForHiddenSingles2();
		bool searchForLockedCandidate();
		bool processPendingAssignments();
		bool processPendingAssignmentsOneByOne();
		bool processPendingAssignmentsAllAtOnce();

		bool searchForNakedPair();

	public:
		static constexpr const unsigned int numDigits = size / (GlobalLookupTablesTemplate<size>::BITS_IN_ONE_DIGIT - 1) + 1;
		array<StaticBitSet_2<size>, size * size> m_NSPossibleValues;
		unsigned int m_cluesToFind;
		SudokuPuzzleBasicStats& m_stats;

	public:
		static const unsigned int m_boxSize;

		static array<StaticBitSet_2<size>, size> m_initializationFiltersRow;
		static array<StaticBitSet_2<size>, size> m_initializationFiltersColumn;
		static array<StaticBitSet_2<size>, size> m_initializationFiltersBox;

		static StaticBitSet_2<size> m_bitsON;

		//For locked candidates
		static PendingAssignmentsCircularQueue4<size> m_pendingAssignmentsQueue;
		static array< array<StaticBitSet_2<size>, getCompileTimeSquareRoot<unsigned int>(size)>, size > m_lockedCandidatesHorizontalStrips;
		static array< array<StaticBitSet_2<size>, size>, getCompileTimeSquareRoot<unsigned int>(size) > m_lockedCandidatesVerticalStrips;

		static StaticBitSet_2<size> onceOrMoreThanOnce;
		static StaticBitSet_2<size>& onlyOnce;
		static StaticBitSet_2<size> moreThanOnce;
		static StaticBitSet_2<size> tempCurrentNSMask;

		static StaticBitSet_2<size> m_updatedRows_hiddenSingles, m_updatedColumns_hiddenSingles, m_updatedBoxes_hiddenSingles;
		static StaticBitSet_2<size> m_updatedRows_lockedCandidate, m_updatedColumns_lockedCandidate; // , m_updatedBoxes_lockedCandidate;
		static StaticBitSet_2<size> m_updatedRows_nakedPairs, m_updatedColumns_nakedPairs, m_updatedBoxes_nakedPairs;

		const GlobalLookupTablesTemplate<size>& globalLookupTablesTemplate;
	};

	
	template<unsigned int size>
	const unsigned int FastestBruteForceSudokuMatrixTemplate_6<size>::m_boxSize = sqrt(size);

	template<unsigned int size>
	array<StaticBitSet_2<size>, size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_initializationFiltersRow;
	template<unsigned int size>
	array<StaticBitSet_2<size>, size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_initializationFiltersColumn;
	template<unsigned int size>
	array<StaticBitSet_2<size>, size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_initializationFiltersBox;
	//vector< vector<int> > FastestBruteForceSudokuMatrixTemplate_6<size>::m_solution(MAX_DIM, vector<int>(MAX_DIM, 0));

	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_bitsON;

	template<unsigned int size>
	array< array<StaticBitSet_2<size>, getCompileTimeSquareRoot<unsigned int>(size)>, size > FastestBruteForceSudokuMatrixTemplate_6<size>::m_lockedCandidatesHorizontalStrips;
	template<unsigned int size>
	array< array<StaticBitSet_2<size>, size>, getCompileTimeSquareRoot<unsigned int>(size) > FastestBruteForceSudokuMatrixTemplate_6<size>::m_lockedCandidatesVerticalStrips;

	template<unsigned int size>
	PendingAssignmentsCircularQueue4<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_pendingAssignmentsQueue;

	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::onceOrMoreThanOnce;
	template<unsigned int size>
	StaticBitSet_2<size>& FastestBruteForceSudokuMatrixTemplate_6<size>::onlyOnce;
	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::moreThanOnce;
	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::tempCurrentNSMask;

	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_updatedRows_hiddenSingles;
	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_updatedColumns_hiddenSingles;
	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_updatedBoxes_hiddenSingles;
	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_updatedRows_lockedCandidate;
	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_updatedColumns_lockedCandidate;
	//template<unsigned int size>
	//StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_updatedBoxes_lockedCandidate;
	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_updatedRows_nakedPairs;
	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_updatedColumns_nakedPairs;
	template<unsigned int size>
	StaticBitSet_2<size> FastestBruteForceSudokuMatrixTemplate_6<size>::m_updatedBoxes_nakedPairs;

	template<unsigned int size>
	FastestBruteForceSudokuMatrixTemplate_6<size>::FastestBruteForceSudokuMatrixTemplate_6(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
		: m_stats(stats)
		, m_cluesToFind(size * size)
		, globalLookupTablesTemplate(GlobalLookupTablesTemplate<size>::globalLookupTablesTemplate)
	{
		for(unsigned int i = 0; i < m_NSPossibleValues.size(); ++i)
			m_NSPossibleValues[i] = globalLookupTablesTemplate.initialbits_2;

		//m_updatedRows_hiddenSingles.m_digits.fill(0);
		//m_updatedColumns_hiddenSingles.m_digits.fill(0);
		//m_updatedBoxes_hiddenSingles.m_digits.fill(0);
		//m_updatedRows_lockedCandidate.m_digits.fill(0);
		//m_updatedColumns_lockedCandidate.m_digits.fill(0);
		////m_updatedBoxes_lockedCandidate.m_digits.fill(0);
		//m_updatedRows_nakedPairs.m_digits.fill(0);
		//m_updatedColumns_nakedPairs.m_digits.fill(0);
		//m_updatedBoxes_nakedPairs.m_digits.fill(0);

		unsigned int cellCount = 0;
		for (unsigned int row = 0; row < size; ++row)
			for (unsigned int column = 0; column < size; ++column, ++cellCount)
				if (dataIn[row][column] != 0)
				{
					--m_cluesToFind;
					m_NSPossibleValues[cellCount] = globalLookupTablesTemplate.bitMask_2[dataIn[row][column]];
					m_pendingAssignmentsQueue.push(row, column);
					//markRowColBoxOnCellUpdate(row, column);
				}
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_6<size>::processPendingAssignments()
	{
		//If there are less pending assignements, process them one by one. 
		//In fact process only first in it and return so that the parent function solve() calls it again since the map is still not empty.
		if (m_pendingAssignmentsQueue.count() < 5)
			return processPendingAssignmentsOneByOne();
		else
			return processPendingAssignmentsAllAtOnce();
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_6<size>::processPendingAssignmentsOneByOne()
	{
		unsigned int row = m_pendingAssignmentsQueue.top().row;
		unsigned int column = m_pendingAssignmentsQueue.top().column;
		m_pendingAssignmentsQueue.pop();

		//if (!m_NSPossibleValues[row * size + column].hasAllBitsON(bitSet))
		//	return false;

		const StaticBitSet_2<size>& bitSet = m_NSPossibleValues[row * size + column];

		//Propagate constraint			
		// Check row
		unsigned int cellIndex = row * size;
		for (unsigned int c = 0; c < size; ++c, ++cellIndex)
		{
			if (c != column && m_NSPossibleValues[cellIndex].hasAllBitsON(bitSet))
			{
				m_NSPossibleValues[cellIndex].setAllBitsOFF(bitSet);
				if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], row, c))
					return false;
			}
		}

		// Check column
		cellIndex = column;
		for (unsigned int r = 0; r < size; ++r, cellIndex += size)
		{
			if (r != row && m_NSPossibleValues[cellIndex].hasAllBitsON(bitSet))
			{
				m_NSPossibleValues[cellIndex].setAllBitsOFF(bitSet);
				if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], r, column))
					return false;
			}
		}

		// Check box
		//Get box index from cell
		//unsigned int boxIndex = m_boxSize * (row / m_boxSize) + (column / m_boxSize);

		//Get box start from box index
		//const int boxRowStart = m_boxSize * (boxIndex / m_boxSize);
		//const int boxColumnStart = m_boxSize * (boxIndex % m_boxSize);

		//Get box start from cell
		//const int boxRowStart = row - row % m_boxSize;
		//const int boxColumnStart = column - column % m_boxSize;
		unsigned int boxIndex = globalLookupTablesTemplate.cellToBoxIndex[row][column];
		const int boxRowStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].first;
		const int boxColumnStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].second;
		
		for (unsigned int i = 0, boxRow = boxRowStart; i < m_boxSize; ++i, ++boxRow)
		{
			if (boxRow == row) continue;
			cellIndex = boxRow * size + boxColumnStart;
			for (unsigned int j = 0, boxColumn = boxColumnStart; j < m_boxSize; ++j, ++boxColumn, ++cellIndex)
				if (boxColumn != column && m_NSPossibleValues[cellIndex].hasAllBitsON(bitSet))
				{
					m_NSPossibleValues[cellIndex].setAllBitsOFF(bitSet);
					if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], boxRow, boxColumn))
						return false;
				}
		}

		//--m_cluesToFind;

		//return so that the parent function solve() calls it again since the map is still not empty.
		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_6<size>::processPendingAssignmentsAllAtOnce()
	{
		//If there are more pending assignements, process them all at once
		for (unsigned int i = 0; i < size; ++i)
		{
			m_initializationFiltersRow[i].resetAllDigits();
			m_initializationFiltersColumn[i].resetAllDigits();
			m_initializationFiltersBox[i].resetAllDigits();
		}

		//Prepare all masks to be used to update NS and HS data structures
		while (!m_pendingAssignmentsQueue.isEmpty())
		{
			unsigned int row = m_pendingAssignmentsQueue.top().row;
			unsigned int column = m_pendingAssignmentsQueue.top().column;
			m_pendingAssignmentsQueue.pop();

			//if (!m_NSPossibleValues[row * size + column].hasAllBitsON(bitSet))
			//	return false;
			const StaticBitSet_2<size>& bitSet = m_NSPossibleValues[row * size + column];

			//If m_pendingAssignmentsQueue already had this entry duplicated before, ignore it and continue
			unsigned int boxIndex = globalLookupTablesTemplate.cellToBoxIndex[row][column];

			//Prepare all masks to be used to update NS data structure
			//If the bit is already ON, there is already a value in pending assignments in this row, column or box
			if (m_initializationFiltersRow[row].hasAllBitsON(bitSet))
				return false;
			m_initializationFiltersRow[row].setAllBitsON(bitSet);

			if (m_initializationFiltersColumn[column].hasAllBitsON(bitSet))
				return false;
			m_initializationFiltersColumn[column].setAllBitsON(bitSet);

			if (m_initializationFiltersBox[boxIndex].hasAllBitsON(bitSet))
				return false;
			m_initializationFiltersBox[boxIndex].setAllBitsON(bitSet);
		}

		//Apply all mask to NS data structure
		for (unsigned int row = 0; row < size; ++row)
		{
			for (unsigned int column = 0; column < size; ++column)
			{
				unsigned int cellIndex = row * size + column;
				unsigned int boxIndex = globalLookupTablesTemplate.cellToBoxIndex[row][column];
				if (m_NSPossibleValues[cellIndex].isOnlyOneBitON()
					&& m_initializationFiltersRow[row].hasAllBitsON(m_NSPossibleValues[cellIndex])
					&& m_initializationFiltersColumn[column].hasAllBitsON(m_NSPossibleValues[cellIndex])
					&& m_initializationFiltersBox[boxIndex].hasAllBitsON(m_NSPossibleValues[cellIndex])
					)
					continue;

				m_bitsON = m_initializationFiltersRow[row];
				m_bitsON |= m_initializationFiltersColumn[column];
				m_bitsON |= m_initializationFiltersBox[boxIndex];

				m_bitsON &= m_NSPossibleValues[cellIndex];
				if (!m_bitsON.isZero())
				{
					m_NSPossibleValues[cellIndex].setAllBitsOFF(m_bitsON);
					if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], row, column))
						return false;
				}
			}
		}

		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_6<size>::checkIfNakedSingle(const StaticBitSet_2<size>& bitSet, const unsigned int& row, const unsigned int& column)
	{
		if (bitSet.isZero())
			return false;
		if (bitSet.isOnlyOneBitON())
		{
			++m_stats.m_nakedSinglesFound;
			--m_cluesToFind;
			m_pendingAssignmentsQueue.push(row, column);
		}

		//markRowColBoxOnCellUpdate(row, column);

		return true;
	}

	template<unsigned int size>
	void FastestBruteForceSudokuMatrixTemplate_6<size>::markRowColBoxOnCellUpdate(const unsigned int& row, const unsigned int& column)
	{
		unsigned int boxIndex = globalLookupTablesTemplate.cellToBoxIndex[row][column];
		m_updatedRows_hiddenSingles.setBitON(row + 1);
		m_updatedColumns_hiddenSingles.setBitON(column + 1);
		m_updatedBoxes_hiddenSingles.setBitON(boxIndex + 1);
		//m_updatedRows_lockedCandidate.setBitON(row + 1);
		//m_updatedColumns_lockedCandidate.setBitON(column + 1);
		////m_updatedBoxes_lockedCandidate.setBitON(boxIndex + 1);
		//m_updatedRows_nakedPairs.setBitON(row + 1);
		//m_updatedColumns_nakedPairs.setBitON(column + 1);
		//m_updatedBoxes_nakedPairs.setBitON(boxIndex + 1);
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_6<size>::searchForHiddenSingles()
	{
		unsigned int value = 0;

		//Check rows
		for (unsigned int row = 0; row < size; ++row)
		{
			//if (!m_updatedRows_hiddenSingles.isBitON(row + 1))
			//	continue;
			//else
			//	m_updatedRows_hiddenSingles.setBitOFF(row + 1);

			onceOrMoreThanOnce.resetAllDigits();
			moreThanOnce.resetAllDigits();

			for (unsigned int column = 0; column < size; ++column)
			{
				tempCurrentNSMask = m_NSPossibleValues[row * size + column];
				tempCurrentNSMask &= onceOrMoreThanOnce;
				moreThanOnce |= tempCurrentNSMask;
				onceOrMoreThanOnce |= m_NSPossibleValues[row * size + column];
			}

			// onceOrMoreThanOnce should have all bits ON
			if (onceOrMoreThanOnce != globalLookupTablesTemplate.initialbits_2)
				return false;
			
			onceOrMoreThanOnce.setAllBitsOFF(moreThanOnce);
			if (onceOrMoreThanOnce.isZero())
				continue;

			for (unsigned int column = 0; column < size; ++column)
			{
				if (m_NSPossibleValues[row * size + column].isOnlyOneBitON())
					continue;

				tempCurrentNSMask = m_NSPossibleValues[row * size + column];
				tempCurrentNSMask &= onceOrMoreThanOnce;

				if (!tempCurrentNSMask.isZero())
				{
					if (!tempCurrentNSMask.isOnlyOneBitON())
						return false;

					++m_stats.m_hiddenSinglesFound;
					m_NSPossibleValues[row * size + column] = tempCurrentNSMask;
					--m_cluesToFind;
					m_pendingAssignmentsQueue.push(row, column);
					//markRowColBoxOnCellUpdate(row, column);
				}
			}
		}

		//Check columns
		for (unsigned int column = 0; column < size; ++column)
		{
			//if (!m_updatedColumns_hiddenSingles.isBitON(column + 1))
			//	continue;
			//else
			//	m_updatedColumns_hiddenSingles.setBitOFF(column + 1);

			onceOrMoreThanOnce.resetAllDigits();
			moreThanOnce.resetAllDigits();

			for (unsigned int row = 0; row < size; ++row)
			{
				tempCurrentNSMask = m_NSPossibleValues[row * size + column];
				tempCurrentNSMask &= onceOrMoreThanOnce;
				moreThanOnce |= tempCurrentNSMask;
				onceOrMoreThanOnce |= m_NSPossibleValues[row * size + column];
			}

			// onceOrMoreThanOnce should have all bits ON
			if (onceOrMoreThanOnce != globalLookupTablesTemplate.initialbits_2)
				return false;

			onceOrMoreThanOnce.setAllBitsOFF(moreThanOnce);
			if (onceOrMoreThanOnce.isZero())
				continue;

			for (unsigned int row = 0; row < size; ++row)
			{
				if (m_NSPossibleValues[row * size + column].isOnlyOneBitON())
					continue;

				tempCurrentNSMask = m_NSPossibleValues[row * size + column];
				tempCurrentNSMask &= onceOrMoreThanOnce;

				if (!tempCurrentNSMask.isZero())
				{
					if (!tempCurrentNSMask.isOnlyOneBitON())
						return false;

					m_NSPossibleValues[row * size + column] = tempCurrentNSMask;
					--m_cluesToFind;
					m_pendingAssignmentsQueue.push(row, column);
					++m_stats.m_hiddenSinglesFound;
					//markRowColBoxOnCellUpdate(row, column);
				}
			}
		}

		//Check boxes
		for (unsigned int boxIndex = 0; boxIndex < size; ++boxIndex)
		{
			//if (!m_updatedBoxes_hiddenSingles.isBitON(boxIndex + 1))
			//	continue;
			//else
			//	m_updatedBoxes_hiddenSingles.setBitOFF(boxIndex + 1);

			onceOrMoreThanOnce.resetAllDigits();
			moreThanOnce.resetAllDigits();

			unsigned int rowStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].first;
			unsigned int columnStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].second;
			for (unsigned int ri = 0, row = rowStart; ri < m_boxSize; ++ri, ++row)
			{
				for (unsigned int ci = 0, column = columnStart; ci < m_boxSize; ++ci, ++column)
				{
					tempCurrentNSMask = m_NSPossibleValues[row * size + column];
					tempCurrentNSMask &= onceOrMoreThanOnce;
					moreThanOnce |= tempCurrentNSMask;
					onceOrMoreThanOnce |= m_NSPossibleValues[row * size + column];
				}
			}

			// onceOrMoreThanOnce should have all bits ON
			if (onceOrMoreThanOnce != globalLookupTablesTemplate.initialbits_2)
				return false;

			onceOrMoreThanOnce.setAllBitsOFF(moreThanOnce);
			if (onceOrMoreThanOnce.isZero())
				continue;

			for (unsigned int ri = 0, row = rowStart; ri < m_boxSize; ++ri, ++row)
			{
				for (unsigned int ci = 0, column = columnStart; ci < m_boxSize; ++ci, ++column)
				{
					if (m_NSPossibleValues[row * size + column].isOnlyOneBitON())
						continue;

					tempCurrentNSMask = m_NSPossibleValues[row * size + column];
					tempCurrentNSMask &= onceOrMoreThanOnce;

					if (!tempCurrentNSMask.isZero())
					{
						if (!tempCurrentNSMask.isOnlyOneBitON())
							return false;

						m_pendingAssignmentsQueue.push(row, column);
						m_NSPossibleValues[row * size + column] = tempCurrentNSMask;
						--m_cluesToFind;
						++m_stats.m_hiddenSinglesFound;
						//markRowColBoxOnCellUpdate(row, column);
					}
				}
			}
		}

		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_6<size>::searchForHiddenSingles2()
	{
		unsigned int count = 0, targetRow = 0, targetColumn = 0, oldValue = 0;

		//Check rows
		for (unsigned int row = 0; row < size; ++row)
		{
			for (unsigned int value = 1; value <= size; ++value)
			{
				count = 0;
				targetColumn = 0;
				for (unsigned int column = 0; column < size && count < 2; ++column)
				{
					if (m_NSPossibleValues[row * size + column].isBitON(value))
					{
						++count;
						targetColumn = column;
					}
				}

				if (count == 0)
					return false;
				if (count == 1 && !m_NSPossibleValues[row * size + targetColumn].isOnlyOneBitON())
				{
					m_pendingAssignmentsQueue.push(row, targetColumn);
					++m_stats.m_hiddenSinglesFound;
					m_NSPossibleValues[row * size + targetColumn] = globalLookupTablesTemplate.bitMask_2[value];
					--m_cluesToFind;
				}
			}

		}

		//Check columns
		for (unsigned int column = 0; column < size; ++column)
		{
			for (unsigned int value = 1; value <= size; ++value)
			{
				count = 0;
				targetRow = 0;
				for (unsigned int row = 0; row < size && count < 2; ++row)
				{
					if (m_NSPossibleValues[row * size + column].isBitON(value))
					{
						++count;
						targetRow = row;
					}
				}

				if (count == 0)
					return false;
				if (count == 1 && !m_NSPossibleValues[targetRow * size + column].isOnlyOneBitON())
				{
					m_pendingAssignmentsQueue.push(targetRow, column);
					++m_stats.m_hiddenSinglesFound;
					m_NSPossibleValues[targetRow * size + column] = globalLookupTablesTemplate.bitMask_2[value];
					--m_cluesToFind;
				}
			}

		}

		//Check boxes
		for (unsigned int boxIndex = 0; boxIndex < size; ++boxIndex)
		{
			for (unsigned int value = 1; value <= size; ++value)
			{
				count = 0;
				targetRow = 0;
				targetColumn = 0;

				unsigned int rowStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].first;
				unsigned int columnStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].second;
				for (unsigned int ri = 0, row = rowStart; ri < m_boxSize && count < 2; ++ri, ++row)
				{
					for (unsigned int ci = 0, column = columnStart; ci < m_boxSize && count < 2; ++ci, ++column)
					{
						if (m_NSPossibleValues[row * size + column].isBitON(value))
						{
							++count;
							targetRow = row;
							targetColumn = column;
						}
					}
				}

				if (count == 0)
					return false;
				if (count == 1 && !m_NSPossibleValues[targetRow * size + targetColumn].isOnlyOneBitON())
				{
					m_pendingAssignmentsQueue.push(targetRow, targetColumn);
					++m_stats.m_hiddenSinglesFound;
					m_NSPossibleValues[targetRow * size + targetColumn] = globalLookupTablesTemplate.bitMask_2[value];
					--m_cluesToFind;
				}
			}
		}

		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_6<size>::searchForLockedCandidate()
	{
		static StaticBitSet_2<size> temp1;
		static StaticBitSet_2<size> temp2;

		//Clear locked candidates
		//Consider horizontal strips
		for (unsigned int stripRowIndex = 0; stripRowIndex < size; ++stripRowIndex)
		{
			for (unsigned int stripColumnIndex = 0; stripColumnIndex < m_boxSize; ++stripColumnIndex)
			{
				m_lockedCandidatesHorizontalStrips[stripRowIndex][stripColumnIndex].resetAllDigits();
				for (unsigned int column = stripColumnIndex * m_boxSize, i = 0; i < m_boxSize; ++column, ++i)
				{
					m_lockedCandidatesHorizontalStrips[stripRowIndex][stripColumnIndex] |=
						m_NSPossibleValues[stripRowIndex * size + column];
				}
			}
		}
		//TODO: If any row is changed in the group, we need to search for locked candidate


		for (unsigned int stripRowIndex = 0; stripRowIndex < size; ++stripRowIndex)
		{
			//if (!m_updatedRows_lockedCandidate.isBitON(stripRowIndex + 1))
			//	continue;
			//else
			//	m_updatedRows_lockedCandidate.setBitOFF(stripRowIndex + 1);

			for (unsigned int stripColumnIndex = 0; stripColumnIndex < m_boxSize; ++stripColumnIndex)
			{
				temp1.resetAllDigits();
				for (unsigned int row = stripRowIndex - stripRowIndex % m_boxSize, i = 0; i < m_boxSize; ++row, ++i)
				{
					if (row == stripRowIndex)
						continue;
					temp1 |= m_lockedCandidatesHorizontalStrips[row][stripColumnIndex];
				}

				temp2.resetAllDigits();
				for (unsigned int column = 0; column < m_boxSize; ++column)
				{
					if (column == stripColumnIndex)
						continue;
					temp2 |= m_lockedCandidatesHorizontalStrips[stripRowIndex][column];
				}
				temp1 ^= temp2;
				if (!temp1.isZero()) // Found locked candidate
				{
					m_stats.m_lockedCandidatesFound += temp1.getCountOfONBits();
					for (unsigned int row = stripRowIndex - stripRowIndex % m_boxSize, i = 0; i < m_boxSize; ++row, ++i)
					{
						if (row == stripRowIndex)
							continue;
						for (unsigned int column = stripColumnIndex * m_boxSize, i = 0; i < m_boxSize; ++column, ++i)
						{
							unsigned int cellIndex = row * size + column;
							if (m_NSPossibleValues[cellIndex].isOnlyOneBitON())
							{
								m_NSPossibleValues[cellIndex].setAllBitsOFF(temp1);
								//markRowColBoxOnCellUpdate(row, column);
								if (m_NSPossibleValues[cellIndex].isZero())
									return false;
							}
							else
							{
								m_NSPossibleValues[cellIndex].setAllBitsOFF(temp1);
								if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], row, column))
									return false;
							}
						}
					}
					for (unsigned int column = 0; column < size; ++column)
					{
						if (stripColumnIndex == (column / m_boxSize))
							continue;

						unsigned int cellIndex = stripRowIndex * size + column;
						if (m_NSPossibleValues[cellIndex].isOnlyOneBitON())
						{
							m_NSPossibleValues[cellIndex].setAllBitsOFF(temp1);
							//markRowColBoxOnCellUpdate(stripRowIndex, column);
							if (m_NSPossibleValues[cellIndex].isZero())
								return false;
						}
						else
						{
							m_NSPossibleValues[cellIndex].setAllBitsOFF(temp1);
							if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], stripRowIndex, column))
								return false;
						}
					}
				}
			}
		}

		//Consider vertical strips
		for (unsigned int stripRowIndex = 0; stripRowIndex < m_boxSize; ++stripRowIndex)
		{
			for (unsigned int stripColumnIndex = 0; stripColumnIndex < size; ++stripColumnIndex)
			{
				m_lockedCandidatesVerticalStrips[stripRowIndex][stripColumnIndex].resetAllDigits();
				for (unsigned int row = stripRowIndex * m_boxSize, i = 0; i < m_boxSize; ++row, ++i)
				{
					m_lockedCandidatesVerticalStrips[stripRowIndex][stripColumnIndex] |=
						m_NSPossibleValues[row * size + stripColumnIndex];
				}
			}
		}

		for (unsigned int stripColumnIndex = 0; stripColumnIndex < size; ++stripColumnIndex)
		{
			//if (!m_updatedColumns_lockedCandidate.isBitON(stripColumnIndex + 1))
			//	continue;
			//else
			//	m_updatedColumns_lockedCandidate.setBitOFF(stripColumnIndex + 1);

		for (unsigned int stripRowIndex = 0; stripRowIndex < m_boxSize; ++stripRowIndex)
		{
				temp1.resetAllDigits();
				for (unsigned int column = stripColumnIndex - stripColumnIndex % m_boxSize, i = 0; i < m_boxSize; ++column, ++i)
				{
					if (column == stripColumnIndex)
						continue;
					temp1 |= m_lockedCandidatesVerticalStrips[stripRowIndex][column];
				}

				temp2.resetAllDigits();
				for (unsigned int row = 0; row < m_boxSize; ++row)
				{
					if (row == stripRowIndex)
						continue;
					temp2 |= m_lockedCandidatesVerticalStrips[row][stripColumnIndex];
				}
				temp1 ^= temp2;
				if (!temp1.isZero()) // Found locked candidate
				{
					m_stats.m_lockedCandidatesFound += temp1.getCountOfONBits();
					for (unsigned int column = stripColumnIndex - stripColumnIndex % m_boxSize, i = 0; i < m_boxSize; ++column, ++i)
					{
						if (column == stripColumnIndex)
							continue;
						for (unsigned int row = stripRowIndex * m_boxSize, i = 0; i < m_boxSize; ++row, ++i)
						{
							unsigned int cellIndex = row * size + column;
							if (m_NSPossibleValues[cellIndex].isOnlyOneBitON())
							{
								m_NSPossibleValues[cellIndex].setAllBitsOFF(temp1);
								//markRowColBoxOnCellUpdate(row, column);
								if (m_NSPossibleValues[cellIndex].isZero())
									return false;
							}
							else
							{
								m_NSPossibleValues[cellIndex].setAllBitsOFF(temp1);
								if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], row, column))
									return false;
							}
						}
					}
					for (unsigned int row = 0; row < size; ++row)
					{
						if (stripRowIndex == (row / m_boxSize))
							continue;

						unsigned int cellIndex = row * size + stripColumnIndex;
						if (m_NSPossibleValues[cellIndex].isOnlyOneBitON())
						{
							m_NSPossibleValues[cellIndex].setAllBitsOFF(temp1);
							//markRowColBoxOnCellUpdate(row, stripColumnIndex);
							if (m_NSPossibleValues[cellIndex].isZero())
								return false;
						}
						else
						{
							m_NSPossibleValues[cellIndex].setAllBitsOFF(temp1);
							if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], row, stripColumnIndex))
								return false;
						}
					}
				}
			}
		}

		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_6<size>::searchForNakedPair()
	{
		bool nakedPairExist = false;
		static StaticBitSet_2<size> buffer;

		//Check rows
		for (unsigned int row = 0; row < size; ++row)
		{
			//if (!m_updatedRows_nakedPairs.isBitON(row + 1))
			//	continue;
			//else
			//	m_updatedRows_nakedPairs.setBitOFF(row + 1);

			buffer.resetAllDigits();
			for (unsigned int column = 0; column < size; ++column)
			{
				unsigned int currentCellIndex = row * size + column;
				if (m_NSPossibleValues[currentCellIndex].getCountOfONBits() != 2)
					continue;

				if (buffer.isZero())
					buffer = m_NSPossibleValues[currentCellIndex];
				else if (buffer == m_NSPossibleValues[currentCellIndex])
					nakedPairExist = true;
				else if (buffer.hasAllBitsON(m_NSPossibleValues[currentCellIndex]))
				{
					//There has been possibility that naked pair of m_NSPossibleValues[row * size + column] before, confirm it before procceeding
					for (unsigned int column2 = 0; column2 < column && !nakedPairExist; ++column2)
						if (m_NSPossibleValues[row * size + column2] == m_NSPossibleValues[currentCellIndex])
							nakedPairExist = true;
				}

				if (nakedPairExist)
				{
					//Naked Pair found
					++m_stats.m_nakedPairsFound;
					for (unsigned int column2 = 0, cellIndex = row * size; column2 < size; ++column2, ++cellIndex)
					{
						if (m_NSPossibleValues[cellIndex] != m_NSPossibleValues[currentCellIndex])
						{
							if (m_NSPossibleValues[cellIndex].hasAtLeastOneBitON(m_NSPossibleValues[currentCellIndex]))
							{
								m_NSPossibleValues[cellIndex].setAllBitsOFF(m_NSPossibleValues[currentCellIndex]);
								if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], row, column2))
									return false;
							}
						}
					}

					buffer.setAllBitsOFF(m_NSPossibleValues[currentCellIndex]);
					nakedPairExist = false;
				}
				else
					buffer |= m_NSPossibleValues[currentCellIndex];
			}
		}

		//Check columns
		for (unsigned int column = 0; column < size; ++column)
		{
			//if (!m_updatedColumns_nakedPairs.isBitON(column + 1))
			//	continue;
			//else
			//	m_updatedColumns_nakedPairs.setBitOFF(column + 1);

			buffer.resetAllDigits();
			for (unsigned int row = 0; row < size; ++row)
			{
				unsigned int currentCellIndex = row * size + column;
				if (m_NSPossibleValues[currentCellIndex].getCountOfONBits() != 2)
					continue;

				if (buffer.isZero())
					buffer = m_NSPossibleValues[currentCellIndex];
				else if (buffer == m_NSPossibleValues[currentCellIndex])
					nakedPairExist = true;
				else if (buffer.hasAllBitsON(m_NSPossibleValues[currentCellIndex]))
				{
					//There has been possibility that naked pair of m_NSPossibleValues[row * size + column] before, confirm it before procceeding
					for (unsigned int row2 = 0; row2 < row; ++row2)
						if (m_NSPossibleValues[row2 * size + column] == m_NSPossibleValues[currentCellIndex])
							nakedPairExist = true;
				}

				if (nakedPairExist)
				{
					//Naked Pair found
					++m_stats.m_nakedPairsFound;
					for (unsigned int row2 = 0, cellIndex = column; row2 < size; ++row2, cellIndex += size)
					{
						if (m_NSPossibleValues[cellIndex] != m_NSPossibleValues[currentCellIndex])
						{
							if (m_NSPossibleValues[cellIndex].hasAtLeastOneBitON(m_NSPossibleValues[currentCellIndex]))
							{
								m_NSPossibleValues[cellIndex].setAllBitsOFF(m_NSPossibleValues[currentCellIndex]);
								if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], row2, column))
									return false;
							}
						}
					}

					buffer.setAllBitsOFF(m_NSPossibleValues[currentCellIndex]);
					nakedPairExist = false;
				}
				else
					buffer |= m_NSPossibleValues[currentCellIndex];
			}
		}

		//Check boxes
		for (unsigned int boxIndex = 0; boxIndex < size; ++boxIndex)
		{
			//if (!m_updatedBoxes_nakedPairs.isBitON(boxIndex + 1))
			//	continue;
			//else
			//	m_updatedBoxes_nakedPairs.setBitOFF(boxIndex + 1);

			buffer.resetAllDigits();
			unsigned int rowStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].first;
			unsigned int columnStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].second;
			for (unsigned int ri = 0, row = rowStart; ri < m_boxSize; ++ri, ++row)
			{
				for (unsigned int ci = 0, column = columnStart; ci < m_boxSize; ++ci, ++column)
				{
					unsigned int currentCellIndex = row * size + column;
					if (m_NSPossibleValues[currentCellIndex].getCountOfONBits() != 2)
						continue;

					if (buffer.isZero())
						buffer = m_NSPossibleValues[currentCellIndex];
					else if (buffer == m_NSPossibleValues[currentCellIndex])
						nakedPairExist = true;
					else if (buffer.hasAllBitsON(m_NSPossibleValues[currentCellIndex]))
					{
						//There has been possibility that naked pair of m_NSPossibleValues[row * size + column] before, confirm it before procceeding
						for (unsigned int ri2 = 0, row2 = rowStart; ri2 < ri && !nakedPairExist; ++ri2, ++row2)
							for (unsigned int ci2 = 0, column2 = columnStart; ci2 < ci && !nakedPairExist; ++ci2, ++column2)
								if (m_NSPossibleValues[row2 * size + column2] == m_NSPossibleValues[currentCellIndex])
									nakedPairExist = true;
					}

					if (nakedPairExist)
					{
						//Naked Pair found
						++m_stats.m_nakedPairsFound;
						for (unsigned int ri2 = 0, row2 = rowStart; ri2 < m_boxSize; ++ri2, ++row2)
							for (unsigned int ci2 = 0, column2 = columnStart; ci2 < m_boxSize; ++ci2, ++column2)
							{
								unsigned int cellIndex = row2 * size + column2;
								if (m_NSPossibleValues[cellIndex] != m_NSPossibleValues[currentCellIndex])
								{
									if (m_NSPossibleValues[cellIndex].hasAtLeastOneBitON(m_NSPossibleValues[currentCellIndex]))
									{
										m_NSPossibleValues[cellIndex].setAllBitsOFF(m_NSPossibleValues[currentCellIndex]);
										if (!checkIfNakedSingle(m_NSPossibleValues[cellIndex], row2, column2))
											return false;
									}
								}
							}

						buffer.setAllBitsOFF(m_NSPossibleValues[currentCellIndex]);
						nakedPairExist = false;
					}
					else
						buffer |= m_NSPossibleValues[currentCellIndex];
				}
			}
		}

		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_6<size>::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions)
	{
		while (m_cluesToFind > 0)
		{
			//Timer t;
			bool result1 = processPendingAssignments();
			//long long ns = t.getDurationTillNowInNanoSeconds();
			//SudokuPuzzleUtils::getLogger() << "\nTime required for processPendingAssignments: " << ns;
			if (!result1)
				return false;

			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;

			if (m_cluesToFind == 0)
				break;

			//t.resetTimer();
			bool result2 = searchForHiddenSingles();
			//ns = t.getDurationTillNowInNanoSeconds();
			//SudokuPuzzleUtils::getLogger() << "\nTime required for searchForHiddenSingles: " << ns;
			if (!result2)
				return false;

			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;

			if (m_cluesToFind == 0)
				return false;

			//t.resetTimer();
			bool result3 = searchForLockedCandidate();
			//ns = t.getDurationTillNowInNanoSeconds();
			//SudokuPuzzleUtils::getLogger() << "\nTime required for searchForLockedCandidate: " << ns;
			if (!result3)
				return false;

			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;

			if (m_cluesToFind == 0)
				return false;

			//t.resetTimer();
			bool result4 = searchForNakedPair();
			//ns = t.getDurationTillNowInNanoSeconds();
			//SudokuPuzzleUtils::getLogger() << "\nTime required for searchForNakedPair: " << ns;
			if (!result4)
				return false;

			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;

			break;
		}

		if (m_cluesToFind == 0 || m_stats.m_valuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
		{
			m_pendingAssignmentsQueue.makeEmpty();
			vector< vector<int> > curentSolution(size, vector<int>(size, 0));
			int index = -1;
			for (unsigned int i = 0; i < size; ++i)
				for (unsigned int j = 0; j < size; ++j)
					curentSolution[i][j] = m_NSPossibleValues[++index].getOnBitPosition();

			solutionSetsOut.push_back(std::move(curentSolution));
			return true; // Found the solution
		}

		//Make a guess
		unsigned int minPossibleCount = size + 1;
		unsigned int index = minPossibleCount;

		//TODO: Instead of finding min every time, keep min and second_min ini memory and whenever the count is updated, update these values
		for (unsigned int i = 0; i < size * size && minPossibleCount != 2; ++i)
		{
			unsigned int count = m_NSPossibleValues[i].getCountOfONBits();
			if (count > 1u && count < minPossibleCount)
			{
				index = i;
				minPossibleCount = count;
			}
		}

		// VIMP TODO: Remove this assert failure: --DONE
		//MyAssert::myRunTimeAssert(minPossibleCount < (size + 1));

		++m_stats.m_cellsTriedOrLinkUpdates;

		for (unsigned int i = 1; i <= size; ++i)
		{
			if (!m_NSPossibleValues[index].isBitON(i))
				continue;

			++m_stats.m_valuesTried;

			//MyAssert::myRunTimeAssert(m_pendingAssignmentsQueue.count() == 0);
			m_pendingAssignmentsQueue.push(index / size, index % size);

			FastestBruteForceSudokuMatrixTemplate_6 s(*this);
			s.m_NSPossibleValues[index] = globalLookupTablesTemplate.bitMask_2[i];
			--s.m_cluesToFind;
			// If current guess is wrong, prepare for next guess
			if (!s.solve(solutionSetsOut, numSolutions))
			{
				m_pendingAssignmentsQueue.makeEmpty();
				++m_stats.m_wrongGuesses;

				//m_updatedRows_hiddenSingles.m_digits.fill(0);
				//m_updatedColumns_hiddenSingles.m_digits.fill(0);
				//m_updatedBoxes_hiddenSingles.m_digits.fill(0);
				//m_updatedRows_lockedCandidate.m_digits.fill(0);
				//m_updatedColumns_lockedCandidate.m_digits.fill(0);
				////m_updatedBoxes_lockedCandidate.m_digits.fill(0);
				//m_updatedRows_nakedPairs.m_digits.fill(0);
				//m_updatedColumns_nakedPairs.m_digits.fill(0);
				//m_updatedBoxes_nakedPairs.m_digits.fill(0);

				continue;
			}

			//If we are here, all cells are successfully filled in and we found a right solution
			//Check if we found all required number of solutions
			if (solutionSetsOut.size() == numSolutions)
				return true;
		}

		return false;
	}
}
