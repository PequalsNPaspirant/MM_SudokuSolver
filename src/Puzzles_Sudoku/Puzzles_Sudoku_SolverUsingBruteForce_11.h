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
#include "Puzzles_Sudoku/Puzzles_Sudoku_StaticBitSet_1.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"

namespace mm {

	//Forward declaraton
	struct SudokuPuzzleBasicStats;

	class FastestBruteForceSudokuMatrix_5
	{
	public:
		static bool solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats);
	};

	template<unsigned int size>
	class FastestBruteForceSudokuMatrixTemplate_5
	{
	public:
		FastestBruteForceSudokuMatrixTemplate_5(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats);
		bool solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions);

	private:
		//bool updateHSCountersRow(const unsigned int& row, const unsigned int& thisBitRemoved);
		//bool updateHSCountersColumn(const unsigned int& column, const unsigned int& thisBitRemoved);
		//bool updateHSCountersBox(const unsigned int& boxIndex, const unsigned int& thisBitRemoved);
		//bool updateHSPossiblePositionsRow(const unsigned int& row, const unsigned int& column, const unsigned int& thisBitRemoved);
		//bool updateHSPossiblePositionsColumn(const unsigned int& row, const unsigned int& column, const unsigned int& thisBitRemoved);
		//bool updateHSPossiblePositionsBox(const unsigned int& boxIndex, const unsigned int& cellIndexInBox, const unsigned int& thisBitRemoved);

		bool checkIfNakedSingle(const unsigned int& row, const unsigned int& column);
		bool searchForHiddenSingles();
		bool searchForHiddenSingles2();
		bool searchForLockedCandidate();
		bool processPendingAssignments();
		bool processPendingAssignmentsOneByOne();
		bool processPendingAssignmentsAllAtOnce();

		bool searchForNakedPair();

	public:
		static constexpr const unsigned int numDigits = size / (GlobalLookupTablesTemplate<size>::BITS_IN_ONE_DIGIT - 1) + 1;
		array<StaticBitSet_1<size>, size * size> m_NSPossibleValues;
		unsigned int m_cluesToFind;
		SudokuPuzzleBasicStats& m_stats;
		//vector< vector<unsigned int> > m_NSPossibleValuesCount;

		//vector< vector<StaticBitSet_1<size>> > m_HSPossiblePositionsRow;
		//vector< vector<unsigned int> > m_HSPossiblePositionsCountRow;
		//vector< vector<StaticBitSet_1<size>> > m_HSPossiblePositionsColumn;
		//vector< vector<unsigned int> > m_HSPossiblePositionsCountColumn;
		//vector< vector<StaticBitSet_1<size>> > m_HSPossiblePositionsBox;
		//vector< vector<unsigned int> > m_HSPossiblePositionsCountBox;

		//References to Global Lookup Tables
		//const vector< vector<unsigned int>>& cellToBoxIndex;
		//const vector< std::pair<unsigned int, unsigned int>>& boxIndexToBoxStart;

	public:
		//static unsigned int m_size;
		static const unsigned int m_boxSize;

	public:
		//static vector< vector<int> > m_solution;
		static array<StaticBitSet_1<size>, size> m_initializationFiltersRow;
		static array<StaticBitSet_1<size>, size> m_initializationFiltersColumn;
		static array<StaticBitSet_1<size>, size> m_initializationFiltersBox;

		//static vector<StaticBitSet_1<size>> m_initializationFiltersRowHS;
		//static vector<StaticBitSet_1<size>> m_initializationFiltersColumnHS;
		//static vector<StaticBitSet_1<size>> m_initializationFiltersBoxHS;

		static StaticBitSet_1<size> m_bitsON;
		//static StaticBitSet_1<size> m_valuesToEliminate;

		//For locked candidates
		static PendingAssignmentsCircularQueue2<size * size> m_pendingAssignmentsQueue;
		//static map<std::pair<unsigned int, unsigned int>, unsigned int> m_pendingAssignmentsHashMap;
		static array< array<StaticBitSet_1<size>, getCompileTimeSquareRoot<unsigned int>(size)>, size > m_lockedCandidatesHorizontalStrips;
		static array< array<StaticBitSet_1<size>, size>, getCompileTimeSquareRoot<unsigned int>(size) > m_lockedCandidatesVerticalStrips;

		static StaticBitSet_1<size> onceOrMoreThanOnce;
		static StaticBitSet_1<size>& onlyOnce;
		static StaticBitSet_1<size> moreThanOnce;
		static StaticBitSet_1<size> tempCurrentNSMask;

		//static GlobalLookupTables2<size> globalLookupTablesTemplate;
		const GlobalLookupTablesTemplate<size>& globalLookupTablesTemplate;
	};

	
	//unsigned int FastestBruteForceSudokuMatrixTemplate_5<size>::size;
	template<unsigned int size>
	const unsigned int FastestBruteForceSudokuMatrixTemplate_5<size>::m_boxSize = sqrt(size);

	template<unsigned int size>
	array<StaticBitSet_1<size>, size> FastestBruteForceSudokuMatrixTemplate_5<size>::m_initializationFiltersRow;
	template<unsigned int size>
	array<StaticBitSet_1<size>, size> FastestBruteForceSudokuMatrixTemplate_5<size>::m_initializationFiltersColumn;
	template<unsigned int size>
	array<StaticBitSet_1<size>, size> FastestBruteForceSudokuMatrixTemplate_5<size>::m_initializationFiltersBox;
	//vector< vector<int> > FastestBruteForceSudokuMatrixTemplate_5<size>::m_solution(MAX_DIM, vector<int>(MAX_DIM, 0));

	//vector<StaticBitSet_1<size>> FastestBruteForceSudokuMatrixTemplate_5<size>::m_initializationFiltersRowHS(MAX_DIM, StaticBitSet_1<size>());
	//vector<StaticBitSet_1<size>> FastestBruteForceSudokuMatrixTemplate_5<size>::m_initializationFiltersColumnHS(MAX_DIM, StaticBitSet_1<size>());
	//vector<StaticBitSet_1<size>> FastestBruteForceSudokuMatrixTemplate_5<size>::m_initializationFiltersBoxHS(MAX_DIM, StaticBitSet_1<size>());

	template<unsigned int size>
	StaticBitSet_1<size> FastestBruteForceSudokuMatrixTemplate_5<size>::m_bitsON;
	//template<unsigned int size>
	//StaticBitSet_1<size> FastestBruteForceSudokuMatrixTemplate_5<size>::m_valuesToEliminate;

	template<unsigned int size>
	array< array<StaticBitSet_1<size>, getCompileTimeSquareRoot<unsigned int>(size)>, size > FastestBruteForceSudokuMatrixTemplate_5<size>::m_lockedCandidatesHorizontalStrips;
	template<unsigned int size>
	array< array<StaticBitSet_1<size>, size>, getCompileTimeSquareRoot<unsigned int>(size) > FastestBruteForceSudokuMatrixTemplate_5<size>::m_lockedCandidatesVerticalStrips;

	template<unsigned int size>
	PendingAssignmentsCircularQueue2<size * size> FastestBruteForceSudokuMatrixTemplate_5<size>::m_pendingAssignmentsQueue;
	//map<std::pair<unsigned int, unsigned int>, unsigned int> FastestBruteForceSudokuMatrixTemplate_5<size>::m_pendingAssignmentsHashMap;

	template<unsigned int size>
	StaticBitSet_1<size> FastestBruteForceSudokuMatrixTemplate_5<size>::onceOrMoreThanOnce;
	template<unsigned int size>
	StaticBitSet_1<size>& FastestBruteForceSudokuMatrixTemplate_5<size>::onlyOnce;
	template<unsigned int size>
	StaticBitSet_1<size> FastestBruteForceSudokuMatrixTemplate_5<size>::moreThanOnce;
	template<unsigned int size>
	StaticBitSet_1<size> FastestBruteForceSudokuMatrixTemplate_5<size>::tempCurrentNSMask;

	//template<unsigned int size>
	//GlobalLookupTables2<size> FastestBruteForceSudokuMatrixTemplate_5<size>::globalLookupTablesTemplate;

	template<unsigned int size>
	FastestBruteForceSudokuMatrixTemplate_5<size>::FastestBruteForceSudokuMatrixTemplate_5(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
		: m_stats(stats)
		, m_cluesToFind(size * size)
		//, m_NSPossibleValues(size * size, globalLookupTablesTemplate.initialbits_1[globalLookupTablesTemplate.sizeToIndexHash[size]])
		//, m_NSPossibleValues(size * size)
		//, m_NSPossibleValuesCount(size, vector<unsigned int>(size, size))
		//, m_HSPossiblePositionsRow(size, vector<StaticBitSet_1<size>>(size + 1, globalLookupTablesTemplate.initialbits_1[globalLookupTablesTemplate.sizeToIndexHash[size]]))
		//, m_HSPossiblePositionsCountRow(size, vector<unsigned int>(size + 1, size))
		//, m_HSPossiblePositionsColumn(size, vector<StaticBitSet_1<size>>(size + 1, globalLookupTablesTemplate.initialbits_1[globalLookupTablesTemplate.sizeToIndexHash[size]]))
		//, m_HSPossiblePositionsCountColumn(size, vector<unsigned int>(size + 1, size))
		//, m_HSPossiblePositionsBox(size, vector<StaticBitSet_1<size>>(size + 1, globalLookupTablesTemplate.initialbits_1[globalLookupTablesTemplate.sizeToIndexHash[size]]))
		//, m_HSPossiblePositionsCountBox(size, vector<unsigned int>(size + 1, size))
		//, cellToBoxIndex(globalLookupTablesTemplate.cellToBoxIndex[m_boxSize])
		//, boxIndexToBoxStart(globalLookupTablesTemplate.boxIndexToBoxStart[m_boxSize])
		, globalLookupTablesTemplate(GlobalLookupTablesTemplate<size>::globalLookupTablesTemplate)
	{
		//m_NSPossibleValues[0] = globalLookupTablesTemplate.initialbits_1;
		for(unsigned int i = 0; i < m_NSPossibleValues.size(); ++i)
		{
			//element.setAllRightmostBitsON(size);
			m_NSPossibleValues[i] = globalLookupTablesTemplate.initialbits_1;
		}

		//if (m_bitsON.m_digits.size() < size)
		//{
		//	m_bitsON.m_digits.resize(size);
		//	m_valuesToEliminate.m_digits.resize(size);
		//}

		//Reset all mask to be used to update NS data structure
		//for (unsigned int i = 0; i < size; ++i)
		//{
		//	m_initializationFiltersRow[i].resetAllDigits();
		//	m_initializationFiltersColumn[i].resetAllDigits();
		//	m_initializationFiltersBox[i].resetAllDigits();

		//	//m_initializationFiltersRowHS[i].resetAllDigits();
		//	//m_initializationFiltersColumnHS[i].resetAllDigits();
		//	//m_initializationFiltersBoxHS[i].resetAllDigits();
		//}

		unsigned int cellCount = 0;
		for (unsigned int row = 0; row < size; ++row)
			for (unsigned int column = 0; column < size; ++column, ++cellCount)
				if (dataIn[row][column] != 0)
				{
					--m_cluesToFind;
					m_NSPossibleValues[cellCount] = globalLookupTablesTemplate.bitMask_1[dataIn[row][column]];
					m_pendingAssignmentsQueue.push(row, column, dataIn[row][column]);
					//m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = dataIn[row][column];
				}
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_5<size>::processPendingAssignments()
	{
		//If there are less pending assignements, process them one by one. 
		//In fact process only first in it and return so that the parent function solve() calls it again since the map is still not empty.
		if (m_pendingAssignmentsQueue.count() < 5)
			//if (m_pendingAssignmentsHashMap.size() < 5)
			return processPendingAssignmentsOneByOne();
		else
			return processPendingAssignmentsAllAtOnce();
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_5<size>::processPendingAssignmentsOneByOne()
	{
		//unsigned int cellIndex = m_pendingAssignmentsQueue.top().cellIndex;
		unsigned int row = m_pendingAssignmentsQueue.top().row;
		unsigned int column = m_pendingAssignmentsQueue.top().column;
		unsigned int value = m_pendingAssignmentsQueue.top().value;
		m_pendingAssignmentsQueue.pop();
		//auto iterator = m_pendingAssignmentsHashMap.begin();
		//unsigned int row = iterator->first.first;
		//unsigned int column = iterator->first.second;
		//unsigned int value = iterator->second;
		//m_pendingAssignmentsHashMap.erase(iterator);

		if (!m_NSPossibleValues[row * size + column].isBitON(value))
			return false;

		//If already assigned, continue
		//if (getCountOfONBits(m_NSPossibleValues[row * size + column]) == 1)
		//	return true;

		//Update NS Data structure: Set all other bits OFF as this cell is solved
		//copyValue(globalLookupTablesTemplate.bitMask_1[value], m_NSPossibleValues[row * size + column]);

		//Propagate constraint			
		// Check row
		unsigned int cellIndex = row * size;
		for (unsigned int c = 0; c < size; ++c, ++cellIndex)
		{
			if (c != column && m_NSPossibleValues[cellIndex].isBitON(value))
			{
				m_NSPossibleValues[cellIndex].setBitOFF(value);
				if (!checkIfNakedSingle(row, c))
					return false;
			}
		}

		// Check column
		cellIndex = column;
		for (unsigned int r = 0; r < size; ++r, cellIndex += size)
		{
			if (r != row && m_NSPossibleValues[cellIndex].isBitON(value))
			{
				m_NSPossibleValues[cellIndex].setBitOFF(value);
				if (!checkIfNakedSingle(r, column))
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
				if (boxColumn != column && m_NSPossibleValues[cellIndex].isBitON(value))
				{
					m_NSPossibleValues[cellIndex].setBitOFF(value);
					if (!checkIfNakedSingle(boxRow, boxColumn))
						return false;
				}
		}

		//--m_cluesToFind;

		//return so that the parent function solve() calls it again since the map is still not empty.
		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_5<size>::processPendingAssignmentsAllAtOnce()
	{
		//If there are more pending assignements, process them all at once
		for (unsigned int i = 0; i < size; ++i)
		{
			m_initializationFiltersRow[i].resetAllDigits();
			m_initializationFiltersColumn[i].resetAllDigits();
			m_initializationFiltersBox[i].resetAllDigits();
		}

		//Prepare all masks to be used to update NS and HS data structures
		//unsigned int pendingAssignements = m_pendingAssignmentsQueue.count();
		while (!m_pendingAssignmentsQueue.isEmpty())
			//for (auto iterator = m_pendingAssignmentsHashMap.begin(); iterator != m_pendingAssignmentsHashMap.end(); ++iterator)
		{
			unsigned int row = m_pendingAssignmentsQueue.top().row;
			unsigned int column = m_pendingAssignmentsQueue.top().column;
			unsigned int value = m_pendingAssignmentsQueue.top().value;
			m_pendingAssignmentsQueue.pop();

			//unsigned int row = iterator->first.first;
			//unsigned int column = iterator->first.second;
			//unsigned int value = iterator->second;

			if (!m_NSPossibleValues[row * size + column].isBitON(value))
				return false;

			//If m_pendingAssignmentsQueue already had this entry duplicated before, ignore it and continue
			unsigned int boxIndex = globalLookupTablesTemplate.cellToBoxIndex[row][column];
			//if (getCountOfONBits(m_NSPossibleValues[row * size + column]) == 1
			//	&& isBitON(m_initializationFiltersRow[row], value)
			//	&& isBitON(m_initializationFiltersColumn[column], value)
			//	&& isBitON(m_initializationFiltersBox[boxIndex], value)
			//	)
			//{
			//	//--pendingAssignements;
			//	continue;
			//}

			//Update NS data structure: Set all other bits OFF as this cell is solved
			//copyValue(globalLookupTablesTemplate.bitMask_1[value], m_NSPossibleValues[row * size + column]);

			//Prepare all masks to be used to update NS data structure
			//If the bit is already ON, there is already a value in pending assignments in this row, column or box
			if (m_initializationFiltersRow[row].isBitON(value))
				return false;
			m_initializationFiltersRow[row].setBitON(value);

			if (m_initializationFiltersColumn[column].isBitON(value))
				return false;
			m_initializationFiltersColumn[column].setBitON(value);

			if (m_initializationFiltersBox[boxIndex].isBitON(value))
				return false;
			m_initializationFiltersBox[boxIndex].setBitON(value);
		}

		//static map<std::pair<unsigned int, unsigned int>, unsigned int> tempHashMap;
		//tempHashMap.clear();
		//tempHashMap.swap(m_pendingAssignmentsHashMap);

		//Apply all mask to NS data structure
		for (unsigned int row = 0; row < size; ++row)
		{
			for (unsigned int column = 0; column < size; ++column)
			{
				//auto iterator = tempHashMap.find(std::pair<unsigned int, unsigned int>(row, column));
				//if (iterator != tempHashMap.end())
				//	continue;

				unsigned int boxIndex = globalLookupTablesTemplate.cellToBoxIndex[row][column];
				if (m_NSPossibleValues[row * size + column].getCountOfONBits() == 1
					&& m_initializationFiltersRow[row].hasAllBitsON(m_NSPossibleValues[row * size + column])
					&& m_initializationFiltersColumn[column].hasAllBitsON(m_NSPossibleValues[row * size + column])
					&& m_initializationFiltersBox[boxIndex].hasAllBitsON(m_NSPossibleValues[row * size + column])
					)
					continue;

				// TODO: Simplify following statement. Avoid temporary objects: --DONE
				//copyValue((m_NSPossibleValues[row * size + column] & (m_initializationFiltersRow[row] | m_initializationFiltersColumn[column] | m_initializationFiltersBox[boxIndex])),
				//	m_bitsON);
				//copyValue(m_initializationFiltersRow[row], m_bitsON);
				m_bitsON = m_initializationFiltersRow[row];
				m_bitsON |= m_initializationFiltersColumn[column];
				m_bitsON |= m_initializationFiltersBox[boxIndex];

				m_bitsON &= m_NSPossibleValues[row * size + column];
				if (!m_bitsON.isZero())
				{
					m_NSPossibleValues[row * size + column].setAllBitsOFF(m_bitsON);
					if (!checkIfNakedSingle(row, column))
						return false;
				}
			}
		}

		//m_cluesToFind -= tempHashMap.size();
		//m_cluesToFind -= pendingAssignements;

		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_5<size>::checkIfNakedSingle(const unsigned int& row, const unsigned int& column)
	{
		if (m_NSPossibleValues[row * size + column].isZero())
			return false;
		if (m_NSPossibleValues[row * size + column].getCountOfONBits() == 1)
		{
			++m_stats.m_nakedSinglesFound;
			--m_cluesToFind;
			unsigned int currentValue = m_NSPossibleValues[row * size + column].getOnBitPosition();
			m_pendingAssignmentsQueue.push(row, column, currentValue);
			//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
			//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = currentValue;
			//else
			//{
			//	unsigned int oldVal = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
			//	if(oldVal != currentValue)
			//		return false;
			//}
		}

		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_5<size>::searchForHiddenSingles()
	{
		unsigned int value = 0;

		//Check rows
		for (unsigned int row = 0; row < size; ++row)
		{
			onceOrMoreThanOnce.resetAllDigits();
			moreThanOnce.resetAllDigits();

			for (unsigned int column = 0; column < size; ++column)
			{
				//copyValue(m_NSPossibleValues[row * size + column], tempCurrentNSMask);
				tempCurrentNSMask = m_NSPossibleValues[row * size + column];
				tempCurrentNSMask &= onceOrMoreThanOnce;
				moreThanOnce |= tempCurrentNSMask;
				onceOrMoreThanOnce |= m_NSPossibleValues[row * size + column];
			}

			// onceOrMoreThanOnce should have all bits ON
			if (onceOrMoreThanOnce != globalLookupTablesTemplate.initialbits_1)
				return false;
			//const BigIntegerBitCollection& ref = globalLookupTablesTemplate.initialbits_1[globalLookupTablesTemplate.sizeToIndexHash[size]];
			//for (unsigned int i = 0; i < onceOrMoreThanOnce.m_digits.size(); ++i)
			//{
			//	if (onceOrMoreThanOnce.m_digits[i] != ref.getDigits()[i])
			//		return false;
			//}
			
			//copyValue(onceOrMoreThanOnce, onlyOnce); // To avoid this copying, onlyOnce is made reference to onceOrMoreThanOnce
			onceOrMoreThanOnce.setAllBitsOFF(moreThanOnce);
			if (onceOrMoreThanOnce.isZero())
				continue;

			for (unsigned int column = 0; column < size; ++column)
			{
				if (m_NSPossibleValues[row * size + column].getCountOfONBits() == 1)
					continue;

				//copyValue(m_NSPossibleValues[row * size + column], tempCurrentNSMask);
				tempCurrentNSMask = m_NSPossibleValues[row * size + column];
				tempCurrentNSMask &= onceOrMoreThanOnce;

				if (!tempCurrentNSMask.isZero())
				{
					//MyAssert::myRunTimeAssert(getCountOfONBits(tempCurrentNSMask) == 1);
					if (tempCurrentNSMask.getCountOfONBits() != 1)
						return false;

					value = tempCurrentNSMask.getOnBitPosition();
					++m_stats.m_hiddenSinglesFound;
					m_NSPossibleValues[row * size + column] = globalLookupTablesTemplate.bitMask_1[value];
					--m_cluesToFind;
					m_pendingAssignmentsQueue.push(row, column, value);
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	value = getOnBitPosition(tempCurrentNSMask);
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = value;
					//}
					//else
					//{
					//	value = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
					//	if (value != getOnBitPosition(tempCurrentNSMask))
					//		return false;
					//}
				}
			}
		}

		//Check columns
		for (unsigned int column = 0; column < size; ++column)
		{
			onceOrMoreThanOnce.resetAllDigits();
			moreThanOnce.resetAllDigits();

			for (unsigned int row = 0; row < size; ++row)
			{
				//copyValue(m_NSPossibleValues[row * size + column], tempCurrentNSMask);
				tempCurrentNSMask = m_NSPossibleValues[row * size + column];
				tempCurrentNSMask &= onceOrMoreThanOnce;
				moreThanOnce |= tempCurrentNSMask;
				onceOrMoreThanOnce |= m_NSPossibleValues[row * size + column];
			}

			// onceOrMoreThanOnce should have all bits ON
			if (onceOrMoreThanOnce != globalLookupTablesTemplate.initialbits_1)
				return false;
			//const BigIntegerBitCollection& ref = globalLookupTablesTemplate.initialbits_1[globalLookupTablesTemplate.sizeToIndexHash[size]];
			//for (unsigned int i = 0; i < onceOrMoreThanOnce.m_digits.size(); ++i)
			//{
			//	if (onceOrMoreThanOnce.m_digits[i] != ref.getDigits()[i])
			//		return false;
			//}

			onceOrMoreThanOnce.setAllBitsOFF(moreThanOnce);
			if (onceOrMoreThanOnce.isZero())
				continue;

			for (unsigned int row = 0; row < size; ++row)
			{
				if (m_NSPossibleValues[row * size + column].getCountOfONBits() == 1)
					continue;

				//copyValue(m_NSPossibleValues[row * size + column], tempCurrentNSMask);
				tempCurrentNSMask = m_NSPossibleValues[row * size + column];
				tempCurrentNSMask &= onceOrMoreThanOnce;

				if (!tempCurrentNSMask.isZero())
				{
					//MyAssert::myRunTimeAssert(getCountOfONBits(tempCurrentNSMask) == 1);
					if (tempCurrentNSMask.getCountOfONBits() != 1)
						return false;

					value = tempCurrentNSMask.getOnBitPosition();
					m_NSPossibleValues[row * size + column] = globalLookupTablesTemplate.bitMask_1[value];
					--m_cluesToFind;
					m_pendingAssignmentsQueue.push(row, column, value);
					++m_stats.m_hiddenSinglesFound;
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	value = getOnBitPosition(tempCurrentNSMask);
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = value;
					//}
					//else
					//{
					//	value = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
					//	if (value != getOnBitPosition(tempCurrentNSMask))
					//		return false;
					//}
				}
			}
		}

		//Check boxes
		for (unsigned int boxIndex = 0; boxIndex < size; ++boxIndex)
		{
			onceOrMoreThanOnce.resetAllDigits();
			moreThanOnce.resetAllDigits();

			unsigned int rowStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].first;
			unsigned int columnStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].second;
			for (unsigned int ri = 0, row = rowStart; ri < m_boxSize; ++ri, ++row)
			{
				for (unsigned int ci = 0, column = columnStart; ci < m_boxSize; ++ci, ++column)
				{
					//copyValue(m_NSPossibleValues[row * size + column], tempCurrentNSMask);
					tempCurrentNSMask = m_NSPossibleValues[row * size + column];
					tempCurrentNSMask &= onceOrMoreThanOnce;
					moreThanOnce |= tempCurrentNSMask;
					onceOrMoreThanOnce |= m_NSPossibleValues[row * size + column];
				}
			}

			// onceOrMoreThanOnce should have all bits ON
			if (onceOrMoreThanOnce != globalLookupTablesTemplate.initialbits_1)
				return false;
			//const BigIntegerBitCollection& ref = globalLookupTablesTemplate.initialbits_1[globalLookupTablesTemplate.sizeToIndexHash[size]];
			//for (unsigned int i = 0; i < onceOrMoreThanOnce.m_digits.size(); ++i)
			//{
			//	if (onceOrMoreThanOnce.m_digits[i] != ref.getDigits()[i])
			//		return false;
			//}

			onceOrMoreThanOnce.setAllBitsOFF(moreThanOnce);
			if (onceOrMoreThanOnce.isZero())
				continue;

			for (unsigned int ri = 0, row = rowStart; ri < m_boxSize; ++ri, ++row)
			{
				for (unsigned int ci = 0, column = columnStart; ci < m_boxSize; ++ci, ++column)
				{
					if (m_NSPossibleValues[row * size + column].getCountOfONBits() == 1)
						continue;

					//copyValue(m_NSPossibleValues[row * size + column], tempCurrentNSMask);
					tempCurrentNSMask = m_NSPossibleValues[row * size + column];
					tempCurrentNSMask &= onceOrMoreThanOnce;

					if (!tempCurrentNSMask.isZero())
					{
						//MyAssert::myRunTimeAssert(getCountOfONBits(tempCurrentNSMask) == 1);
						if (tempCurrentNSMask.getCountOfONBits() != 1)
							return false;

						value = tempCurrentNSMask.getOnBitPosition();
						m_pendingAssignmentsQueue.push(row, column, value);
						m_NSPossibleValues[row * size + column] = globalLookupTablesTemplate.bitMask_1[value];
						--m_cluesToFind;
						++m_stats.m_hiddenSinglesFound;
						//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, column)) == m_pendingAssignmentsHashMap.end())
						//{
						//	++m_stats.m_hiddenSinglesFound;
						//	value = getOnBitPosition(tempCurrentNSMask);
						//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = value;
						//}
						//else
						//{
						//	value = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)];
						//	if (value != getOnBitPosition(tempCurrentNSMask))
						//		return false;
						//}
					}
				}
			}
		}

		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_5<size>::searchForHiddenSingles2()
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
				if (count == 1 && m_NSPossibleValues[row * size + targetColumn].getCountOfONBits() != 1)
				{
					m_pendingAssignmentsQueue.push(row, targetColumn, value);
					++m_stats.m_hiddenSinglesFound;
					m_NSPossibleValues[row * size + targetColumn] = globalLookupTablesTemplate.bitMask_1[value];
					--m_cluesToFind;
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(row, targetColumn)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, targetColumn)] = value;
					//}
					//else
					//{
					//	oldValue = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, targetColumn)];
					//	if (value != oldValue)
					//		return false;
					//}
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
				if (count == 1 && m_NSPossibleValues[targetRow * size + column].getCountOfONBits() != 1)
				{
					m_pendingAssignmentsQueue.push(targetRow, column, value);
					++m_stats.m_hiddenSinglesFound;
					m_NSPossibleValues[targetRow * size + column] = globalLookupTablesTemplate.bitMask_1[value];
					--m_cluesToFind;
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(targetRow, column)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(targetRow, column)] = value;
					//}
					//else
					//{
					//	oldValue = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(targetRow, column)];
					//	if (value != oldValue)
					//		return false;
					//}
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
				if (count == 1 && m_NSPossibleValues[targetRow * size + targetColumn].getCountOfONBits() != 1)
				{
					m_pendingAssignmentsQueue.push(targetRow, targetColumn, value);
					++m_stats.m_hiddenSinglesFound;
					m_NSPossibleValues[targetRow * size + targetColumn] = globalLookupTablesTemplate.bitMask_1[value];
					--m_cluesToFind;
					//if (m_pendingAssignmentsHashMap.find(std::pair<unsigned int, unsigned int>(targetRow, targetColumn)) == m_pendingAssignmentsHashMap.end())
					//{
					//	++m_stats.m_hiddenSinglesFound;
					//	m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(targetRow, targetColumn)] = value;
					//}
					//else
					//{
					//	oldValue = m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(targetRow, targetColumn)];
					//	if (value != oldValue)
					//		return false;
					//}
				}
			}
		}

		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_5<size>::searchForLockedCandidate()
	{
		static StaticBitSet_1<size> temp1;
		static StaticBitSet_1<size> temp2;

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

		for (unsigned int stripRowIndex = 0; stripRowIndex < size; ++stripRowIndex)
		{
			for (unsigned int stripColumnIndex = 0; stripColumnIndex < m_boxSize; ++stripColumnIndex)
			{
				temp1.resetAllDigits();
				for (unsigned int iRow = stripRowIndex - stripRowIndex % m_boxSize, i = 0; i < m_boxSize; ++iRow, ++i)
				{
					if (iRow == stripRowIndex)
						continue;
					temp1 |= m_lockedCandidatesHorizontalStrips[iRow][stripColumnIndex];
				}

				temp2.resetAllDigits();
				for (unsigned int iColumn = 0; iColumn < m_boxSize; ++iColumn)
				{
					if (iColumn == stripColumnIndex)
						continue;
					temp2 |= m_lockedCandidatesHorizontalStrips[stripRowIndex][iColumn];
				}
				temp1 ^= temp2;
				if (!temp1.isZero()) // Found locked candidate
				{
					m_stats.m_lockedCandidatesFound += temp1.getCountOfONBits();
					for (unsigned int iRow = stripRowIndex - stripRowIndex % m_boxSize, i = 0; i < m_boxSize; ++iRow, ++i)
					{
						if (iRow == stripRowIndex)
							continue;
						for (unsigned int column = stripColumnIndex * m_boxSize, i = 0; i < m_boxSize; ++column, ++i)
						{
							if (m_NSPossibleValues[iRow * size + column].getCountOfONBits() == 1)
							{
								m_NSPossibleValues[iRow * size + column].setAllBitsOFF(temp1);
								if (m_NSPossibleValues[iRow * size + column].isZero())
									return false;
							}
							else
							{
								m_NSPossibleValues[iRow * size + column].setAllBitsOFF(temp1);
								if (!checkIfNakedSingle(iRow, column))
									return false;
							}
						}
					}
					for (unsigned int column = 0; column < size; ++column)
					{
						if (stripColumnIndex == (column / m_boxSize))
							continue;

						if (m_NSPossibleValues[stripRowIndex * size + column].getCountOfONBits() == 1)
						{
							m_NSPossibleValues[stripRowIndex * size + column].setAllBitsOFF(temp1);
							if (m_NSPossibleValues[stripRowIndex * size + column].isZero())
								return false;
						}
						else
						{
							m_NSPossibleValues[stripRowIndex * size + column].setAllBitsOFF(temp1);
							if (!checkIfNakedSingle(stripRowIndex, column))
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

		for (unsigned int stripRowIndex = 0; stripRowIndex < m_boxSize; ++stripRowIndex)
		{
			for (unsigned int stripColumnIndex = 0; stripColumnIndex < size; ++stripColumnIndex)
			{
				temp1.resetAllDigits();
				for (unsigned int iColumn = stripColumnIndex - stripColumnIndex % m_boxSize, i = 0; i < m_boxSize; ++iColumn, ++i)
				{
					if (iColumn == stripColumnIndex)
						continue;
					temp1 |= m_lockedCandidatesVerticalStrips[stripRowIndex][iColumn];
				}

				temp2.resetAllDigits();
				for (unsigned int iRow = 0; iRow < m_boxSize; ++iRow)
				{
					if (iRow == stripRowIndex)
						continue;
					temp2 |= m_lockedCandidatesVerticalStrips[iRow][stripColumnIndex];
				}
				temp1 ^= temp2;
				if (!temp1.isZero()) // Found locked candidate
				{
					m_stats.m_lockedCandidatesFound += temp1.getCountOfONBits();
					for (unsigned int iColumn = stripColumnIndex - stripColumnIndex % m_boxSize, i = 0; i < m_boxSize; ++iColumn, ++i)
					{
						if (iColumn == stripColumnIndex)
							continue;
						for (unsigned int row = stripRowIndex * m_boxSize, i = 0; i < m_boxSize; ++row, ++i)
						{
							if (m_NSPossibleValues[row * size + iColumn].getCountOfONBits() == 1)
							{
								m_NSPossibleValues[row * size + iColumn].setAllBitsOFF(temp1);
								if (m_NSPossibleValues[row * size + iColumn].isZero())
									return false;
							}
							else
							{
								m_NSPossibleValues[row * size + iColumn].setAllBitsOFF(temp1);
								if (!checkIfNakedSingle(row, iColumn))
									return false;
							}
						}
					}
					for (unsigned int row = 0; row < size; ++row)
					{
						if (stripRowIndex == (row / m_boxSize))
							continue;

						if (m_NSPossibleValues[row * size + stripColumnIndex].getCountOfONBits() == 1)
						{
							m_NSPossibleValues[row * size + stripColumnIndex].setAllBitsOFF(temp1);
							if (m_NSPossibleValues[row * size + stripColumnIndex].isZero())
								return false;
						}
						else
						{
							m_NSPossibleValues[row * size + stripColumnIndex].setAllBitsOFF(temp1);
							if (!checkIfNakedSingle(row, stripColumnIndex))
								return false;
						}
					}
				}
			}
		}
		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_5<size>::searchForNakedPair()
	{
		bool nakedPairExist = false;
		static StaticBitSet_1<size> buffer;

		//Check rows
		for (unsigned int row = 0; row < size; ++row)
		{
			buffer.resetAllDigits();
			for (unsigned int column = 0; column < size; ++column)
			{
				if (m_NSPossibleValues[row * size + column].getCountOfONBits() != 2)
					continue;

				if (buffer.isZero())
					//copyValue(m_NSPossibleValues[row * size + column], buffer);
					buffer = m_NSPossibleValues[row * size + column];
				else if (buffer == m_NSPossibleValues[row * size + column])
					nakedPairExist = true;
				else if (buffer.hasAllBitsON(m_NSPossibleValues[row * size + column]))
				{
					//There has been possibility that naked pair of m_NSPossibleValues[row * size + column] before, confirm it before procceeding
					for (unsigned int column2 = 0; column2 < column && !nakedPairExist; ++column2)
						if (m_NSPossibleValues[row * size + column2] == m_NSPossibleValues[row * size + column])
							nakedPairExist = true;
				}

				if (nakedPairExist)
				{
					//Naked Pair found
					++m_stats.m_nakedPairsFound;
					for (unsigned int column2 = 0; column2 < size; ++column2)
						if (m_NSPossibleValues[row * size + column2] != m_NSPossibleValues[row * size + column])
						{
							if (m_NSPossibleValues[row * size + column2].hasAtLeastOneBitON(m_NSPossibleValues[row * size + column]))
							{
								m_NSPossibleValues[row * size + column2].setAllBitsOFF(m_NSPossibleValues[row * size + column]);
								if (!checkIfNakedSingle(row, column2))
									return false;
							}
						}

					buffer.setAllBitsOFF(m_NSPossibleValues[row * size + column]);
					nakedPairExist = false;
				}
				else
					buffer |= m_NSPossibleValues[row * size + column];
			}
		}

		//Check columns
		for (unsigned int column = 0; column < size; ++column)
		{
			buffer.resetAllDigits();
			for (unsigned int row = 0; row < size; ++row)
			{
				if (m_NSPossibleValues[row * size + column].getCountOfONBits() != 2)
					continue;

				if (buffer.isZero())
					//copyValue(m_NSPossibleValues[row * size + column], buffer);
					buffer = m_NSPossibleValues[row * size + column];
				else if (buffer == m_NSPossibleValues[row * size + column])
					nakedPairExist = true;
				else if (buffer.hasAllBitsON(m_NSPossibleValues[row * size + column]))
				{
					//There has been possibility that naked pair of m_NSPossibleValues[row * size + column] before, confirm it before procceeding
					for (unsigned int row2 = 0; row2 < row; ++row2)
						if (m_NSPossibleValues[row2 * size + column] == m_NSPossibleValues[row * size + column])
							nakedPairExist = true;
				}

				if (nakedPairExist)
				{
					//Naked Pair found
					++m_stats.m_nakedPairsFound;
					for (unsigned int row2 = 0; row2 < size; ++row2)
						if (m_NSPossibleValues[row2 * size + column] != m_NSPossibleValues[row * size + column])
						{
							if (m_NSPossibleValues[row2 * size + column].hasAtLeastOneBitON(m_NSPossibleValues[row * size + column]))
							{
								m_NSPossibleValues[row2 * size + column].setAllBitsOFF(m_NSPossibleValues[row * size + column]);
								if (!checkIfNakedSingle(row2, column))
									return false;
							}
						}

					buffer.setAllBitsOFF(m_NSPossibleValues[row * size + column]);
					nakedPairExist = false;
				}
				else
					buffer |= m_NSPossibleValues[row * size + column];
			}
		}

		//Check boxes
		for (unsigned int boxIndex = 0; boxIndex < size; ++boxIndex)
		{
			buffer.resetAllDigits();
			unsigned int rowStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].first;
			unsigned int columnStart = globalLookupTablesTemplate.boxIndexToBoxStart[boxIndex].second;
			for (unsigned int ri = 0, row = rowStart; ri < m_boxSize; ++ri, ++row)
			{
				for (unsigned int ci = 0, column = columnStart; ci < m_boxSize; ++ci, ++column)
				{
					if (m_NSPossibleValues[row * size + column].getCountOfONBits() != 2)
						continue;

					if (buffer.isZero())
						//copyValue(m_NSPossibleValues[row * size + column], buffer);
						buffer = m_NSPossibleValues[row * size + column];
					else if (buffer == m_NSPossibleValues[row * size + column])
						nakedPairExist = true;
					else if (buffer.hasAllBitsON(m_NSPossibleValues[row * size + column]))
					{
						//There has been possibility that naked pair of m_NSPossibleValues[row * size + column] before, confirm it before procceeding
						for (unsigned int ri2 = 0, row2 = rowStart; ri2 < ri && !nakedPairExist; ++ri2, ++row2)
							for (unsigned int ci2 = 0, column2 = columnStart; ci2 < ci && !nakedPairExist; ++ci2, ++column2)
								if (m_NSPossibleValues[row2 * size + column2] == m_NSPossibleValues[row * size + column])
									nakedPairExist = true;
					}

					if (nakedPairExist)
					{
						//Naked Pair found
						++m_stats.m_nakedPairsFound;
						for (unsigned int ri2 = 0, row2 = rowStart; ri2 < m_boxSize; ++ri2, ++row2)
							for (unsigned int ci2 = 0, column2 = columnStart; ci2 < m_boxSize; ++ci2, ++column2)
								if (m_NSPossibleValues[row2 * size + column2] != m_NSPossibleValues[row * size + column])
								{
									if (m_NSPossibleValues[row2 * size + column2].hasAtLeastOneBitON(m_NSPossibleValues[row * size + column]))
									{
										m_NSPossibleValues[row2 * size + column2].setAllBitsOFF(m_NSPossibleValues[row * size + column]);
										if (!checkIfNakedSingle(row2, column2))
											return false;
									}
								}

						buffer.setAllBitsOFF(m_NSPossibleValues[row * size + column]);
						nakedPairExist = false;
					}
					else
						buffer |= m_NSPossibleValues[row * size + column];
				}
			}
		}
		return true;
	}

	template<unsigned int size>
	bool FastestBruteForceSudokuMatrixTemplate_5<size>::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int& numSolutions)
	{
		while (m_cluesToFind > 0)
		{
			if (!processPendingAssignments())
				return false;
			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;
			//if (!m_pendingAssignmentsHashMap.empty())
			//	continue;

			if (m_cluesToFind > 0 && !searchForHiddenSingles())
				return false;
			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;
			//if (!m_pendingAssignmentsHashMap.empty())
			//	continue;

			if (m_cluesToFind > 0 && !searchForLockedCandidate())
				return false;
			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;
			//if (!m_pendingAssignmentsHashMap.empty())
			//	continue;

			if (m_cluesToFind > 0 && !searchForNakedPair())
				return false;
			if (!m_pendingAssignmentsQueue.isEmpty())
				continue;
			//if (!m_pendingAssignmentsHashMap.empty())
			//	continue;

			break;
		}

		//bool solved = true;
		if (m_cluesToFind == 0 || m_stats.m_valuesTried >= SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
		{
			m_pendingAssignmentsQueue.makeEmpty();
			vector< vector<int> > curentSolution(size, vector<int>(size, 0));
			for (unsigned int i = 0; i < size /*&& solved*/; ++i)
				for (unsigned int j = 0; j < size /*&& solved*/; ++j)
				{
					//unsigned int count = getCountOfONBits(m_NSPossibleValues[i * size + j]);
					//if (count == 1)
					curentSolution[i][j] = m_NSPossibleValues[i * size + j].getOnBitPosition();
					//else
					//	solved = false;
				}

			//if (solved)
			//{
			solutionSetsOut.push_back(std::move(curentSolution));
			return true; // Found the solution
						 //}
		}

		//Make a guess
		unsigned int minPossibleCount = size + 1;
		unsigned int row = minPossibleCount;
		unsigned int column = minPossibleCount;

		//TODO: Instead of finding min every time, keep min and second_min ini memory and whenever the count is updated, update these values
		for (unsigned int i = 0; i < size && minPossibleCount != 2; ++i)
		{
			for (unsigned int j = 0; j < size && minPossibleCount != 2; ++j)
			{
				unsigned int count = m_NSPossibleValues[i * size + j].getCountOfONBits();
				if (count > 1u && count < minPossibleCount)
				{
					row = i;
					column = j;
					minPossibleCount = count;
				}
			}
		}

		// VIMP TODO: Remove this assert failure: --DONE
		MyAssert::myRunTimeAssert(minPossibleCount < (size + 1));
		//if (minPossibleCount == (size + 1))
		//{
		//	vector< vector<int> > curentSolution(size, vector<int>(size, 0));
		//	for (unsigned int i = 0; i < size; ++i)
		//		for (unsigned int j = 0; j < size; ++j)
		//			curentSolution[i][j] = m_NSPossibleValues[i * size + j].getOnBitPosition();

		//	solutionSetsOut.push_back(std::move(curentSolution));
		//	return true;
		//}

		++m_stats.m_cellsTriedOrLinkUpdates;

		for (unsigned int i = 1; i <= size; ++i)
		{
			if (!m_NSPossibleValues[row * size + column].isBitON(i))
				continue;

			++m_stats.m_valuesTried;

			MyAssert::myRunTimeAssert(m_pendingAssignmentsQueue.count() == 0);
			m_pendingAssignmentsQueue.push(row, column, i);
			//MyAssert::myRunTimeAssert(m_pendingAssignmentsHashMap.size() == 0);
			//m_pendingAssignmentsHashMap[std::pair<unsigned int, unsigned int>(row, column)] = i;

			FastestBruteForceSudokuMatrixTemplate_5 s(*this);
			s.m_NSPossibleValues[row * size + column] = globalLookupTablesTemplate.bitMask_1[i];
			--s.m_cluesToFind;
			//copyValue(globalLookupTablesTemplate.bitMask_1[i], s.m_NSPossibleValues[row * size + column]);
			// If current guess is wrong, prepare for next guess
			if (!s.solve(solutionSetsOut, numSolutions))
			{
				m_pendingAssignmentsQueue.makeEmpty();
				//m_pendingAssignmentsHashMap.clear();

				++m_stats.m_wrongGuesses;
				continue;
			}

			//If we are here, all cells are successfully filled in and we found a right solution

			//Check if we found required number of solutions
			if (solutionSetsOut.size() == numSolutions)
				return true;
		}

		return false;
	}
}
