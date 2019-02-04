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
#include <string>
#include <vector>
#include <fstream> //for file read/write
#include <cstdio> //for int rename( const char *old_filename, const char *new_filename );
#include <ctime>
using namespace std;

#include <windows.h> // to go throu all files in a directory

#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_UniversalDatabase.h"

namespace mm {
	string SudokuPuzzleUniversalDatabase::m_sudokuPuzzleLibraryFilePath("");
	map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator> SudokuPuzzleUniversalDatabase::m_sudokuPuzzleLibrary;
	map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>::iterator SudokuPuzzleUniversalDatabase::m_currentEntry = m_sudokuPuzzleLibrary.end();

	int SudokuPuzzleUniversalDatabase::m_initialCount = 0;
	int SudokuPuzzleUniversalDatabase::m_duplicatePuzzlesCount = 0;
	string SudokuPuzzleUniversalDatabase::m_puzzleCategory = "  // No category";

	vector<string> split(const string& theString, const string& theDelimiter)
	{
		vector<string> theStringVector;
		size_t  start = 0, end = 0;
		while (end != string::npos)
		{
			end = theString.find(theDelimiter, start);

			// If at end, use length=maxLength.  Else use length=end-start.
			theStringVector.push_back(theString.substr(start, (end == string::npos) ? string::npos : end - start));

			// If at end, use start=maxSize.  Else use start=end+delimiter.
			start = ((end > (string::npos - theDelimiter.size())) ? string::npos : end + theDelimiter.size());
		}

		return theStringVector;
	}

	const map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>& SudokuPuzzleUniversalDatabase::readExistngLibrary(const string& libraryPath, const bool SkipStatistics)
	{
		m_sudokuPuzzleLibraryFilePath = libraryPath;
		//SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);
		SudokuPuzzleUtils::getLogger() << "\nReading existing library file " << libraryPath;
		Timer t;
		ifstream sudokuPuzzleLibraryFile(libraryPath, ios_base::in);
		string line;
		if (sudokuPuzzleLibraryFile.is_open())
		{
			std::getline(sudokuPuzzleLibraryFile, line); // Skip header

			while (std::getline(sudokuPuzzleLibraryFile, line))
			{
				if (line.empty() 
					|| ((line[0] < '0' || line[0] > '9') && line[0] != ' '))
					continue;

				vector<string> tokens = split(line, ",");

				//if (!SudokuPuzzleUniversalDatabase::addToLibrary(tokens[0]) || SkipStatistics)
				if (!SudokuPuzzleUniversalDatabase::addToLibrary(stoi(tokens[0]), tokens[1]))
					continue;

				int index = 1;
				SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_category = tokens[++index];
				SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_clues = stoi(tokens[++index]);
				SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = stoull(tokens[++index]);
				SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = stoi(tokens[++index]);

				for (size_t i = 0; i < SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base.size() && (index + 6) < tokens.size(); ++i)
				{
					++index;					
					SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[i].m_cellsTriedOrLinkUpdates = stoull(tokens[++index]);
					SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[i].m_valuesTried = stoull(tokens[++index]);
					SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[i].m_time = stoull(tokens[++index]);
					SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[i].m_solutions = tokens[++index];
					SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[i].m_numInvalidSolutions = stoi(tokens[++index]);
				}

			}

			sudokuPuzzleLibraryFile.close();
		}

		m_initialCount = m_sudokuPuzzleLibrary.size();

		SudokuPuzzleUtils::getLogger() << "\nTotal initial count of Puzzles in library: " << m_initialCount;
		SudokuPuzzleUtils::getLogger() << "\nTime required to read existing library: " << t.getDurationStringTillNowInNanoSeconds();
		//SudokuPuzzleUtils::getLogger().setPrintOnScreen(false);

		return m_sudokuPuzzleLibrary;
	}

