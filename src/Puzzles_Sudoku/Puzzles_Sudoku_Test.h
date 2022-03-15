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
#include <string>
#include <iomanip> //for setw(n)
using namespace std;

#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_UniversalDatabase.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Generator.h"

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_0.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_1.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_2.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_3.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_4.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_5.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_6.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_7.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_8.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_9.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_10.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_11.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_12.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_13.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_14.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingDancingLinks_1.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingDancingLinks_2.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingDancingLinks_3.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingDancingLinks_4.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingDancingLinks_5.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Solver_FsssSolver.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Solver_JSolve.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Solver_bb_sudoku.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingMicrosoftZ3Solver_1.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingGoogleORTools_1.h"

namespace mm {

	class SudokuPuzzleData;

	class PuzzlesSudokuTester
	{
	public:
		static void testPuzzlesSudokuSolverUsingBruteForce_0(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut);
		static void testPuzzlesSudokuSolverUsingBruteForce_1(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut);
		static void testPuzzlesSudokuSolverUsingBruteForce_2(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut);
		static bool testPuzzlesSudokuSolverUsingDancingLinks_1(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut);

		template<typename SudokuMatrixType>
		static bool solveSudokuPuzzle(const vector< vector<int> >& dataIn, const int numSolutions);

		static void solveSudokuPuzzleUsingDifferentAlgo(const vector< vector<int> >& sudokuPuzzleData, const int numSolutions, const int solveUsingTheseAlgos, const bool presentInLibrary);

		static void solveSudokuPuzzleDataSetsFromFile(const string& fullFilePath, const int dimension, const int numSolutions);
		static string wchar_t2string(const wchar_t *wchar);
		static wchar_t* string2wchar_t(const string &str);
		static void solveSudokuPuzzleDataSetsFromDirectory(const string& directory_path, const int dimension, const int numSolutions);

		static void generateAndTestRandomSudokuPuzzleDataSets(const int count, const int dimension, const int numSolutions, const int solveUsingTheseAlgos);

		static void solveHardcodedSudokuPuzzles(const int numSolutions);
		static void solveHardcodedSudokuPuzzles(const vector<SudokuPuzzleData>& sudokuPuzzleDataSets, const int numSolutions);

		static void readExistngLibrary(const string& libraryFilePath, const bool solveAllPuzzlesInLibraryAgain, const int dimension, const int numSolutions, const int solveUsingTheseAlgos);
		static void readExistngLibrary(const string& libraryFilePath, const bool solveAllPuzzlesInLibraryAgain, const int numSolutions, const int solveUsingTheseAlgos);
		static void testSudokuPuzzlesWithDimension9();

		template<typename SudokuMatrixType>
		static int getAlgoIndex();

		template<typename SudokuMatrixType>
		static string getAlgoShortName();

		static string getAlgoShortName(int index);

		template<typename SudokuMatrixType>
		static int getAlgoFlag();

		static int getAllAlgoFlag();

		template<typename SudokuMatrixType>
		static void exhaustiveTestSudokuPuzzlesWithDimension9Template();

		static void exhaustiveTestSudokuPuzzlesWithDimension9();

		static void testSudokuPuzzlesOfAllDimensions(const int numPuzzlesToGenerate, const int numSolutions, bool solveLibraryPuzzlesAgain);
		static void testSudokuPuzzlesOfDifferentDimensions(const vector<unsigned int>& dimensions, const int numPuzzlesToGenerate, const int numSolutions, bool solveLibraryPuzzlesAgain);

		static void debugSpecialPuzzleOrAlgorithm_1();
		static void debugSpecialPuzzleOrAlgorithm_2();
		static void debugSpecialPuzzleOrAlgorithm_3();
		static void debugSpecialPuzzleOrAlgorithm_4();
		static void debugSpecialPuzzleOrAlgorithm_5();
		static void debugSpecialPuzzleOrAlgorithm_6();
		static void debugSpecialPuzzleOrAlgorithm_7();
		static void debugSpecialPuzzleOrAlgorithm_8();
		static void debugSpecialPuzzleOrAlgorithm_9();
		static void debugSpecialPuzzleOrAlgorithm_10();
		static void debugSpecialPuzzleOrAlgorithm_11();

		static bool m_printInput;
		static bool m_printOutput;
	};

	template<typename SudokuMatrixType>
	string PuzzlesSudokuTester::getAlgoShortName()
	{
		return getAlgoShortName(PuzzlesSudokuTester::getAlgoIndex<SudokuMatrixType>());
	}

	template<typename SudokuMatrixType>
	int PuzzlesSudokuTester::getAlgoFlag()
	{
		return 1 << PuzzlesSudokuTester::getAlgoIndex<SudokuMatrixType>();
	}

	template<typename SudokuMatrixType>
	void PuzzlesSudokuTester::exhaustiveTestSudokuPuzzlesWithDimension9Template()
	{
		const int dimension = 9;
		const int numSolutions = 2;
		const string libraryFilePath("../../../test/reports/Sudoku/exhaustiveTestSudokuPuzzlesWithDimension9_" + to_string(PuzzlesSudokuTester::getAlgoIndex<SudokuMatrixType>()) + ".csv");
		PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, false, dimension, numSolutions, PuzzlesSudokuTester::getAllAlgoFlag());