	bool SudokuPuzzleUniversalDatabase::writeToLibrary()
	{
		//All collected Sudoku Puzzle Library data will be saved in a file with the same name, so rename existing file before saving data
		Timer t;
		int result = std::rename(m_sudokuPuzzleLibraryFilePath.c_str(), string(m_sudokuPuzzleLibraryFilePath + t.getCurrentLocalTimeInNanoSeconds()).c_str());
		//SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);
		if (result == 0)
			SudokuPuzzleUtils::getLogger() << "\nExisting Sudoku Puzzle Library File " << m_sudokuPuzzleLibraryFilePath.c_str() << " is successfully renamed.";
		else
			SudokuPuzzleUtils::getLogger() << "\nError renaming Existing Sudoku Puzzle Library File " << m_sudokuPuzzleLibraryFilePath.c_str();
		//SudokuPuzzleUtils::getLogger().setPrintOnScreen(false);

		ofstream sudokuPuzzleLibraryFile(m_sudokuPuzzleLibraryFilePath, ios_base::out);
		if (sudokuPuzzleLibraryFile.is_open())
		{
			//Write column names
			string columns("Dimension,Sudoku Puzzle,Category,Clues,GenerationTime,GeenrationIterations,"
				"BruteForce: Strategy 1,CellsTried,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"BruteForce: Strategy 1 & 2,CellsTried,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"BruteForce: Strategy 1 & Optimized-2,CellsTried,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"BruteForce: With Naked & Hidden,CellsTried,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"BruteForce: Fastest,CellsTried,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"DancingLinks: Naive,LinkUpdates,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"DancingLinks: NoMemoryAllocation,LinkUpdates,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"DancingLinks: NoMemoryAllocation & ReadyStructure,LinkUpdates,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"DancingLinks: Fastest,LinkUpdates,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"FsssSolver,LinkUpdates,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"JSolve,LinkUpdates,ValuesTried,Time(ns),Solution,numInvalidSolutions,"
				"bb_sudoku,LinkUpdates,ValuesTried,Time(ns),Solution,numInvalidSolutions\n"
			);
			sudokuPuzzleLibraryFile.write(columns.c_str(), columns.length());

			//const int length = 81 + 100 + 2 + 12 + 2
			//	+ 0 + 6 + 6 + 12 + 81 + 3
			//	+ 0 + 6 + 6 + 12 + 81 + 3
			//	+ 0 + 6 + 6 + 12 + 81 + 3
			//	+ 0 + 6 + 6 + 12 + 81 + 3
			//	+ 0 + 6 + 6 + 12 + 81 + 3
			//	+ 0 + 6 + 6 + 12 + 81 + 3;
			string currentRow;
			for (std::map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>::iterator it = m_sudokuPuzzleLibrary.begin(); it != m_sudokuPuzzleLibrary.end(); ++it)
			{
				currentRow = to_string(it->first.first) + "," + it->first.second + "," + it->second.m_category + "," + to_string(it->second.m_clues) + "," + to_string(it->second.m_puzzleGenerationTime) + "," + to_string(it->second.m_puzzleGenerationIterations);
				sudokuPuzzleLibraryFile.write(currentRow.c_str(), currentRow.length());

				for (size_t i = 0; i < it->second.m_base.size(); ++i)
				{
					string stream(",," + to_string(it->second.m_base[i].m_cellsTriedOrLinkUpdates) + "," + to_string(it->second.m_base[i].m_valuesTried) + "," + to_string(it->second.m_base[i].m_time) + "," + it->second.m_base[i].m_solutions + "," + to_string(it->second.m_base[i].m_numInvalidSolutions));
					sudokuPuzzleLibraryFile.write(stream.c_str(), stream.length());
				}

				sudokuPuzzleLibraryFile.write("\n", 1);
				//flush() will ensure than the contents are written to file on disk immediately, Otherwise we need to wait until close() is called on it
				//sudokuPuzzleLibraryFile.flush();
			}
			/*
			//Add summary at the end of csv file:
			string summary = "\n\n\nSummary:"
				"\nInitial count in Sudoku Puzzles Univeral Library:," + to_string(m_initialCount)
				+ "\nFinal count in Sudoku Puzzles Univeral Library:," + to_string(m_sudokuPuzzleLibrary.size())
				+ "\nTotal new Sudoku Puzzles solved:," + to_string(m_sudokuPuzzleLibrary.size() - m_initialCount)
				+ "\nCount of Sudoku Puzzles already in Univeral Library:," + to_string(m_duplicatePuzzlesCount)
				+ "\nTime required to write all puzzles to The Library:," + t.getDurationStringTillNowInNanoSeconds();

			sudokuPuzzleLibraryFile.write(summary.c_str(), summary.length());
			*/
			sudokuPuzzleLibraryFile.close();
		}

		//SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);
		SudokuPuzzleUtils::getLogger() << "\nSummary:";
		SudokuPuzzleUtils::getLogger() << "\nInitial count in Sudoku Puzzles Univeral Library   : " << m_initialCount;
		SudokuPuzzleUtils::getLogger() << "\nFinal count in Sudoku Puzzles Univeral Library     : " << m_sudokuPuzzleLibrary.size();
		SudokuPuzzleUtils::getLogger() << "\nTotal new Sudoku Puzzles solved                    : " << m_sudokuPuzzleLibrary.size() - m_initialCount;
		SudokuPuzzleUtils::getLogger() << "\nCount of Sudoku Puzzles already in Univeral Library: " << m_duplicatePuzzlesCount;
		SudokuPuzzleUtils::getLogger() << "\nTime required to write all puzzles to The Library  : " << t.getDurationStringTillNowInNanoSeconds();
		//SudokuPuzzleUtils::getLogger().setPrintOnScreen(false);

		m_sudokuPuzzleLibrary.clear();
		m_initialCount = 0;
		m_duplicatePuzzlesCount = 0;

		return true;
	}

	bool SudokuPuzzleUniversalDatabase::addToLibrary(const unsigned int dimension, const string& newPuzzle)
	{
		std::pair<std::map<std::pair<unsigned int, string>, SudokuPuzzleStats>::iterator, bool> ret = m_sudokuPuzzleLibrary.insert(std::pair<std::pair<unsigned int, string>, SudokuPuzzleStats>({ dimension, newPuzzle }, SudokuPuzzleStats()));
		m_currentEntry = ret.first;

		if (!ret.second) // failed to insert because the equivalent entry already exists in map
			++m_duplicatePuzzlesCount;

		return ret.second;
	}

	void SudokuPuzzleUniversalDatabase::setCurrentCategory(const string& category)
	{
		m_puzzleCategory = "  // " + category;
	}

	const string& SudokuPuzzleUniversalDatabase::getCurrentCategory()
	{
		return m_puzzleCategory;
	}

}