		SudokuPuzzleUniversalDatabase::setCurrentCategory("Randomly generated");
		const int count = 1000000;
		//const bool addToLibrary = false;
		cout << "\n";
		int iterations;
		long long timeRequiredToGeneratePuzzle = 0;
		const int groupSize = ((1 << 9) - 1); // 255
		for (int t = 0; t < count; ++t)
		{
			if (((t + 1) & groupSize) == groupSize)
				cout << "\rGenerating random puzzle: " << t + 1 << " of " << count;
			//SudokuPuzzleUtils::getLogger() << "\nGenerating random puzzle: " << t + 1 << " of " << count;
			iterations = 0;
			//PuzzlesSudokuTester::solveSudokuPuzzle<SudokuMatrixType>(SudokuPuzzleGenerator::generateSudokuPuzzle(dimension, iterations, timeRequiredToGeneratePuzzle), getAlgoIndex<SudokuMatrixType>(), numSolutions);
			solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleGenerator::generateSudokuPuzzle(dimension, iterations, timeRequiredToGeneratePuzzle), numSolutions, PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixType>(), false);
			SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = iterations;
			SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = timeRequiredToGeneratePuzzle;
		}

		SudokuPuzzleUniversalDatabase::writeToLibrary();
	}

	template<typename SudokuMatrixType>
	bool PuzzlesSudokuTester::solveSudokuPuzzle(const vector< vector<int> >& dataIn, const int numSolutions)
	{
		SudokuPuzzleUtils::getLogger() << "\n" << setw(35) << PuzzlesSudokuTester::getAlgoShortName<SudokuMatrixType>();

		const int index = PuzzlesSudokuTester::getAlgoIndex<SudokuMatrixType>();
		vector< vector< vector<int> > > solutionSetOut(0, vector< vector<int> >(dataIn.size(), vector<int>(dataIn.size(), 0)));
		SudokuPuzzleBasicStats stats;
		Timer t;
		bool result = SudokuMatrixType::solve(dataIn, solutionSetOut, numSolutions, stats);
		stats.m_time = t.getDurationTillNowInNanoSeconds();
		SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index] = stats;

		//SudokuPuzzleUtils::getLogger() << "\nTime required to solve                  : " << Timer::getDurationStringInNanoSeconds(SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_time);
		//SudokuPuzzleUtils::getLogger() << "\nNo. of total cells tried or Link Updates: " << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_cellsTriedOrLinkUpdates;
		//SudokuPuzzleUtils::getLogger() << "\nNo. of total values tried               : " << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_valuesTried;
		//SudokuPuzzleUtils::getLogger() << "\nNo. of total wrong guesses              : " << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_wrongGuesses;
		//SudokuPuzzleUtils::getLogger() << "\nNo. of total naked singles found        : " << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_nakedSinglesFound;
		//SudokuPuzzleUtils::getLogger() << "\nNo. of total hidden singles found       : " << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_hiddenSinglesFound;
		//SudokuPuzzleUtils::getLogger() << "\nNo. of total locked candidates found    : " << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_lockedCandidatesFound;
		//SudokuPuzzleUtils::getLogger() << "\nNo. of total naked pairs found          : " << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_nakedPairsFound;

		SudokuPuzzleUtils::getLogger()
			<< setw(20) << Timer::getCommaSeparatedDurationString(SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_time)
			<< setw(15) << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_cellsTriedOrLinkUpdates
			<< setw(15) << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_valuesTried
			<< setw(15) << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_wrongGuesses
			<< setw(15) << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_nakedSinglesFound
			<< setw(15) << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_hiddenSinglesFound
			<< setw(20) << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_lockedCandidatesFound
			<< setw(15) << SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_nakedPairsFound;

		if (result)
		{
			SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_solutions = SudokuPuzzleUtils::convertToSingleLineString(solutionSetOut);
			SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_numInvalidSolutions = SudokuPuzzleUtils::validateSudokuSolution(solutionSetOut);
		}
		else
		{
			SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_solutions = "No solution found";
			//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			//SetConsoleTextAttribute(hConsole, 9);
			SudokuPuzzleUtils::getLogger(BrightBlue) << "\nNo solution found";
			//SetConsoleTextAttribute(hConsole, 30);
			SudokuPuzzleUtils::getLogger().setConsoleTextColour(ConsoleTextColour::BrightWhite);
		}

		if (m_printOutput)
		{
			//Print solution. It may be partial/wrong in case of error.
			//if (SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[index].m_numInvalidSolutions > 0)
			SudokuPuzzleUtils::printSudokuGrid(solutionSetOut, dataIn);
		}

		return result;
	}

	enum SudokuSolvingAlgoTypes
	{
		bruteForce_Strategy_1 = 0,
		bruteForce_Strategy_1And2,
		bruteForce_Strategy_1AndOptimized2,
		bruteForce_Strategy_withNakedAndHidden,
		bruteForce_Fastest_1,
		bruteForce_Fastest_2,
		bruteForce_Fastest_3,
		bruteForce_Fastest_4,
		bruteForce_Fastest_5,
		bruteForce_Fastest_6,
		bruteForce_Fastest_7,
		bruteForce_Fastest_14,
		DancingLinks_Naive,
		DancingLinks_NoMemoryAllocation,
		DancingLinks_NoMemoryAllocationAndReadyStructure,
		DancingLinks_Fastest_1,
		Fsss_Solver,
		JSolve,
		bb_sudoku,
		microsoft_z3_solver_1,
		google_OR_tools_1,

		totalAlgos
	};
}

