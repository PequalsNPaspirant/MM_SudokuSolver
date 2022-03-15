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

#include "Puzzles_Sudoku/Puzzles_Sudoku_Test.h"
#include "Timer/Timer.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Solver.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_UniversalDatabase.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Generator.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_TestData.h"

/*
Nice to know:

A 9 x 9 Sudoku puzzle looks like following 9 x 9 grid of numbers ranging from 1 to 9. The puzzle contains missing cells which are to be filled up:

A1 A2 A3 | A4 A5 A6 | A7 A8 A9
B1 B2 B3 | B4 B5 B6 | B7 B8 B9
C1 C2 C3 | C4 C5 C6 | C7 C8 C9
---------+----------+---------
D1 D2 D3 | D4 D5 D6 | D7 D8 D9
E1 E2 E3 | E4 E5 E6 | E7 E8 E9
F1 F2 F3 | F4 F5 F6 | F7 F8 F9
---------+----------+---------
G1 G2 G3 | G4 G5 G6 | G7 G8 G9
H1 H2 H3 | H4 H5 H6 | H7 H8 H9
I1 I2 I3 | I4 I5 I6 | I7 I8 I9

The goal is to fill up missing cells in such a way that:
1. All cells in every row     has unique number
2. All cells in every column  has unique number
3. All cells in every 3x3 box has unique number

==============================================================================================================================================================

Sudoku is known to be NP-Complete. There are no known algorithms to solve a general NxN Sudoku in polynomial time,
but if you find one or show that one does not exist, the Clay Institute has a million dollars for you.
The “Dancing Links” algorithm (or a sudoku-specific variant of it) is particularly well-suited to solving
a typical sudoku puzzle, such as you might find in a newspaper.
Steven Skiena briefly discusses Sudoku in his book, The Algorithm Design Manual.
An interesting technique for finding all solutions to exact cover problems (like Sudoku) is Dancing Links: https://en.wikipedia.org/wiki/Dancing_Links
https://www.ocf.berkeley.edu/~jchu/publicportal/sudoku/sudoku.paper.html

The article redefines the problem as a graph with the puzzle boxes as vertices with edges linking boxes that are connected either by being in the same row,
column or 9x9 box.  From this point it is a graph coloring problem with 9 colors, (aka k-coloring) https://en.wikipedia.org/wiki/Graph_coloring
For a normal game of Sudoku this means that the run of the mill brute-force algorithm will run in O(k^n) where k=9 and n=81, or about 2 * 10^77.
The most efficient algorithm that they talk about for k-coloring runs in O(n^2n) or about 2 * 10^26.
The original Sudoku link mentions that even though the numbers I have written above are rather large a good program can run this algorithm on a normal PC
for the normal game so fast that it becomes challenging to measure the exact computation time.

==============================================================================================================================================================

Solution by CSP (Constraint satisfaction Problem)

Lets say we have a sample of 9 x 9 Sudoku puzzle:

4 . . |. . . |8 . 5
. 3 . |. . . |. . .
. . . |7 . . |. . .
------+------+------
. 2 . |. . . |. 6 .
. . . |. 8 . |4 . .
. . . |. 1 . |. . .
------+------+------
. . . |6 . 3 |. 7 .
5 . . |2 . . |. . .
1 . 4 |. . . |. . .

If we replace the dots in above puzzle by all possible values it can take:

4		1679	12679	|	139		2369	269		|	8		1239	5
26789	3		1256789 |	14589   24569   245689	|	12679	1249	124679
2689	15689   125689	|   7		234569  245689	|	12369   12349   123469
------------------------+---------------------------+-------------------------
3789	2		15789	|	3459	34579	4579	|	13579	6		13789
3679	15679   15679	|	359		8		25679	|   4		12359   12379
36789	4		56789	|	359		1		25679	|	23579   23589   23789
------------------------+---------------------------+-------------------------
289		89		289		|   6		459		3		|	1259	7		12489
5		6789	3		|   2		479		1		|   69		489     4689
1		6789	4		|	589     579     5789	|	23569   23589   23689

If we consider all possible combinations of these values to solve puzzle, the total number of combinations we should consider is:
Consider that in the grid2 above, A2 has 4 possibilities (1679) and A3 has 5 possibilities (12679); together that's 20, and if we keep multiplying,
4 * 5 * 3 * 4 * 3 * 4 * 5 * 7 * 5 * 5 * 6 * 5 * 4 * 6 * 4 * 5 * 6 * 6 * 6 * 5 * 5 * 6 * 4 * 5 * 4 * 5 * 4 * 5 * 5 * 4 * 5 * 5 * 3 * 5 * 5 * 5 * 5 * 5 * 3 * 5 * 5 * 5 * 5 * 3 * 2 * 3 * 3 * 4 * 5 * 4 * 3 * 2 * 3 * 4 * 4 * 3 * 3 * 4 * 5 * 5 * 5 = 4.62838344192 × 10^38
we get 4.62838344192 × 10^38 possibilities for the whole puzzle. How can we cope with that? There are two choices.

First, we could try a brute force approach. Suppose we have a very efficient program that takes only one instruction to evaluate a position,
and that we have access to the next-generation computing technology, let's say a 10GHz processor with 1024 cores, and let's say we could afford a million of them,
and while we're shopping, let's say we also pick up a time machine and go back 13 billion years to the origin of the universe and start our program running.
We can then compute that we'd be almost 1% done with this one puzzle by now:

((10 GHz) * 1024 * 1 million * (13 billion years)) / 4.6e38 = 0.00913229997 = 0.913229997 percent

The second choice is to somehow process much more than one possibility per machine instruction. That seems impossible,
but fortunately it is exactly what constraint propagation does for us. We don't have to try all 4 × 10^38 possibilities because
as soon as we try one we immediately eliminate many other possibilities. For example, square H7 of this puzzle has two possibilities, 6 and 9.
We can try 9 and quickly see that there is a contradiction. That means we've eliminated not just one possibility, but fully half of the 4 × 10^38 choices.

In fact, it turns out that to solve this particular puzzle we need to look at only 25 possibilities and
we only have to explicitly search through 9 of the 61 unfilled squares; constraint propagation does the rest.
For the list of 95 hard puzzles, on average we need to consider 64 possibilities per puzzle, and in no case do we have to search more than 16 squares.

What is the search algorithm? Simple: first make sure we haven't already found a solution or a contradiction, and
if not, choose one unfilled square and consider all its possible values. One at a time, try assigning the square each value, and
searching from the resulting position. In other words, we search for a value d such that we can successfully search for a solution
from the result of assigning square s to d. If the search leads to an failed position, go back and consider another value of d.
This is a recursive search, and we call it a depth-first search because we (recursively) consider all possibilities under values[s] = d before we consider a different value for s.

To avoid bookkeeping complications, we create a new copy of values for each recursive call to search.
This way each branch of the search tree is independent, and doesn't confuse another branch. (This is why I chose to implement
the set of possible values for a square as a string: I can copy values with values.copy() which is simple and efficient.
If I implemented a possibility as a Python set or list I would need to use copy.deepcopy(values), which is less efficient.)
The alternative is to keep track of each change to values and undo the change when we hit a dead end. This is known as backtracking search.
It makes sense when each step in the search is a single change to a large data structure, but is complicated when each assignment can lead to many other changes via constraint propagation.

There are two choices we have to make in implementing the search: variable ordering (which square do we try first?) and
value ordering (which digit do we try first for the square?). For variable ordering, we will use a common heuristic called
minimum remaining values, which means that we choose the (or one of the) square with the minimum number of possible values.
Why? Consider grid2 above. Suppose we chose B3 first. It has 7 possibilities (1256789), so we'd expect to guess wrong with probability 6/7.
If instead we chose G2, which only has 2 possibilities (89), we'd expect to be wrong with probability only 1/2. Thus we choose the square with
the fewest possibilities and the best chance of guessing right. For value ordering we won't do anything special; we'll consider the digits in numeric order.

Reference: Peter Norvig (co-author of Artificial Intelligence and CTO of Google) discussed them: http://norvig.com/sudoku.html

For more Sudoku Strategies:
http://www.sudokudragon.com/sudokustrategy.htm

===========================================================================================================================================================================================

Fastest Sudoku solvers:

BBSolve, fsss, and JSolve

*/

namespace mm {

	bool PuzzlesSudokuTester::m_printInput = true;
	bool PuzzlesSudokuTester::m_printOutput = true;

	string PuzzlesSudokuTester::getAlgoShortName(int index)
	{
		switch (index)
		{
		case SudokuSolvingAlgoTypes::bruteForce_Strategy_1: return "BruteForce: Strategy 1";
		case SudokuSolvingAlgoTypes::bruteForce_Strategy_1And2: return "BruteForce: Strategy 1 & 2";
		case SudokuSolvingAlgoTypes::bruteForce_Strategy_1AndOptimized2: return "BruteForce: Strategy 1 & Optimized-2";
		case SudokuSolvingAlgoTypes::bruteForce_Strategy_withNakedAndHidden: return "BruteForce: With Naked & Hidden";
		case SudokuSolvingAlgoTypes::bruteForce_Fastest_1: return "BruteForce: Fastest 1";
		case SudokuSolvingAlgoTypes::bruteForce_Fastest_2: return "BruteForce: Fastest 2";
		case SudokuSolvingAlgoTypes::bruteForce_Fastest_3: return "BruteForce: Fastest 3";
		case SudokuSolvingAlgoTypes::bruteForce_Fastest_4: return "BruteForce: Fastest 4";
		case SudokuSolvingAlgoTypes::bruteForce_Fastest_5: return "BruteForce: Fastest 5";
		case SudokuSolvingAlgoTypes::bruteForce_Fastest_6: return "BruteForce: Fastest 6";
		case SudokuSolvingAlgoTypes::bruteForce_Fastest_7: return "BruteForce: Fastest 7";
		case SudokuSolvingAlgoTypes::bruteForce_Fastest_14: return "BruteForce: Fastest 14";
		case SudokuSolvingAlgoTypes::DancingLinks_Naive: return "DancingLinks: Naive";
		case SudokuSolvingAlgoTypes::DancingLinks_NoMemoryAllocation: return "DancingLinks: NoMemoryAllocation";
		case SudokuSolvingAlgoTypes::DancingLinks_NoMemoryAllocationAndReadyStructure: return "DancingLinks: NoMemoryAllocation & ReadyStructure";
		case SudokuSolvingAlgoTypes::DancingLinks_Fastest_1: return "DancingLinks: Fastest";
		case SudokuSolvingAlgoTypes::Fsss_Solver: return "Fsss Solver";
		case SudokuSolvingAlgoTypes::JSolve: return "JSolve";
		case SudokuSolvingAlgoTypes::bb_sudoku: return "bb_sudoku";
		}

		return "";
	}

	template<>
	int PuzzlesSudokuTester::getAlgoIndex<SudokuMatrix3>() { return SudokuSolvingAlgoTypes::bruteForce_Strategy_1; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<SudokuMatrix4>() { return SudokuSolvingAlgoTypes::bruteForce_Strategy_1And2; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<SudokuMatrix5>() { return SudokuSolvingAlgoTypes::bruteForce_Strategy_1AndOptimized2; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<SudokuMatrix6>() { return SudokuSolvingAlgoTypes::bruteForce_Strategy_withNakedAndHidden; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FastestBruteForceSudokuMatrix_1>() { return SudokuSolvingAlgoTypes::bruteForce_Fastest_1; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FastestBruteForceSudokuMatrix_2>() { return SudokuSolvingAlgoTypes::bruteForce_Fastest_2; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FastestBruteForceSudokuMatrix_3>() { return SudokuSolvingAlgoTypes::bruteForce_Fastest_3; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FastestBruteForceSudokuMatrix_4>() { return SudokuSolvingAlgoTypes::bruteForce_Fastest_4; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FastestBruteForceSudokuMatrix_5>() { return SudokuSolvingAlgoTypes::bruteForce_Fastest_5; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FastestBruteForceSudokuMatrix_6>() { return SudokuSolvingAlgoTypes::bruteForce_Fastest_6; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FastestBruteForceSudokuMatrix_7>() { return SudokuSolvingAlgoTypes::bruteForce_Fastest_7; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FastestBruteForceSudokuMatrix_14>() { return SudokuSolvingAlgoTypes::bruteForce_Fastest_14; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<SudokuMatrixDancingLinks2>() { return SudokuSolvingAlgoTypes::DancingLinks_Naive; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<SudokuMatrixDancingLinks3>() { return SudokuSolvingAlgoTypes::DancingLinks_NoMemoryAllocation; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<SudokuMatrixDancingLinks4>() { return SudokuSolvingAlgoTypes::DancingLinks_NoMemoryAllocationAndReadyStructure; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FastestDancingLinksSudokuMatrix>() { return SudokuSolvingAlgoTypes::DancingLinks_Fastest_1; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<FsssSolver>() { return SudokuSolvingAlgoTypes::Fsss_Solver; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<JSolveMatrix>() { return SudokuSolvingAlgoTypes::JSolve; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<bb_sudoku_Matrix>() { return SudokuSolvingAlgoTypes::bb_sudoku; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<Microsoft_z3_solver_1>() { return SudokuSolvingAlgoTypes::microsoft_z3_solver_1; }
	template<>
	int PuzzlesSudokuTester::getAlgoIndex<Google_OR_tools_1>() { return SudokuSolvingAlgoTypes::google_OR_tools_1; }

	void PuzzlesSudokuTester::testPuzzlesSudokuSolverUsingBruteForce_0(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut)
	{
		Timer t;
		t.resetTimer();
		SudokuMatrix_0 obj(dataIn);
		obj.execute(solutionOut);
		cout << "\nTime required to solve: " << t.getDurationStringTillNowInNanoSeconds() << endl;
	}

	void PuzzlesSudokuTester::testPuzzlesSudokuSolverUsingBruteForce_1(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut)
	{
		//This is C++ implemenation of http://norvig.com/sudoku.html by Pau Fernandez. source: https://github.com/pauek/norvig-sudoku

		Sudoku::init();
		string line(81, '0');
		int size = dataIn.size();
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				line[i * size + j] = '0' + dataIn[i][j];

		Timer t;
		t.resetTimer();
		if (auto S = solve(unique_ptr<Sudoku>(new Sudoku(line)))) {
			cout << "\nTime required to solve: " << t.getDurationStringTillNowInNanoSeconds() << endl;
			cout << "\nSolution:\n";
			S->convertToVectorOfVector(solutionOut);
			SudokuPuzzleUtils::getLogger() << "\nNo. of total cells tried: " << Sudoku::m_countCellsTried;
			SudokuPuzzleUtils::getLogger() << "\nNo. of total values tried: " << Sudoku::m_countValuesTried;
		}
		else {
			SudokuPuzzleUtils::getLogger() << "\nERROR: No solution";
		}
	}

	void PuzzlesSudokuTester::testPuzzlesSudokuSolverUsingBruteForce_2(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut)
	{
		//This is solution using just first strategy: if any cell is assigned, remove that value from its Peers possible values

		Timer t;
		t.resetTimer();
		SudokuMatrix2 obj(dataIn);
		obj.solve(solutionOut);
		SudokuPuzzleUtils::getLogger() << "\nTime required to solve: " << t.getDurationStringTillNowInNanoSeconds() << "\n";
	}

	bool PuzzlesSudokuTester::testPuzzlesSudokuSolverUsingDancingLinks_1(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut)
	{
		SudokuMatrixDancingLinks1 obj(dataIn);
		if (obj.execute(solutionOut))
		{
			SudokuPuzzleUtils::getLogger() << "\nNo. of total cells tried: " << obj.m_countLinkUpdates;
			SudokuPuzzleUtils::getLogger() << "\nNo. of total values tried: " << obj.m_countValuesTried;
		}
		else
			SudokuPuzzleUtils::getLogger() << "\nCould not find solution";

		return true;
	}

	void PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(const vector< vector<int> >& sudokuPuzzleData, const int numSolutions, int solveUsingTheseAlgos, const bool presentInLibrary)
	{
		if (!presentInLibrary)
		{
			bool addedSuccessfully = SudokuPuzzleUniversalDatabase::addToLibrary(sudokuPuzzleData.size(), SudokuPuzzleUtils::convertToSingleLineString(sudokuPuzzleData));
			if (!addedSuccessfully)
				return;
		}

		if (m_printInput)
		{
			//SudokuPuzzleUtils::getLogger() << "\nPuzzle:";
			SudokuPuzzleUtils::printSudokuGrid(sudokuPuzzleData);
		}

		SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_category = SudokuPuzzleUniversalDatabase::getCurrentCategory();
		SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_clues = SudokuPuzzleUtils::countNumberOfClues(sudokuPuzzleData);

		SudokuPuzzleUtils::getLogger(BrightCyan)
			<< "\n\n"
			<< setw(35) << "Algo Short Name"
			<< setw(20) << "Time (ns)"
			<< setw(15) << "Cells"
			<< setw(15) << "Values"
			<< setw(15) << "Wrong Guesses"
			<< setw(15) << "Naked Singles"
			<< setw(15) << "Hidden Singles"
			<< setw(20) << "Locked Candidates"
			<< setw(15) << "Naked Pairs";

		//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		//SetConsoleTextAttribute(hConsole, 30);
		SudokuPuzzleUtils::getLogger().setConsoleTextColour(ConsoleTextColour::BrightWhite);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_3: This is MOST EFFICIENT, FAST and easy implementation of above using bitwise operations --------------------------";
		if(solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>())
			PuzzlesSudokuTester::solveSudokuPuzzle<SudokuMatrix3>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_4: This is an efficient implementation of same strategy in http://norvig.com/sudoku.html --------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>())
			PuzzlesSudokuTester::solveSudokuPuzzle<SudokuMatrix4>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_5: This is my own algo. Reference: http://norvig.com/sudoku.html --------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>())
			PuzzlesSudokuTester::solveSudokuPuzzle<SudokuMatrix5>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_6: This is my own algo. Implemented Naked and Hidden Single concepts --------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>())
			PuzzlesSudokuTester::solveSudokuPuzzle<SudokuMatrix6>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_7: This is FASTEST 1 algo. Implemented with Naked and Hidden Single concepts --------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_1>())
			PuzzlesSudokuTester::solveSudokuPuzzle<FastestBruteForceSudokuMatrix_1>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_7: This is FASTEST 2 algo. Implemented with Naked and Hidden Single concepts --------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_2>())
			PuzzlesSudokuTester::solveSudokuPuzzle<FastestBruteForceSudokuMatrix_2>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_7: This is FASTEST 2 algo. Implemented with Naked and Hidden Single concepts --------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_3>())
			PuzzlesSudokuTester::solveSudokuPuzzle<FastestBruteForceSudokuMatrix_3>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_7: This is FASTEST 2 algo. Implemented with Naked and Hidden Single concepts --------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_4>())
			PuzzlesSudokuTester::solveSudokuPuzzle<FastestBruteForceSudokuMatrix_4>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_7: This is FASTEST 2 algo. Implemented with Naked and Hidden Single concepts --------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_5>())
			PuzzlesSudokuTester::solveSudokuPuzzle<FastestBruteForceSudokuMatrix_5>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolver_7: This is FASTEST 2 algo. Implemented with Naked and Hidden Single concepts --------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_6>())
			PuzzlesSudokuTester::solveSudokuPuzzle<FastestBruteForceSudokuMatrix_6>(sudokuPuzzleData, numSolutions);

		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_7>())
			PuzzlesSudokuTester::solveSudokuPuzzle<FastestBruteForceSudokuMatrix_7>(sudokuPuzzleData, numSolutions);

		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_14>())
			PuzzlesSudokuTester::solveSudokuPuzzle<FastestBruteForceSudokuMatrix_14>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolverUsingDancingLinks_2: more efficient and no memeory leak --------------------------------------------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>())
			PuzzlesSudokuTester::solveSudokuPuzzle<SudokuMatrixDancingLinks2>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolverUsingDancingLinks_3: no memeory allocation --------------------------------------------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>())
			PuzzlesSudokuTester::solveSudokuPuzzle<SudokuMatrixDancingLinks3>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolverUsingDancingLinks_4: no memeory allocation & use of ready structure --------------------------------------------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>())
			PuzzlesSudokuTester::solveSudokuPuzzle<SudokuMatrixDancingLinks4>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\ntestPuzzlesSudokuSolverUsingDancingLinks_5: no memeory allocation & use of ready structure --------------------------------------------------------------";
		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FastestDancingLinksSudokuMatrix>())
			PuzzlesSudokuTester::solveSudokuPuzzle<FastestDancingLinksSudokuMatrix>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\n FsssSolver --------------------------------------------------------------";
		if (sudokuPuzzleData.size() == 9 && (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<FsssSolver>()))
			PuzzlesSudokuTester::solveSudokuPuzzle<FsssSolver>(sudokuPuzzleData, numSolutions);

		//Third party solvers:

		//SudokuPuzzleUtils::getLogger() << "\n JSolve --------------------------------------------------------------";
		if (sudokuPuzzleData.size() == 9 && (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<JSolveMatrix>()))
			PuzzlesSudokuTester::solveSudokuPuzzle<JSolveMatrix>(sudokuPuzzleData, numSolutions);

		//SudokuPuzzleUtils::getLogger() << "\n bb sudoku --------------------------------------------------------------";
		if (sudokuPuzzleData.size() == 9 && (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<bb_sudoku_Matrix>()))
			PuzzlesSudokuTester::solveSudokuPuzzle<bb_sudoku_Matrix>(sudokuPuzzleData, numSolutions);

		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<Microsoft_z3_solver_1>())
			PuzzlesSudokuTester::solveSudokuPuzzle<Microsoft_z3_solver_1>(sudokuPuzzleData, numSolutions);

		if (solveUsingTheseAlgos & PuzzlesSudokuTester::getAlgoFlag<Google_OR_tools_1>())
			PuzzlesSudokuTester::solveSudokuPuzzle<Google_OR_tools_1>(sudokuPuzzleData, numSolutions);

		//Verify if all algorithms produce same solution(s)
		for (int k = 1; k < SudokuSolvingAlgoTypes::totalAlgos; ++k)
		{
			if(SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[k - 1].m_numInvalidSolutions == 0 
				&& SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[k].m_numInvalidSolutions == 0)
			{
				if (SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[k - 1].m_numInvalidSolutions == SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[k].m_numInvalidSolutions
					&& SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[k - 1].m_solutions == SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[k].m_solutions)
					SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_base[k].m_solutions = "Same solution as previous algorithm";
			}
		}

		//SudokuPuzzleUtils::getLogger() << "\nPress any key to proceed...";
		//cin.get();
	}

	void PuzzlesSudokuTester::solveSudokuPuzzleDataSetsFromFile(const string& fullFilePath, const int dimension, const int numSolutions)
	{
		/*
		File format:
		Each line in file will start with a puzzle and // comments
		...3.....439.58.........3....79...2.....8............9...2....3..2..4.8....8.....  // Randomly generated
		*/
		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);
		SudokuPuzzleUtils::getLogger() << "\nSolving sudoku puzzles from file: " << fullFilePath;
		SudokuPuzzleUtils::getLogger().setPrintOnScreen(false);

		SudokuPuzzleUniversalDatabase::setCurrentCategory(fullFilePath);

		ifstream sudokuFile(fullFilePath, ios_base::in);

		// newlines will be skipped unless we stop it from happening
		sudokuFile.unsetf(std::ios_base::skipws);

		// Count the newlines with an algorithm specialized for counting
		unsigned int lineCount = std::count(std::istreambuf_iterator<char>(sudokuFile), std::istreambuf_iterator<char>(), '\n');

		sudokuFile.seekg(0); // rewind
		//sudokuFile.setf(std::ios_base::skipws);

		string line;
		if (sudokuFile.is_open())
		{
			int index = 0;
			cout << "\n";
			while (std::getline(sudokuFile, line))
			{
				//cout << "\rSolving sudoku puzzle " << ++index << " of " << lineCount;
				SudokuPuzzleUtils::getLogger() << "\nSolving sudoku puzzle " << index << " of " << lineCount;
				const string puzzle(line.substr(0, line.find("//", 0)));
				const string comment(line.substr(line.find("//", 0) + 2, string::npos));
				SudokuPuzzleUniversalDatabase::setCurrentCategory(comment);
				solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(puzzle, dimension), numSolutions, PuzzlesSudokuTester::getAllAlgoFlag(), false);
			}
		}
	}

	string PuzzlesSudokuTester::wchar_t2string(const wchar_t *wchar)
	{
		string str = "";
		int index = 0;
		while (wchar[index] != 0)
		{
			str += (char)wchar[index];
			++index;
		}
		return str;
	}

	wchar_t* PuzzlesSudokuTester::string2wchar_t(const string &str)
	{
		static wchar_t wchar[260];
		size_t index = 0;
		while (index < str.size())
		{
			wchar[index] = (wchar_t)str[index];
			++index;
		}
		wchar[index] = 0;
		return wchar;
	}

	void PuzzlesSudokuTester::solveSudokuPuzzleDataSetsFromDirectory(const string& directory_path, const int dimension, const int numSolutions)
	{
//#ifdef _WIN32
//#ifdef _MSC_VER

		WIN32_FIND_DATA fd;
#ifdef _MSC_VER
		//HANDLE hFind = ::FindFirstFile(string2wchar_t(directory_path + "/*.*"), &fd);
		HANDLE hFind = ::FindFirstFile(string(directory_path + "/*.*").c_str(), & fd);
#elif __GNUC__
		HANDLE hFind = ::FindFirstFile(string(directory_path + "/*.*").c_str(), &fd);
#endif
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				// read all (real) files in current folder
				// , delete '!' read other 2 default folder . and ..
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
#ifdef _MSC_VER
					//string directoryName(wchar_t2string(fd.cFileName));
					string directoryName(fd.cFileName);
#elif __GNUC__
					string directoryName(fd.cFileName);
#endif
					if (directoryName != "." && directoryName != "..")
						solveSudokuPuzzleDataSetsFromDirectory(directory_path + "/" + directoryName, dimension, numSolutions);
				}
				else
				{
#ifdef _MSC_VER
					//string fileName(wchar_t2string(fd.cFileName));
					string fileName(fd.cFileName);
#elif __GNUC__
					string fileName(fd.cFileName);
#endif
					solveSudokuPuzzleDataSetsFromFile(directory_path + "/" + fileName, dimension, numSolutions);
				}
			} while (::FindNextFile(hFind, &fd));
			::FindClose(hFind);
		}

//#elif __linux__
//#elif __GNUC__

//#else

//#endif
	}

	void PuzzlesSudokuTester::generateAndTestRandomSudokuPuzzleDataSets(const int count, const int dimension, const int numSolutions, const int solveUsingTheseAlgos)
	{
		SudokuPuzzleUniversalDatabase::setCurrentCategory("Randomly generated");
		cout << "\n";
		int iterations;
		long long timeRequiredToGeneratePuzzle = 0;
		for (int t = 0; t < count; ++t)
		{
			//cout << "\rGenerating random puzzle (Dimension: " << dimension << "): " << t + 1 << " of " << count;
			SudokuPuzzleUtils::getLogger() << "\n\n============= Generating random puzzle (Dimension: " << dimension << "): " << t + 1 << " of " << count;
			iterations = 0;
			solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleGenerator::generateSudokuPuzzle(dimension, iterations, timeRequiredToGeneratePuzzle), numSolutions, solveUsingTheseAlgos, false);
			SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = iterations;
			SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = timeRequiredToGeneratePuzzle;
		}
	}

	void PuzzlesSudokuTester::solveHardcodedSudokuPuzzles(const vector<SudokuPuzzleData>& sudokuPuzzleDataSets, const int numSolutions)
	{
		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);
		SudokuPuzzleUtils::getLogger() << "\n\n======================== Total number of puzzles in this set: " << sudokuPuzzleDataSets.size() << " ========================\n";
		SudokuPuzzleUtils::getLogger().setPrintOnScreen(false);

		cout << "\n";
		for (size_t i = 0; i < sudokuPuzzleDataSets.size(); i++)
		{
			cout << "\rSudoku Puzzle no.: " << i + 1 << " of " << sudokuPuzzleDataSets.size();
			SudokuPuzzleUtils::getLogger() << "\nSudoku Puzzle no.: " << i + 1 << " of " << sudokuPuzzleDataSets.size();
			SudokuPuzzleUtils::getLogger() << "\nCategory: " << (sudokuPuzzleDataSets[i].getCategory().empty() ? SudokuPuzzleUniversalDatabase::getCurrentCategory() : sudokuPuzzleDataSets[i].getCategory());
			
			if(!sudokuPuzzleDataSets[i].getCategory().empty())
				SudokuPuzzleUniversalDatabase::setCurrentCategory(sudokuPuzzleDataSets[i].getCategory());
			solveSudokuPuzzleUsingDifferentAlgo(sudokuPuzzleDataSets[i].getPuzzleData(), numSolutions, PuzzlesSudokuTester::getAllAlgoFlag(), false);
		}
	}

	void PuzzlesSudokuTester::solveHardcodedSudokuPuzzles(const int numSolutions)
	{
		const vector<SudokuPuzzleData>& simpleSudokuPuzzleDataSets = SudokuPuzzleDataFactory::getSimpleSudokuPuzzleDataSets();
		solveHardcodedSudokuPuzzles(simpleSudokuPuzzleDataSets, numSolutions);

		const vector<SudokuPuzzleData>& hardSudokuPuzzleDataSets = SudokuPuzzleDataFactory::getHardSudokuPuzzleDataSets();
		solveHardcodedSudokuPuzzles(hardSudokuPuzzleDataSets, numSolutions);

		const vector<SudokuPuzzleData>& hardestSudokuPuzzleDataSets = SudokuPuzzleDataFactory::getHardestSudokuPuzzleDataSets();
		solveHardcodedSudokuPuzzles(hardestSudokuPuzzleDataSets, numSolutions);

		const vector<SudokuPuzzleData>& sudokuPuzzleDataSetsFromSource1_easy = SudokuPuzzleDataFactory::getSudokuPuzzleDataSetsFromSource1_easy();
		solveHardcodedSudokuPuzzles(sudokuPuzzleDataSetsFromSource1_easy, numSolutions);

		const vector<SudokuPuzzleData>& sudokuPuzzleDataSetsFromSource1_simple = SudokuPuzzleDataFactory::getSudokuPuzzleDataSetsFromSource1_simple();
		solveHardcodedSudokuPuzzles(sudokuPuzzleDataSetsFromSource1_simple, numSolutions);

		const vector<SudokuPuzzleData>& sudokuPuzzleDataSetsFromSource1_intermediate = SudokuPuzzleDataFactory::getSudokuPuzzleDataSetsFromSource1_intermediate();
		solveHardcodedSudokuPuzzles(sudokuPuzzleDataSetsFromSource1_intermediate, numSolutions);

		const vector<SudokuPuzzleData>& sudokuPuzzleDataSetsFromSource1_expert = SudokuPuzzleDataFactory::getSudokuPuzzleDataSetsFromSource1_expert();
		solveHardcodedSudokuPuzzles(sudokuPuzzleDataSetsFromSource1_expert, numSolutions);
	}

	void PuzzlesSudokuTester::readExistngLibrary(const string& libraryFilePath, const bool solveAllPuzzlesInLibraryAgain, const int dimension, const int numSolutions, const int solveUsingTheseAlgos)
	{
		const map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>& refSudokuPuzzleLibrary = SudokuPuzzleUniversalDatabase::readExistngLibrary(libraryFilePath, solveAllPuzzlesInLibraryAgain);
		const int librarySize = refSudokuPuzzleLibrary.size();
		int index = 0;
		const bool presentInLibrary = true;
		if (solveAllPuzzlesInLibraryAgain)
		{
			cout << "\n";
			for (map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>::const_iterator it = refSudokuPuzzleLibrary.begin(); it != refSudokuPuzzleLibrary.end(); ++it)
			{
				//cout << "\rSolving puzzle no. " << ++index << " of " << librarySize;
				SudokuPuzzleUtils::getLogger() << "\n\n========Solving puzzle no. " << ++index << " of " << librarySize;
				solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(it->first.second, dimension), numSolutions, solveUsingTheseAlgos, presentInLibrary);
			}
		}
	}

	void PuzzlesSudokuTester::readExistngLibrary(const string& libraryFilePath, const bool solveAllPuzzlesInLibraryAgain, const int numSolutions, const int solveUsingTheseAlgos)
	{
		const map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>& refSudokuPuzzleLibrary = SudokuPuzzleUniversalDatabase::readExistngLibrary(libraryFilePath, solveAllPuzzlesInLibraryAgain);
		const int librarySize = refSudokuPuzzleLibrary.size();
		int index = 0;
		const bool presentInLibrary = true;
		if (solveAllPuzzlesInLibraryAgain)
		{
			cout << "\n";
			for (map<std::pair<unsigned int, string>, SudokuPuzzleStats, comparator>::const_iterator it = refSudokuPuzzleLibrary.begin(); it != refSudokuPuzzleLibrary.end(); ++it)
			{
				//cout << "\rSolving puzzle no. " << ++index << " of " << librarySize;
				SudokuPuzzleUtils::getLogger() << "\n\n========Solving puzzle no. " << ++index << " of " << librarySize;
				const int dimension = it->first.first;
				solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(it->first.second, dimension), numSolutions, solveUsingTheseAlgos, presentInLibrary);
			}
		}
	}

	int PuzzlesSudokuTester::getAllAlgoFlag()
	{
		// All 6 righmost flags are on
		return ((1 << SudokuSolvingAlgoTypes::totalAlgos) - 1);
	}

	void PuzzlesSudokuTester::testSudokuPuzzlesWithDimension9()
	{
		const int dimension = 9;
		const int numSolutions = 2;		
		PuzzlesSudokuTester::readExistngLibrary("../../../test/reports/Sudoku/universalSudokuPuzzleDataLibrary.csv", true, dimension, numSolutions, PuzzlesSudokuTester::getAllAlgoFlag());
		
		solveHardcodedSudokuPuzzles(numSolutions);
		solveSudokuPuzzleDataSetsFromDirectory("../../../test/data/Sudoku", dimension, numSolutions);
		solveSudokuPuzzleDataSetsFromFile("../../../test/reports/Sudoku/universalSudokuPuzzleDataLibrary.sudoku", dimension, numSolutions);
		const int numPuzzlesToGenerate = 50;
		generateAndTestRandomSudokuPuzzleDataSets(numPuzzlesToGenerate, dimension, numSolutions, PuzzlesSudokuTester::getAllAlgoFlag());

		SudokuPuzzleUniversalDatabase::writeToLibrary();
	}

	void PuzzlesSudokuTester::exhaustiveTestSudokuPuzzlesWithDimension9()
	{
		// Test 1 million random generated puzzles
		// Test Brute Force algo
		PuzzlesSudokuTester::exhaustiveTestSudokuPuzzlesWithDimension9Template<SudokuMatrix4>();
		//PuzzlesSudokuTester::exhaustiveTestSudokuPuzzlesWithDimension9Template<SudokuMatrix5>();

		// Test Dancing links
		//PuzzlesSudokuTester::exhaustiveTestSudokuPuzzlesWithDimension9Template<SudokuMatrixDancingLinks4>();
	}

	void PuzzlesSudokuTester::testSudokuPuzzlesOfAllDimensions(const int numPuzzlesToGenerate, const int numSolutions, bool solveLibraryPuzzlesAgain)
	{
		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);
		//const string libraryFilePath("../../../test/reports/Sudoku/universalSudokuPuzzlesLibraryOfDimension_" + to_string(dimension) + ".csv");
		const int algoFlag =
			0
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_3>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_7>()
			| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_14>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			;

		//PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, solveLibraryPuzzlesAgain, dimension, numSolutions, algoFlag);

		for (unsigned int boxDim = 8; boxDim <= 20; ++boxDim)
		{
			unsigned int dimension = boxDim * boxDim;
			generateAndTestRandomSudokuPuzzleDataSets(numPuzzlesToGenerate, dimension, numSolutions, algoFlag);

			//SudokuPuzzleUtils::getLogger() << "\nPress any key to proceed...";
			//cin.get();
		}

		//SudokuPuzzleUniversalDatabase::writeToLibrary();

		SudokuPuzzleUtils::getLogger() << "\nPress any key to proceed...";
		cin.get();
	}

	void PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(const vector<unsigned int>& dimensions, const int numPuzzlesToGenerate, const int numSolutions, bool solveLibraryPuzzlesAgain)
	{
		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);
		const string libraryFilePath("../../../test/reports/Sudoku/universalMultiDimensionalSudokuPuzzlesLibrary.csv");
		const int algoFlag =
			0
			| PuzzlesSudokuTester::getAlgoFlag<bb_sudoku_Matrix>()
			| PuzzlesSudokuTester::getAlgoFlag<JSolveMatrix>()
			| PuzzlesSudokuTester::getAlgoFlag<FsssSolver>()
//			| PuzzlesSudokuTester::getAlgoFlag<FastestDancingLinksSudokuMatrix>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_2>()
			| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_1>()
			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			;			

		PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, solveLibraryPuzzlesAgain, numSolutions, algoFlag);

		for(unsigned int i = 0; i < dimensions.size(); ++i)
			generateAndTestRandomSudokuPuzzleDataSets(numPuzzlesToGenerate, dimensions[i], numSolutions, algoFlag);

		SudokuPuzzleUniversalDatabase::writeToLibrary();

		//SudokuPuzzleUtils::getLogger() << "\nPress any key to proceed...";
		//cin.get();
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_1()
	{
		/*
		string example(
		"9 . 3 | . 1 . | . . ."
		". . . | . . . | 3 9 ."
		". . . | . . . | 4 5 ."
		"------ + ------ - +------"
		". . . | . . . | . . 1"
		". . . | . 5 . | . . ."
		". 8 . | 3 . . | . . ."
		"------ + ------ - +------"
		"8 6 . | . . . | . . ."
		"3 9 . | . . . | . . ."
		". 5 . | . . . | . . 3");
		*/

		/*
		string example(
		". . . | . . . | 9 . . "
		". . . | . . . | . . . "
		". 2 . | 9 1 . | . 7 . "
		"------+-------+------"
		". 8 . | . . . | . 4 7 "
		". . . | . . . | . 2 8 "
		". . . | . . 9 | . . . "
		"------+-------+------"
		". . . | . . 3 | 7 . . "
		"5 9 . | . 7 . | . 1 . "
		". 7 . | . . . | . . .");
		*/

		//vector< vector<int> > solutions(9, vector<int>(9, 0));
		//testPuzzlesSudokuSolver_4(example.getPuzzleData(), solutions);

		// Hardest- Here is the impossible puzzle that took 1439 seconds: From http://norvig.com/sudoku.html. This can not be solved by current algorithms
		// This puzzle takes maximum time to exit algo with no solutions
		//string example(".....5.8. | ...6.1.43 | ......... | .1.5..... | ...1.6... | 3.......5 | 53.....61 | ........4 | .........");
		//solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(example, dimension));

		//const int numSolutions = 5;
		//PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(".........|.........|........5|86.53....|3......58|...8..37.|...2635..|..3.....1|......8..", dimension), numSolutions);
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_2()
	{
		string example(
			". . | 4 ."
			". . | 3 ."
			"---- - +---- -"
			". 1 | 2 ."
			"2 . | 1 .");

		const int numSolutions = 1;
		const bool presentInLibrary = false;
		PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(example, 4),
			numSolutions,
//			PuzzlesSudokuTester::getAllAlgoFlag()
//			PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			,
			presentInLibrary);

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_3()
	{
		string example(
			"3 2 | 1 4"
			"1 4 | 3 2"
			"---- - +---- -"
			"1 3 | 4 1"
			"4 1 | 2 3"
		);

		const int numSolutions = 1;
		const bool presentInLibrary = false;
		PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(example, 4),
			numSolutions,
			//			PuzzlesSudokuTester::getAllAlgoFlag()
			//			PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			//PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
						PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			,
			presentInLibrary);

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_4()
	{
		string example(
			".  .  .  . | .  .  .  8 | .  .  .  . | 10  .  .  ."
			"13  . 12  . | .  .  .  . | 16  .  .  . | .  .  .  ."
			". 16  .  . | .  .  .  . | .  .  .  . | .  .  .  ."
			"15  .  .  . | 12  . 11  4 | . 10  .  . | .  . 13  7 "
			"------------ - +------------ - +------------ - +------------ -"
			".  .  8  . | .  . 12  . | .  .  .  . | .  .  .  ."
			".  7  .  . | .  .  . 15 | 12  .  3 16 | .  .  .  ."
			"2  .  .  . | 16  . 14 10 | . 15  9  . | .  .  .  ."
			".  .  .  . | . 13  .  . | .  .  .  . | .  .  5  ."
			"------------ - +------------ - +------------ - +------------ -"
			".  .  .  . | .  .  .  . | .  9  .  . | .  .  .  ."
			".  .  .  . | .  .  .  . | .  .  .  7 | 1  .  4 12 "
			"3 12  .  . | 6  .  .  1 | .  .  8  . | .  .  .  ."
			".  . 11  . | .  .  .  5 | .  4  .  . | .  .  .  ."
			"------------ - +------------ - +------------ - +------------ -"
			".  .  .  . | .  .  .  . | 10  .  4  . | . 13  .  ."
			".  8  5  . | .  .  .  . | .  .  .  . | .  .  .  ."
			".  . 14  . | . 10  2  . | .  .  .  . | .  8  .  ."
			".  .  4  . | 13  .  .  9 | 6  .  .  . | .  .  .  ."
		);

		const int numSolutions = 1;
		const bool presentInLibrary = false;
		PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(example, 16),
			numSolutions,
			//			PuzzlesSudokuTester::getAllAlgoFlag()
			//			PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
			//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//			| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			,
			presentInLibrary);

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();

		/*
			Algo Short Name : BruteForce: With Naked & Hidden
			Solution no. : 1
			5  4  3  7 | 1 16  6  8 | 13 11 14 15 | 10  9 12  2
			13  9 12 14 | 7  5 10  3 | 16  2  1  4 | 8  6 11 15
			10 16  6 11 | 14 15 13  2 | 9  8  7 12 | 3  4  1  5
			15  1  2  8 | 12  9 11  4 | 3 10  5  6 | 14 16 13  7
			------------ - +------------ - +------------ - +------------ -
			11 14  8  3 | 2  1 12  6 | 4  7 10  5 | 13 15  9 16
			4  7  9 13 | 11  8  5 15 | 12  6  3 16 | 2  1 10 14
			2  5  1  6 | 16  3 14 10 | 8 15  9 13 | 12 11  7  4
			12 15 10 16 | 4 13  9  7 | 14  1  2 11 | 6  3  5  8
			------------ - +------------ - +------------ - +------------ -
			14  2  7  1 | 10  4 16 13 | 11  9 12  3 | 15  5  8  6
			8  6 13  5 | 9  2  3 11 | 15 14 16  7 | 1 10  4 12
			3 12 15  4 | 6 14  7  1 | 5 13  8 10 | 9  2 16 11
			16 10 11  9 | 15 12  8  5 | 1  4  6  2 | 7 14  3 13
			------------ - +------------ - +------------ - +------------ -
			6 11 16 12 | 8  7 15 14 | 10  3  4  1 | 5 13  2  9
			9  8  5 10 | 3  6  4 12 | 2 16 13 14 | 11  7 15  1
			1 13 14 15 | 5 10  2 16 | 7 12 11  9 | 4  8  6  3
			7  3  4  2 | 13 11  1  9 | 6  5 15  8 | 16 12 14 10

			Time required to solve : 26, 554, 873 nanoseconds
			No.of total cells tried or Link Updates : 667
			No.of total values tried : 1384

			Algo Short Name : BruteForce: Strategy 1 & Optimized - 2
			Solution no. : 1
			5  4  3  7 | 1 16  6  8 | 13 11 14 15 | 10  9 12  2
			13  9 12 14 | 7  5 10  3 | 16  2  1  4 | 8  6 11 15
			10 16  6 11 | 14 15 13  2 | 9  8  7 12 | 3  4  1  5
			15  1  2  8 | 12  9 11  4 | 3 10  5  6 | 14 16 13  7
			------------ - +------------ - +------------ - +------------ -
			11 14  8  3 | 5  2 12  6 | 4  7 10 13 | 15  1 16  9
			4  7  9  5 | 8 11  1 15 | 12  6  3 16 | 2 10 14 13
			2  6  1 13 | 16  3 14 10 | 8 15  9  5 | 12 11  7  4
			12 15 10 16 | 4 13  9  7 | 11  1  2 14 | 6  3  5  8
			------------ - +------------ - +------------ - +------------ -
			8  5  7  1 | 2  4  3 13 | 14  9 12 10 | 11 15  6 16
			14 10 13  6 | 9  8 15 11 | 2  3 16  7 | 1  5  4 12
			3 12 15  4 | 6 14 16  1 | 5 13  8 11 | 7  2  9 10
			16  2 11  9 | 10 12  7  5 | 15  4  6  1 | 13 14  8  3
			------------ - +------------ - +------------ - +------------ -
			7  3 16  2 | 11  1  5 12 | 10 14  4  8 | 9 13 15  6
			9  8  5 15 | 3  6  4 14 | 1 12 13  2 | 16  7 10 11
			6 13 14 12 | 15 10  2 16 | 7  5 11  9 | 4  8  3  1
			1 11  4 10 | 13  7  8  9 | 6 16 15  3 | 5 12  2 14

			Time required to solve : 2, 744, 065 nanoseconds
			No.of total cells tried or Link Updates : 90
			No.of total values tried : 90

			*/
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_5()
	{		
		vector<string> examples({
			// Amazing puzzle with 21 clues, requires 172 guesses (requires 39 guesses with modified algo)
			"8..........36......7..9.2...5...7.......457.....1...3...1....68..85...1..9....4..",
			//required guesses: 18
			"..53.....8......2..7..1.5..4....53...1..7...6..32...8..6.5....9..4....3......97..",
			//required guesses: 22
			". . 7 | 5 . . | 4 . .. . 4 | . . . | . . .. . 5 | . . . | . . .. . 3 | . . . | . . .. . 6 | . 3 . | . . .. . . | . . 7 | 5 . 3. . . | . 9 . | . . .. . . | 3 7 6 | 2 8 .3 . . | . . . | . . .",
			//required guesses: 32
			".........|...3.6.4.|.........|....2....|.61.....3|9..7....8|.3.......|.74...9.2|2.6......",
			//required guesses: 29
			"45....9.8|.8.......|..6......|.........|.........|.........|9458.....|....5784.|827......",
			//required guesses: 28
			"........3|..673....|....6....|.........|.658.....|.........|698.....5|251.....4|3....1...",
			//required guesses: 27
			".........|.....9..4|.....83..|..8.9....|....24...|....3....|...45....|.7591....|..9.8....",
			//required guesses: 0
			".......1.4.........2...........5.4.7..8...3....1.9....3..4..2...5.1........8.6...",
			//required guesses: 0
			".......1.4.........2...........5.6.4..8...3....1.9....3..4..2...5.1........8.7...",
			//required guesses: 0
			".......12....35......6...7.7.....3.....4..8..1...........12.....8.....4..5....6..",
			//required guesses: 0
			".......12..36..........7...41..2.......5..3..7.....6..28.....4....3..5..........."
		});

		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);
		
		const int solveUsingTheseAlgos =
			0
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_2>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_3>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_5>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_7>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_14>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestDancingLinksSudokuMatrix>()
			//| PuzzlesSudokuTester::getAlgoFlag<FsssSolver>()
			//| PuzzlesSudokuTester::getAlgoFlag<JSolveMatrix>()
			//| PuzzlesSudokuTester::getAlgoFlag<bb_sudoku_Matrix>()	
			| PuzzlesSudokuTester::getAlgoFlag<Microsoft_z3_solver_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<Google_OR_tools_1>()
			;

		m_printInput = true;
		m_printOutput = false;
		unsigned int numSolutions = 1;
		unsigned int dimension = 9;

		//const string libraryFilePath("../../../test/reports/Sudoku/universalMultiDimensionalSudokuPuzzlesLibrary.csv");
		//PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, solveLibraryPuzzlesAgain, numSolutions, algoFlag);

		for (unsigned int i = 0; i < examples.size(); ++i)
		{
			int iterations = 0;
			long long timeRequiredToGeneratePuzzle = 0;
			SudokuPuzzleUtils::getLogger() << "\n\nPuzzle No. " << i + 1;
			PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(examples[i], dimension), numSolutions, solveUsingTheseAlgos, false);
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = iterations;
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = timeRequiredToGeneratePuzzle;
		}

		//SudokuPuzzleUniversalDatabase::writeToLibrary();
		m_printInput = true;
		m_printOutput = true;

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_6()
	{
		vector<string> examples({
			"  .  .  .  .  .  . |  1 25  .  .  . 24 |  .  .  .  .  .  . |  .  .  .  .  2  . |  .  .  .  .  .  . |  . 21  .  .  .  . "
			"  .  .  .  .  .  . |  .  .  .  .  .  . |  .  .  .  .  .  . |  .  .  .  . 24  . |  .  .  .  .  .  . | 31  .  .  .  .  . "
			"  . 29  . 20 26 34 |  .  .  .  .  . 16 |  .  .  .  .  .  . | 13  .  .  .  .  8 |  .  .  .  .  . 10 |  .  .  .  .  .  . "
			"  .  .  .  .  .  4 |  .  .  .  .  .  . |  .  .  .  .  .  . | 15  . 36  .  6  . |  .  .  .  .  2  . |  .  .  .  .  5  3 "
			"  .  . 18  . 11 16 |  .  .  . 31  .  . |  .  .  .  .  .  . |  .  .  .  .  .  9 |  .  .  .  . 17  . |  .  .  . 10  .  . "
			"  5  .  .  . 35  3 | 14  .  .  . 12  . | 11  .  .  .  .  . |  .  . 27  .  . 28 |  .  .  .  .  .  6 |  .  2  .  .  .  . "
			"-------------------+-------------------+-------------------+-------------------+-------------------+-------------------"
			"  .  . 11  7 24  . |  . 16  .  .  .  . |  .  .  .  .  .  . |  .  .  .  9  5  . |  .  . 26 21  3  . |  .  .  .  .  .  . "
			"  . 35  .  .  .  . |  .  .  .  .  .  . |  . 33  .  .  . 36 | 21  .  .  8 34  . |  .  .  .  .  4  . |  .  .  .  .  .  . "
			"  .  .  .  . 34  . |  .  .  .  .  . 11 |  .  .  .  .  .  . |  .  .  .  .  .  . |  .  .  .  .  .  9 | 14 27 24  .  . 26 "
			" 17  .  .  5  . 10 |  .  .  .  .  .  . |  . 35  . 27 15 34 | 18  . 19 31  .  . |  .  .  .  .  8  . |  .  .  .  .  . 16 "
			"  .  .  . 33  . 12 |  .  .  . 32  . 26 |  9  .  .  .  .  . |  .  6  . 30  7 15 | 11  .  .  .  .  . |  .  .  .  .  .  . "
			"  .  .  .  .  .  . |  . 15 18  .  .  . |  . 17  .  .  .  . |  .  .  .  .  . 36 |  1  .  .  .  . 22 |  .  .  .  . 19 20 "
			"-------------------+-------------------+-------------------+-------------------+-------------------+-------------------"
			"  .  .  .  4  .  . |  .  .  .  .  .  . |  .  .  .  .  .  . | 25  .  .  .  .  3 |  .  .  .  .  . 29 |  .  .  .  .  .  . "
			"  .  2  .  . 18  . |  .  .  .  .  .  . |  .  3 35 36  .  . |  . 27 17  .  .  . |  5  .  .  .  .  . |  .  .  .  .  .  . "
			"  .  .  .  .  5  . |  .  .  .  .  .  1 |  .  .  . 33 31  . | 34  .  .  .  .  . |  .  .  .  .  .  . |  .  .  .  .  .  4 "
			"  .  .  .  .  .  . |  .  .  3  .  .  . |  .  .  .  .  . 12 | 19  .  .  .  .  . |  .  .  .  .  .  . |  7  .  .  .  . 36 "
			"  .  .  .  3  .  . |  .  .  2 15  .  . | 14  .  5  . 34  . |  .  . 20  .  .  . |  .  .  .  .  6  . |  .  .  .  .  .  . "
			"  8  .  .  .  .  . |  .  .  .  .  .  . |  . 13  .  .  .  . |  .  .  2 22  .  . |  .  .  .  . 23  . |  .  9  .  .  .  . "
			"-------------------+-------------------+-------------------+-------------------+-------------------+-------------------"
			"  . 10 26  . 13  . | 11  .  .  .  .  6 |  .  7  .  .  .  . | 29  . 22 24 27 31 |  .  .  .  .  .  . |  4 36  . 33  .  . "
			"  .  .  .  . 28  . |  .  .  .  .  .  . |  .  .  . 15  .  . |  .  .  .  .  . 20 |  .  .  .  . 12  3 | 30 22  .  .  .  . "
			" 31  .  4 15  .  . |  2 32  .  .  . 29 |  .  .  . 28 18  3 |  .  .  .  .  .  . |  .  8  .  1  .  . |  . 24 26  .  .  . "
			"  .  .  .  . 30  . |  .  .  .  .  .  . |  .  .  .  .  . 33 |  .  . 23  .  .  . |  .  .  .  .  . 13 |  .  .  .  . 27  . "
			" 19  3  9 24  .  . |  .  .  .  .  .  . |  5  .  .  . 13  . |  .  7  .  .  .  . |  .  .  .  .  .  . |  .  .  .  . 32  . "
			"  .  . 12  .  .  . |  . 30  . 26  .  . |  .  .  2 32  .  . |  . 28 33  .  .  . |  .  .  .  .  . 19 |  .  .  .  .  .  . "
			"-------------------+-------------------+-------------------+-------------------+-------------------+-------------------"
			"  .  .  .  .  .  . |  .  .  . 27  .  . | 10  .  .  .  .  . |  .  .  4  .  .  . |  . 28  .  .  .  . | 29  .  .  .  .  . "
			"  .  . 32  . 29  . |  .  .  .  . 28  . |  .  .  .  .  .  . |  .  .  .  .  .  . |  . 25  .  .  .  . | 24  .  . 30 35  7 "
			"  .  .  .  . 16  . |  .  .  . 10  .  . | 36  .  .  .  .  . |  .  .  .  .  .  . |  .  .  .  .  .  . |  . 11 34  .  .  . "
			"  . 12 31  .  .  . |  .  .  .  .  .  . |  .  .  .  .  .  . | 32 26  .  .  .  . | 24 13 21  7 14  4 | 15  .  .  .  .  . "
			"  .  .  .  6 15 13 |  .  .  .  .  .  . |  .  .  3  7 23  . |  .  .  .  .  .  . |  . 10  5  9  .  . |  .  .  .  .  .  . "
			"  .  .  .  .  .  8 |  .  .  .  .  . 13 |  .  .  .  .  .  . |  .  .  . 25  .  . |  .  .  .  .  .  . |  .  .  .  .  .  . "
			"-------------------+-------------------+-------------------+-------------------+-------------------+-------------------"
			" 25  .  .  .  .  . |  .  .  . 14  .  . | 31  .  .  .  . 30 |  .  .  .  .  .  . |  .  .  .  .  . 18 |  .  .  5  .  .  . "
			"  .  .  . 26  .  . | 10  . 32 29  .  . |  .  .  . 25  6  . |  .  8  . 19  .  . |  .  .  .  .  .  . |  .  .  .  .  .  . "
			"  .  . 13  .  .  . |  .  .  .  .  . 12 |  .  .  .  .  5 35 |  .  .  . 36  .  . |  .  .  .  . 16  . |  .  . 17 18  .  . "
			"  .  4  .  .  .  . |  .  . 20 17  .  . |  .  . 18  . 21  . |  1  .  .  .  .  . |  .  . 13  5  . 28 | 16  .  .  . 22 19 "
			"  .  . 17  .  .  . |  .  .  .  .  .  . |  . 34  .  .  .  . |  .  .  .  .  . 10 | 27  .  .  .  .  . |  .  .  .  .  .  . "
			"  . 21  .  .  . 11 |  .  .  .  .  4  . | 32  .  . 20 33  . |  .  . 12  .  .  . |  .  7  .  .  .  . |  .  . 36  .  .  . "
		});

		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);

		const int solveUsingTheseAlgos =
			0
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_2>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_3>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_5>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_7>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestDancingLinksSudokuMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<FsssSolver>()
			////| PuzzlesSudokuTester::getAlgoFlag<JSolveMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<bb_sudoku_Matrix>()
			| PuzzlesSudokuTester::getAlgoFlag<Microsoft_z3_solver_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<Google_OR_tools_1>()
			;

		unsigned int numSolutions = 1;
		unsigned int dimension = 36;

		//const string libraryFilePath("../../../test/reports/Sudoku/universalMultiDimensionalSudokuPuzzlesLibrary.csv");
		//PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, solveLibraryPuzzlesAgain, numSolutions, algoFlag);

		for (unsigned int i = 0; i < examples.size(); ++i)
		{
			int iterations = 0;
			long long timeRequiredToGeneratePuzzle = 0;
			SudokuPuzzleUtils::getLogger() << "\n\nPuzzle No. " << i + 1;
			PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(examples[i], dimension), numSolutions, solveUsingTheseAlgos, false);
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = iterations;
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = timeRequiredToGeneratePuzzle;
		}

		//SudokuPuzzleUniversalDatabase::writeToLibrary();

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_7()
	{
		vector<string> examples({
			"  .  .  . 34 29  .  . |  .  .  .  .  4  .  . | 27  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  5  9  . |  .  . 36  .  .  .  . |  .  .  .  .  . 38 40 "
			"  .  9 33 25  .  .  . |  . 36  . 18 29 24 34 | 43 32 35 44  . 28  . |  . 31 23  .  .  .  . | 12 37  .  .  . 11  . |  .  .  .  .  . 26  . |  3  .  .  .  .  .  . "
			"  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  . 45  .  .  .  .  9 |  .  .  8  .  . 38  . | 46  .  . 40 48  .  . |  . 10  .  .  .  .  . |  .  .  .  .  .  . 47 "
			"  . 16  . 15  . 38  . |  .  .  .  .  .  . 25 | 40 48 49  .  .  .  . |  .  .  .  .  .  . 46 |  .  .  .  .  . 47  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . "
			"  .  .  .  .  .  .  . |  . 46  . 23  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  . 12  . |  .  . 29 21 15  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  . 37 "
			"  .  .  . 27  .  . 39 | 26  .  .  .  .  .  . |  .  .  .  .  .  . 46 | 10  .  .  .  .  .  . |  .  .  .  . 49  7  . |  .  .  .  .  .  .  . |  .  .  .  8  .  .  . "
			"  .  .  .  .  .  .  . |  7  . 32  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  4  .  .  .  .  .  . | 23  . 28  .  .  . 34 |  .  .  .  .  . 48  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  . 28  .  .  . 12 31 |  . 41  .  .  .  .  . |  4  .  .  .  .  .  7 |  . 30  .  .  .  .  . |  . 24 32  . 34  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . "
			"  6 38  . 29 19  .  . |  .  . 33  .  .  .  . | 49  .  . 47  .  .  . |  .  .  .  .  .  .  . |  .  .  .  . 11 27  . |  .  .  .  .  .  .  . |  .  .  .  . 25  . 16 "
			" 16  .  .  .  .  .  . |  .  .  .  .  .  .  . |  . 36  .  .  .  . 27 |  .  .  .  .  .  .  . |  .  .  .  .  .  . 47 | 33 42  .  .  .  . 40 |  .  .  .  .  .  .  . "
			"  .  .  . 17  .  .  . | 19  .  .  .  .  .  . | 22  .  .  .  . 35  . |  .  .  .  .  .  .  . |  . 28 20  .  .  .  . |  .  . 26 18  . 32  . |  8  .  .  .  .  .  . "
			"  .  .  .  . 30  .  . |  . 17 29 16  .  .  . |  .  .  .  . 25  .  . |  .  .  .  . 48  .  . |  .  .  8  .  . 14  . |  .  .  .  .  .  .  . |  .  .  . 22  .  .  . "
			"  .  .  .  .  . 43 14 |  .  .  . 24 35  .  . | 41  .  .  .  .  .  . |  . 16 29  .  .  .  . |  .  .  . 17  .  .  . |  .  .  .  .  . 22 31 |  .  .  .  .  7 15 18 "
			"  .  .  . 13  .  .  7 |  .  . 15  . 28 38  . |  .  .  . 37  .  .  . | 49  .  .  .  .  .  8 | 39  . 10  .  6  .  . |  .  .  .  .  . 16  . |  .  .  .  .  .  .  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  . 23  .  .  .  .  . |  .  .  .  .  . 26 33 |  .  .  .  .  .  .  . | 48  .  .  .  .  .  . "
			"  .  . 29  .  .  .  . |  . 22 40  .  .  .  . | 38  .  .  .  .  .  3 |  .  .  . 43  .  .  . |  .  .  .  .  .  .  . |  .  .  . 23  .  .  . |  .  .  .  .  1 14  . "
			"  .  .  . 30 37  .  . |  2  .  . 14  .  .  . |  . 43  . 22  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  . 12 42  .  .  . 41 | 15 16  .  . 47 46  . "
			" 25  .  . 16  .  .  . |  .  .  . 28 46 26  . |  . 19  2  .  .  . 42 | 29  . 22  . 20 27  . |  .  .  . 12  .  .  . |  .  . 47  .  .  . 49 |  . 38  .  .  .  .  . "
			"  .  .  .  .  .  5  . |  .  .  .  .  .  .  . | 39  6  .  .  .  .  . |  .  .  .  .  . 18 12 |  .  .  . 48  .  .  . |  .  .  9 16  .  7  . |  .  .  . 36 37  .  . "
			"  .  .  .  6 10  .  . |  . 34  . 20  .  .  . |  .  .  . 33  .  . 18 |  . 13  .  . 32  . 31 |  .  .  .  . 47 19  . |  .  .  .  .  .  .  . |  4  2  . 17  .  . 39 "
			" 42  .  . 31  .  .  . |  .  .  .  . 45 25 39 | 23  .  . 36 47  . 24 | 30  .  9  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  . 10 | 49  .  .  . 11  .  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  .  .  . 14  .  .  . |  .  .  .  .  .  .  . |  .  .  .  . 43  .  . |  .  . 19  .  .  . 15 |  .  . 24  .  .  .  . |  . 13 27 28  . 17  . |  .  .  .  .  .  .  . "
			"  .  .  .  .  .  .  . |  . 14  .  .  .  .  . |  .  .  .  .  . 20  . |  . 24  . 16 27  .  . |  .  5  .  . 23  .  . | 26 49  .  .  .  .  . |  .  .  . 48 15  .  . "
			"  .  .  . 49  .  . 24 |  .  .  . 19  .  . 40 |  .  .  .  .  .  .  . |  .  .  .  .  .  . 20 |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  . 34  .  .  .  . "
			"  .  .  .  .  . 19  . | 10  .  .  .  .  .  . | 34  .  .  .  . 33  . | 38  .  .  . 40 25  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  . 18  .  .  . "
			"  .  .  .  .  .  . 20 |  . 49  . 43  .  6  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  . 31  .  .  .  . 15 |  .  .  .  .  .  .  . "
			"  .  .  .  . 46 16  . |  .  .  .  .  .  .  . |  .  .  .  .  8  .  2 |  . 18 34  . 10  .  . |  .  .  .  .  .  .  . | 43  . 25  .  .  .  . |  .  . 27 19 32  .  . "
			" 15  . 40  .  . 47  . |  .  .  .  .  .  .  . |  .  .  .  . 27  .  . |  .  .  .  4 39  9  . |  .  3  .  .  .  8 34 |  .  . 30  .  .  . 18 |  .  .  .  .  .  .  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  .  .  .  .  .  .  . |  .  .  .  .  . 23  . |  .  .  .  . 42 15  . |  . 33 14  .  . 39  . |  .  . 36  .  .  .  . |  .  .  . 24  .  .  . |  .  . 22  . 17  . 19 "
			"  . 12  .  .  . 41  . |  .  .  .  .  .  .  . |  . 40  .  . 44  .  . | 19  .  .  . 15 42  . |  .  . 26  .  .  .  . |  .  .  .  .  .  .  . |  .  5  .  .  .  .  . "
			"  . 26  .  .  .  . 48 |  .  .  .  . 16 30  . |  .  .  . 41  .  .  . | 32 17  .  .  .  .  . |  9  .  .  .  .  .  . |  .  .  . 22  .  . 33 |  .  .  . 35  . 21  . "
			"  .  5 49  .  .  .  . |  4 38  8  .  .  .  . |  .  .  .  .  7 47  . |  .  .  . 45  .  .  . |  . 15  .  .  .  .  . | 31  .  .  .  .  .  6 |  .  .  .  .  .  .  . "
			"  . 11  .  .  .  .  . |  .  . 25  .  . 31  . |  .  .  .  .  .  .  . |  4  .  .  .  .  .  . |  .  .  .  .  .  . 35 | 49  . 44  .  .  .  . |  .  .  .  .  .  .  . "
			"  . 22  . 47 35 10  . |  3  .  . 21 13  .  . |  . 17  . 11 20 12 38 | 48  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . "
			" 27  .  .  .  .  .  . |  . 15  .  .  . 46  . |  . 37  4 39 28  .  . |  .  7  .  .  .  .  . |  .  .  .  .  .  .  . |  . 48  .  .  . 21  . | 34  .  .  .  .  .  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  . 45  .  7 36 29  . |  .  .  .  .  . 16  . |  2  .  .  .  . 27 12 |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . "
			" 35  .  .  .  . 17 47 |  .  9  .  . 38  . 12 |  3  . 36  .  .  .  . |  .  .  .  .  .  .  . |  .  . 42  4  .  . 24 | 30 32  .  .  .  .  . |  .  .  .  .  .  .  . "
			"  .  .  . 22 38 13 37 | 31  .  .  .  .  .  7 | 42  .  . 40 19  1  . |  .  .  .  .  . 48  . |  .  .  . 34  .  .  . |  . 27  .  .  .  .  . | 33  .  .  .  . 24 43 "
			"  . 49  .  .  .  .  . |  .  .  .  . 39 48  . |  . 11  .  .  .  .  . | 40  .  .  . 47 37  . | 25 22  .  .  .  .  . |  8  .  .  .  .  .  . |  . 41 35  .  .  .  . "
			"  .  .  .  . 33  .  . |  .  .  . 29  .  .  . |  .  .  .  . 21 44  . |  . 22  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  . 19 |  .  .  .  .  .  .  . "
			"  .  . 23  .  . 18 26 |  .  .  .  . 15  .  . |  .  .  .  .  .  .  . |  . 49 27  .  .  .  . |  .  .  .  . 19  .  . |  . 40  5  .  .  .  . |  .  .  .  .  .  .  . "
			"  .  .  .  .  . 31  . | 27  .  .  4  .  . 26 |  .  .  .  .  .  .  . |  . 36  .  .  .  .  . | 29  .  .  .  .  .  . |  .  .  3  .  .  .  . | 28 21  .  . 30  .  9 "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  .  .  .  .  .  .  . |  .  .  .  .  . 44  . |  . 47  .  .  .  .  . |  . 25 17  . 18  .  . |  .  .  .  .  .  .  . | 22  .  .  .  .  .  . |  .  .  .  4  .  .  . "
			"  1  . 10  .  . 11  . |  .  . 34 42  .  . 38 |  .  .  .  .  .  3  . |  . 37 26  .  .  . 47 |  .  .  .  .  .  .  . |  . 21  .  .  .  .  . |  .  .  .  .  .  .  5 "
			"  .  .  .  .  .  . 12 | 43  .  .  .  . 36  . |  .  7  .  .  . 48  . |  .  .  .  .  .  .  . |  .  . 22  .  . 49  . |  . 47  . 17 19 29  . |  .  .  .  .  .  .  . "
			"  .  .  .  .  . 33  . |  .  . 35  . 32  .  . |  .  .  . 46  .  .  . |  . 19  .  .  .  .  . | 36  .  . 16  7 48 26 |  .  1 39  .  . 30  . |  .  .  .  .  .  .  . "
			" 49 40  .  .  .  .  . |  .  .  .  .  .  3  5 |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  . 14  .  6  .  .  . |  .  .  .  . 20  .  . |  .  .  .  .  .  .  . "
			"  .  .  .  .  .  .  . |  .  1 18  .  .  .  . |  .  .  .  .  .  . 43 |  .  .  . 10 30  .  . | 27  .  .  .  .  .  . | 16  .  .  .  . 31  . | 17  .  .  .  . 32  . "
			"  . 27  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  . 21  . |  .  .  .  .  .  .  . |  .  .  .  . 40 10  . | 48 33  .  .  .  .  . | 35 46  .  .  .  .  . "
		});

		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);

		const int solveUsingTheseAlgos =
			0
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_2>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_3>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_5>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_6>()
			| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_7>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestDancingLinksSudokuMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<FsssSolver>()
			////| PuzzlesSudokuTester::getAlgoFlag<JSolveMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<bb_sudoku_Matrix>()
			| PuzzlesSudokuTester::getAlgoFlag<Microsoft_z3_solver_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<Google_OR_tools_1>()
			;

		unsigned int numSolutions = 1;
		unsigned int dimension = 49;

		//const string libraryFilePath("../../../test/reports/Sudoku/universalMultiDimensionalSudokuPuzzlesLibrary.csv");
		//PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, solveLibraryPuzzlesAgain, numSolutions, algoFlag);

		for (unsigned int i = 0; i < examples.size(); ++i)
		{
			int iterations = 0;
			long long timeRequiredToGeneratePuzzle = 0;
			SudokuPuzzleUtils::getLogger() << "\n\nPuzzle No. " << i + 1;
			PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(examples[i], dimension), numSolutions, solveUsingTheseAlgos, false);
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = iterations;
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = timeRequiredToGeneratePuzzle;
		}

		//SudokuPuzzleUniversalDatabase::writeToLibrary();

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_8()
	{
		vector<string> examples({
			" 16 21  .  .  . 39 19 |  .  .  .  .  .  . 44 |  .  .  .  .  .  .  . |  .  .  .  .  .  . 45 |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  . 48 14 "
			"  .  .  .  . 13  .  . | 26  .  .  .  .  .  . |  .  .  .  .  . 42 33 |  .  .  .  .  .  .  . |  . 31  .  . 11  . 16 |  . 24  . 14  .  . 18 |  .  .  .  .  .  .  . "
			"  .  .  . 17 22  .  5 | 27  .  2 11  . 14 31 |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  7  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . "
			"  .  .  .  .  .  . 47 |  .  .  .  .  .  . 40 |  .  . 26  7  .  .  . | 28  .  .  .  .  . 10 |  . 14  .  . 27  .  . |  .  . 29  .  .  .  . |  .  8  .  . 20  4  . "
			"  .  .  .  .  . 18 15 |  .  .  .  .  .  .  . |  .  .  . 36 32  .  . |  .  .  . 31  .  .  . | 45 41  .  .  .  .  . | 34  . 17  .  .  .  . |  .  .  1  . 42  .  . "
			"  .  . 20 25 23  .  . | 33  .  .  .  .  .  . |  .  . 35  .  .  .  . |  .  .  .  .  .  . 42 | 12  . 15  .  .  5  . | 21  .  .  .  .  . 19 |  .  .  . 22  .  .  . "
			"  .  .  .  .  .  .  . |  .  .  8 45  . 24 47 |  .  .  .  .  .  .  . | 39  .  .  .  .  .  . |  .  .  .  .  .  6 13 |  .  . 48  .  .  .  . |  .  .  .  .  .  .  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  .  .  . 35  .  .  . |  .  .  .  . 14  .  . |  . 21  .  .  .  8  . |  .  .  1  .  .  .  . | 41  .  .  .  .  .  . | 11  . 15  .  .  .  . |  .  .  .  . 26  .  . "
			" 17  .  .  .  . 48  . |  .  .  .  .  .  . 29 |  .  .  . 10  .  .  . |  .  .  . 35  .  .  . | 32  4  .  .  .  .  . |  .  .  .  .  .  . 40 | 46  .  .  9  .  3  . "
			" 11  . 16  .  .  7 46 | 39  .  . 22  .  .  . |  .  .  .  . 31  .  . | 33  .  .  .  .  .  . |  .  .  8  . 29  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . "
			"  . 42  .  9  .  . 28 |  .  .  .  .  .  .  . |  . 12  . 17 45 37  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  . 13  .  .  .  .  . "
			"  .  . 43  .  .  .  . |  .  .  .  .  . 16  . |  4  .  .  .  . 15  . |  .  .  .  . 34  .  . |  3  .  .  .  .  .  . |  9  .  . 32  .  .  . |  .  .  .  8  .  .  . "
			"  .  . 10  .  .  . 13 |  .  .  .  .  3  .  . | 49  .  .  .  .  .  . |  .  .  5  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  . 45 |  .  .  .  .  .  .  . "
			"  . 47  .  .  .  8 41 |  .  .  . 40  . 25 10 |  .  .  . 48  .  .  . |  .  .  .  .  .  9  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  5  .  .  4  .  . 22 "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  .  .  7  .  . 29  . |  .  .  . 43  . 19 11 |  .  .  .  8  .  . 14 |  .  .  .  .  .  .  . |  .  .  .  .  . 24  . |  .  .  .  .  .  .  . |  .  .  .  .  .  6 46 "
			"  .  .  . 41  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  3 |  . 13 48  .  .  .  . |  .  .  .  .  . 34  . |  .  .  .  .  .  .  . "
			"  . 19 48 16  .  .  . |  .  6  .  . 12  .  . | 43 20  2 46  .  .  . |  .  .  .  1 44  .  . |  .  3  .  .  8  .  . |  .  .  .  .  .  .  . |  .  .  . 14  . 15  . "
			"  .  .  .  .  .  .  . |  .  .  .  . 10  .  . |  .  .  . 33  .  .  . | 35  .  . 45  .  . 36 |  .  .  .  .  .  .  . | 32  .  .  .  .  .  . |  .  4  .  .  .  .  . "
			"  . 40  .  . 31 23  . |  . 41  .  .  .  .  . |  .  . 12 25 30  .  . |  . 38 32  .  .  . 15 |  .  .  . 22  .  .  . | 33  .  .  .  .  .  . |  .  2  .  .  .  .  . "
			"  .  .  .  .  1  .  . | 42  .  .  .  . 18  . |  .  .  . 29  .  .  . |  .  9 20  .  .  .  . |  . 47  .  . 28  .  . | 45  .  .  .  .  .  . | 40  .  .  7  .  . 32 "
			"  .  .  .  .  .  .  . |  .  .  .  .  .  . 27 |  . 37  .  . 44  .  . | 47 48 30  .  .  .  . | 42 49 35  6 38  .  7 | 17  .  .  .  .  .  . |  .  .  3  5  .  .  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			" 43  .  .  .  .  . 45 |  .  . 33  . 35 11 41 |  . 42 37  .  .  .  . |  .  .  .  .  .  .  . |  . 32  .  . 47 31  . |  .  .  .  .  .  .  . |  .  .  .  .  .  . 34 "
			"  .  .  .  . 12 25 27 | 28  . 23  .  .  .  . |  .  .  .  .  6 14  . |  .  . 36 37  .  .  . |  .  . 33  .  .  .  . |  .  . 18  .  7  .  . |  .  .  .  .  .  . 42 "
			"  . 14 26  . 46  .  . |  .  .  .  .  .  .  . |  .  .  .  . 17  . 41 |  .  .  .  .  .  .  . |  .  . 34 40 49 11  . |  . 20  .  .  .  .  . |  .  .  .  .  .  .  . "
			"  .  .  .  .  .  .  . |  . 34 12  .  .  .  . |  .  .  . 21  .  .  . |  .  .  .  . 19 28  . | 35  . 20  .  .  .  . |  .  .  .  .  . 36  . |  .  .  .  .  .  5 37 "
			"  .  .  .  .  .  3 31 |  .  .  5  .  6  .  . |  . 18  .  .  .  .  . |  .  1 24  .  .  7  . | 15  . 10  .  .  . 30 |  2  .  .  .  .  .  . |  . 29 13 41  .  .  . "
			"  .  .  .  .  .  .  . |  .  .  .  .  .  .  . | 33  2  .  .  . 22  . |  .  . 21  8  .  .  . |  .  .  .  .  .  . 28 |  .  .  .  .  .  .  . |  .  .  .  .  .  . 11 "
			"  .  . 28  .  .  .  . |  .  . 47  .  . 26 30 |  . 36  .  .  .  . 13 |  .  .  .  .  .  .  . |  .  .  .  .  2  .  . |  5 31 38  .  .  . 17 |  .  .  .  .  .  .  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  .  .  .  . 35  9  . |  .  .  .  .  .  .  . |  .  .  .  . 26 33 16 |  .  .  4  .  . 10  . |  .  .  . 43 12 13  . | 29  .  . 23 18  .  . |  .  .  .  .  .  . 39 "
			" 27 26  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  . 23  . |  6  .  .  .  .  .  . | 19  .  .  .  .  .  . | 49 48  .  .  .  .  . |  .  . 25 17  .  .  . "
			"  .  .  . 13 36 30  . |  .  3  .  .  .  .  . |  .  7 48  2  8 20  . |  .  .  .  .  1  .  . |  .  .  . 44  .  4  . |  .  .  .  .  .  .  . |  .  .  .  .  . 45 40 "
			"  .  . 37 48  .  . 24 |  .  . 39  . 23  .  . |  .  .  .  .  .  .  . |  . 16 38 30  7  .  . | 46  .  . 41  .  .  . |  . 32  .  .  .  .  . |  .  .  .  .  .  .  . "
			"  .  . 42 11 29  .  . |  4 45 10 13 22  .  . |  .  .  .  .  .  .  . |  . 24  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  . 26  9  . 27 |  .  .  .  .  . 38 35 "
			"  .  . 49  . 38 12  . |  .  .  . 24  9  . 46 |  .  .  .  .  . 19  . |  .  . 23  . 18  .  . |  .  .  .  .  .  .  . |  .  .  . 40  .  .  . |  . 28  .  3  .  .  . "
			"  .  .  .  .  .  .  . |  . 18  .  .  .  . 16 | 30 17  .  . 38  .  9 |  .  .  .  .  .  . 29 |  .  .  .  .  .  .  . |  . 11 19  .  .  .  . |  .  .  .  .  .  .  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  .  .  . 30 28  .  . |  . 43  .  .  .  .  . |  .  .  .  .  .  .  . | 44  .  .  .  .  .  . |  .  .  .  .  . 25  . | 37  .  .  .  .  . 13 |  .  . 31 34  . 39 33 "
			"  .  .  .  .  . 35  . |  .  .  .  .  .  .  . | 21 14  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  . 23  . 49 45 "
			"  .  .  .  .  .  .  . |  .  .  .  .  .  6  9 | 22  . 33  .  .  . 37 |  .  .  .  .  . 34  . |  .  .  .  .  .  .  . |  .  . 26  .  .  .  . |  .  .  .  .  .  .  . "
			"  .  . 39  .  .  .  . |  .  .  .  .  .  . 36 | 41  .  .  .  .  .  . |  .  .  .  .  .  . 31 |  .  .  . 33 26  . 48 | 20  .  .  .  .  3 43 |  .  .  .  .  .  .  . "
			"  .  .  5  .  .  .  . |  .  .  . 12 33  .  . | 10  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  . 17  .  .  . |  .  .  .  .  .  . 34 |  .  .  .  . 37  . 36 "
			"  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  7  .  . 43 27  . |  .  .  .  4  .  .  . |  .  . 11  .  .  .  . |  8 10  . 32  .  .  . "
			"  .  . 47  .  .  6  . |  . 14  .  .  .  . 48 | 42  .  . 34 20 40  . |  . 19  .  . 23  .  . |  .  . 24 28  .  .  . | 36  .  .  .  .  .  . |  .  .  .  .  .  .  . "
			"----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------"
			"  .  .  .  . 40 33 26 |  .  .  .  .  2  .  . |  .  . 43  .  .  . 21 |  9 10  .  .  . 18  . |  .  .  .  .  .  .  . |  . 25 20  .  .  .  . |  .  .  .  .  .  .  . "
			"  .  .  .  . 14 45  8 |  .  .  .  .  .  .  . |  .  .  .  .  . 18  . |  .  .  . 48  .  .  . |  .  .  .  . 34 46  . |  .  . 37  .  .  .  . |  .  . 38 39 35  .  . "
			"  5 17 18  .  .  .  . |  7  . 44  . 26  .  . |  .  .  .  .  .  .  . |  .  .  .  .  . 43 35 | 37  .  .  .  . 10  . | 48  .  .  . 36  .  . | 25  .  .  .  . 14  . "
			"  .  .  .  . 16  .  . |  .  .  .  .  .  .  . |  . 39 41  .  .  . 49 |  .  .  . 42  .  .  . |  .  .  .  3 22  .  . |  .  .  .  . 33  .  . |  .  .  7  6  .  .  . "
			"  .  .  .  .  .  .  . |  .  . 41  . 34  .  . |  .  .  .  .  . 44  . |  .  .  . 29 38  .  . |  . 26  .  . 21  .  . |  .  .  .  .  .  .  . |  .  .  .  .  . 37  . "
			"  .  .  .  .  .  .  . |  .  .  . 47  4 21  . |  .  .  .  .  .  . 17 |  .  .  .  .  .  .  . |  .  .  .  .  .  .  . |  .  .  . 46  .  .  . |  .  .  .  .  .  .  . "
			"  .  .  .  .  . 27  . |  .  .  .  .  .  . 49 |  . 33  .  .  .  .  . | 45  .  .  .  .  .  . |  .  .  .  .  .  .  . | 10  .  .  8  .  . 29 |  .  .  .  . 28  . 12 "
		});

		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);

		const int solveUsingTheseAlgos =
			0
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_2>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_3>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_5>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_6>()
			| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_7>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestDancingLinksSudokuMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<FsssSolver>()
			////| PuzzlesSudokuTester::getAlgoFlag<JSolveMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<bb_sudoku_Matrix>()
			| PuzzlesSudokuTester::getAlgoFlag<Microsoft_z3_solver_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<Google_OR_tools_1>()
			;

		unsigned int numSolutions = 1;
		unsigned int dimension = 49;

		//const string libraryFilePath("../../../test/reports/Sudoku/universalMultiDimensionalSudokuPuzzlesLibrary.csv");
		//PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, solveLibraryPuzzlesAgain, numSolutions, algoFlag);

		for (unsigned int i = 0; i < examples.size(); ++i)
		{
			int iterations = 0;
			long long timeRequiredToGeneratePuzzle = 0;
			SudokuPuzzleUtils::getLogger() << "\n\nPuzzle No. " << i + 1;
			PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(examples[i], dimension), numSolutions, solveUsingTheseAlgos, false);
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = iterations;
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = timeRequiredToGeneratePuzzle;
		}

		//SudokuPuzzleUniversalDatabase::writeToLibrary();

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_9()
	{
		vector<string> examples({
			". . . . . 55 . . . 53 . . . . 10 81 . . 85 . . . . . 19 . . 72 16 41 . . . 38 6 39 . 13 . 90 . 4 63 18 30 79 35 26 82 76 89 65 95 73 23 29 34 42 67 . 58 . 74 . 68 57 8 . . . 91 11 75 . . 80 . . . . . 97 . . 33 69 . . . . 28 . . . 21 . . . . ."
			". 76 . . . . . 21 . 4 . 65 . . . . 0 . 84 61 . . 91 3 . 46 11 23 33 2 57 . . . 37 31 . . 14 83 67 . . 72 . 44 . 19 8 78 47 85 92 . 49 . 22 . . 50 88 15 . . 70 62 . . . 16 34 87 68 9 77 . 39 1 . . 5 29 . 36 . . . . 38 . 54 . 81 . . . . . 82 ."
			". . 0 . . . . . 48 . . . . . 44 59 . 45 . 79 . 20 . . 32 62 30 25 55 49 . 9 47 . . . 22 56 72 94 11 97 6 87 27 69 . 92 38 40 28 91 21 . 39 54 24 82 19 41 17 52 73 89 . . . 76 13 . 57 60 46 26 86 43 . . 66 . 83 . 95 . 58 81 . . . . . 23 . . . . . 68 . ."
			". . . . 33 . 69 . 83 70 . . . 64 . 40 67 68 35 42 58 79 . . 14 48 59 57 39 7 15 . 17 . . 93 85 44 99 24 . . 25 62 . . 65 71 2 28 80 10 55 3 . . 78 43 . . 11 26 38 31 96 . . 53 . 66 98 8 50 18 51 73 . . 97 21 89 34 75 45 84 . 12 . . . 30 46 . 32 . 13 . . . ."
			". . . . 22 . . . 8 71 96 . . . 56 . . 29 16 18 . . 1 89 . . 68 73 63 77 74 . 62 26 . 98 4 40 3 78 90 . 49 24 . 36 0 . 53 41 5 83 . 58 25 . 30 17 . 33 91 72 75 21 50 . 46 61 . 65 44 52 81 76 . . 6 88 . . 79 32 37 . . 64 . . . 99 43 86 . . . 57 . . . ."
			". 54 . . 51 . 32 23 43 15 71 . . 58 38 6 31 13 . . 94 84 . . . 42 . 56 . . . 52 73 67 . 77 20 49 . 87 . 5 96 66 99 . 74 . . 22 8 . . 98 . 26 70 61 57 . 39 . 83 69 90 . 3 95 19 . . . 78 . 12 . . . 85 36 . . 21 55 68 48 28 . . 44 62 53 27 14 . 1 . . 79 ."
			". 58 . . 44 . 84 65 . . . 14 15 . 48 . . . . 76 6 . . . . 99 . 52 . 60 . 75 . 96 42 . 50 41 19 34 73 . 94 . . . . 46 37 . . 36 18 . . . . 71 . 63 9 87 82 97 . 64 1 . 30 . 4 . 3 . 5 . . . . 40 49 . . . . 35 . 88 2 . . . 7 8 . 56 . . 47 ."
			". 27 . . . 61 99 . 95 91 . 74 . 57 92 8 89 19 . 24 . 78 70 40 85 44 . 80 29 . . . 28 . 69 46 60 66 68 36 32 17 . . 83 15 51 64 . 93 7 . 72 76 62 79 . . 2 81 14 10 63 41 0 6 . 48 . . . 45 53 . 96 71 37 84 22 . 13 . 42 18 90 30 23 . 20 . 94 77 . 38 4 . . . 9 ."
			"89 41 . 73 . 18 7 . . 36 . 98 55 . 87 . 11 66 . 91 . . 27 8 96 13 95 88 31 . 81 58 . 2 64 71 63 1 33 . 21 50 10 . 12 68 48 3 . 39 37 . 97 40 45 75 . 90 14 46 . 47 49 56 94 84 20 . 44 85 . 79 92 62 70 65 23 32 . . 53 . 19 93 . 67 . 77 86 . 25 . . 99 24 . 76 . 22 60"
			". 67 . . 28 . 97 62 66 85 . 2 49 . 47 . 78 37 32 93 . . 0 22 21 . 50 . . 15 10 79 . . 91 16 48 80 76 27 . 59 70 86 14 60 95 88 33 43 99 84 53 4 77 68 1 56 11 . 5 54 71 34 92 18 . . 23 42 94 . . 58 . 20 29 38 . . 52 41 74 26 . 65 . 17 82 . 6 55 31 90 . 45 . . 72 ."
			". . . . . . 68 . 65 43 . . . . 6 73 92 10 46 . . . . . 25 . 99 . 36 31 50 . . . . 47 8 . . 17 41 . . 16 66 95 18 42 24 32 22 70 5 0 13 2 87 . . 30 71 . . 58 57 . . . . 59 84 83 . 37 . 39 . . . . . 1 81 4 53 55 . . . . 89 62 . 54 . . . . . ."
			". . . . 16 94 . 86 31 27 . . . 41 . . 71 . 90 72 . . . 4 . 79 . 98 30 8 . 69 15 . 89 32 21 82 40 26 . 49 78 33 6 35 . 87 . 81 36 . 34 . 11 85 20 80 52 . 38 60 50 22 53 68 . 70 7 . 28 19 25 . 92 . 74 . . . 57 37 . 2 . . 97 . . . 10 59 44 . 65 55 . . . ."
			". 75 . . . 77 90 84 82 . . 29 . 52 95 49 . 7 . 32 . . 80 87 . 67 . . 71 18 . . . 34 . . 73 . 81 92 . 48 53 28 5 . 59 31 40 25 57 44 46 62 . 39 96 47 76 . 69 45 . 14 . . 21 . . . 63 54 . . 35 . 0 64 . . 24 . 9 . 98 68 15 . 13 . . 85 38 86 60 . . . 43 ."
			". 71 . . 46 . . 41 . 28 . 45 14 . 4 20 53 75 . . 12 32 83 . 88 26 93 6 13 . 90 19 3 . 49 22 52 79 37 2 . 9 . . . 96 . 73 39 11 84 81 91 . 99 . . . 59 . 61 40 35 18 87 0 . 55 85 54 . 76 17 34 72 51 . 98 38 42 . . 25 50 7 82 . 10 21 . 77 . 47 . . 23 . . 92 ."
			"57 . . . 53 24 78 48 88 99 . 47 . . 50 16 . 36 79 . . . . 44 89 35 86 19 . 10 . . . 83 68 7 28 76 91 4 15 13 . . . 64 14 74 22 67 55 73 51 49 37 . . . 72 23 84 95 46 25 2 96 66 . . . 32 . 21 61 94 29 65 . . . . 17 27 . 5 62 . . 52 . 70 6 42 71 26 82 . . . 8"
			". . . . 52 . . 20 4 18 . . 96 . 34 . 94 65 . . . . 37 28 69 39 38 81 51 97 29 31 46 0 84 57 72 74 41 14 . 45 . 83 . . 90 91 19 62 93 58 75 15 . . 98 . 33 . 92 76 77 67 42 5 24 44 32 43 26 82 40 87 3 59 47 60 . . . . 61 35 . 66 . 64 . . 88 78 48 . . 95 . . . ."
			"10 . . . . 63 83 . 59 . . 35 . 0 . 88 . . 93 . 34 . 11 58 52 43 66 64 . 61 53 . 65 23 . 60 86 . 5 67 97 . 7 71 72 27 21 8 46 56 94 3 89 95 6 1 19 25 . 14 47 78 . 82 15 . 26 30 . 12 90 . 33 75 99 62 55 16 . 22 . 70 . . 45 . 42 . 77 . . 80 . 13 50 . . . . 81"
			". 0 . . 21 . 33 . 37 61 . . . . 22 23 26 80 . 87 5 . 57 14 . . 47 . 50 17 85 99 . 10 24 25 13 59 42 63 . . 79 55 60 . 54 36 88 12 78 90 48 53 . 92 38 35 . . 62 28 3 65 93 73 75 . 83 4 70 89 . 97 . . 7 18 . 1 43 . 84 94 72 49 . . . . 52 45 . 76 . 34 . . 46 ."
			". 8 . 81 . 66 . 32 . . . 78 . . 82 97 . 55 . 40 1 0 . . 72 . . 53 48 90 38 71 . 27 43 . 62 58 64 95 68 92 37 76 . . 86 84 94 44 56 41 28 21 . . 7 88 69 26 6 51 23 39 . 20 74 . 63 33 52 13 79 . . 96 . . 73 45 54 . 67 . 60 46 . . 83 . . . 99 . 14 . 61 . 75 ."
			". . . . . 3 14 . 47 25 81 . . 98 . 28 27 59 43 31 . 91 84 . . . 46 94 68 76 80 . 77 55 35 36 . 39 78 . 63 . 85 . 38 26 61 . . 75 12 . . 29 82 42 . 83 . 17 . 64 34 . 79 13 48 19 . 11 8 5 2 6 . . . 93 53 . 20 44 65 58 92 . 87 . . 56 49 96 . 30 90 . . . . ."
			". 95 . . . . . 38 68 . . . . 85 . 2 14 35 53 92 . . . 59 42 70 . . 1 . . . . 66 . 75 74 37 80 15 . 99 9 6 26 94 73 69 93 24 98 17 4 97 50 10 89 46 23 . 41 11 28 33 61 . 51 . . . . 88 . . 48 54 72 . . . 12 84 78 8 83 . 67 . . . . 3 55 . . . . . 65 ."
			". . . . . 39 42 29 36 81 . 90 . 19 . 22 37 56 40 11 . . 77 52 79 80 44 58 . 12 . 76 . . 88 45 17 5 97 38 48 83 . 34 . 41 13 20 . 2 62 . 24 18 33 . 72 . 60 67 7 59 47 1 69 8 . . 43 . 99 . 6 53 16 50 96 55 . . 92 10 82 51 75 . 32 . 4 . 85 9 46 49 28 . . . . ."
			". 3 . . 10 99 4 51 . 96 . . . 88 32 . 43 46 72 . 63 68 . 23 34 75 89 65 . . 44 . 81 71 98 . 82 70 92 28 86 14 . 74 25 90 85 . 11 97 59 30 . 31 35 45 37 . 79 5 80 50 0 16 . 36 73 38 . 57 . . 26 60 1 12 19 . 64 49 . 9 58 21 . 94 55 . . . 78 . 67 91 95 41 . . 61 ."
			". 97 . 27 . 83 . . 85 66 . . . 3 . 51 . 99 38 16 . . 4 91 98 . 26 96 43 45 93 22 . 6 52 8 31 . 50 54 . . 75 53 . 12 . . . 19 71 . . . 70 . 40 84 . . 25 90 . 68 46 42 72 . 21 9 37 67 74 79 . 95 33 94 . . 77 89 88 . 59 . 65 . . . 63 39 . . 82 . 24 . 58 ."
			". 37 . 61 . . . 44 60 . . . . 97 31 34 48 . 80 71 3 54 . . . 38 78 8 74 6 95 36 41 90 77 . 91 21 58 99 7 . 1 92 52 23 10 15 47 57 69 75 43 88 68 76 65 94 . 42 89 81 39 29 . 26 4 56 66 98 27 25 0 83 82 . . . 2 9 96 16 . 46 13 20 45 . . . . 17 40 . . . 14 . 33 ."
			". . . . . 15 50 59 54 72 . 82 20 45 42 95 61 33 . 25 . . . 0 62 97 . . 73 39 26 . 4 . 51 9 . 96 49 68 78 30 . . . . . 35 87 . . 56 32 . . . . . 77 28 94 48 14 . 99 40 . 63 . 6 66 44 . . 38 24 69 . . . 90 . 47 86 18 3 29 2 91 . 21 81 12 5 71 . . . . ."
			". . 32 . 31 92 12 69 2 . . 9 77 49 28 . . . 68 98 15 46 . 64 50 . 22 47 82 25 . 40 . . 86 79 33 7 29 62 . . 39 0 96 89 72 65 70 42 81 27 44 99 14 13 41 91 . . 78 20 30 88 45 71 . . 35 . 87 84 51 56 . 36 8 . 21 90 97 23 . . . 85 76 24 54 . . 38 11 16 83 53 . 37 . ."
			". . . . . . 98 52 84 77 . 54 . 81 . 18 13 86 17 21 . . 94 . 51 14 88 36 83 27 . 32 56 1 . . 89 11 35 12 55 62 . . 49 . 38 16 71 60 9 26 73 57 . 82 . . 0 92 53 44 67 79 . . 64 15 10 . 3 23 34 68 65 47 . 42 . . 37 72 7 41 63 . 95 . 39 . 66 22 45 20 . . . . . ."
			". 9 46 . 94 11 45 76 24 56 . 73 . . 79 . 96 . 62 70 . 48 . 86 90 10 29 5 . 40 83 . 39 19 20 55 . 72 30 23 4 . 22 88 36 3 80 . . 37 6 . . 52 66 34 21 51 . 78 12 17 58 . 95 54 65 77 . 13 35 . 89 41 91 31 98 . 15 . 81 26 . 99 . 53 . . 57 . 92 25 74 59 47 64 . 97 32 ."
			". 88 . . 62 25 . 43 74 41 . 91 44 . . . 4 26 66 . . . . 72 18 . 81 61 . 9 78 53 85 . 14 . 3 73 24 . . 21 98 8 46 . 58 40 68 77 90 38 20 39 . 16 95 2 22 . . 83 37 96 . 31 . 60 27 93 13 . 52 10 . 45 30 . . . . 50 1 69 . . . 15 6 . 87 34 86 . 54 7 . . 36 ."
			". 18 . . . 72 9 . 90 11 . . . 37 66 10 45 6 . 97 . . . 62 68 88 3 . 15 84 48 . . 5 32 86 . . 69 . . . . 1 16 . 92 81 35 94 33 4 7 34 . 31 73 . . . . 27 . . 56 50 54 . . 79 64 49 . 57 28 67 60 . . . 98 . 38 91 25 71 74 . . . 26 29 . 55 99 . . . 21 ."
			"25 . . . 39 14 37 . . 44 46 . . 70 17 38 77 . 47 22 72 . 74 . . . 28 31 78 87 23 63 68 7 . . 54 89 52 18 19 10 . 97 29 21 . 51 12 84 20 48 93 . 61 43 85 . 98 24 57 92 96 80 . . 83 9 82 26 53 62 99 66 . . . 90 . 11 65 8 . 64 36 56 41 . . 45 5 . . 94 30 58 . . . 67"
			". . . . . . 51 47 86 . . . 34 43 72 . . 48 5 64 . 71 97 . 81 9 . 59 20 . . 21 2 99 . . 36 3 13 35 62 . 61 . 75 58 98 4 6 55 88 45 63 68 38 87 . 27 . 10 70 85 53 77 . . 42 65 33 . . 0 8 . 50 1 . 91 29 . 31 14 83 . . 7 94 26 . . . 93 57 25 . . . . . ."
			". 32 . 29 . 35 3 . 79 33 . . . 14 61 44 68 73 88 52 . . 47 42 17 . 43 1 27 0 . . . . 31 20 80 46 11 16 24 . . 63 70 5 87 30 25 50 72 62 36 74 8 91 67 . . 9 86 38 60 12 51 28 . . . . 22 37 19 54 . 77 85 95 . . 40 76 15 84 4 93 58 . . . 56 41 . 92 49 . 69 . 53 ."
			". 99 . 19 . . 55 57 . 52 . 95 . . . 7 54 12 8 39 33 11 . 98 . 86 69 4 26 94 . 14 49 . 41 70 37 77 17 . . 28 38 42 89 56 . 27 . 71 53 . 80 . 2 23 64 50 5 . . 6 78 62 22 43 . 59 20 . 18 34 97 24 45 . 13 . 92 25 10 75 72 68 32 . . . 51 . 84 . 3 96 . . 88 . 83 ."
			". 15 . 24 . 98 . 5 13 95 . 33 57 . 29 . . 94 60 . 51 . 8 . 54 53 55 . 38 . 56 30 . . 47 . 84 19 43 . . 96 76 . . 67 . 77 91 66 41 16 12 . 1 . . 26 78 . . 3 90 63 . 7 . . 25 61 . 39 . 20 93 23 . 48 . 88 . 73 59 . . 42 . 82 69 . 65 35 34 . 75 . 28 . 31 ."
			". 66 . . 77 . 54 42 45 94 80 . . 90 91 15 55 2 71 . . 18 . 67 46 61 . 99 . 19 76 74 38 93 60 34 92 25 83 85 . . 44 47 53 43 82 . 64 26 14 21 . 35 52 40 81 59 . . 30 37 72 24 84 23 31 4 17 5 58 . 12 . 27 33 68 . 69 . . 88 16 3 11 78 96 . . 86 50 98 73 6 . 36 . . 63 ."
			". . 16 . 91 . . 27 . 84 26 . 98 99 35 . 50 0 . . . 90 92 12 . 89 63 45 25 13 58 . 1 51 87 62 88 75 4 . . 32 18 39 40 85 33 57 73 46 15 71 76 86 69 3 60 44 66 . . 2 41 94 48 47 97 11 . 68 74 96 5 78 7 . 83 70 6 . . . 77 22 . 52 61 23 . 34 72 . 95 . . 19 . 82 . ."
			". . . . 83 30 70 2 1 . . 18 67 . 24 93 69 96 . 82 91 22 39 . 41 . . 37 79 16 . . 57 98 . 12 . 10 59 . 17 . 68 . 3 54 45 80 36 99 65 97 84 6 94 51 . 11 . 13 . 49 32 . 64 . 81 88 . . 72 46 15 . . 42 . 35 56 31 48 . 0 19 20 27 . 47 43 . . 8 33 85 44 89 . . . ."
			". 92 73 . 61 81 . 6 12 . . 13 . 76 20 84 74 . 9 28 32 93 36 . 7 . 14 . 5 85 33 67 . . . . 15 29 . 55 . 31 48 11 88 52 69 . . 34 83 . . 37 58 30 49 75 42 . 19 . 8 71 . . . . 1 21 82 16 . 43 . 40 . 63 3 87 60 2 . 62 54 24 70 . 35 . . 64 22 . 91 78 . 51 77 ."
			". . . . 89 19 13 4 27 . . . 1 62 . . 56 50 41 67 . 42 . 17 99 8 58 29 . 33 . . . 25 36 2 75 . 26 22 . 23 . 46 98 82 6 28 65 30 73 7 90 91 78 95 77 . 49 . 44 32 . 61 71 97 60 . . . 10 . 45 31 39 5 18 . 43 . 14 74 40 76 . . 84 81 . . . 70 96 47 15 9 . . . ."
			". . 42 . . 9 62 70 94 . . . . 68 . 66 35 8 63 49 . 97 93 45 48 50 65 7 . . 3 . 33 40 55 18 87 30 23 51 84 76 . 38 34 81 5 41 . 88 64 . 29 83 22 21 53 . 80 27 28 99 20 57 26 58 77 98 . 95 . . 82 44 74 86 67 96 61 . 72 15 2 73 0 . 37 . . . . 36 17 60 19 . . 39 . ."
			". 24 . 8 . 73 . 78 18 . . 55 97 71 . 36 . 38 . 51 . . 87 21 3 85 75 16 84 32 62 . . 35 1 44 14 86 . 60 . 42 11 25 93 7 63 . 74 31 70 94 . 19 47 0 23 28 43 . 27 . 76 91 39 72 45 . . 29 81 4 20 40 15 88 79 56 . . 33 . 66 . 65 . 53 67 68 . . 5 41 . 10 . 49 . 48 ."
			"44 . 10 . 50 28 96 56 91 . . 32 . 94 . 98 . 34 81 88 31 . 60 . 95 . 57 9 11 83 . . 89 58 16 74 47 93 15 43 . 85 . 19 87 . 53 24 51 . . 82 26 75 . 4 54 . 13 . 33 55 92 23 78 21 2 49 . . 6 68 73 59 . 0 . 69 . 52 25 77 41 . 42 . 30 . 36 . . 72 8 65 84 80 . 61 . 29"
			". . . . 74 31 . 40 64 21 . . . 2 86 58 19 14 52 78 . 70 22 . 44 . 72 27 53 . 99 . 5 77 . 82 49 81 54 57 . 26 . 73 . 10 68 . 15 . . 6 . 1 41 . 79 . 55 . 50 66 56 42 9 . 13 24 . 89 . 95 7 17 . 11 . 23 98 . 32 87 97 75 85 51 3 . . . 91 76 4 . 16 63 . . . ."
			". 85 . . 43 . 67 . . 60 . 20 . . 33 91 17 . 39 . 76 . 2 80 . 92 79 . 19 1 . 0 . 12 65 . 59 71 48 45 47 . 21 78 18 40 3 72 44 . . 89 15 36 56 11 63 5 . 51 87 53 70 93 . 25 68 . 31 . 97 90 . 49 29 . 57 54 . 32 . 22 . 34 6 4 . . 7 . 24 . . 98 . 66 . . 28 ."
			". 79 . 0 . 33 87 36 . 37 . 10 82 30 80 75 . . 73 85 . . . 77 . 55 . 62 88 . 19 91 95 . . 38 97 31 . 64 35 . 67 2 92 45 4 86 43 70 18 74 42 48 71 61 66 57 . 72 15 . 94 46 47 . . 34 6 52 . 41 13 . 8 . 84 . . . 16 12 . . 29 28 24 27 98 . 83 . 68 1 40 . 32 . 11 ."
			". 81 . 2 82 . . 90 15 . 22 23 . . 26 72 65 95 70 . 56 34 . 41 . 40 98 12 46 . . . . . 8 6 68 . 28 . 58 . . . 80 . 20 . 1 49 24 86 . 16 . 44 . . . 32 . 75 . 73 36 17 . . . . . 50 37 19 64 . 94 . 93 66 . 57 10 11 89 83 . . 47 39 . 88 69 . . 18 21 . 55 ."
			". . . . 86 95 53 97 . 80 92 . . 7 . . 79 5 87 15 . 43 61 . 71 . 36 89 . 64 . 83 50 24 34 . 56 84 73 88 . . . 32 91 48 . 17 57 96 25 99 98 . 81 38 52 . . . 22 8 62 4 . 12 90 67 40 . 9 . 28 30 . 85 . 14 75 . 63 13 18 1 . . 93 . . 60 51 . 82 26 27 20 . . . ."
			". 48 54 . 1 22 5 75 20 . 31 . 27 93 59 29 42 57 28 83 . 26 13 90 10 96 73 . . 24 70 37 78 76 67 66 41 32 85 98 33 52 12 . . 9 . 97 95 36 35 34 30 . 3 . . 68 40 84 0 43 51 19 88 82 18 86 11 7 16 . . 2 47 55 53 72 91 . 17 99 49 80 21 45 56 94 . 8 . 87 79 46 92 25 . 74 38 ."
			". 17 34 . 24 78 8 73 99 . 55 . 61 69 71 46 36 51 97 38 . 16 3 84 87 52 32 . . 75 1 80 13 14 62 96 35 60 44 86 93 74 50 . . 25 . 11 42 63 0 43 47 . 9 . . 23 94 19 45 91 29 30 59 83 27 72 39 76 88 . . 15 40 81 58 20 4 . 95 90 64 33 67 92 68 65 . 66 . 10 37 22 5 49 . 89 57 ."
			". . . . 48 75 65 22 . 54 77 . . 59 . . 29 83 27 57 . 15 10 . 13 . 24 26 . 73 . 78 37 41 70 . 76 98 66 32 . . . 9 33 61 . 12 52 89 58 40 3 . 30 35 68 . . . 18 19 7 43 . 86 0 82 88 . 55 . 72 47 . 16 . 71 63 . 80 45 8 17 . . 50 . . 49 79 . 14 74 87 44 . . . ."
			". 86 . 53 29 . . 95 72 . 44 92 . . 7 79 47 15 11 . 0 89 . 66 . 49 64 43 6 . . . . . 46 73 24 . 21 . 91 . . . 39 . 96 . 55 16 38 37 . 45 . 25 . . . 65 . 4 . 8 40 67 . . . . . 75 14 33 30 . 70 . 42 77 . 58 60 63 69 13 . . 93 18 . 27 51 . . 31 20 . 94 ."
			". 89 . 13 . 4 63 19 . 57 . 48 41 21 62 56 . . 1 50 . . . 37 . 68 . 42 17 . 36 11 92 . . 26 25 22 . 69 98 . 28 82 94 46 30 66 23 6 95 49 78 85 90 73 93 77 . 91 60 . 64 32 3 . . 97 71 80 . 43 87 . 31 . 51 . . . 76 38 . . 81 74 88 55 84 . 96 . 53 52 70 . 9 . 35 ."
			". 74 . . 84 . 88 . . 38 . 43 . . 2 19 58 . 18 . 28 . 44 53 . 47 67 . 94 72 . 5 . 49 99 . 77 57 82 81 69 . 0 10 71 73 29 83 26 . . 55 41 8 59 96 12 79 . 1 13 42 89 66 . 24 50 . 9 . 11 98 . 36 17 . 80 7 . 34 . 51 . 32 61 87 . . 3 . 4 . . 62 . 90 . . 45 ."
			". . . . 79 36 . 33 69 76 . . . 80 30 60 75 85 82 89 . 62 63 . 59 . 51 14 77 . 17 . 91 97 . 53 9 64 38 31 . 43 . 45 . 2 70 . 90 . . 39 . 72 42 . 57 . 74 . 96 46 52 5 6 . 15 81 . 94 . 65 22 8 . 78 . 13 41 . 44 28 26 16 27 12 98 . . . 68 40 83 . 7 32 . . . ."
			"91 . 45 . 7 56 66 28 44 . . 3 . 40 . 90 . 88 76 34 18 . 95 . 60 . 83 39 35 57 . . 27 47 63 15 58 43 74 93 . 51 . 22 37 . 79 62 85 . . 13 17 20 . 97 14 . 82 . 2 23 1 55 38 49 33 21 . . 0 48 69 46 . 6 . 73 . 99 70 86 5 . 71 . 36 . 30 . . 84 64 61 72 67 . 65 . 12"
			". 11 . 62 . 70 . 9 32 . . 4 63 16 . 35 . 49 . 8 . . 48 69 93 25 91 97 45 65 55 . . 87 3 23 40 51 . 30 . 54 41 81 84 38 88 . 76 5 21 80 . 27 29 64 10 53 92 . 77 . 95 99 14 98 28 . . 20 86 61 96 74 44 89 24 82 . . 73 . 43 . 79 . 46 0 37 . . 19 75 . 36 . 85 . 71 ."
			". . 51 . . 90 2 71 61 . . . . 26 . 65 72 53 45 95 . 12 5 46 78 56 74 34 . . 8 . 29 68 4 28 79 52 6 42 80 1 . 13 58 99 49 60 . 20 44 . 67 32 31 24 62 . 86 16 35 73 48 75 54 85 63 17 . 84 . . 76 64 19 92 66 37 50 . 11 83 39 9 91 . 47 . . . . 0 97 43 88 . . 77 . ."
			". . . . 85 46 58 26 77 . . . 39 33 . . 91 9 64 74 . 38 . 19 2 76 1 86 . 79 . . . 59 61 48 12 . 10 71 . 44 . 40 47 78 8 21 75 3 11 88 56 51 15 50 5 . 89 . 68 93 . 53 31 16 87 . . . 83 . 54 29 49 97 32 . 90 . 34 4 23 82 . . 7 6 . . . 73 24 81 42 99 . . . ."
			". 60 40 . 23 47 . 74 28 . . 19 . 72 43 41 32 . 34 48 9 59 81 . 97 . 52 . 95 96 22 2 . . . . 99 35 . 3 . 6 4 58 62 49 55 . . 98 87 . . 10 63 88 27 92 45 . 42 . 44 85 . . . . 16 53 1 29 . 50 . 79 . 8 0 17 67 7 . 31 26 14 18 . 94 . . 11 76 . 93 46 . 25 37 ."
			". . . . 15 5 24 98 71 . . 58 60 . 11 62 85 27 . 94 53 40 54 . 8 . . 10 49 55 . . 30 84 . 43 . 50 0 . 79 . 77 . 74 37 66 76 96 83 17 78 1 70 12 41 . 99 . 89 . 63 61 . 25 . 52 7 . . 23 32 48 . . 21 . 9 39 2 6 . 92 87 82 73 . 44 81 . . 75 65 4 35 28 . . . ."
			". . 11 . 18 . . 72 . 17 87 . 83 66 37 . 10 97 . . . 44 68 15 . 24 84 2 62 3 32 . 96 42 48 69 5 65 86 . . 35 81 59 22 1 94 93 41 92 31 46 19 77 7 33 0 73 4 . . 13 79 27 58 39 95 50 . 40 67 51 80 28 57 . 76 36 49 . . . 30 98 . 63 89 25 . 38 23 . 26 . . 52 . 55 . ."
			". 77 . . 66 . 56 0 62 8 49 . . 91 90 55 15 1 51 . . 99 . 70 75 57 . 18 . 48 60 38 74 92 76 83 93 85 34 25 . . 95 43 13 47 26 . 20 82 40 28 . 22 79 14 59 81 . . 31 24 5 37 17 4 30 23 84 72 33 . 65 . 89 58 41 . 10 . . 78 86 29 97 88 9 . . 16 73 32 50 7 . 39 . . 87 ."
			". 65 . 3 . 96 . 35 78 48 . 75 88 . 14 . . 52 23 . 64 . 17 . 47 83 0 . 42 . 31 6 . . 82 . 81 16 20 . . 25 30 . . 63 . 90 7 87 91 18 8 . 36 . . 67 62 . . 12 45 38 . 66 . . 51 60 . 94 . 71 54 22 . 19 . 85 . 93 55 . . 76 . 4 58 . 59 49 56 . 41 . 10 . 13 ."
			". 83 . 70 . . 21 30 . 58 . 40 . . . 69 93 82 67 96 77 37 . 79 . 91 16 22 34 23 . 57 26 . 28 59 98 53 12 . . 36 80 54 17 14 . 68 . 45 51 . 94 . 84 65 11 29 97 . . 74 87 49 73 88 . 75 64 . 42 56 35 52 55 . 31 . 46 38 19 27 50 48 47 . . . 66 . 33 . 61 9 . . 89 . 62 ."
			". 91 . 10 . 27 93 . 80 16 . . . 35 99 50 30 81 98 0 . . 29 25 92 . 13 90 12 63 . . . . 58 4 51 9 62 75 40 . . 85 8 39 46 18 32 33 3 66 69 56 76 15 44 . . 86 97 94 68 2 67 11 . . . . 14 6 70 7 . 74 59 5 . . 22 52 34 28 23 21 17 . . . 95 79 . 82 43 . 19 . 54 ."
			". . . . . . 19 67 87 . . . 8 65 63 . . 39 21 12 . 4 35 . 58 33 . 11 98 . . 49 14 37 . . 44 91 70 77 89 . 27 . 0 42 71 38 28 9 23 5 2 90 80 53 . 64 . 82 93 62 15 6 . . 29 43 22 . . 92 30 . 24 18 . 97 34 . 68 79 46 . . 75 51 32 . . . 48 84 66 . . . . . ."
			"50 . . . 69 34 59 . . 88 79 . . 17 70 77 38 . 42 3 73 . 6 . . . 87 60 76 28 40 68 63 54 . . 7 18 95 89 29 12 . 21 19 97 . 2 10 15 43 98 61 . 93 20 32 . 48 55 83 80 26 92 . . 57 35 91 96 75 86 90 81 . . . 99 . 4 64 56 . 65 49 8 33 . . 13 71 . . 0 1 27 . . . 16"
			". 61 . . . 6 41 . 97 73 . . . 20 76 74 84 28 . 4 . . . 5 36 32 85 . 56 14 79 . . 15 33 27 . . 94 . . . . 52 65 . 34 48 31 69 30 42 58 25 . 83 75 . . . . 71 . . 1 99 19 . . 8 40 3 . 98 43 82 87 . . . 62 . 53 60 95 2 35 . . . 22 91 . 51 64 . . . 80 ."
			". 10 . . 3 51 . 99 53 20 . 69 72 . . . 83 47 6 . . . . 7 66 . 56 68 . 89 98 81 87 . 44 . 71 28 84 . . 11 29 90 86 . 97 33 14 85 45 79 35 5 . 59 15 37 30 . . 16 57 50 . 38 . 36 62 0 12 . 39 1 . 76 49 . . . . 94 48 42 . . . 22 55 . 67 95 78 . 13 2 . . 52 ."
			". 80 72 . 93 59 1 15 34 67 . 52 . . 45 . 95 . 20 33 . 13 . 73 53 11 39 41 . 37 51 . 10 46 26 49 . 68 9 96 76 . 35 31 78 84 64 . . 27 74 . . 28 32 19 55 36 . 7 85 70 6 . 75 63 94 40 . 14 24 . 57 38 22 66 92 . 44 . 86 3 . 90 . 60 . . 29 . 12 71 21 88 81 79 . 5 89 ."
			". . . . . . 79 25 0 65 . 63 . 89 . 4 76 23 44 26 . . 18 . 28 17 9 92 72 81 . 85 53 3 . . 57 47 67 73 46 68 . . 59 . 77 98 21 58 16 22 64 12 . 90 . . 38 39 82 96 93 83 . . 32 31 5 . 45 70 29 11 10 13 . 52 . . 69 19 33 56 15 . 6 . 49 . 86 54 87 75 . . . . . ."
			". . 82 . 95 38 91 58 19 . . 64 53 39 85 . . . 36 35 70 76 . 1 31 . 21 30 59 71 . 34 . . 13 80 66 15 75 37 . . 69 94 45 6 24 9 99 73 10 23 50 47 4 98 46 89 . . 51 33 18 11 86 52 . . 61 . 54 40 62 48 . 7 81 . 88 72 8 25 . . . 84 5 83 67 . . 57 90 44 3 77 . 56 . ."
			". . . . . 13 27 83 55 64 . 7 38 78 3 57 51 16 . 99 . . . 43 4 60 . . 91 26 52 . 22 . 93 50 . 54 8 48 28 56 . . . . . 75 81 . . 29 70 . . . . . 61 87 72 68 9 . 21 92 . 42 . 2 95 20 . . 79 37 5 . . . 30 . 14 77 34 89 0 59 65 . 1 82 63 15 39 . . . . ."
			". 6 . 98 . . . 8 33 . . . . 74 81 13 18 . 29 86 14 36 . . . 19 27 24 85 88 25 56 32 89 2 . 1 12 61 11 49 . 16 91 55 50 60 5 62 38 82 0 96 92 73 9 58 48 . 57 64 79 97 44 . 15 53 87 76 67 47 59 42 65 68 . . . 23 46 41 43 . 37 80 72 39 . . . . 69 66 . . . 70 . 4 ."
			". 31 . 12 . 69 . . 73 32 . . . 28 . 5 . 98 77 58 . . 50 82 55 . 25 46 64 22 86 59 . 33 94 29 43 . 79 7 . . 65 89 . 0 . . . 72 13 . . . 44 . 91 41 . . 34 88 . 20 35 3 78 . 45 30 36 21 4 63 . 87 90 51 . . 66 85 17 . 24 . 54 . . . 11 83 . . 38 . 53 . 26 ."
			". 30 . . 26 29 89 39 . 14 . . . 15 19 . 22 11 94 . 80 58 . 57 77 93 12 87 . . 88 . 76 17 27 . 65 38 45 5 95 66 . 41 48 34 2 . 83 13 63 60 . 67 24 62 3 . 25 18 23 1 91 59 . 74 7 8 . 47 . . 55 16 53 99 86 . 71 96 . 61 70 92 . 10 4 . . . 46 . 85 73 9 35 . . 0 ."
			". . . . . 44 61 87 70 35 . 24 . 31 . 48 34 71 12 30 . . 67 74 49 3 6 54 . 78 . 41 . . 95 58 90 99 63 21 52 47 . 23 . 92 57 1 . 10 76 . 68 42 43 . 94 . 75 88 4 29 98 81 66 56 . . 55 . 93 . 85 82 83 27 9 0 . . 46 20 11 96 73 . 64 . 45 . 40 18 19 50 17 . . . . ."
			". 94 . . . . . 11 7 . . . . 79 . 96 87 70 0 60 . . . 2 33 16 . . 86 . . . . 69 . 30 19 23 55 72 . 63 82 3 4 88 37 22 67 80 34 1 66 78 85 6 51 21 8 . 65 84 13 17 49 . 12 . . . . 15 . . 41 35 14 . . . 99 53 68 81 93 . 57 . . . . 47 92 . . . . . 27 ."
			". . . . . 32 81 . 22 12 78 . . 82 . 25 97 40 2 55 . 53 72 . . . 90 0 65 42 43 . 71 62 38 64 . 95 87 . 10 . 84 . 68 76 44 . . 86 77 . . 26 28 56 . 7 . 21 . 39 33 . 63 89 6 20 . 23 96 73 49 85 . . . 79 13 . 47 46 91 54 3 . 83 . . 67 99 14 . 18 31 . . . . ."
			". 40 . 68 . 45 . 7 . . . 21 . . 60 92 . 77 . 10 78 33 . . 15 . . 63 96 99 75 48 . 8 50 . 94 17 47 61 66 24 42 95 . . 32 34 3 18 2 9 13 30 . . 97 87 70 0 49 58 59 86 . 90 71 . 57 27 39 14 67 . . 84 . . 83 12 4 . 76 . 28 1 . . 79 . . . 89 . 62 . 93 . 69 ."
			". 87 . . 38 . 92 . 10 69 . . . . 0 39 88 44 . 17 43 . 52 54 . . 61 . 58 66 96 65 . 86 53 5 23 67 60 20 . . 8 27 97 . 56 7 29 21 1 68 6 14 . 94 25 19 . . 26 82 12 78 37 30 47 . 15 31 62 24 . 99 . . 22 33 . 55 85 . 32 74 48 70 . . . . 98 50 . 28 . 4 . . 81 ."
			"4 . . . . 20 49 . 56 . . 70 . 34 . 94 . . 96 . 39 . 69 51 37 27 97 85 . 38 84 . 31 72 . 41 0 . 57 74 50 . 91 17 2 83 62 99 45 90 86 33 10 79 75 93 8 98 . 15 24 67 . 76 32 . 92 5 . 77 59 . 60 3 87 26 71 40 . 47 . 66 . . 64 . 63 . 73 . . 12 . 21 78 . . . . 25"
			". . . . 75 . . 77 42 50 . . 37 . 52 . 49 32 . . . . 56 71 80 22 18 55 87 20 97 66 70 73 12 81 34 92 1 6 . 40 . 4 . . 25 53 48 59 39 76 65 61 . . 47 . 44 . 21 14 17 45 8 10 69 94 72 36 30 27 64 35 23 63 78 11 . . . . 89 24 . 33 . 98 . . 38 60 58 . . 16 . . . ."
			"47 . . . 73 54 95 3 67 74 . 81 . . 98 27 . 31 89 . . . . 68 84 7 76 91 . 46 . . . 45 80 78 55 33 36 39 38 0 . . . 70 75 85 69 61 42 50 82 17 40 . . . 24 29 48 9 11 64 97 19 99 . . . 41 . 93 88 6 8 15 . . . . 18 56 . 86 44 . . 87 . 32 90 49 57 96 22 . . . 1"
			". 21 . . 0 . . 91 . 9 . 66 74 . 67 26 23 87 . . 2 77 16 . 57 5 17 95 14 . 24 44 99 . 85 42 10 63 25 59 . 88 . . . 55 . 79 98 54 92 32 27 . 48 . . . 90 . 75 65 4 28 83 29 . 73 93 3 . 81 18 69 97 70 . 86 89 7 . . 51 43 19 96 . 72 31 . 20 . 52 . . 15 . . 41 ."
			". 16 . . . 86 17 94 51 . . 42 . 1 41 71 . 72 . 84 . . 45 30 . 29 . . 4 62 . . . 21 . . 18 . 32 82 . 77 87 35 23 . 81 78 49 47 85 52 11 43 . 36 80 20 63 . 56 22 . 60 . . 38 . . . 46 58 . . 95 . 91 25 . . 2 . 93 . 99 37 75 . 97 . . 65 10 67 59 . . . 5 ."
			". . . . 34 48 . 24 57 97 . . . 50 . . 16 . 56 36 . . . 92 . 41 . 75 44 86 . 93 54 . 11 91 83 4 7 76 . 22 74 64 15 80 . 58 . 14 60 . 37 . 51 55 45 18 73 . 66 25 81 95 98 1 . 96 2 . 29 9 77 . 61 . 43 . . . 59 62 . 0 . . 52 . . . 42 26 70 . 6 87 . . . ."
			". . . . . . 36 . 58 62 . . . . 8 53 20 69 14 . . . . . 83 . 70 . 24 93 49 . . . . 37 30 . . 79 57 . . 96 43 65 11 89 9 1 67 59 99 66 91 84 74 . . 64 16 . . 40 85 . . . . 35 51 38 . 72 . 56 . . . . . 82 29 78 10 95 . . . . 47 94 . 27 . . . . . ."
			". 25 . . 76 . 75 17 40 59 . 30 84 . 73 . 99 61 95 90 . . 12 33 91 . 2 . . 11 37 55 . . 57 14 32 83 31 97 . 8 19 44 67 72 78 13 89 52 66 35 49 50 92 47 6 22 85 . 79 7 16 15 60 80 . . 70 24 48 . . 77 . 34 93 68 . . 36 98 94 5 . 29 . 51 10 . 81 58 54 64 . 71 . . 18 ."
			"38 42 . 7 . 82 73 . . 30 . 5 69 . 83 . 80 91 . 66 . . 96 31 27 74 4 17 8 . 64 35 . 63 81 33 2 0 71 . 12 34 3 . 21 57 39 10 . 48 75 . 45 46 97 37 . 9 51 40 . 56 85 47 44 78 59 . 94 49 . 28 32 70 62 15 61 92 . . 93 . 6 53 . 54 . 43 72 . 16 . . 29 52 . 26 . 60 22"
			". 22 . . . 93 34 . 39 92 . 96 . 56 23 82 70 18 . 29 . 73 38 63 1 54 . 67 89 . . . 12 . 74 3 26 78 98 40 31 53 . . 90 24 41 49 . 0 48 . 25 33 60 5 . . 83 58 46 21 65 72 28 61 . 2 . . . 55 88 . 76 44 95 81 52 . 27 . 99 79 66 32 11 . 14 . 35 97 . 69 86 . . . 85 ."
			". 14 . . 98 . 74 10 . . . 51 50 . 53 . . . . 45 62 . . . . 90 . 20 . 30 . 47 . 22 5 . 29 94 65 56 27 . 92 . . . . 6 97 . . 19 39 . . . . 24 . 93 67 89 99 52 . 76 17 . 4 . 43 . 31 . 26 . . . . 16 71 . . . . 80 . 58 1 . . . 15 68 . 75 . . 70 ."
			". 45 . . 2 . 94 55 3 5 17 . . 10 9 52 81 43 . . 71 72 . . . 37 . 51 . . . 86 84 70 . 88 38 90 . 13 . 82 26 79 54 . 76 . . 35 29 . . 96 . 89 42 34 65 . 8 . 22 98 77 . 58 57 68 . . . 59 . 0 . . . 11 56 . . 31 61 62 97 40 . . 87 93 21 28 12 . 48 . . 44 ."
			". . . . 27 . . . 52 87 74 . . . 57 . . 24 33 19 . . 85 29 . . 35 78 40 5 69 . 82 60 . 68 11 36 46 66 83 . 64 15 . 75 93 . 17 51 79 2 . 81 72 . 86 16 . 76 43 41 25 10 12 . 14 6 . 63 71 22 84 96 . . 54 53 . . 18 30 73 . . 59 . . . 42 39 4 . . . 65 . . . ."
			". . . . 58 . 31 . 11 79 . . . 48 . 21 86 76 15 62 99 52 . . 43 6 60 66 93 53 42 . 75 . . 19 96 34 51 41 . . 46 98 . . 23 94 16 91 32 12 74 63 . . 71 13 . . 1 97 55 87 30 . . 64 . 82 69 33 83 5 90 4 . . 72 89 39 35 24 49 88 . 2 . . . 7 61 . 95 . 92 . . . ."
			". . 85 . . . . . 35 . . . . . 46 78 . 93 . 37 . 69 . . 0 36 15 83 22 50 . 8 79 . . . 61 27 16 80 14 33 88 60 9 86 . 70 59 95 68 11 77 . 53 99 56 1 84 4 40 34 42 54 . . . 18 92 . 20 7 38 13 58 94 . . 30 . 26 . 3 . 17 41 . . . . . 89 . . . . . 90 . ."
			". 33 . . . . . 16 . 6 . 60 . . . . 40 . 22 68 . . 14 39 . 58 7 79 9 59 54 . . . 15 99 . . 93 44 56 . . 77 . 62 . 25 61 65 52 20 87 . 55 . 43 . . 3 37 31 . . 81 53 . . . 38 73 97 41 51 18 . 21 50 . . 45 47 . 89 . . . . 12 . 0 . 30 . . . . . 91 ."
			". . . . . 23 . . . 24 . . . . 58 31 . . 75 . . . . . 65 . . 84 97 82 . . . 20 30 89 . 87 . 49 . 80 45 29 81 66 22 96 5 74 26 57 88 59 0 8 61 70 64 . 3 . 86 . 19 95 39 . . . 2 85 10 . . 17 . . . . . 48 . . 46 91 . . . . 27 . . . 53 . . . . ."
			});

		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);

		const int solveUsingTheseAlgos =
			0
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_2>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_3>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_5>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_7>()
			| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_14>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestDancingLinksSudokuMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<FsssSolver>()
			////| PuzzlesSudokuTester::getAlgoFlag<JSolveMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<bb_sudoku_Matrix>()
			| PuzzlesSudokuTester::getAlgoFlag<Microsoft_z3_solver_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<Google_OR_tools_1>()
			;

		unsigned int numSolutions = 1;
		unsigned int dimension = 100;

		//const string libraryFilePath("../../../test/reports/Sudoku/universalMultiDimensionalSudokuPuzzlesLibrary.csv");
		//PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, solveLibraryPuzzlesAgain, numSolutions, algoFlag);

		for (unsigned int i = 0; i < examples.size(); ++i)
		{
			int iterations = 0;
			long long timeRequiredToGeneratePuzzle = 0;
			SudokuPuzzleUtils::getLogger() << "\n\nPuzzle No. " << i + 1;
			PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(examples[i], dimension, 0), numSolutions, solveUsingTheseAlgos, false);
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = iterations;
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = timeRequiredToGeneratePuzzle;
		}

		//SudokuPuzzleUniversalDatabase::writeToLibrary();

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_10()
	{
		vector<string> examples({
			". . . . . 55 . . . 53 . . . . 10 81 . . 85 . . . . . 19 . . 72 16 41 . . . 38 6 39 . 13 . 90 . 4 63 18 30 79 35 . 82 76 89 65 95 73 23 29 34 42 67 . 58 . 74 . 68 . 8 . . . 91 11 75 . . 80 . . . . . 97 . . 33 69 . . . . 28 . . . 21 . . . . ."
			". 76 . . . . . 21 . 4 . 65 . . . . 0 . 84 61 . . 91 3 . 46 11 23 33 2 57 . . . 37 31 . . 14 83 67 . . 72 . 44 . 19 8 78 47 85 92 . 49 . 22 . . 50 88 15 . . 70 62 . . . 16 34 87 68 9 77 . 39 1 . . 5 29 . 36 . . . . 38 . 54 . 81 . . . . . 82 ."
			". . 0 . . . . . 48 . . . . . 44 59 . 45 . 79 . 20 . . 32 62 30 25 55 49 . 9 47 . . . 22 56 72 94 11 97 6 87 27 69 . 92 38 40 28 91 21 . 39 54 24 82 19 41 17 52 73 89 . . . 76 13 . 57 60 46 26 86 43 . . 66 . 83 . 95 . 58 81 . . . . . 23 . . . . . 68 . ."
			". . . . 33 . 69 . 83 70 . . . 64 . 40 67 68 35 42 58 79 . . 14 48 59 57 39 7 15 . 17 . . 93 85 44 99 24 . . 25 62 . . 65 71 2 28 80 10 55 3 . . 78 43 . . 11 . 38 31 96 . . 53 . 66 98 8 50 18 51 73 . . 97 21 89 34 75 45 84 . 12 . . . 30 46 . 32 . 13 . . . ."
			". . . . 22 . . . 8 71 96 . . . 56 . . 29 16 18 . . 1 89 . . 68 73 63 77 74 . 62 26 . 98 4 40 3 78 90 . 49 24 . 36 0 . . 41 5 83 . 58 25 . 30 17 . 33 . 72 75 . 50 . 46 61 . 65 44 52 81 76 . . 6 88 . . 79 32 37 . . 64 . . . 99 43 86 . . . 57 . . . ."
			". 54 . . 51 . 32 23 43 15 71 . . 58 38 6 31 13 . . 94 84 . . . 42 . 56 . . . 52 73 67 . 77 . 49 . 87 . 5 96 . 99 . 74 . . . 8 . . 98 . 26 70 . 57 . 39 . 83 69 90 . 3 95 19 . . . 78 . 12 . . . 85 36 . . 21 55 68 48 28 . . 44 62 53 27 14 . 1 . . 79 ."
			". 58 . . 44 . 84 65 . . . 14 15 . 48 . . . . 76 6 . . . . 99 . 52 . 60 . 75 . 96 42 . 50 41 . 34 73 . 94 . . . . 46 37 . . 36 18 . . . . 71 . 63 9 87 82 97 . 64 1 . 30 . 4 . 3 . 5 . . . . 40 49 . . . . 35 . 88 2 . . . 7 8 . 56 . . 47 ."
			". 27 . . . 61 99 . 95 91 . 74 . 57 92 8 89 19 . 24 . 78 70 40 85 44 . 80 29 . . . 28 . 69 46 . 66 . 36 32 17 . . 83 . 51 64 . . . . 72 76 62 79 . . 2 81 14 10 . 41 0 6 . 48 . . . 45 53 . 96 71 37 84 22 . 13 . 42 18 90 30 23 . 20 . 94 77 . 38 4 . . . 9 ."
			"89 41 . 73 . 18 7 . . 36 . 98 55 . 87 . 11 66 . 91 . . 27 8 96 13 95 88 31 . 81 58 . 2 64 71 . 1 33 . 21 50 10 . 12 68 48 3 . 39 37 . 97 40 45 75 . 90 14 46 . 47 49 56 94 84 20 . 44 85 . 79 92 62 70 65 23 32 . . 53 . 19 93 . 67 . 77 86 . 25 . . 99 24 . 76 . 22 60"
			". 67 . . 28 . 97 62 66 85 . 2 49 . 47 . 78 37 32 93 . . 0 22 21 . 50 . . 15 10 79 . . 91 16 . 80 76 27 . 59 70 86 14 60 95 . 33 43 99 84 53 4 77 68 1 56 11 . 5 54 . 34 . 18 . . 23 42 94 . . 58 . 20 29 38 . . 52 41 74 26 . 65 . 17 82 . 6 55 31 90 . 45 . . 72 ."
			". . . . . . 68 . 65 43 . . . . 6 73 92 10 46 . . . . . 25 . 99 . 36 31 50 . . . . 47 8 . . 17 41 . . 16 66 95 . 42 24 . 22 70 5 0 13 2 87 . . 30 71 . . 58 57 . . . . 59 84 83 . 37 . 39 . . . . . 1 81 4 53 55 . . . . 89 62 . 54 . . . . . ."
			". . . . 16 94 . 86 31 27 . . . 41 . . 71 . 90 72 . . . 4 . 79 . 98 30 8 . 69 15 . 89 32 21 82 40 26 . 49 78 33 6 35 . 87 . 81 36 . 34 . 11 85 20 80 52 . . . 50 22 . 68 . 70 7 . 28 19 25 . 92 . 74 . . . 57 37 . 2 . . 97 . . . 10 59 44 . 65 55 . . . ."
			". 75 . . . 77 90 84 82 . . 29 . 52 95 49 . 7 . 32 . . 80 87 . 67 . . 71 18 . . . 34 . . 73 . . 92 . 48 53 28 5 . 59 31 40 25 57 44 46 62 . 39 96 47 76 . 69 45 . 14 . . 21 . . . 63 54 . . 35 . 0 64 . . 24 . 9 . 98 68 15 . 13 . . 85 38 86 60 . . . 43 ."
			". 71 . . 46 . . 41 . 28 . 45 14 . 4 20 53 75 . . 12 32 83 . 88 26 93 6 13 . 90 19 3 . 49 22 52 79 . 2 . 9 . . . 96 . 73 39 . 84 81 91 . 99 . . . 59 . 61 40 35 . 87 0 . 55 85 54 . 76 17 34 72 51 . 98 38 42 . . 25 50 7 82 . 10 21 . 77 . 47 . . 23 . . 92 ."
			"57 . . . 53 24 78 48 88 99 . 47 . . 50 16 . 36 79 . . . . 44 89 35 86 19 . 10 . . . 83 68 . 28 76 91 4 15 13 . . . 64 14 . 22 67 55 73 51 49 37 . . . 72 23 84 95 46 25 2 96 66 . . . 32 . 21 61 94 29 65 . . . . 17 27 . 5 62 . . 52 . 70 6 42 71 26 82 . . . 8"
			". . . . 52 . . 20 4 18 . . 96 . 34 . 94 65 . . . . 37 28 69 39 38 81 51 97 29 31 . 0 84 57 72 74 . 14 . 45 . 83 . . 90 91 19 62 93 58 . 15 . . 98 . 33 . 92 76 77 67 42 5 24 44 32 43 26 82 40 87 3 59 47 60 . . . . 61 35 . 66 . 64 . . 88 78 48 . . 95 . . . ."
			"10 . . . . 63 83 . 59 . . 35 . 0 . 88 . . 93 . 34 . 11 58 52 . 66 64 . 61 53 . 65 23 . 60 86 . 5 67 97 . 7 71 72 27 21 8 46 56 . 3 89 95 6 1 19 25 . 14 47 78 . . 15 . 26 30 . 12 90 . 33 75 99 62 55 16 . 22 . 70 . . 45 . 42 . 77 . . 80 . 13 50 . . . . 81"
			". 0 . . 21 . 33 . 37 61 . . . . 22 23 26 80 . 87 5 . 57 14 . . 47 . . 17 85 99 . 10 24 25 13 59 42 63 . . 79 55 60 . . 36 88 12 78 90 48 53 . 92 38 35 . . 62 28 3 . 93 73 75 . 83 4 70 89 . 97 . . 7 18 . 1 43 . 84 94 72 49 . . . . 52 45 . 76 . 34 . . 46 ."
			". 8 . 81 . 66 . 32 . . . 78 . . 82 97 . 55 . 40 1 0 . . 72 . . 53 48 90 38 71 . 27 43 . 62 58 64 95 68 92 37 76 . . 86 84 94 44 56 41 28 21 . . 7 88 69 . . 51 23 39 . 20 74 . 63 33 52 13 79 . . 96 . . 73 45 54 . 67 . 60 46 . . 83 . . . 99 . 14 . 61 . 75 ."
			". . . . . 3 14 . 47 25 81 . . 98 . 28 27 59 43 31 . 91 84 . . . 46 94 68 76 80 . 77 55 35 36 . 39 78 . 63 . 85 . 38 26 61 . . 75 12 . . 29 82 42 . 83 . 17 . 64 34 . 79 . 48 19 . 11 8 5 2 6 . . . 93 53 . 20 44 65 58 92 . 87 . . 56 49 96 . 30 90 . . . . ."
			". 95 . . . . . 38 68 . . . . 85 . 2 14 35 53 92 . . . 59 42 70 . . 1 . . . . 66 . 75 74 . 80 15 . 99 . 6 26 94 73 69 93 . 98 17 . 97 50 10 89 46 23 . 41 11 . . 61 . 51 . . . . 88 . . 48 54 72 . . . 12 84 78 8 83 . 67 . . . . 3 55 . . . . . 65 ."
			". . . . . 39 42 29 36 81 . 90 . 19 . 22 37 56 40 11 . . 77 52 79 80 44 58 . 12 . 76 . . 88 45 17 5 . . 48 83 . 34 . 41 13 20 . 2 62 . 24 18 33 . 72 . 60 67 7 59 47 1 69 8 . . 43 . 99 . 6 53 16 50 96 55 . . 92 10 82 51 75 . 32 . 4 . 85 9 46 49 28 . . . . ."
			". 3 . . 10 99 4 51 . 96 . . . 88 32 . 43 46 72 . 63 68 . 23 34 75 89 65 . . 44 . . 71 98 . 82 . 92 28 86 14 . 74 25 90 . . 11 97 59 30 . 31 35 45 37 . 79 5 80 50 0 16 . 36 73 38 . 57 . . . 60 1 12 19 . 64 49 . 9 58 21 . 94 55 . . . 78 . 67 91 95 41 . . 61 ."
			". 97 . 27 . 83 . . 85 66 . . . 3 . 51 . 99 38 16 . . 4 . 98 . 26 96 43 45 93 22 . 6 52 8 31 . 50 54 . . 75 53 . 12 . . . 19 71 . . . 70 . 40 84 . . 25 90 . . 46 42 72 . 21 . 37 67 . 79 . 95 33 94 . . 77 89 88 . 59 . 65 . . . 63 39 . . 82 . 24 . 58 ."
			". 37 . 61 . . . 44 60 . . . . 97 31 34 48 . 80 71 . 54 . . . 38 78 8 74 6 95 36 41 90 77 . 91 21 58 99 7 . 1 92 52 23 10 15 47 57 69 75 43 88 . 76 . 94 . 42 89 81 39 29 . . 4 56 66 98 27 25 0 83 82 . . . 2 . 96 16 . 46 13 20 45 . . . . 17 40 . . . 14 . 33 ."
			". . . . . 15 50 59 54 72 . 82 20 45 42 95 61 33 . 25 . . . 0 62 97 . . 73 39 26 . 4 . 51 9 . 96 . 68 78 30 . . . . . 35 87 . . 56 . . . . . . 77 28 94 48 . . 99 40 . 63 . 6 66 44 . . . 24 69 . . . 90 . 47 86 18 3 29 2 91 . 21 81 12 5 71 . . . . ."
			". . 32 . 31 92 12 69 2 . . 9 77 49 28 . . . 68 98 15 46 . 64 50 . 22 47 82 25 . 40 . . 86 79 33 7 . 62 . . 39 0 96 89 72 65 70 42 81 27 44 99 14 13 41 91 . . 78 20 . 88 45 71 . . 35 . 87 84 51 56 . 36 8 . 21 90 97 23 . . . 85 76 24 54 . . 38 11 16 83 53 . 37 . ."
			". . . . . . 98 52 84 77 . 54 . 81 . 18 13 86 17 21 . . 94 . 51 14 88 36 83 27 . 32 56 1 . . 89 11 35 12 55 62 . . 49 . 38 16 71 60 9 26 73 57 . 82 . . 0 92 53 44 67 79 . . 64 15 10 . 3 23 34 68 65 47 . 42 . . 37 72 7 41 63 . 95 . 39 . 66 22 45 20 . . . . . ."
			". 9 46 . 94 11 45 76 24 56 . 73 . . 79 . 96 . 62 70 . 48 . 86 90 10 29 5 . 40 83 . 39 19 20 55 . 72 30 23 4 . 22 88 36 3 80 . . 37 6 . . 52 . 34 . 51 . 78 . 17 58 . 95 54 65 77 . 13 35 . 89 41 91 31 98 . 15 . 81 26 . 99 . 53 . . 57 . 92 25 74 59 47 64 . 97 32 ."
			". 88 . . 62 25 . 43 74 41 . 91 44 . . . 4 26 66 . . . . 72 18 . 81 61 . 9 78 53 85 . 14 . . 73 24 . . . 98 8 46 . 58 40 68 . . 38 . 39 . 16 95 2 22 . . . . 96 . 31 . 60 27 93 13 . 52 10 . 45 30 . . . . 50 1 69 . . . 15 6 . 87 34 86 . 54 7 . . 36 ."
			". 18 . . . 72 9 . 90 11 . . . 37 66 10 45 6 . 97 . . . 62 . 88 3 . 15 84 48 . . 5 32 86 . . 69 . . . . 1 16 . 92 81 35 94 33 4 7 34 . 31 73 . . . . 27 . . 56 50 54 . . 79 64 49 . 57 28 67 60 . . . 98 . 38 91 25 71 74 . . . 26 29 . 55 99 . . . 21 ."
			"25 . . . 39 14 37 . . 44 46 . . 70 17 38 77 . 47 22 72 . 74 . . . 28 31 78 87 23 63 68 7 . . 54 89 52 18 19 10 . 97 29 21 . 51 12 . . . 93 . 61 43 85 . 98 24 57 92 96 80 . . 83 9 82 26 53 62 99 66 . . . 90 . 11 65 8 . 64 36 56 41 . . 45 5 . . 94 30 58 . . . 67"
			". . . . . . 51 47 86 . . . 34 43 72 . . 48 5 64 . 71 97 . 81 9 . 59 20 . . 21 2 99 . . 36 . 13 35 62 . 61 . 75 58 98 4 . 55 88 45 63 68 38 87 . 27 . 10 70 . 53 77 . . 42 65 33 . . 0 8 . 50 1 . 91 29 . 31 14 83 . . 7 94 26 . . . 93 57 25 . . . . . ."
			". 32 . 29 . 35 3 . 79 33 . . . 14 61 44 68 73 88 52 . . 47 42 17 . 43 1 27 0 . . . . 31 20 80 46 11 16 24 . . 63 70 5 87 30 . 50 72 62 36 74 8 91 67 . . 9 86 38 60 12 51 28 . . . . 22 37 19 54 . 77 85 95 . . 40 76 15 84 4 93 58 . . . 56 41 . 92 49 . 69 . 53 ."
			". 99 . 19 . . 55 57 . 52 . 95 . . . 7 54 12 8 39 33 11 . 98 . 86 69 4 26 94 . 14 49 . 41 70 37 77 17 . . 28 . 42 89 56 . 27 . 71 53 . 80 . 2 23 64 50 5 . . 6 78 62 22 43 . 59 20 . 18 34 97 24 45 . 13 . 92 25 10 75 72 68 32 . . . 51 . 84 . 3 96 . . 88 . 83 ."
			". 15 . 24 . 98 . 5 13 95 . 33 57 . 29 . . 94 60 . 51 . 8 . 54 53 55 . 38 . 56 30 . . 47 . . 19 43 . . 96 76 . . 67 . 77 91 . . 16 12 . 1 . . 26 78 . . . 90 63 . 7 . . 25 61 . 39 . 20 93 23 . 48 . 88 . 73 59 . . 42 . 82 69 . 65 35 34 . 75 . 28 . 31 ."
			". 66 . . 77 . 54 42 45 94 80 . . 90 91 15 55 2 71 . . 18 . 67 46 61 . 99 . 19 76 74 38 93 60 34 92 25 83 85 . . 44 47 53 43 82 . 64 26 14 21 . 35 52 40 81 59 . . 30 37 72 24 . 23 31 4 17 5 58 . 12 . 27 33 68 . 69 . . 88 16 3 11 78 96 . . 86 50 98 73 6 . 36 . . 63 ."
			". . 16 . 91 . . 27 . 84 26 . 98 99 35 . 50 0 . . . 90 92 12 . 89 63 45 25 13 58 . 1 51 87 62 88 75 4 . . 32 18 39 40 85 33 57 . 46 . 71 76 86 69 3 60 44 66 . . . 41 . 48 47 97 11 . 68 74 96 5 78 7 . 83 70 6 . . . 77 22 . 52 61 23 . 34 72 . 95 . . 19 . 82 . ."
			". . . . 83 30 70 2 1 . . 18 67 . 24 93 69 96 . 82 91 22 39 . 41 . . 37 79 16 . . 57 98 . 12 . 10 59 . 17 . 68 . 3 . . 80 36 99 65 97 . 6 94 51 . 11 . 13 . . 32 . 64 . 81 88 . . 72 46 15 . . 42 . 35 56 31 48 . 0 19 20 27 . 47 43 . . 8 33 85 44 89 . . . ."
			". 92 73 . 61 81 . 6 12 . . 13 . 76 20 84 74 . 9 28 32 93 36 . 7 . 14 . 5 85 . 67 . . . . . 29 . 55 . . 48 11 . 52 69 . . 34 . . . 37 58 . 49 75 42 . 19 . 8 71 . . . . 1 21 82 16 . 43 . 40 . 63 3 87 60 2 . 62 54 24 70 . 35 . . 64 22 . 91 78 . 51 77 ."
			". . . . 89 19 13 4 27 . . . 1 62 . . 56 50 41 67 . 42 . 17 99 8 58 29 . 33 . . . 25 36 2 . . 26 22 . 23 . . . 82 6 28 65 30 73 7 90 . 78 95 77 . 49 . 44 32 . 61 71 97 60 . . . 10 . 45 31 39 5 18 . 43 . 14 74 40 76 . . 84 81 . . . 70 96 47 15 9 . . . ."
			". . 42 . . 9 62 70 94 . . . . 68 . 66 35 8 63 49 . 97 93 45 48 50 . 7 . . 3 . 33 40 55 18 87 . 23 51 84 76 . 38 34 81 5 41 . 88 64 . 29 83 22 21 53 . 80 27 28 99 20 . 26 58 77 98 . 95 . . 82 44 74 86 67 96 61 . 72 15 2 73 0 . 37 . . . . 36 17 60 19 . . 39 . ."
			". 24 . 8 . 73 . 78 18 . . 55 97 71 . 36 . 38 . 51 . . 87 21 3 85 75 16 84 32 62 . . 35 1 44 . 86 . 60 . 42 11 25 93 7 63 . 74 31 70 94 . 19 47 0 23 28 43 . 27 . 76 91 39 72 45 . . 29 81 4 20 40 15 88 79 56 . . 33 . 66 . 65 . 53 67 68 . . 5 41 . 10 . 49 . 48 ."
			"44 . 10 . 50 28 96 56 91 . . 32 . 94 . 98 . 34 81 88 31 . 60 . 95 . 57 9 11 83 . . 89 58 16 74 47 93 15 43 . 85 . 19 87 . 53 24 51 . . 82 26 75 . 4 54 . 13 . 33 55 92 23 . 21 2 49 . . 6 68 73 59 . 0 . 69 . 52 25 77 41 . 42 . 30 . 36 . . 72 8 65 84 80 . 61 . 29"
			". . . . 74 31 . 40 64 21 . . . 2 86 58 19 14 52 78 . 70 22 . 44 . . 27 53 . 99 . 5 77 . 82 . 81 54 57 . 26 . 73 . 10 68 . 15 . . 6 . . 41 . 79 . 55 . 50 66 56 . 9 . 13 24 . 89 . 95 7 17 . 11 . 23 98 . 32 87 97 75 85 51 3 . . . 91 76 4 . 16 63 . . . ."
			". 85 . . 43 . 67 . . 60 . 20 . . 33 91 17 . 39 . 76 . 2 80 . 92 79 . 19 1 . 0 . 12 65 . 59 71 48 45 47 . 21 78 18 40 3 72 44 . . 89 . . . 11 63 5 . 51 87 53 70 93 . 25 68 . 31 . 97 90 . 49 29 . 57 54 . 32 . 22 . 34 6 4 . . 7 . 24 . . 98 . 66 . . 28 ."
			". 79 . 0 . 33 87 36 . 37 . 10 82 30 80 75 . . 73 85 . . . 77 . 55 . 62 88 . 19 91 95 . . 38 97 31 . 64 35 . 67 . 92 45 4 86 43 . 18 . 42 48 71 61 66 57 . 72 15 . 94 46 47 . . 34 6 52 . 41 13 . 8 . 84 . . . 16 12 . . 29 28 24 27 98 . 83 . 68 1 40 . 32 . 11 ."
			". 81 . 2 82 . . 90 15 . 22 23 . . 26 72 65 95 70 . 56 34 . 41 . 40 98 12 46 . . . . . 8 6 . . 28 . 58 . . . 80 . 20 . 1 49 . 86 . 16 . 44 . . . 32 . 75 . 73 36 17 . . . . . 50 37 19 64 . 94 . 93 66 . 57 10 11 89 83 . . 47 39 . 88 69 . . 18 21 . 55 ."
			". . . . 86 95 53 97 . 80 92 . . 7 . . 79 5 87 15 . 43 61 . 71 . 36 89 . 64 . 83 50 24 34 . 56 84 73 88 . . . 32 . 48 . 17 57 . 25 99 98 . 81 38 52 . . . 22 . 62 4 . 12 90 67 40 . 9 . 28 30 . 85 . 14 75 . 63 13 18 1 . . 93 . . 60 51 . 82 26 27 20 . . . ."
			". 48 54 . 1 22 5 75 20 . 31 . 27 93 59 29 42 57 28 83 . 26 13 90 10 96 73 . . 24 70 37 . 76 67 66 41 . . . 33 52 12 . . 9 . 97 . 36 35 34 30 . 3 . . 68 . 84 0 . 51 19 88 . 18 86 11 7 16 . . . 47 55 53 72 91 . 17 99 49 80 21 45 56 94 . 8 . 87 79 46 92 25 . 74 38 ."
			". 17 34 . 24 78 8 73 99 . 55 . 61 69 71 46 36 . 97 38 . 16 3 84 87 52 32 . . 75 1 80 13 14 62 96 35 . . . 93 74 50 . . 25 . . 42 63 0 . 47 . 9 . . 23 94 19 45 91 . . 59 83 27 . 39 76 88 . . 15 40 81 58 20 4 . 95 90 64 33 67 . 68 65 . 66 . 10 37 22 5 49 . 89 57 ."
			". . . . 48 75 65 22 . 54 77 . . 59 . . 29 83 27 57 . 15 10 . 13 . 24 26 . 73 . 78 37 41 70 . 76 98 66 32 . . . 9 33 . . 12 52 89 58 . 3 . 30 35 68 . . . 18 . . 43 . 86 0 82 88 . 55 . 72 47 . 16 . 71 63 . 80 45 8 17 . . 50 . . 49 79 . 14 74 87 44 . . . ."
			". 86 . 53 29 . . 95 72 . 44 92 . . 7 79 47 15 11 . 0 89 . 66 . 49 64 43 6 . . . . . 46 73 24 . 21 . 91 . . . 39 . 96 . . 16 38 37 . 45 . 25 . . . 65 . 4 . 8 40 67 . . . . . 75 14 33 30 . 70 . 42 77 . 58 60 63 69 13 . . 93 18 . 27 51 . . 31 20 . 94 ."
			". 89 . 13 . 4 63 19 . 57 . 48 41 21 62 56 . . 1 50 . . . 37 . 68 . 42 17 . 36 11 92 . . 26 25 22 . . 98 . 28 82 94 46 30 66 23 6 95 . 78 85 90 73 93 77 . 91 60 . 64 . 3 . . 97 71 80 . 43 87 . 31 . 51 . . . 76 38 . . 81 74 88 55 84 . 96 . 53 52 70 . 9 . 35 ."
			". 74 . . 84 . 88 . . 38 . 43 . . 2 19 58 . 18 . 28 . 44 53 . 47 67 . 94 72 . 5 . . 99 . . 57 . 81 69 . 0 10 71 . 29 83 26 . . 55 41 8 59 96 12 79 . 1 . . 89 66 . 24 50 . 9 . 11 98 . 36 17 . 80 7 . 34 . 51 . 32 61 87 . . 3 . 4 . . 62 . 90 . . 45 ."
			". . . . 79 36 . 33 69 76 . . . 80 30 60 75 85 . 89 . 62 63 . 59 . 51 14 77 . 17 . 91 97 . 53 9 64 . 31 . 43 . . . 2 70 . 90 . . 39 . 72 42 . 57 . 74 . . 46 52 . 6 . 15 81 . 94 . 65 22 8 . 78 . 13 41 . 44 28 26 16 27 12 98 . . . 68 40 83 . 7 32 . . . ."
			"91 . 45 . 7 56 66 28 44 . . 3 . 40 . 90 . 88 76 34 18 . 95 . 60 . 83 39 35 57 . . 27 47 63 15 58 . 74 93 . 51 . 22 37 . 79 62 85 . . . 17 20 . 97 14 . 82 . 2 23 1 55 38 49 33 21 . . 0 48 69 46 . 6 . 73 . 99 70 86 5 . 71 . 36 . 30 . . 84 64 61 72 67 . 65 . 12"
			". 11 . 62 . 70 . 9 32 . . 4 63 16 . 35 . 49 . 8 . . 48 69 93 25 91 97 45 65 55 . . 87 3 23 40 51 . 30 . 54 41 81 84 38 88 . 76 5 21 80 . 27 29 64 10 53 92 . 77 . 95 99 14 98 28 . . 20 86 61 96 74 44 89 24 82 . . 73 . 43 . 79 . 46 0 37 . . 19 75 . 36 . 85 . 71 ."
			". . 51 . . 90 2 71 61 . . . . 26 . 65 72 53 45 95 . 12 . 46 78 56 74 34 . . 8 . 29 68 . 28 . 52 6 42 80 1 . 13 58 99 . 60 . 20 . . 67 32 31 24 62 . 86 16 35 73 48 75 54 85 63 17 . 84 . . 76 64 19 92 66 37 50 . 11 83 39 9 91 . 47 . . . . 0 97 43 88 . . 77 . ."
			". . . . 85 46 58 26 77 . . . 39 33 . . 91 9 64 74 . 38 . 19 2 76 1 86 . 79 . . . 59 61 48 12 . 10 71 . 44 . 40 47 78 8 21 75 3 . 88 56 51 15 50 5 . 89 . 68 93 . 53 31 16 87 . . . 83 . 54 29 49 97 32 . 90 . 34 4 23 82 . . 7 6 . . . 73 24 81 42 99 . . . ."
			". 60 40 . 23 47 . 74 28 . . 19 . 72 43 41 32 . 34 48 9 59 81 . 97 . 52 . 95 96 22 2 . . . . . 35 . 3 . 6 4 58 62 49 55 . . 98 87 . . 10 63 88 27 92 45 . 42 . 44 85 . . . . 16 53 1 29 . 50 . 79 . 8 . 17 67 7 . 31 26 14 18 . 94 . . 11 76 . 93 46 . 25 37 ."
			". . . . 15 5 24 98 71 . . 58 60 . 11 62 85 27 . 94 53 40 54 . 8 . . 10 49 55 . . 30 84 . 43 . 50 0 . 79 . 77 . 74 37 66 . 96 . 17 78 1 70 12 41 . 99 . 89 . 63 61 . 25 . 52 7 . . 23 32 48 . . 21 . 9 39 2 6 . 92 87 82 73 . 44 81 . . 75 65 4 35 28 . . . ."
			". . 11 . 18 . . 72 . 17 87 . 83 66 37 . 10 97 . . . 44 68 15 . 24 84 2 . 3 32 . 96 42 48 69 5 65 86 . . 35 81 59 22 1 94 93 41 92 31 46 19 77 7 33 0 73 4 . . 13 79 27 58 39 95 50 . 40 67 51 80 28 57 . 76 36 49 . . . 30 98 . 63 89 25 . 38 23 . 26 . . 52 . 55 . ."
			". 77 . . 66 . 56 0 62 8 49 . . 91 90 55 15 1 51 . . 99 . 70 75 57 . 18 . 48 60 38 74 92 76 83 93 85 34 25 . . 95 43 . 47 26 . 20 82 40 28 . 22 79 14 . 81 . . 31 24 5 37 . . 30 23 84 72 33 . 65 . 89 58 41 . 10 . . 78 86 29 97 88 9 . . 16 73 32 50 7 . 39 . . 87 ."
			". 65 . 3 . 96 . 35 78 48 . 75 88 . 14 . . 52 23 . 64 . 17 . 47 83 0 . 42 . 31 6 . . 82 . 81 16 20 . . 25 30 . . 63 . 90 7 87 . 18 8 . 36 . . 67 62 . . 12 45 38 . 66 . . 51 60 . 94 . 71 54 22 . 19 . 85 . 93 55 . . 76 . 4 58 . 59 49 56 . 41 . 10 . 13 ."
			". 83 . 70 . . 21 30 . 58 . 40 . . . 69 93 82 67 96 77 37 . 79 . 91 16 22 34 23 . 57 26 . 28 59 . 53 12 . . 36 80 54 17 . . 68 . . . . 94 . . 65 11 29 97 . . 74 87 49 73 88 . 75 64 . 42 56 35 52 55 . 31 . 46 38 19 27 50 48 47 . . . 66 . 33 . 61 9 . . 89 . 62 ."
			". 91 . 10 . 27 93 . 80 16 . . . 35 99 50 30 81 98 0 . . 29 25 92 . 13 90 12 63 . . . . 58 4 51 9 62 75 40 . . 85 8 39 46 . 32 33 . 66 69 . 76 15 44 . . 86 97 94 68 2 67 11 . . . . 14 6 70 7 . 74 59 5 . . 22 52 34 28 23 21 17 . . . 95 79 . 82 43 . 19 . 54 ."
			". . . . . . 19 67 87 . . . 8 65 63 . . 39 21 12 . 4 35 . 58 33 . 11 98 . . 49 14 37 . . 44 . 70 77 89 . 27 . 0 42 71 38 28 . . 5 2 90 80 53 . 64 . 82 93 62 . 6 . . 29 . 22 . . 92 30 . 24 18 . 97 34 . 68 79 46 . . 75 51 32 . . . 48 84 66 . . . . . ."
			"50 . . . 69 34 59 . . 88 79 . . 17 70 77 38 . 42 3 73 . 6 . . . 87 60 76 28 40 68 63 54 . . 7 18 95 89 29 12 . 21 19 97 . 2 10 15 43 98 61 . 93 20 32 . . 55 83 80 26 92 . . 57 . 91 96 75 86 90 81 . . . 99 . 4 64 56 . 65 49 8 33 . . 13 71 . . 0 1 27 . . . 16"
			". 61 . . . 6 41 . 97 73 . . . 20 76 74 84 28 . 4 . . . 5 36 32 85 . 56 . 79 . . 15 33 27 . . 94 . . . . 52 65 . 34 48 31 69 . 42 58 25 . 83 75 . . . . 71 . . 1 99 19 . . 8 40 3 . 98 43 82 87 . . . 62 . 53 60 95 2 35 . . . 22 91 . 51 64 . . . 80 ."
			". 10 . . 3 51 . 99 53 20 . 69 72 . . . 83 47 6 . . . . 7 66 . 56 68 . 89 98 81 87 . 44 . 71 28 84 . . 11 29 90 86 . 97 33 14 85 45 . 35 5 . 59 15 37 30 . . . 57 50 . 38 . 36 62 0 12 . 39 1 . 76 49 . . . . 94 48 42 . . . 22 55 . 67 95 78 . 13 2 . . 52 ."
			". 80 72 . 93 59 1 15 34 67 . 52 . . 45 . 95 . 20 33 . 13 . 73 53 11 39 41 . 37 51 . 10 46 26 49 . 68 9 96 76 . 35 31 . 84 64 . . 27 74 . . 28 32 . 55 36 . 7 . 70 6 . 75 63 94 40 . 14 24 . 57 38 22 66 92 . 44 . 86 3 . 90 . 60 . . 29 . 12 71 21 88 81 79 . 5 89 ."
			". . . . . . 79 25 0 65 . 63 . 89 . 4 76 23 44 26 . . 18 . 28 17 9 92 72 81 . 85 53 3 . . . 47 . 73 46 68 . . 59 . 77 98 . 58 16 22 64 12 . 90 . . 38 39 82 . 93 83 . . 32 31 5 . 45 70 29 . 10 13 . 52 . . 69 19 33 56 15 . 6 . 49 . 86 54 87 75 . . . . . ."
			". . 82 . 95 38 91 58 19 . . 64 53 39 85 . . . 36 35 70 76 . 1 31 . 21 30 59 71 . 34 . . 13 80 66 . 75 37 . . 69 94 45 6 24 9 . 73 . 23 50 47 4 98 46 89 . . . 33 18 11 86 52 . . 61 . 54 40 62 48 . 7 81 . 88 72 8 25 . . . 84 5 83 67 . . 57 90 44 3 77 . 56 . ."
			". . . . . 13 27 83 55 64 . 7 38 78 3 57 51 16 . 99 . . . 43 . 60 . . 91 26 52 . 22 . 93 50 . 54 8 . 28 56 . . . . . 75 81 . . 29 70 . . . . . 61 87 72 68 9 . 21 92 . 42 . 2 95 20 . . 79 37 5 . . . 30 . 14 77 34 89 0 59 65 . 1 82 63 15 39 . . . . ."
			". 6 . 98 . . . 8 33 . . . . 74 81 13 18 . 29 86 14 36 . . . 19 27 24 85 88 25 56 32 89 2 . 1 . 61 . 49 . 16 91 55 50 60 5 62 38 . 0 96 . 73 9 58 48 . 57 64 79 97 44 . 15 53 87 76 67 47 59 42 65 68 . . . 23 46 41 43 . 37 80 72 39 . . . . 69 66 . . . 70 . 4 ."
			". 31 . 12 . 69 . . . 32 . . . 28 . 5 . 98 77 58 . . 50 82 55 . 25 46 64 22 86 59 . 33 94 29 43 . . 7 . . 65 89 . 0 . . . 72 13 . . . 44 . 91 41 . . 34 88 . . 35 3 78 . 45 30 36 21 . 63 . . 90 51 . . 66 85 17 . 24 . 54 . . . 11 83 . . 38 . 53 . 26 ."
			". 30 . . 26 29 89 39 . 14 . . . 15 19 . 22 11 94 . 80 58 . 57 77 93 12 87 . . 88 . 76 17 27 . 65 . 45 . 95 66 . 41 48 34 2 . 83 13 63 60 . 67 24 62 3 . 25 18 23 1 91 59 . 74 7 8 . 47 . . . 16 53 99 86 . 71 96 . 61 70 92 . 10 4 . . . 46 . 85 73 9 35 . . 0 ."
			". . . . . 44 61 87 70 35 . 24 . 31 . 48 34 71 12 30 . . 67 74 49 3 6 54 . 78 . 41 . . 95 58 90 99 63 21 52 47 . 23 . 92 57 1 . . 76 . 68 . 43 . 94 . 75 88 . 29 98 . 66 56 . . 55 . 93 . 85 82 83 27 9 0 . . 46 20 11 96 73 . 64 . 45 . 40 18 19 50 17 . . . . ."
			". 94 . . . . . 11 7 . . . . 79 . 96 87 70 0 60 . . . 2 33 16 . . 86 . . . . 69 . 30 19 23 55 72 . 63 82 3 4 88 37 22 67 80 34 1 66 78 85 6 . 21 8 . 65 84 13 17 49 . 12 . . . . 15 . . 41 35 14 . . . 99 53 68 81 93 . 57 . . . . 47 92 . . . . . 27 ."
			". . . . . 32 81 . 22 12 78 . . 82 . 25 97 40 2 55 . 53 72 . . . 90 0 65 42 43 . 71 62 38 64 . 95 87 . 10 . 84 . 68 76 44 . . 86 . . . 26 28 56 . 7 . 21 . 39 33 . 63 89 6 20 . 23 96 73 49 85 . . . 79 13 . 47 46 91 54 3 . 83 . . 67 99 14 . 18 31 . . . . ."
			". 40 . 68 . 45 . 7 . . . 21 . . 60 92 . 77 . 10 78 33 . . 15 . . 63 96 99 75 48 . 8 50 . . . . 61 66 24 42 95 . . 32 34 3 . . 9 13 30 . . 97 87 70 0 49 . 59 86 . 90 71 . 57 27 39 14 67 . . 84 . . 83 12 4 . 76 . 28 1 . . 79 . . . 89 . 62 . 93 . 69 ."
			". 87 . . 38 . 92 . 10 69 . . . . 0 39 88 44 . 17 43 . 52 54 . . 61 . 58 66 96 65 . 86 53 5 23 . 60 20 . . 8 27 97 . 56 7 29 . 1 68 6 14 . 94 25 19 . . 26 82 12 78 37 30 47 . 15 31 62 24 . 99 . . 22 33 . 55 85 . 32 74 48 70 . . . . 98 50 . 28 . 4 . . 81 ."
			"4 . . . . 20 49 . 56 . . 70 . 34 . 94 . . 96 . 39 . 69 51 37 27 97 85 . 38 84 . 31 72 . 41 0 . 57 74 50 . 91 17 2 83 62 99 45 . 86 33 10 79 75 93 8 98 . 15 24 67 . 76 32 . 92 5 . 77 59 . 60 3 87 26 71 40 . 47 . 66 . . 64 . 63 . 73 . . 12 . 21 78 . . . . 25"
			". . . . 75 . . 77 42 50 . . 37 . 52 . 49 32 . . . . 56 71 80 22 18 55 87 20 97 66 70 73 12 81 34 92 1 6 . 40 . 4 . . 25 53 . 59 39 76 65 61 . . 47 . 44 . 21 14 17 45 8 10 69 94 72 36 30 27 . 35 23 63 78 11 . . . . 89 24 . 33 . 98 . . 38 60 58 . . 16 . . . ."
			"47 . . . 73 54 95 3 67 74 . 81 . . 98 27 . 31 89 . . . . 68 84 7 76 91 . 46 . . . 45 . 78 55 33 36 39 38 0 . . . . . 85 69 61 . 50 82 17 40 . . . 24 29 48 . 11 . 97 19 99 . . . 41 . 93 88 6 8 15 . . . . 18 56 . 86 44 . . 87 . 32 90 49 57 96 22 . . . 1"
			". 21 . . 0 . . 91 . 9 . 66 74 . 67 26 23 87 . . 2 77 16 . 57 5 17 95 14 . 24 44 99 . 85 42 10 . 25 59 . 88 . . . 55 . . . . . . 27 . 48 . . . 90 . . 65 4 28 83 29 . 73 93 3 . 81 18 69 97 70 . 86 89 7 . . 51 43 19 96 . 72 31 . 20 . 52 . . 15 . . 41 ."
			". 16 . . . 86 17 94 51 . . 42 . 1 41 71 . 72 . 84 . . 45 30 . 29 . . 4 62 . . . 21 . . 18 . 32 82 . 77 87 35 23 . 81 78 49 47 85 52 11 43 . 36 80 20 63 . 56 22 . 60 . . 38 . . . 46 58 . . 95 . 91 25 . . 2 . 93 . 99 37 75 . 97 . . 65 10 67 59 . . . 5 ."
			". . . . 34 48 . 24 57 97 . . . 50 . . 16 . 56 36 . . . 92 . 41 . 75 44 86 . 93 54 . 11 91 83 4 7 76 . 22 74 64 15 80 . 58 . 14 60 . 37 . 51 55 45 18 73 . 66 . 81 . 98 1 . 96 2 . 29 9 77 . 61 . 43 . . . 59 62 . 0 . . 52 . . . 42 26 70 . 6 87 . . . ."
			". . . . . . 36 . 58 62 . . . . 8 53 20 69 14 . . . . . 83 . 70 . 24 93 49 . . . . 37 30 . . 79 57 . . . 43 65 11 89 9 1 67 59 99 66 91 84 74 . . 64 16 . . 40 85 . . . . 35 51 38 . 72 . 56 . . . . . 82 29 78 10 95 . . . . 47 94 . 27 . . . . . ."
			". 25 . . 76 . 75 17 40 59 . 30 84 . 73 . 99 61 95 90 . . 12 33 91 . 2 . . 11 37 55 . . 57 14 32 83 . 97 . 8 19 44 67 72 78 13 89 52 66 . 49 . 92 47 6 22 85 . 79 7 16 15 60 80 . . 70 24 48 . . 77 . 34 93 68 . . 36 98 94 5 . 29 . 51 10 . 81 58 54 64 . 71 . . 18 ."
			"38 42 . 7 . 82 73 . . 30 . 5 69 . 83 . 80 91 . 66 . . 96 31 27 74 4 17 8 . 64 35 . 63 81 33 2 0 71 . 12 34 3 . 21 57 39 10 . 48 75 . . 46 . 37 . 9 51 40 . 56 85 47 44 . 59 . 94 49 . 28 32 . 62 15 61 92 . . 93 . . 53 . 54 . 43 72 . 16 . . 29 52 . 26 . 60 22"
			". 22 . . . 93 34 . 39 92 . 96 . 56 23 82 70 18 . 29 . 73 38 63 1 54 . 67 89 . . . 12 . 74 3 26 78 98 40 31 53 . . 90 24 . 49 . 0 48 . 25 33 60 5 . . 83 58 46 21 65 72 28 61 . 2 . . . 55 88 . 76 44 95 81 52 . 27 . 99 79 66 32 11 . 14 . 35 97 . 69 86 . . . 85 ."
			". 14 . . 98 . 74 10 . . . 51 50 . 53 . . . . 45 62 . . . . 90 . 20 . 30 . 47 . 22 5 . 29 94 65 56 27 . 92 . . . . 6 97 . . . 39 . . . . 24 . 93 67 89 99 52 . 76 17 . 4 . 43 . 31 . 26 . . . . 16 71 . . . . 80 . 58 1 . . . 15 68 . 75 . . 70 ."
			". 45 . . 2 . 94 55 3 5 17 . . 10 9 52 81 43 . . 71 72 . . . 37 . 51 . . . 86 84 70 . 88 38 90 . 13 . 82 26 79 54 . 76 . . 35 29 . . 96 . 89 42 34 65 . 8 . 22 98 77 . 58 57 68 . . . 59 . 0 . . . 11 56 . . 31 61 62 97 40 . . 87 93 21 28 12 . 48 . . 44 ."
			". . . . 27 . . . 52 87 74 . . . 57 . . 24 33 19 . . 85 29 . . 35 78 40 5 69 . 82 60 . 68 11 36 46 66 83 . 64 15 . 75 93 . 17 51 79 2 . 81 72 . 86 16 . 76 43 41 . 10 12 . 14 6 . 63 71 22 84 96 . . 54 53 . . 18 30 73 . . 59 . . . 42 39 4 . . . 65 . . . ."
			". . . . 58 . 31 . 11 79 . . . 48 . 21 86 76 15 62 99 52 . . 43 6 60 66 93 53 42 . 75 . . 19 96 34 51 41 . . 46 98 . . 23 94 16 . 32 12 74 63 . . 71 13 . . 1 97 55 87 30 . . 64 . 82 69 33 83 5 90 4 . . 72 89 39 35 24 49 88 . 2 . . . 7 61 . 95 . 92 . . . ."
			". . 85 . . . . . 35 . . . . . 46 78 . 93 . 37 . 69 . . 0 36 15 83 22 50 . 8 79 . . . 61 27 16 80 14 33 88 60 9 86 . 70 59 95 68 11 77 . 53 99 56 1 84 4 40 34 42 54 . . . 18 92 . 20 7 38 13 58 94 . . 30 . 26 . 3 . 17 41 . . . . . 89 . . . . . 90 . ."
			". 33 . . . . . 16 . 6 . 60 . . . . 40 . 22 68 . . 14 39 . 58 7 79 9 59 54 . . . 15 99 . . . 44 56 . . 77 . 62 . 25 61 65 52 20 87 . 55 . 43 . . 3 37 . . . 81 53 . . . 38 73 97 41 51 18 . 21 50 . . 45 47 . 89 . . . . 12 . 0 . 30 . . . . . 91 ."
			". . . . . 23 . . . 24 . . . . 58 31 . . 75 . . . . . 65 . . 84 97 82 . . . 20 30 89 . 87 . 49 . 80 45 29 81 66 22 96 5 . 26 57 88 59 0 8 61 70 64 . 3 . 86 . 19 95 39 . . . 2 85 10 . . 17 . . . . . 48 . . 46 91 . . . . 27 . . . 53 . . . . ."
		});

		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);

		const int solveUsingTheseAlgos =
			0
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_2>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_3>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_5>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_7>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_14>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestDancingLinksSudokuMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<FsssSolver>()
			////| PuzzlesSudokuTester::getAlgoFlag<JSolveMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<bb_sudoku_Matrix>()
			| PuzzlesSudokuTester::getAlgoFlag<Microsoft_z3_solver_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<Google_OR_tools_1>()
			;

		unsigned int numSolutions = 1;
		unsigned int dimension = 100;

		//const string libraryFilePath("../../../test/reports/Sudoku/universalMultiDimensionalSudokuPuzzlesLibrary.csv");
		//PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, solveLibraryPuzzlesAgain, numSolutions, algoFlag);

		for (unsigned int i = 0; i < examples.size(); ++i)
		{
			int iterations = 0;
			long long timeRequiredToGeneratePuzzle = 0;
			SudokuPuzzleUtils::getLogger() << "\n\nPuzzle No. " << i + 1;
			PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(examples[i], dimension, 0), numSolutions, solveUsingTheseAlgos, false);
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = iterations;
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = timeRequiredToGeneratePuzzle;
		}

		//SudokuPuzzleUniversalDatabase::writeToLibrary();

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();
	}

	void PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_11()
	{
		vector<string> examples({
			"155   . 185 154 148 183   .   . 048 149 013 097 095   . 112 165 184   . 162   .   . 066   . 023 119   . 060 158 011 217 135   . 207 167 177 134   .   . 004 123 071 082 070 005 003 113 130 106 065 200   . 093 101 088 190 132 105 161 137 204 205 140 073   . 159 056 078 036   . 019 089 122 030 146 062 053   . 153 091 133 035   . 188 221 042 009 178 126   . 156 080 047 098   .   . 046 049 157 179 201 173 150 108 020 115 202 010   . 214 057 041 152 102 187 086 051 052   . 210 110 104 014 037 147 171 096 186 189 172 069   .   . 018 129 127 136   . 079 192 160 084 081 109   . 111 208 026 007   . 181 059 044 022 075 033 058   . 012 121 094 131   . 163 197 219 027 125 072 138 076 061 169 024 141   . 040 168 120 034 176 215 117 008 015 099 001 170   .   . 211 032 016 166   . 124 139 182 100 180   . 000 068   . 203   .   . 175   . 092 031 128   . 164 090 194 142 114   .   . 025 054 145 222   . 017"
			//"  .   . 213 160   . 192 084   .   . 109 074 079 026 111 151 033 094 197 059   . 174 163 075   .   .   . 219 058 044   . 141 040 006 176 061 169   .   . 024 027 034 168 138   . 076 039 016 029   .   .   . 032 124 216 001 015 008 211 117 166 000 175 068   . 139 182 180 100   . 196 103 203 085 031 087 194   . 199 025 128 145 090 191 116 017 142   . 114 164 054 097 013 154   .   .   .   . 218   . 112 038 148 095 028 149   . 083   . 165 119 066 002 023 217 011 144 143   . 055   . 004 003 134 082 070 005   . 118   .   .   .   . 207 123 177 137 200 106   . 161 088 132 113 093 130 065 064 190   . 204 056 036 122 078 030 089   . 146 205 019 077   . 140 062 073 221 188 133 156 053 042 035 153 178   .   .   . 091 050 126 201   . 046 209 108 107 179   .   . 098 049 157 150 080 047   . 202 086 210   .   .   . 010 187 063   . 041 110 052 102 096 129 147 172 014 018   .   . 104 189   . 037 186   .   ."
			//"178   . 156 212   . 035 188 221 153 042   . 224 009 091 126 047 107 046   .   . 173 080 179 150 108   . 201 098 157 049   . 041 057 223   . 010   .   .   . 051 102 063 187   . 214 127 189 172 206 069   . 171 014 198 104   . 037 147 129 186 136   . 151   . 026 081 208   . 109   . 160 213 074 181 192 033   . 094 044 059 197 163 174 121 131 045 075 219 058 022   . 076 168 024   .   . 138 120 141 176 125 034   . 027 061 015 008   . 124   . 029   .   .   . 016   . 216   . 170 001 196 085   . 103 203 182 031 100 175   .   . 092 139 000   . 116 114 090 145 164 191 128 017 054 025 220 142 199   . 222 195 218 185 155 013   . 095   . 028 048 149   . 097   . 112 144 143 162 217   . 002 083 184 055   . 066 158 011 023 060 167   . 134 005 207 004   .   .   . 082   . 118 071 067   . 064 132 130 065   . 137 200 105 093 161   .   . 190 088 204 056 205 089 140   . 159 073 021 019 036   . 062 146   . 077"
			//"210 202 187   . 102 010 052   . 110   .   . 041 057 086 223 014 104 037 096   . 198 172 206   . 018 127 069 147   . 171 208 079 026   .   . 192   . 084 007 136 181   . 213 081 074 131 044 163 075 219   . 022 033   . 094 059 197 058 121 012 027 040 176 125 006   . 141   . 061 024 168 138 076 034 169 124   . 001 016 015 008 029 216 117 039 099 170   . 211   . 175 085 103   .   .   . 203 100 180 068 092 031 139 000 193 128 145   . 194 142 090 017 222 054 025 116 191   . 220 199 048 013 183 154 185 195 148 218 097   .   .   . 095 028 149   . 060   . 083 158 143 162 002 217 011 055 119 184   . 023 005 118 070 135 003 082 207   . 123   . 177 043 067 134 167 137 088 132 093 101 113   . 190 065   . 106 161 130 204 200 073 205 062   . 159 019 078 077   . 089   .   . 146 140 030 009   . 091 178 042 221   . 035 156 212   . 224 153 188 050 209 020 098 080   .   . 201   . 107 157 049   . 150 179 115"
			//"  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 009   .   .   .   . 156   .   .   .   .   .   .   . 173   .   .   .   .   .   .   .   .   .   .   . 223   .   .   .   .   .   .   .   . 051   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 026   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 006   .   . 034   .   . 027   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 103   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 038   .   .   .   .   .   .   .   . 097   .   .   .   .   .   .   .   .   .   .   . 002   .   .   .   .   .   .   . 207   .   .   .   . 123   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   ."
			//"180 182 203   . 031 087 092   . 196 193 085   . 139 100 068 194 199 145   .   . 191 090 220 222 142   . 114 164 025 054 155   . 095 112   . 183   . 038 048 028 148 154 185   . 013 002 011 066 055 060   .   . 165 143 184 162 083 158 144 023 123 067 167 043 207 005 135 118 177 004 082 070 003 071 134   .   . 190 130 132 105 106 088 137 113   . 065 200 161 093 140 030   .   .   . 062 122 036 078 073   . 146 159 205 077 133 035   .   . 009 224 042 050 156 091 221   .   . 178 153 107 047 046 098   . 209 150 157 080 201   .   .   . 020 115 051 223 041 010   . 052 202 152 187 086 210 057 110   .   . 096 189 171 206 014 147 018 186 129 104 127 198 172 037 069 136 084 081 213 074 109 192 007   .   . 079 160 111 181 151 219   . 197 059 045 094 075 131   . 058   . 044 012   . 033 006 072 120 141   . 027 176 169 138 168   .   . 024 125 034 015 117 211   . 124 099 215   . 001 016 032   . 166 170 039"
			//"220 128   . 164 222 145 191 116 199   . 194 090 142 025   . 013 048 183 195   . 038   . 155   .   . 149 112   . 218   . 055 066 119 060 002 083   .   . 184 144   . 158 217 162 165 177 118 067 135 167   .   . 003 043 004 005 134 082 123 071 137 106 200   . 064 132 065 130 113 190 161 093 101 204 105 030   . 019 036 056   . 140 021 205 077 159 078 073 089 122 224 053 212   .   . 035   . 091 178 126 188 050 009 221 042   . 046   . 047 108 080 115 150 049 157 020 173   . 179   . 110 214 010 063 187 202 102 086   . 223   .   . 057 051 152 129 069 172 037 147 198 096 127 171   . 206 018 104   . 186 081 111 213 208 074 160 026 181 136 007 109   . 079 192 151 121 174 059 022 033 131 197 094   .   . 163 058 044 012 219 176 027 169 072   . 024 141   .   . 168 138 120 034 040 076   . 015   . 170 039   .   . 008   . 211   . 029 001 216 166   . 000 103 175 085   . 068 092 196 100 203 087   . 180 193"
			//"  . 059   . 058 012 197 174 121 094   .   . 163 045 044 219 076 024 169 072   . 125 040   . 034 006   . 176 168 120 138 170 029 099 215 039 008   . 216 001 117 166 211   . 015 124 193 100 175 180 068   . 203 085 092 196 182   . 103 000 031 116 090   . 191 142 128   . 025   . 199   .   . 194 222 145 013   . 048 218 195 183 097 038 028 149 095 155 112 154 185 066 165 158   .   .   . 217 011 055 060 143 023 119   . 002 005 134   . 003   . 067   . 071   . 118   . 043   . 135 004 190   . 105 161 093 132 204 130 106   .   .   . 064 137 113 205 073 140 062 089 021 056 077 122 036 078 159 019   . 146 133 091 156   .   . 212   . 050   . 153 042 188   . 035 126 020 173 209   . 047 115 046 107 179   . 080 098 157 150 201 223 051   . 202 057 110 210 152   . 063 187 086 102 041 214 018 096 189 206   . 129 069   . 171 147   . 172 104 198 186 081 136 160 079   .   . 151 084 007 111 213 192   . 208   ."
			//"135 005 070 082   . 134 043   . 004 177   .   . 207 118 167   . 190   . 132   . 088 106 065   . 064   . 200 161 130 093 078 140 159 073 077   .   . 021 019 205 146 089   . 056 030 042 091 224 178 126   . 156 053 188 153 133 035   .   . 050 020 080 201 173 108 209 179 157 115 107 098 049 047 150 046 214   .   . 086 202 010 041 052 051 152 057 210 223 063 187 172 014   . 104   .   . 171 189   . 069 198 186 018 129 127 081   .   . 074 026 079 109 181 213 111 136 084   .   . 007 094 033 197 058 022 059   . 044 163   .   . 174   . 121 131 027 176 040 169 168 125 072 061 138 120 141 006   .   . 034 015 016 032 170 124 211 099 166 117 001 039 216 029 008 215 000   .   . 203 085 193 087 196 180   . 175 103 100 031 068 114 116   . 128 142 199 220 017   .   . 054 025 222 090 194 095 195 218 155   . 028   . 183 185 154   . 097   . 038   . 162 144 158   .   . 119 060   . 184 011   . 083 023 055 002"
			//"065 132   . 161 204 105 088   . 190 113   . 106   . 130   . 030 019 062 056   . 021 140 078 146 159   .   . 089 036 122 178 224 009 126 042 035   .   . 153 221 050 212   . 133 053 115 157 080 179   .   . 049 047 173 107 209   . 098   . 150 051   . 223   . 057 202   .   . 152 110 063 187 214 102 010 014   . 104 189 096 037 172 198 129 127 018 206 069 147 171 079 074 160   .   .   . 213 111 208 151   . 181 026   . 109   . 197   . 033 045 163 131   . 022 044 121 174   . 075   . 024   . 169 168   . 072 034 120 040   .   .   . 006 027 061 117 215 029 008 211 216 015 039 032 016 170 099 001   . 166 182 100 203 180 085 103 139   .   . 196 193   . 175   . 068 116   . 128   . 194 017 145 199 220   .   . 164 025 222 114 112 028   . 195 095 048 155   .   . 154 185 218 148 097 013 119 162 011   .   . 144 060 083 217 158   . 066 184 143 023   . 123   . 067   . 207 167   . 004 118 070 134   . 135 177"
			//"141 072 138 168 034 169 125   . 024 061 076   . 006 120 176 124 001 008 015   . 216 029 170   . 099   . 215 211 016 032 180 175 139 068 193 087   .   . 196 000   . 103 203 182 085 017 025 090 220 114 142 054 194 191 199 128 145   . 116 222 028 097 112   .   . 195 155 218   . 048 154 185 013 148 183   .   . 184 011 162 083 066 143 144 002 119 055 060 158 217 067 003 082   .   . 134 070   . 135 167 043   . 207 123 177 132 105   . 101 064 106 113 204 093 130 137 088   . 065 190 019 030 062   . 122 056 146   . 140 073   .   . 159 205 077 221 126 224 035 212 188 133 042 156 091 178 009 153   .   . 209 157 049 179 047 098   . 150 020 107   .   . 080 046 201 051 052   . 187 214 152 010 110 210 057 041 063 086 102 223 069 129 037 096   . 104 206   .   . 147 171 189 186 172 014 026 081 111 208   . 136   . 192 213 160   . 079 007 084 181 059 121 058   . 033 045 219   . 094 044 022 197 012 075 131"
			//"055 162 217 158   .   . 143 144 184 002 165 066 119   . 060 003 004 134 005   . 043   . 135 071 207 177 167 082   . 070   . 106 064 200   . 105   .   . 190 137 204 161 093 132 101   . 036 140 078 073   . 122 030   . 019   . 062 089   . 146 221 224 126 188 009 133 178 091   . 153 212 156 053 050   . 047   . 107 157 209 046   . 173 020 115 108 179 201   . 049 041 214 063   .   .   .   . 086 210 223 052 102 057   . 152 096 037   . 014 018 172 127 186 171 189 129 198   . 206 104 007   . 192 160 213 081 181 111   .   .   .   . 026 136 109 121   . 163 197 058 174 059 131   . 044 075 045 094   . 012   . 120 138 141 076 168   . 034 027 024 061 125 040 169 176 117   . 015 032   . 039   . 001 170   . 029 211 016 166   . 068 000 087 182 139 196 180   .   . 103   . 100 031 175   . 142   . 025 220 017 116 114 145   . 164   . 090 199 191 222 195   . 154 097 013 095 112 038 048   .   . 183 148 155 149"
			//"170 015 032 211 166 008 216 117 001 039 124 029 099 016 215 085   . 087 182   . 092 175 180 031 139 193 068 103 100 203 220 090 142 114 017 145   .   . 199 116 222 164 054   . 194 149 218 097 155 112   . 185 013 038   . 195 183   .   . 148 144 066 060 143 119 162 055 011 002 184 158 217 165 023 083 003 071 004 118 005 134 067 043 123 177 207 135 167   . 070   . 101 161   .   . 105 093 130 065 200 088 204 064   . 113 056 062   . 030 159 140 077 146 122 036 205 021   . 078 019 153   . 035 212 156 133 050 091 224 126   .   . 009 221   . 020   . 080 046 098 173 209 115 049 157 179 108 107 047 150 202 086 187 210 214 063 057 102 051 110 152 052 041 010 223 129   .   . 171 014   . 037 104 206   . 172 147 189 186 069 151   . 192 081 026 007 208   .   . 160 213 111 181 079 074 045 059 044 075 131 121 219 197 022 058   . 163 094   . 012 072 027 168 040 076 006 176 125 024 120 138 169 034 141 061"
			//"206 096 171 147 186 037 198   . 104 127 014 172 018 189   . 074 007 192 081   . 084   . 208 181 026   . 151 160 111 213 075 163 045 219 131 197   .   . 094 121 012 058 022   . 033   . 120   . 141   .   . 138 076 125 024 072   . 168 027 034 117 029 215 216 099 015   . 016 039 001 211 032 124 166 008 085   .   . 100 182 087 175 092 000 193   .   .   . 103 203 090 194   .   .   .   . 054   . 220 114 191 222 142 116 017   . 183   . 013 095 097 149 148 185 218 028 038   . 155   . 184 165 083 158 217 162 023   . 066   .   .   .   . 144 002 123 167   .   .   . 043 005 177 070 118 135 207   .   . 071 132 130 093 065 101 161 064 204   . 190 113 088 106 105 200 205 021 056   . 030 077 062 019 078   .   . 089   . 146   . 126   . 035 133 009 153 178   .   . 212 156 091 050 224 053 108 209 157 179   . 020 201 046   . 098   . 080 107 173 150   . 051 063 041 214 057 223   . 110 086 187 010 102 210 152"
			//"179 209 049 098 150 046 173 020 107 115   . 080 108 157 201 214 110 010 202   . 052 041 210 102 057   .   . 063 086   . 206   . 018 069 127 037   . 198 104 129 186 147 171 096 014   . 111 079 208 151   . 213 074 084 007 081 192 160 136 181 121 163   . 174 045 059 075 044 131 094 058 022 033 012 197 076 034 024 120 072 169 040 125 027   . 006   . 176   . 138 029 124 211   .   .   . 032 016 170 215 216 166 099 117 039 182 087   . 085 139   .   . 031   .   . 000 092   . 180 196 199 194 145 164 054 128 222 025 090   .   .   . 142 116 017 028   . 097   . 154   . 195 149 185 218 155 095 048 013 148 162 011 217 055 165 158 119 023 144 184 002 143   . 083 060 123 043 005 070 003 177 134 004 135   . 067 082 118 071   . 200 137 105 132 064 190 065 113   . 161 093 130 204   . 101   . 056 036   .   . 205 073 062 122 089   . 140 019 021 146 133 221 212 224   . 009 126 188 153 091 156 035 050 178 042"
			//"000 139 175 203 085 193 103 182 068   . 180 087 092 196 166   . 114 017   .   . 164 145 116 194 191   . 031 054 199 090 028 183 038 222 218 149   .   . 112 195 013 185 097 095   . 011 184 083 144 148   . 066 055 158 060   . 002 217 162 165 005 134 023   . 043 207 123 004 118 167 070 067 135 003 177   .   . 200 190 064 113 105 161 132 130 088 137 071 093 106 062 078   .   .   . 077 140 019 205 204 089 030 021 056 036   . 042   . 178   . 035 091 053 224 153   . 212   . 221   . 201 179 115 049 080 108 047 107 046 050   .   .   . 209 157 202 150 010 152 187 063 057 086 041 110 051 052 223   .   . 018 104 172 129 206 171 198 014 096 069 189   . 037 127 102 081 160 026 079   . 111 109 151 136   . 192 213 007 074 186   . 059 131 045 174 219 121   .   . 022 163 094 033 197 075 125 006 024 027   . 072 012 061 040 138   .   . 176 168   . 099 015 032 008 170   . 034 211 215 001 029 039 124 117 016"
			//"028 095 097 185 013 149 154   . 112   . 155 183 038 048 222 055 060 002 119   . 158 083   . 165 143   . 148 217 184 066 123 134 043 023 118 177   . 082   . 005   . 070 067 207 135 130   . 105 137 071 088 106 065   . 200 064 113 093 132 101 056 062 204 089 021 159 205 019 036 073 122 140 078 030 077 178   .   . 153 009 042   . 212   . 091 188 221 146   . 224   . 179   .   .   .   . 080 107 020 050 098 047 173 209 157 057 152   . 210 052 010 086 214 041 110 202 063   . 051 223 069 206 127 171 172 018 014 104 037   .   .   .   . 096   . 081   . 192 109 213 160   . 111   . 007 136 084   .   . 074 045 094 163 121 075 022 174 033 059 219 044 058 197 131 181 072 168 006 040 141 120   . 176 027 125 169 138 024   . 012 034 015 039 099   . 215   . 016   . 032 029 001 124 008 170 092 139 196 000   . 182 166   . 175 203   . 087 068 103 085 142 128 054 145 220   . 031   . 114 199 090 017 194 116 025"
			//"116 142 090 054 194 017 164   .   . 025   . 145 191 199 031   . 112 149   .   .   . 183 028 013 038   . 222 185   . 097 144 083   . 148 011 002   . 158 060 162 165 217 066 119 055 118 004 134 123 023   . 067 135 082 167 207 177 070 005 003 132 105 071   . 088 064 137 190 130 200 093 106 065 101 113 078   . 073 019 159 077   . 089 056 036 021 205 204 122 140 035 178   .   .   .   . 224   . 221 146 212 053 188 133   . 108 115   . 179 173 046 157 047 080 107 209 098   . 020 201   . 210 152 187 041 057 214   . 010   .   .   .   . 202 086 096 102 037 127 171 147 018 189   . 104 129 198 069   . 014 026 007 079 136 208 213 084 074 081 151 111   . 192 109 186 059 058 045 163 075 044 131 219 121   . 197 022 094 033 181 012 072 061 006 125 176 027 120   . 138 040 024   . 169 141 216   . 001 117   . 015 034 039 029   .   .   . 215 211   . 139 182 203 087   . 092   .   . 068 196 175 193 085 000 100"
			//"205   . 140   .   . 077 089 056 073 036 078 062 021 019 204 178 126 042 009   . 212 035 221 053 188   .   . 156 153   . 020   . 173 050 157 115   . 098 201 209 047   . 080 108 179 086 110 010 051 150   . 041 210 063 223 057 152 187 202 214   . 037 102 147 198 018 129 104 189 069 171 172   . 014 127 208   . 151 007 026   . 192 160 081 111 084 136 186 213 079 197   . 022   .   .   . 163 094 121 181 058 033 174 059   . 006 061   . 141 125 169 120 076 040 024 072 168   . 027 176   . 170 039 032 029 099 124 001 008   .   .   . 216   . 016 182 166 087 193 203 103 139 100 175   . 000 092 068   . 085 142 199   . 116 220 054 191 194 128 114 025 164 145 017   . 195 154 095 097 155 218 149 112 028   . 183 185 048 013 222 148 162 002   . 143 060 144 011   . 217 066 184 165   . 055   . 207 004   .   . 005 023 177 067 070   . 134 167 082 003 064 132 093 105 065 088 071 161 200 190   .   . 101   . 130"
			//"129 018   . 171 014   . 147 096 069 189 206   . 198 104 102   . 151 109 026   . 160 192 136 074 084 111 186 213 007 079   . 197 174 181 044 131   .   . 219 059 033 022   . 045 075 120 024   . 027   .   .   . 141 168 176 006 061 138 072 076 015 008 034 211 216 099 117 001 016 215 032 029 170 124 039 180   .   . 196 139 193 087   . 182 100 092 000 166 203 175 145 220   .   .   .   . 090 199 116 031 164 194 191 128 025 095 149   . 155 038 183 218   . 097 048 195 154   . 028 112 060 055 002 217 066 119 165 184 083   .   .   .   . 162 011 005 023 134 177 070 082 207   . 067 004 123 043   .   . 003 064 190 106 137 065 093 088 101 132 200 130 161 105 113 071 056 089 159 140 078 036 077 073   .   .   . 122   . 030 204 146 133   . 009 188 126 221   .   . 156 224 153 053 035   . 173 108 107 020 157 209 050 115 080 049   . 046 201 098   . 057 202 187   . 210 052 150 063 223   . 041 152   . 051 086"
			//"121 045   . 022 033 131 058   . 219   .   . 197 174 094 181 141 176 061 006   . 168 169 027 076 125   .   . 138 024 040 117 008 216 034 016   .   .   . 215 015 124 032 029 099   . 100 196 087 000 166   . 175   . 103 068 139   . 203 182 085 128 145 031   . 191 142   . 199 025 114 054 090 220 194 017 155   . 112 048 095 149 183 154 195 218 038 028 222 185   . 083   . 217   .   .   . 066 184 144 148 158 165 143   . 011 207 177   . 135 043 134 118 003 067 004 005 082   . 123 167 200   . 113 093 106 064 101 190 105   .   .   . 088   . 130   . 204 062 077 122 089 159 036 140 019 205 021 073   . 030 009 153 224 221 178 156 188 053   . 126 091   . 035 042 146 209 098 108   . 179 157 115   . 020   . 046 049 107 047 050   . 202 152 057 052 223 051   .   .   . 041 110 214 010 210 198 018 104   .   . 096 102 127 172 171   . 037 069 147 014 026 081 213 192   .   . 186   . 151 007 079 109   . 136 111"
			//"144   . 066 217 165 002   .   . 060 011   . 083 143 184 148 135 167 177   .   . 082 134   . 003 043   . 023   . 004 067 137 105 088 071 130 113   . 161 200 132 101 093 106 064 065   .   . 062 205 204   .   . 078   . 073 159 077 122   . 030 133 035 146 212 188 009 221 153   . 126   . 224 178 053 042 179   . 201 107 108 115 046 098 209   . 173 020 050   . 080   . 210   .   .   .   . 041 110 051 150 063 214   . 202 086   . 127   . 206 198 037 189   . 172 104 096 147   . 129   . 151 208   . 213 079 026 074 007 192   .   .   .   . 081   . 059   . 197 131 022   . 045 044 163 094 121 174 219   . 033 006 024 040 027   . 138   . 076 072 176 120 168 169 061 012 015   . 099 029 170 016   . 215   .   . 008 032 001   .   . 166 182 193 139 092 068 000 100   . 203 175 196 085 087 180 191 142   . 116   . 128 031   . 090 054   .   . 114 164 194 095 195 185 183   . 038 222   .   . 048 097 149 013   . 218"
			//"221 009 224 156 053 042 212   . 126   . 178 035 188 153 146 179 201 115 108   . 098 046 020 047 173   . 050 049 107 080 051 010 052 150 086 152   .   . 223 202 214 187 041 057 210 189 104 037 129 102   .   . 206 147 069   . 127   . 096 014   . 192 186 160 084 026   . 007 111 151 213 079   . 074 109 075   . 219 094   . 131 197 058 059 044 174 121 181 022   .   . 141 138   . 006 061   . 024 027 012 168 076 125 072 120 099 039   . 170 216 008 016 124 029 001 015 211   . 117 215 068 180 193 203 175 139 085 196   . 166 000   . 092 182   .   . 031 145 017 054 164 142 025 090 199   . 191 114   . 194 095 048   . 028 155 185 038 013   . 112 218 154 183 149   . 162 158   . 066   . 011 002 060   .   . 083 217 184 165 148 023 005 177 207 043 167 123   .   . 070 067 004 003 134 135 088 064 190 137   . 132 071 113 106 093   . 105 200 161 101 159 056 122 062 078   . 204   . 073 019 140 077 030 205 036"
			//"136   . 079   . 074 109   . 081 151 111 208 192 084 007 186 075 219 131   .   . 058 197 121 033 174   . 181 022 094 163 027 169 125 012 120   .   .   . 176 072 076 138 040   . 141 016   . 008 117 034   . 029 170 211 215 099 039 032 015 124 182 087 166 103 092   . 000 196   . 068 203 175 180 085 193 220   . 114 199 142 017 145 164   . 025 191 116 031 054 090 183 155 185   .   .   .   . 048 028 222 154 013 038 195 218 119 002   . 055 143 083 011 165 066 184 162 158   . 144 060 167 135 177 070 067 207 003 004   .   .   .   . 043 005 118 132 071 105 113 093 161   . 130 106 190 137 088 200   . 101 159 019 140 205 078 122   . 030 056   . 036 089 062 077 204 133 212 009 224 178 091 042 126 221   . 035 156 153   . 146 050   . 115 108 173 201 020   .   .   . 080 107 047 046 179 052 057 110 051   . 202 150 152 041 187   .   . 223 063 214 018 096 171 037 206 198 102 147   . 104 172   . 014   . 189"
			//"020 108   . 049 047 115 098 209 201 157 179 046 173 107 050 210 223 152 057   . 063 010 051 214 052   . 150 187 110 041 129   . 198 102 189 127   .   . 069 096 014 171   . 018 206 111   . 192   . 186   . 079 208 160 151 026 109 213   . 074 059 197 181   . 174 045 121   .   . 219 022 163 075 033 131 141   .   . 024 006   . 169 168 072 120 125 027 012 138 040 008 170   .   .   .   . 029 001 117 034 211 124 216 015 016 139   .   . 180 092 087 100 085 175 196 182 103   .   . 068 114 220 017 054 090 142 194 199 145   .   .   .   . 128 025 195 222 183 149 185 154 095 218 097   . 028 038   .   . 013 119 184 066 144 055 217   .   . 162 060 011   . 083 002 148 005   . 207 067 135 118 177 167 123   . 134   . 004   . 023 071 132   . 064 088 200 137   .   . 093 106 190 101   . 065 021 159 019 205   . 056 204 077 140 122   . 062 073 089 030 009 133 156 035 178 188 146 212 126 153 224 042   . 221 091"
			//"123 207   . 070 003   . 082   .   .   .   . 134 043   . 023 065   . 113 064   . 161 105 137 101   .   . 071   . 190 106 205 062 021 204 036 077   .   . 073 056 030 122 140 159 078 091 153   . 221 146   . 224   . 212 126 009 042 156 133 053 209 046 050 098 173 108 020 107 157 201 049   . 179 047 115 210   . 223 110 057 152   .   . 202   . 052 051 150 187 041   . 206 171   .   .   . 172 104 129 102 147 014 198 096 189 026   .   . 208 084 192 111 074 079 007 081 160   .   . 151 219 075 131 022 163 045 033 094 197   .   .   . 174 059   . 072 012 169 061 138   . 006   .   . 024 027 125 176   . 076 099 001 029   . 170 032 216 124 015 215 016 211 008 039 034 182 103 139 175 180 100 193   . 000   . 087 203   . 085 166 031 128 017 142 191 114 116   .   . 054 090 199 194 145 220 038 095   . 028   .   . 222 149 097 185   . 183 112   . 013 119   . 217 083   .   .   .   . 060   . 066 002   . 144 011"
			//"117   . 029 032 124 039 211   . 215 016 170   . 216   . 034 180 068 193 139   . 103   . 000   . 092 100 166 203 196 175 116 145   . 031 025 017   .   . 114 128   .   . 090   . 220 218 048   . 028 222   . 097 155   . 112 095 149 185 195   . 162 083   . 158   . 119 144 184 011 060 217   . 055 165   . 135   . 167 004 207 177   . 082 005 118 043 123 023 070 067 105 065 093   .   .   . 106   . 137 071 161 101 088 132 130 159 077   . 078 021 062   . 030   . 019 056 089   . 205 073 126 178 042 156 224 009 053   . 035   .   .   . 188 133 091 209 050 046 115 049 098 108 157   . 107 020 173 201   . 047   . 110 041   . 210 187 052 214 202 223   . 063   . 152 150   . 147 018 172 206 189   . 069 129   . 037 171   . 014 102 186   . 109   .   . 151 136   .   . 213 079 007   . 192 208 174 045 094 121 044 059   . 131   . 022   . 197 219 058 033 006   . 138   . 141 125 012   . 176 024 040 061 076   . 120"
			//"027 006 040 138 076 061 168 072 176 120   . 169 125 024 012 170 215 039 099   . 211 008 117 124 216   . 034 032 001 029 000 087   . 166 100 193   .   . 068 182 085 203 175 139   . 025 199 145 116   .   . 090 220   . 114 142 017 054 128 194 195 183 222 154 038 095 028 048 218   . 185 097 155 013 149 055   .   . 184 119 002 083 158 162 011 143 144 148 217 066 134 135 070   .   .   . 067 004 123 023 082 003 043 005 118 064   .   . 065 088 105 130 101 106 190 132 161   .   . 200 073 078 077 122 140 159 030 019 062   .   .   . 021 056 036 133 146 035 042 156 212 009 091 224 153 221 188   .   . 053 108 107 080 020 179   . 173 047 209 201 157 098 046 115 050 202 063 057 041 210 086   . 223 051   .   . 187 110 214 150   . 096 127 018 198 069 129   .   . 171 172 104   . 037 206 084 026 007 136   . 081 186 109 079 213   . 192 151 160 074 045 059 022 197   . 174 181 058 219 094 163 131 033 121 044"
			//"051 057 041 187 214 152 063   . 223 086   . 010 052 110 150 206 069 127 018   . 147 037 129 014 198 189 102 171 104   .   . 192 084 186 111 109   . 160 151 081 074 213 079 026 208 044 094 197 121   .   . 163 075 058 219 045 131 022 059 033 072 169 012   .   . 006 027 024 120 176 138 040 141   . 061 170   . 215 001   . 039 008 211 015 016 216 117   . 032 029 087 180 203 068   .   . 175 196   . 166 103   . 092 182 100 142 017   .   . 191 145 025 194 090 199 128   .   . 116 114 112 155 149   . 097 095   . 048 183   .   . 154 038 195 218 162 148   . 002 217 158 119 011 066 184   . 143 060   . 165 207   . 067 123 135 070 043 003 005 167   .   . 134 177 023 132 161 064 106 065 130 113 200 137   .   . 093 190 101 071 204 056 077 159 021 073 205 036   . 122 140 019 030 062   .   . 009 153 221 091 133 146 042 224 156   . 035 126 212 053 108 209 049 046   . 173 050   . 201 107 080 115 047 020 157"
			//"137 064 106 093 101 113 161   . 200   .   . 105 088 190 071 078 073 077   .   . 089 062   . 030   .   .   . 122 019 140 221 035 188 146 091 042   . 212 126 133 053 156 224 009 178 157 107 046 020 050   . 080 179 098 201 108   .   . 209 047 202   . 150 063 052 057 051 110   . 223   . 041 210 214 152   . 014 069 104 018 127 037 147 096 189   . 129 102   . 172 192 208 213   .   .   . 079   . 136 186 160 074 084 081 111 045 131   . 075 174 197 044 033 163 094 059 058   . 121 219 176 141 061 138 040 006 076   . 169   .   .   . 125 072 120 015   . 008 039   . 211 099 016 029 001 117 216 215 170   . 139 196 175 000   . 203   . 085 182 068 100 103 087   . 166 128 164   .   . 220 025 017 114 116   . 145 054 199 194 031 222 195 149 095 038 112 028 218   . 185 097 048 013 183 155 143 119 184   .   .   . 148   . 066 217   .   . 060 158 165 207 005 070 134   .   . 023   . 167 004 067 177 003 123 118"
			//"  . 038 183 097 155   . 185   . 222 048   . 149 154 112 194 144 148   . 143   . 217 002 162 055 158   .   . 066 060 083 005 177 082 165 004 118   . 070 023 207 135 067 134 043 123 190 200 113 132 003   . 105 137 093 071   . 130 106 064 065 159 077 101 122 089 021 056 073 019 204 140   .   . 078 036 221 178 146   . 188 091 042 156 009 153 212 133 030 224 035 115 020 080   .   .   . 046 201 209   .   . 179 098   . 107   . 086   . 051 063 152 110 210 010 223 057 187   . 202   . 102   . 189 172   .   . 206 069 127   .   .   . 147 018 104 026 014 109 111 079 213 084 007 192 151 081   . 186 136 208 174 219   .   . 121 163 058 075 045 181 094 022 131 044 074 006 138 125 169   . 024 120 012 072   . 061 040 176 141 033 076 099 016 216 211 034 015 001   . 029 008 215 170 039 117 103 092 068   .   . 139 124 100 087 175   . 193   . 203 180 191 142 090 017   . 164 085   . 031   . 145 025 220 128   ."
			//"015   . 008 029 170 016 032 099 034 001   . 039 211 215   . 000 166 100   .   . 203 193 182 180 103   . 124 175 068 087 128 017   . 085 199 025   .   . 031   .   . 090 145 191 116 048 112 149 195 194   . 183 028 185 222 038 218   .   . 155 119 002 013 217 158 143 162 060 184   . 066   .   . 055 011 123   . 023   . 043   . 177 070 207 004 082 005 165 067 134 113 137 106   .   .   . 105 200 132 003 093 065 161 064 190 021 036   . 205 089 077 019 078 062 073 159 122   . 056 204 146 221 091 224 035 188 178 126 042   .   .   . 212 009 153 108 053 115 157 080 049 173 107 046   . 209   . 050   . 179 052 223   .   . 051   . 063 210 057 150 110 187 152 086 047 018   .   . 037 129 104 189 102 096   . 127 172 069 206 214 014 026 111 084   .   . 081   .   . 079 192 151   . 109 136 058 174 219 059   . 045 074 044 197 163   .   . 181 022 075   . 006 040 061   . 168 033 138 012 176 169 120 141   . 024"
			//"  . 092 087 175 180 100 203   . 166 196 000 193 103 068 124 116 031 025 191   . 054   . 128 220 164   . 085 090 114 145 195 149 154 194   . 218   .   . 222 095   . 097   . 038 028 184 060 002   . 013   . 083 144 217 148 143 011 066 119 055 207 177 165 070   . 043 005 167   . 023 067 134 123 135 118   .   .   . 200 088   . 113 093   . 190 161   .   .   . 105 077 205 140   .   .   . 062 073 056 101   . 078 089 159 019 188 091   . 221 212   . 153 178 035   . 009 156   . 133 146 050 020 157 080   . 173 179 201 115   .   .   . 098 108 107 057   .   .   . 041 187   . 110 010   . 202 063   .   .   . 198 069 037 096 129 172   . 206 018 102   . 171 127 189 214 026 213 084 192 136 007 111 186 081   . 109   . 151 208 014 074 045   . 174   . 181 059   .   . 163   . 219 075 131 121 168 125 176 072   . 006 033 120   . 040   . 061 012 138 141 216 099 029 039 117 211 076   . 034 215 008 016 170 015   ."
			//"202 052 010 041 210 086 187   . 150 110 051 152 063 223 047 129 102   . 198   .   . 127   . 206 147   . 214 172 069 037 081 109 160 014   . 111   . 213   . 026 208 079 192 084 136 094 219 131 059 074   . 197 121 022 181 174 044 163   .   . 006 061 033 138 168 125 072 176 024 012   . 169 027 141   . 117   . 034   . 216   . 039 032   . 001 211   . 076 029 008 193 000 175   .   .   . 087 068 182 124 203 180 103 139 196 191   .   . 116 164 017 199 220 145 114 142 054   .   . 031 222 028 218 097 183 038 155 112 149   .   .   . 154 095 048 119 013 002   . 066 217   . 184 083   . 162   . 148   . 055   . 167 134 005   . 067 082 135 207 023 004 070 177 118 165   .   . 088 105 137 190 130 071 132   . 113 106 200 065 003 101 159 036 021 089 204   . 019   . 140   . 073 078 077 205 212 188 126 133   . 009 030   . 035   .   . 042   . 156 178 173 108 080 115 020 098 053   . 050 201 046 157 179 209 107"
			//"133 188 035 224 178 091 156   . 146 153   .   . 212 126 030 020 050 157   .   . 049 115 209 179 098   . 053 080 201 046 202 152 063 047 110 086   .   . 150 057 210 041 010 052 051 104 069 127 096 214   . 037 129 171   . 198   . 172 018 206 026 109 014 213 160 084 081 151 007 186 079 192 136 208 111 121   . 181 219 174 044 131   . 045 094 058   . 074   .   . 061 027 040 012   .   . 169 176 072 033 138 141   . 006 024 216 016   . 117   . 039   . 170   . 215   . 032   . 015 034 166 000   . 175 087 092 180 068 193   .   . 203 103 139 196   .   . 017   . 090 054 191   . 145 114 128 164 031   . 220 038 112 183 195 028 097 154 155 095 222 048 185 149 218 194 119 217 143   . 144   . 011 148 162   . 002 066 060 055 013 165 207 118 043 082 023 005   .   . 067 134 167 135 177 123 161 088 200 132   . 064 003 130 105 106   .   . 071 093 065 021 159 140   .   . 089 101   . 204 073 062 036 078 056 019"
			//"059 174 197 163 075 044 022   . 181 094 121   . 058 219 074 027   . 120 125   . 138 061 072   . 168   . 033   . 176 169   . 039 211 076 001 016   . 032 034 099 170   . 008 216 117 196 068 193 182 124   . 087 000 203 166 092 100 175 139 180 142 017 085 054 164 191   . 114 199   . 090   . 116 220 025 028   . 222 112   . 218 149 185 095 048 154 195   . 097 183 002 144 066   .   .   . 083 060   . 013 217 055 158 119 184 043 118   . 123   . 177 004 135 134 167   . 070   . 005 023 071 137 130 106 105 088   . 200 113   .   .   . 161 064 190 159 101   . 036 140 122 021 019 062 073   . 089 204   . 078 188 126 035   . 221   . 212 178   . 146 153 156 042 091 030 108 049 173 046 020 107 157 050 209   . 115 080 201 179 053 047 057 086   . 063 150 202 110   . 041 010 223 210 152   . 147 198   . 096   . 018   . 189 037 172   . 127 102   . 206 084 026 079   . 136 160 014   . 186 151 192 111 208 081 007"
			//"072 125 169 040 141 120 138 006 012 024 027 061 168 176 033 117 034 016   .   . 032 039 015 170   .   . 076 029 215 008 182 193 103 124 196 100   .   . 166   . 180 175 087 092 000 199 114 017 128 085   . 145 116 054 031 191 025 090 142 220 095 149 194   . 154 038 195 112 048 222 097 183 028 155 218   .   . 148 060 143 011 002 217 119 184 158 162 013 066 083 177 123 067   .   .   . 134 167 005 165 070 135 082   . 004   . 130   . 137 161   . 190 065 105   . 064 093   . 132   . 204   . 036 140 062 021 078 073 077   .   .   . 089 159 019 009 030 042 091 224 156 188 153 035 126 133 212 146   .   . 173 201 046 209 020 080 098 179 108 050 107   . 115 157 053 057 187 052 010 051 110 086 150 202   . 152 041 223 210 047 214 018 189 198 147   . 096   .   . 172 037 069 206 127 129 160 084 151 081   .   . 014 111 192 079   .   . 186 213 208 174 045 163 131 121 058 074 022 181 219 197 044 075 059 094"
			//"  . 198 037 172 206 189 171 018 102   .   . 127 147 069 214   . 186 111 084   . 213 109 081 208 160 007 014 079 151 192 059 131 058 074 094 044   .   . 181 045 075 163 197 174 121 024 176 061 072 033   . 169   . 138 012   . 120 040 006 141 099   . 076   . 211 216 015 215 001 034 029 008 117 170 016 000   . 166 068 092   . 193 203 139 196 103 182 124 175 087 017 116 090   .   . 025 145 114 128 085 054 220 164 142 199 038 218   . 028 154 149 048   . 183 112 095 185   . 195 222 148 144 011 066 083 143 055 060 002 013   .   . 158 119 184 207 165 177 118 067 070 043 004 134   . 005 082 023   . 135 088 200 105 132 137 106 161 065 064 071 190   . 113   . 003 159 122 021 062   . 019 036   . 056   . 077 140 073 078 101 030 009 091 188 212 146 133   .   . 224 035 126 178 042 221 098 173 201 209 107 108 053 157 046 080   . 115 050 049   . 052 057 041 152   .   . 047 187 150 223 010 086 210 202   ."
			//"056   . 062 140 078 036 122 159   . 019 205 077 089 073 101 221 146 091 188   . 156 042 133 178 212 153 030 224 126 035 209 115 098 053 107 157   . 049 050 108 179 080 046 173   .   . 223   . 202 047   . 010 051   . 150 052 086 041 057 210 018 127 214 171   . 198 096 069 104 102 172 037 129 206 189 136   . 186 151 084   .   . 213 026 007 160 081 014 079 192   . 121 163   .   .   . 197 219 059 074   . 075 058   . 094 125 120   . 027 168 061 024 141 169 176 006 138   . 072 012 034   . 016 029   . 216 170 215 039   .   .   . 211 099   . 139 124 193 100 175 203 092 196   .   . 182 103 166   . 180 191 114 145 128 116 090 164 220 142 031   . 054 017 025 085 095 185 038 183 028 048   . 222 195   . 149 097   . 155   .   . 119 011 143 158 148 162 184   . 066 083 060 055 002 144 082 043 167 005 004 207 165 118 134 067   . 177 023 070 135 088 064 106 113 137 161   . 093 071 200 105 130 065   . 190"
			//"005 043 134 067 135 118   .   . 023 004   . 177 082 167 165 137 071 130 088   . 093 113 132 065 161   . 003 106 200 105 056 077 089 101 019 036   .   . 204 159 078 140 062 021 205 153 126 042 133 030   . 035 221 156 146   . 091 224 009 178 108 115 053   . 098 173 209 201 107   .   . 046 020 179 157 051 210 150 223 052 086 152 187 057 110 063 202 047 041 010   . 129 172   .   . 189 037 069 096 214 171   . 147 018 104   . 111   . 136 160 109 007 208 192 151 026 213   . 081   . 181 121 044   . 197 174 075 219 131 074   .   . 058 045   . 006 033 061 120 040 138 125 024 169 176 072 168 012 027 141 216 215 008 015   .   . 211 170 099 034 001   . 039 016 076 139 203 092 087   . 196 100 166 182   . 193 175 068 180 124 085 142 025 191 164 031 128   .   . 090 145 114 220 017 116 154 038 112 195   . 095 194 218 183 097   . 149 222 185 155 143 119 066 002   . 158 013   .   . 060 083 011 055 162 184"
			//"  . 088   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 156   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 187   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 075   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 203   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 048   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 005   ."
			//"209   .   . 080 179 157 049   .   . 107   . 115 098 201 053 051 150 086 052   . 187   . 202 210 063   . 047 041 223 010 096 127 147 214 104 189   .   . 102 018 206   . 037 198 129 007 151 109   . 014   . 192 136 213 186 084   . 079 026 208 045 131 074 022 058 174 059 219 094 181 163 197 121 075 044 027 141 012 176 125   . 061 138 006 024 168 072 033 040 169 039   . 029   .   .   . 008 215 015 076 032   . 211   . 001 092 100   . 000 103 193 196 180 087 068 139 203   . 182 166 031   . 025   . 145 191 220 114 017   .   .   . 164   . 199 095 194 149 218 097 185 038 048 183   . 195 154 222 028 155 143 060 083 162 144 066 158 055 119 148 184 217 002 011 013 207 070 043   . 123 004 118 023 005   . 177   . 167 135 165 003 064 130   . 161 071 132   .   . 106 105 200 065 113 137 089 021 073 056   . 159 101 036   . 140   . 077 204 122 078 188 009 224 042   . 212   .   . 146 126 035 091   .   . 153"
			//"162 143 083 066 055   . 217   . 148   . 144 002   . 060 013 123 023 118 043   . 070 177 005 135 082   . 165 067 167 134 132   . 161 003 190 130   . 093   . 064 065 106 105 088 137 019 073 077 056 101   . 062 205 122 204 021 036 140 159 078   . 042 030 156 212 188 133 126 153 146 224 035 221 178 091 020   .   . 201 173 157 115 049 108 107 098 209 053 080 046 152 051 041   .   .   . 010 223 202 047   . 210 063 057 110 198 189   . 129 147 127   . 206   . 069 018 171   . 096 102 186 136 111 079   . 084 208 151 109   .   .   . 160 026 007 045 074 131 044 163 022 174 094 197 219 059 058   .   . 075 125 176 169 072 027 040 168 141 006 012 024 138 061 120   . 099 032 216 008 117 001 016 034 015   . 039 029 215 170 076 124 139 100 092 103 166   . 196   . 175 087 068 180   . 000 164 191 114 128   . 142 085 025 145 090   . 017 031 054 220 038 095   . 149 028   . 194   . 222   . 183 218 155 195 048"
			//"081 084 192 079 208 111 213 026 186 007 136 109 160 151 014 121 181 044 174   . 022 131 059 075 058 094 074 163 219   . 072 061 168   . 024 120   .   . 012 006 141 040 169 125 027 001 215 039 015   . 211 008 117 032 034   . 016 029 099 170 139   .   . 203 103 092 182 068 196 166   .   . 000 180 100   .   . 031 114 191 025   .   .   .   . 164 128 085 090 145 149 028 097   .   .   . 183 112 195 194 185 155 154   . 048 143 011   . 144   . 002 184 055 083 060   . 217   . 162 148 023   . 118 067 134 043 135 167 177   .   .   . 082 207 004 064 003 113 130 106   .   .   .   . 200 132 161 071   .   . 021 073 062   .   . 140 089 078 159 204 019 122   .   . 101 009 156 188 035   . 153 091 146 133 212   . 224 126 178 030 053 108 157 173 098 050 209   .   . 080 046   . 179 115 020   . 052 223 202 110 057 047 086 010 041   . 152 150 187 210 198 018 172 127 129 147 214 171 102 069 037 189 206 096 104"
			//"128 191 145 090 220   . 054 142 031 199 116   . 164   . 085   . 222 218 038   . 185 149 195 155 154   . 194 097 112 183 162 002 158 013 184   .   .   . 148 119 055 066 083   . 144   . 167 177 005 165   . 134 123 070 023 043 118 067 207 135 064 113 003   . 161 088 132 200 190 071 106 105 137 065 130   .   . 204 073 021 036   . 122 159 019 089 056 101 140 062   . 221 224   .   . 091 035 126   . 030 156 178 212 009 153 173 157   .   . 098 115 107   . 046 201 108   .   . 209 050 150 051 086 041 010 052   . 223 152 047   .   . 063 057   . 018 214 127 189 172 171 198 104   . 069 096 147 102   .   . 084 151 192 081 136 079 160 208 026 186 007   . 109 111 014 045 022 174 197 121 094 044 181 059   . 131 163 219 075   . 033   . 120 125 168 012 072   .   .   . 169 176 141 061 027 211 216 215 015   . 099 076 016 008 029   . 039 034 032   . 092   . 175   . 000 103 124 203 166   . 087 100 180 182 196"
			//"026 160 109   . 136 007 079   . 014   . 081 111 213 186 206 059   . 094 058   . 163 044 045 121 022   . 208 197 181 131 006   . 138 075 176 024   .   . 033 125 027 169 061 168   . 215 034 016 099 141   . 039 015 029 076 211 001 008   . 117 092 100 170 175 203 103   . 166 068 124 087   .   . 000 196 128   . 085 031 164 199 025 090   . 114 054 142 180 145 017 218 195 183   .   . 048 149 222 095 220 097 028 185 038 112 158   .   . 162 217 011 060 144 002 148 143 066   .   . 013 165 005 004 134 177 082 123 023 118 055   .   . 070 043 167 088 135 130 190 105 106   . 200 113 071 064 093 003   . 137 089 204   .   . 056 062 122 205   . 101 073 140 036 019 065 188   . 212 042 133 126 153 030 009   . 091 035 146 221 078   . 173 107 098 049 053 108   .   . 046 115 050 020   . 209 187 063 150 057   . 052 179 110 152 010   . 086 047   . 051 147 198 037 189 096   . 210   . 214 102 127   . 129 018 069"
			//"064 161   . 105 137 190 106 088 003 200   . 130 093 071 135 056 101   . 089   . 140 036 159 205 122   .   . 062 204 077 009 091 156 078   . 153   .   . 030 188 221 035 042 212 133 201   . 157   . 178   . 115 209 080 053 098 107 046 173 020 052   .   . 041 187 063   . 150 223 047 010 152 202 051 110 096   . 214   . 147   . 189 172 198 069 171 018 210 037 127 111 081 192   .   .   . 109 186 026 206 079   .   . 084 151 058 094   . 059 022 044 219 121 131 181 174 163   . 045 074 033 072   .   . 061 168 027 012 120   .   .   . 138 125 176 216 141 016 001 008 029 211 215 039   . 099   . 076   . 117 103 166 193 139 182 087 203 000   . 124 068 175   .   . 170 191 090 164 017 128 114 199 085 142   . 025   . 031   . 180 220 038 048 154 185 194 095   .   . 183   . 222 028 218 195 217 158 148   .   . 143 155 184 002 083   . 011   . 066 144 082 043 134 118   . 070 055 067 165 023 177 004   . 207 167"
			//"  . 154 149 183 028 048 097   . 194 112   .   . 185 222 220 162   . 184 158   . 066 011 119 144 217   . 155 083 148   .   .   . 070 055   . 004   .   . 165 043 123 134 177   . 005 200 071 130 064 135 093 113 132   . 003 161 190   . 088 137 021 036 065   . 122 089 159 204 073 101 062 077 056 205 019   .   . 030 146 212 153 091 224 188 126 156 009 078 035 042 157   .   .   .   .   . 115 050   .   .   . 020 049 173   . 063 110   . 202 187 086 223 051 152 150 052 041   . 057 047   . 096 104 037   .   .   . 102 189   .   .   .   .   . 069 084 206 111 007 192 079 160 151 109 186 026 213 014   .   . 058 181 131 045 059 197 022 121 174 074 219   . 044 094 208 125 040   . 061 072 176   . 033 006 138 120 169 012 027 075 141   . 001 211 032 076 099   .   . 008   . 034 117   .   .   . 103 166 139   . 092 170 196 193 087   . 100 124   . 000 164 191 145   .   . 054 180   . 085 031 017 199 116 142   ."
			//"159 089 077 062 205 019 140   . 101 073   . 036 122 204 065   . 030   . 212   . 224 091 009 221 156 126 078 035 146 042 108   . 049 178 201 107   . 080 053 173 020 046 115 098 209 223   . 086 057 179   . 152 202 041 047 063 110 010 052 051   . 189 210 172   . 147 018   . 069 214 037   . 096 129 104 081   . 014 186 160   . 111 079 084 151 213   . 206 192 109 044 059 197   .   .   . 131 181 045 208 163 121   . 174 219   . 024   . 072 138 120 176 027 061 012 125 040   . 006   . 076 015   . 008 039 211 117 034 016   .   .   . 032 216 215 092 170 100   . 087 175 103 068 193   . 139 203 124   . 000 164 031 017   . 128 145 054   . 191 085   . 090 025 199   . 038 097 154 149 195 112 048 194 095   . 218 183 222   . 220 155 143 184 158 217 013 119 060   . 083 002 148 144   . 162 070 082 023 207 167 043 055 004 177 134   . 118   . 067   . 161 088 105 130   . 093 135   . 003 071 113 190 137 064 200"
			//"108 098 115 046 020 107 080 173 053 201 209 157 049 050 178 202 047 110 063   . 041 086 057 051   . 223 179 010   . 152 018 189 171 210 069 104   . 172 214 198   . 037 127 147 096 151 186   . 026 206   . 109 081 079 014 160 007 192   . 136 174 044 208   . 022 058 045 181 219 074 197 131 059 121 094 072   .   . 012 168 024 120 040   . 176 138   . 075   . 061 016 015 008   .   .   . 039 034 099 141 029 117 032 216 215 103 196   . 182 203 100 068 000 193 166 092 175   . 139 124 085 128 199 145 017 164 116 031 025   .   .   . 054 191 114 038   . 218   . 183 097   . 112 149 222 095 185   .   . 028 158 148 002 119 162 083 217 144 143 013 060   . 011 184 155 043   . 082 177 005 167 004 165 207   . 118 134   . 123 055 135 088 190 161   . 003 064 200   . 105 113 071 137 130 132 122   . 204 159 073   . 065 019 077 062   . 036 101 140 205 212 188 035 091 133 156 078 224 030 146 042 153 221 009 126"
			//"057 063 152 010 051 110 041   . 047 223 202 086 187 150   . 096 214   . 147   . 172 189 018   . 171   .   . 037 102 127 026 111   . 206 151 007   .   . 014 084 136   . 109   . 081 219 181 044 045 208   . 131   . 163 074   . 094 197 174 121 125 120 075 040 138 168 006 012 176 033 169 061 072 027 024 015   . 076 034 211 001 016 029 216 215 032 099 141 008 039 100   . 087   .   .   . 193   . 139 170 175   . 203 092 068   . 199   . 128 054 025 114 116 017 031 191 090   . 142   . 194 195 048   . 149 154 028   . 218   .   .   . 185   . 112 143 155 011 184 083 066 158 060 002 148 119 217 013   . 144 082 023 177 207 005 134 070 123 043 165 167 067 118 004 055 088 106 161 113   . 200 190   . 064   . 130 105 071 137 135 065   . 019   . 122 101 159   .   . 062 077 204   . 036 056 156 212 146   .   . 188   . 153 042 035   . 091   . 224 221   . 173 046 157 209 049 178   . 053 050 115 107 020 108 201"
			//"018 147 127   . 129   . 172 198 214 069   . 189 171 102 210 081   . 007 160   . 079 111 026 136 213 151 206 192 186   . 045 044 022 208 219 094   .   . 074 174 121 197 131 058   . 176 012 120 006 075   . 061 072 040 033 168 024 169 125 027   . 016 141 029 032 211 099 034 215   .   . 039 015 117 001 182   .   . 166 103 196 100 175 092 068 203 139 170 087   . 025 128 145   .   .   . 017 031 142 180 090 116 054 191 114 154 048   .   . 185 218 112 028 149 222 038   .   . 095 194 013 162 184 083 002 158 144 148 011   .   .   . 217 143 060   . 055 118 004 134 067 082 167 177 023 207 070   .   . 123 161 071 113 064   .   . 093 137 088 003 200 106 130 190   . 021 140 089 077 056 073 019 101 159   . 036 062 204 205 065   . 188 153 212 156 030 009   .   . 035 042 146 221 091 133   . 098 050 108 201 173 178 107 115 046   . 157 053   . 020 063 052 010 086   . 187 179 041 047   . 152   . 051 057 223"
			//"139 103 193 087 000 196 175   . 124 068 182 100   . 166 170 128 085 199 164   . 090   . 142   . 054   . 180 145 031   . 095 218 185   . 112 048   . 097 194 038 028 183   . 154 195 060 148 011 119 155   . 002 162 066 013 158 184 083 143 144 043 118 055 067 070 082 207   . 167 165 134 177 005   . 004 132   . 003 071 161 190 130 106 088 200   . 064 135 105 113 036 056 062   .   .   . 077 204 159   . 140 205 122 021   . 212 153   .   . 156 091 126 221 042 146 188   .   . 009 030   . 209 107 046 115   . 020 050 157   .   .   . 049 173 201 052 179 086 110   . 041 063 223 152 150 057 187 047   . 051 147   . 127 018 096 037 171   . 198 214 069 172 189 104 210 084 079 160 109 081 151 007 014 026   . 111 192 186 136 206 208 174   . 058 022 074 045 219   . 197 131   . 121 044 059   . 168 012 006   . 125   . 024   . 169   . 120 033 040 027 211 216   . 016 015 032 141   . 076 034 039 001 117 099 215"
			//"119 158 002 083 144 184 066 143 013 060   .   . 217 148   . 005 165 004 082   . 067   . 207 123 070   . 055 134 023   . 064 130 093 135   . 190   .   . 003 088 137   . 113 161 132   . 204 036 159 065   . 077 056 140 101 089   . 062 021 205 188 091 078   . 156 212   . 146 126 030 035 042 133 221 153 209   . 053 050 098 107   . 080 173 201   . 108 178 046 115   . 202 010   .   . 110 152 150   . 179   .   . 187 052   . 147 104   . 096 171   . 069 129 127   . 198 172   . 018 214   . 081 007   .   . 160   . 186 111 206   .   . 213 084   . 174 208 044 094   . 163 058 219   . 181 045 022 074   . 121 168 012 061 006 072 169 138 027   . 033 176   . 120 024 075 216 029 211   . 015 215 001 076 099   . 016 008 034 117   . 170 092 196   . 203 124 139   .   . 087   . 166 000 100 182   . 164 031 142   . 191 180 199   . 145   . 025 085 090 116   . 038 183   .   . 185 220 097 194 222 149 048 028 095 112"
			//"009   . 042 035 221 153 224   .   . 126   . 091   . 146 078 209   . 107 098   .   . 157 108 020 049   . 178 046 050 115 057 086 187 179 223 110   . 041 047 052 051 010 152 063 202 069 102 189 018 210   . 127 096 172 214 147 104 037 198 129 084 111 206   . 213 160 026 186   . 014 192 109 081 136 007 059   . 074 181 058 094   . 163 174 219   . 045 208 197 131 120 072 169   .   .   . 061 012 006 075   . 027 138 125 176 211 001   . 015 032 016 215 117 039 034 216 029   . 099 076 124 182 196 087   . 103 000 166 100   .   .   . 203 092 068 191 180 025 199   . 090 164 114   . 031 142 054 085   . 116 154 222 149 095 195 183   . 028 038 194 112   . 218 048 220 143 066 158 002 162 060 184 013 119   . 011 083 148 144 155 055 043 004 082 070 165 207 167   . 134 177 023 123 118 005 093 161 071 064   . 088 135 190 113   .   . 130 003   . 137 089 021   . 036   . 122   .   . 101 204 077 019 205   . 073"
			//"099 211 039 008 117 001 029 216 076 215 015 016 032 034 141   . 124 196 103   . 175 100 139 000 203   . 170 087 166 193 142 025 054 180 114 199   .   . 085 191 116 145 017 164 128 112 222 218 095 220   . 149 195 097 194   .   . 183 038 028 143 011 155   . 217 158   . 148 060 013 083 002 162 144   . 005 123   . 023 082 004 118 067 043 167 070 207 055 134 177 130 132 105   .   .   . 113 071 064 135 106 137 093 088 200 089 019   .   . 122   .   . 205   .   . 021   .   . 159 101 030 133 153 035 042 212 221 146 091   .   .   . 156 188 126 173 178 157 107 046 080 098 201 115 050 108 049   . 209 020   . 150 152 057 202 010 187 051   . 047 223   . 086 110 179 198 172 147   .   . 069 104 214 018   . 189 037 102 129 210 206 084 007 160 213 014 026   .   . 192 109 186 136 111 081 022 058 181 045   . 174 208 094 131 197   . 044 074 163   . 168 125 169 120 072 138 075 040 033 012 061 024 027 006 176"
			//"006 168 061 169 027 024 040   .   . 176 072 120 138 012 075 015 076   .   .   . 029 016 099 117   .   . 141 008 034 039 139 100 203 170 068 196   .   . 124 092 000 087 193 103 182 114 031 025 142 180   .   . 128 090   . 164 199 145 191 116 038 218 220 097 185 154 095 222 112 194 183 149 195 028 048 162   . 013 148 158 184 011 066 143   . 217 119 155 083 002 118 005 134   .   .   . 177   . 207 055 067 123   .   . 167 161 190   . 132 093 130 200 137 113 071 088 106   . 064 003 101   .   . 062 077 089 205   . 036   .   .   . 122 021 073 188 078 091 153 035   . 212 126 042 146 009 156 030   . 221 098 050 115 108 209 046 049 020 173 053 201 080 157 107 178 052 041 063 152 202   . 110 047   .   . 086 010 150 051 179 210 198 104 147 171 214 018   .   . 037 127 102 129 189 096 213 160 186 026   .   . 206 007 109 192   .   .   . 079 136 058 174 197 044 059 022   .   . 074 181 131 094 121 045 219"
			//"207 082 177 134 123 004 067   . 165 167 005 118 070 023 055 132   . 190 161   . 106 130 064 137 093 200   . 105 071 113 159 036 122 065   . 019   . 140 101   . 205 062   . 089 056 126 146 091 009 078   . 042 133 224 030 212 153 035 188 221 173 157 178 080 049 098 108   . 201 053 046 115 209   . 107 202   . 047 150 063 110 086 041 052 223   .   . 179 010 152 189   .   .   .   .   . 127 102 018 210 172 129 171 198 069 160 007   . 081 213 111   .   .   . 186 084 079   . 026 014 074 059 094 197 131 058 121 181 044   .   .   .   .   . 219 125 075 120   .   . 040 168 176 061 012 006 138 033   . 027 211   . 039 099 015 008 032   . 216 076 215 029 016 001 141 092 175 103 193 182 068 196 124 139   . 100 087 166 000 170 180 191   . 164 054   . 142 114   . 145   . 031 116 025 128 185 154 222   . 112 038 220 048 149 183   . 218 194   . 028 158 143 083 011 162 217 155   . 013 148 002 184 144 119 060"
			//"045   . 131 197 121 094 163   .   . 219 059 044 022 181   . 072   .   . 168   . 040 120 006 027 138   . 075 169 012 061 099 016 032 141 215 001   .   . 076 216 117   . 039   . 015 068   . 100   . 170   . 193 182   . 124   . 196 087 092 000   . 025 180 090 054 164 142 031   . 085 145 017 128 116 199 195   . 194 222   .   . 218 097   .   . 185   .   . 183   . 011 162 083 013   .   . 002 148   . 155 066 144 217 143 060 082 004   . 005 070 118 167 123 177 023 043 067   . 207 165 003 132 190 105 113 161   . 071 130   .   . 106 093 088 200   . 065   .   . 062   .   . 073 077   .   . 122 101   . 205 212 146 042 009 133 035   . 221 188 030 126 224 091 153   . 173 080 098 115   . 201   . 053 108   . 157   . 050   . 178 179   . 110   . 187 047 057   .   . 010 152 150 051 086 202 171 147 102 018   . 198 210 104 127 037   . 189   .   . 129   . 084 192 111 081 213   .   . 014 186 109 007 136   . 151"
			//"142 164 017 145   .   . 090 191   . 114   . 025 054 031 180 195 194 048 154   .   . 218 095   . 185   . 220   . 222 149 119 011 217 155 060 184   .   .   . 143 144 083 002 158 162 167 023 118 207 055   . 177 005 067 165 082 004 134 043 123 088 130 135 106 093 161 064 071 200 003 105 113 132 137 190 056   . 101 204 089 019 036 140   . 073 122 159 065 062 077 091 133 035   .   .   . 042 146 009 078 224 221 156 188 126 098 107   . 209 049 157 201 020 115 050 173 080   . 108 053 047 202 110 010 152 063 051 150 086   .   .   . 187 052 223 198 210 189 104 037 172   . 069 127 102 018 171 214   . 129 160 186 109 026 081 192 213 136 084 014 151 079 111 007 206 174 163 058 131 059 219 094 074 045   . 044 197 181 121 208 075 125 024 168 138 033   .   .   . 169 061 012 027 120 072 032 211   . 099   . 216   . 001 039   .   . 016 076 029 117 103 092 087 100   . 203   . 175 124   .   . 196 000 139 068"
			//"191 054   . 017   . 114 145   . 180 031   .   . 090 085 000 095   . 112 185   . 183 048   . 195 097   . 116 149 194   . 143   . 066 028 148 060   .   . 155 158 162 002 011 217 119 023 165 004 043 144   . 118 207 134 055 070 167   .   . 005 161 190 123 105 106 093 088 003 071 135 113 130 064 132 200 159   . 065 101 122 073 019 062 089 204 140   . 137 077 036 153 009 042   .   .   . 091 030 188 205 035 133 224   . 146 049   .   . 108   .   . 050 209 157   .   . 046   .   . 178 179   . 223 152 086 187 202 047 110   .   .   . 041 063 150 147 051 104   . 127 037 171 102 189 214 198 172 210   . 096 213 014 111 084 026 109 079 081 160 206 186 192 007 151 129 058   .   . 044 045 181 219 208 174   . 094 131 074 059 136 121 168 176 138 040 075 125   .   . 061 120 033 072   . 006   . 032 076 216   . 211 027   . 016 039   . 001 141   . 015 203 103 193   .   . 175 117   . 170 124   . 068   . 092 166"
			//"052 187 086 152 202 223 010 063 179 150 057   . 041 047 020 018 210 069 171   . 037 104 198   . 172   . 051 127 214 189 084 007 079 129 186 151   .   . 206 160 081   . 111 213 026   . 074 094 174   .   . 044 045 197 208   . 219 131   .   . 168 024 121 169 040 138 125 033 012 075 061 120 006 072   . 099   . 141 076   . 215 001 008 211 034   . 216 027 039 016   . 139 193   .   .   .   . 124 092   . 087   . 175 103 166   . 114   . 142 090 199 031 128 025 085 164 145   . 191   . 220 095 112   . 218   . 195 194   .   .   .   . 097 154   . 158 028 184 060   . 083 217 148 011 013   . 066 155   . 162   . 165 118 043 207 177 067 005 082 055 023 134 004 167 144   .   . 093 130   . 071 200 135 088   .   . 113 003 132   . 137 089 073   . 140 065 021   .   . 077 036 101 056 019 159 224 156 030 188   . 212   . 126 091 042   . 153 078 035 133 049 098 115   . 108 080 221 046 178 053 157 201 209 173 050"
			//"174 022   . 131 059 219 197   . 208 181   . 094 163 074 136   . 075 176 138   . 169 024 125 072 040   . 121 061   . 120 216 001 029 027 034 215   . 008 141 211 015 039   . 032 099 166 124 196 092 117   . 100 139 087 170 203 068   . 103   . 164   .   .   . 090 054 191 085 031 180 017 025 142 128   . 095   .   . 194 185 112   . 183 154 222 097 038 116 149 218 184 119 002   .   .   . 011 013 143 028 083 162 066 158 148 070 167   . 207 067 004 023   . 118 165 082 134   . 043 055 135 064 200 113 130 093 132 003 190   .   .   . 106 161 071 089 137 019 073 077 062 122 204   . 101 021 140   .   . 056   . 030 091 188 009 042 224 133 212 078 146   .   .   . 205   . 046   . 157 108 050 201 178 173   . 107 115 053 209 221 020 063   . 187 041 179 052 150   . 152 086 047 202 110 057 172   . 214 198   . 147 051 069 189 127   . 104 210 037   . 213 160 109 007   . 079 129   . 206 014 111 151   . 084 186"
			//"021 122 036 077 056 073 062   . 065 204 159   . 140 101 137 009 078 126 156   . 035 153 188 133 224   . 205 042 030 091 173 107 080 221 050   .   . 046 178 098 209 115 157 049 108 150 047 110 052 020   . 086 057 010 179 187 223   .   . 202 147 104 051 037 172 171 198 214   . 210 127 189 018 096 069   .   . 206 014 213 151 007 192 160 186 079 084 129 109   .   .   . 131   .   .   . 044 074 174   .   . 059 163 058   . 138 176   . 006 040 024 012   . 120 033 168 169   . 125 075   . 099 215 039   .   . 015 076 001   .   .   . 029   .   .   . 117 196 068 193 087 203 166 100 124 092 175 170   .   . 054 085 025 191 142 017   . 128 164 180 031 145 199 114 000 154   .   . 218 095 222 112 220 038   . 048 149 194 195 116 028 158 060 217 066 155 143 148   .   . 011 013 162 184 119 067 070 165 043   . 082 144 167 118 177   . 004 055 134 005 093 161 113   . 064 106 123   . 135 003 130 200 132 088 071"
			//"188   . 091 042 133 126 035 212 078   . 009 153 224 030 205 108 178 201 049   . 046 107 173 209 080 050   . 115 053 157   . 110 041 020 150 223   .   . 179 063 202 152   . 187 057   . 214   . 198   .   . 189 018 037 210 171 069   .   . 096 160 007 129   .   . 213 084 014 186 206 109 111 026 081 151 045   . 208   .   . 219   . 197 058 181 163 174   . 131 044 024   . 061 075   .   . 120 033 125   . 169 072 040 168 012 032 215   . 099 029 001   .   .   . 076 211 008   . 216 141 170 139 068 193 100   . 182 124 196   .   . 087 175   . 166 164 000   . 114 017 145 054 031   . 085   .   . 180   . 128 185 194 218 038 095 149 097 195 154 220   .   . 048 112 116 158   .   . 011 119 148 060 155 143   .   . 002   . 162   . 144 082   . 070 067 055 043   .   . 177 118 165 005 004   . 106 093 003   . 071 161 123 200 130 113   . 190 135 105 132 122 089 077 019 159   . 137 062 065 101 036 073 056   . 204"
			//"  .   . 011 002 162   . 083 158   . 148   . 184 066 013 028 207 055 167 070   . 134 004 043 005   . 023 144   . 165 118 088 190 106 123 071 200   . 105 135   . 132 113 130 093 064 204 101   . 021 137   . 036 159 062 065 122 073 077 089 056 212 153 205   . 224 156 188 030   .   . 042 091   . 133 126 108 209 178 053 049 201   . 046 098 050 080 173 221 115 157 110 057 152   .   .   . 086 047 052   .   . 202 041 063   .   . 069   .   . 172 104 102   . 189 214 147   .   . 198   .   . 026 151 109   .   . 081 014 007   .   .   . 079 160 186 058 136 094 219 131 197 022 181   . 074 174 163 208 045 059 138 033   . 125 006   .   . 072 168 075 012   . 024 176 121 211 008 032 016 099 034 215 141 216   . 001 039   . 015 027 117 103 068 203 175   . 092 166   . 193 100 124 182 196 139 090 054   . 191 031   . 000 114 025 017   . 199 180 145 128 185 154 149 048   . 097   . 183 220   . 218 112 195   .   ."
			//"092 203 100 193   . 068 087   . 170 166 139 196 175 124 117 142 180 114 054   . 145   . 191 128 090   . 000 017 085 025 038 048 097 116 222 112   .   . 220 154   . 149 218   . 095 148 013 184   . 028   . 011 119 083 155 217 060 002 158   . 082 004   . 134 067 070 043 165   . 055 177 118 207 005 167 064   . 135 003 093   . 190 105 161 071 106 088 123 113 130   . 159 077   .   .   . 036 101 021   . 062 056 140   . 204   . 126   . 009 224   . 146   . 091   . 212 035   . 188   . 178   . 201 115 157   . 209 053 107   .   .   . 080 098   . 063 020 110 223 152 010 187 150 086   . 052 041 179   . 202 171 214 189 198 018 127   . 096 147 210 102 037   . 069 051   . 192 213 111 026 186 151 206 084   . 007   . 014 081 129 136   . 219 022   . 208 174   .   . 131 044 074 059 094 045 040 138 033 125   . 168 121 176   . 061   . 024 075 169 072 032 211 039 001 099 029 027   . 141 076   . 215 015 216 034"
			//"038 185 218   . 195 112 183   . 220 222 095 048 097 194 116 119 155 060 217   . 083 184   . 162 066 148 028 002 013   . 043 004 067 144 023 167   .   . 055 082 005 177 118   . 207 071 003   . 088 123   . 130 064 105 135 093 200 113 161 132 089 019 137 062 140 122 021 101 204 065 077 036 159 056 073 009   . 078   . 156 126 153 035 212 146 224 188 205 042 091 107 108 115   .   .   .   . 053 173 221 046 209 080 098 050 187 223   . 057 041 110 150   . 086 047 063 010   . 052 179 210 018 069 127 189 171 096 214   .   .   .   . 172 147 102 160 129 007 151 109 192 213 186 111 014 084   . 206   . 081 022 074 044 174 045 131 163 059 058 208 181 197 094 219 136 168 169 138 120 006 012 176 075 125   . 024 061   . 072 121 027   . 215 032 029 141 216   .   . 039 016 076 015 001 099   . 203 124 092 166 103 117   . 100 193   . 196 170 087 182 054 164 017 199 142 090 000   . 180 085 025   . 128 191 031"
			//"043 070 118 177 005 167 134   . 055   .   . 004 067   . 144   . 135 200 093   . 105 190   . 132 106 071 123 113 003 130 021 019 140 137 204 073   .   . 065   . 056   . 036   . 159 146   . 153   . 205   . 091 009 035 078 156 126 042 212 133 098   .   .   . 080 049 173 053 050 178   . 157 108 209 201 057   .   . 047 187 223 110 010   . 150 041 052 020 152 086   . 018 127   . 171   . 189 214 198 051 037 096 172 147 102 213 151   . 026 079 007 186   . 111 014 160 192   . 084 206 208 045 219 131 044 022 059 074 094   . 174   . 163 058   . 168 121 024 176 061 169   . 012 120 033 125 040   .   . 072 032 076 016 216   . 039 029 015 211 141 034   .   .   . 027 103 087 203 100 139 166 068 170 092   . 196   . 124   . 117 000   . 114   . 090   . 191   .   . 017 025 085 128 199 142 097 185 194 038 222 154 116   . 218 149   . 048 220 183   . 217   . 002 184   .   . 028   . 155 013 011 060 162 143 148"
			//"173 049 157 115 209 201 046   . 178 050 108   . 080 053   . 057 179 223 187   . 010 110 052 202 041   . 020 152 047 086 198 104 172 051   . 069   .   . 210 147 096 127 189 171 018 186 014 007 084 129   . 111 026 192 206 213 151 109 160 081 058 094 136 197 163 022 174 074 181 208 131 044   . 059 219 006   . 075 033 138 176 024 169 168 012   . 125   . 061   . 001 099 039   .   .   . 016 076   . 027 008 015 029   . 034 203 068   . 139 175 196   .   .   . 124 103 087   . 092 170 180   . 114 017 025 054   . 085 199   .   .   . 090 164 031   . 116   . 112   . 183 185 222 218 194 038 097 220   . 195 217 013   . 143 119 002 066 162 158 155 148 083 184 060 028 082 134 070 118 207 023 167 055 043   . 004 177 165 005 144 123 161 200 093 106 135 088   .   . 113   . 003 132 190 064 140 122 101 021   . 089 137 073 036 077   . 019 065 062 056   . 212 042   . 009 224 205   . 078 030 091 126 133 188 146"
			//"198 171 189 127 096 069 037 147 210 102 018 104 172   . 051 026 206 151 213   . 192 007 084   . 079   . 129 109 014 111   . 094 163 136 181 219   . 197 208 058 059 131 044 022 045 012 033 024 125 121   . 120 006 169 075 138 176 061 168 072   . 001 027 008 029   . 216 076 034   . 039 016 099 015 215 139   .   . 124 203 068 196   . 103 166 175 092 117 193 100 199 142 017   .   .   . 025 085   . 000 145   .   . 164 031 185 112   .   . 097   . 222   . 218   . 154   .   . 038 220 155 119   .   . 011 217   . 013 184   .   .   . 066 158 148 082 144 004 167 177 134 070   . 118 165 043 067   .   . 005 093 003 130 088 064   . 106 132 161   . 071 105 190 200   . 089 062 122 036 159 204 073 065 021   . 019 077 101 056 137 205 212 126 156 224 078 188 146   . 042 091 030 133 153   . 080 049 053 173   . 098   . 201 157 115   . 107 178 046 209 187   . 152 110 057 041 020 010 179 047 086 223 202 052 150"
			//"084 213 111 109 081   . 192   . 206 186 026 007 079 014 129 045   . 219 022   .   .   . 174 059 163   . 136 131 074 044 125 024 040 121 012 176   .   . 075 168   . 061 120 138 006   . 076 001 216 027 029 016 099   . 141 032   . 039 211 015 103 196   .   .   .   . 092 124   .   . 193 100 139 182   . 142   . 180 085 054 114 199 145   . 031 090 191   .   . 025 048 095 149   .   . 112 218 194 038 116 183 195 097 154 222 217   .   . 119 066 184 148   . 011 013 158 083   .   . 155 055 207 167 177 118 070 005 165 004 144   .   . 067 082 023 161   .   . 200 113 105   . 071 130 003 088 106 135   . 132   . 101 036 021 159   .   . 056 089   .   .   .   . 073 137 212 035 156   . 009 146   . 078 188 224 153 042 030 133   . 221 098 201 049   . 178 173   .   . 115 157 053 209 107 108 041 187 047 052   . 063 020 223   .   .   . 110 179   . 202 171 147 127 104 018 172 051   . 210   . 189 069 096 198 102"
			//"125 138 120 061 072 176 169 168 075 012 006 024 040 033 121 099 141   .   .   . 008   . 216 015 029   . 027 039 076 016 092 196   . 117 166 068   .   .   . 103 182 193 100 203   . 031 085 199 191   .   . 025 142 145 180   . 114   . 164 128   . 048 116 183 097 185 038   . 222 220   . 218   . 195 112 119   . 155   . 217 060 184 083   . 148 066 143   . 002 011 004   . 177   .   .   . 118 165 043 144 134 005 067 082 023 093 200   . 064 106 190 071   . 130 003 161 105   . 088 135 065 159 073 077 036 122 056 101 019   .   .   . 140   . 204 212 205   . 126 042 035   . 146 091 030 188   . 078   . 133 049 053   . 173   . 115 080   . 098 178 050 046 107 201   . 063 010   . 086   . 150 223 179 052   .   . 152 047 202 020   . 147 069 171 172 210   .   .   . 127 189 214   . 104 018 079 213 014 084   . 160 129 151   . 109   .   .   . 192 081 022 058 131 094 045 163 136 197 208 074 044 219 059 174 181"
			//"  . 032 016 039 015 215 008 211 141 034   . 001 029 076 027 139 170 068   .   . 087   .   . 182 175   . 117 193 124 100 191 199 090 000 031 114   . 145 180 164 128 017 025 054 142 222 194 048 038 116   . 218 095 183 220 185 112 149 154 195 158 184   . 083 066 217 143 013 148 155 002 011 119 162 060 207   .   . 165 070 167 004 134 082 023 067 043 144 177 118 190 064 113   .   .   . 130 003 088 123 105 132   . 161 071 122 073   . 159 140 019 204   . 036 101 089 062   . 021 065 078 009   . 042 091 156 133 030 153   .   .   . 224 212 146 098 221 107 201 115 046 049 050 157 053 173 080   .   . 209 187 047 086 052 057 152 041 202 063 179 150 010   . 223 020 147 037 171 189 018 102 069 210 198   . 104 127 214 096 051 129 160 151 213 079 206 084 186   . 109 111 014 081 007 026 163 022 074 174   . 058 136   .   . 131   .   . 208 197 059 138 168 061 024   . 040 121 169 075 033 120 176 072 125   ."
			//"088 093 130 113 132 200 105 161 135 071   . 190 106 003 123 159 065 073 122   . 062 019 021 056 140   . 137 077 101   . 188 153   . 205 146 126   .   . 078   . 133 042 091 156 009 050 053 107 173 221   . 157 108 046 178 049   . 115 098 209 063 110 020 010 041 187 052   . 150 179 152   . 057 202 223 018   . 210 214 171 069 104 037   . 102 172   . 051 127 189 007 026 109   .   .   . 111 014 084 129   .   .   .   . 186 022 219   . 045 163 094 181   . 044 074 058 197   . 174 208 075   .   .   .   . 138 072 033 024   .   .   . 040 168 012 211 027 001   . 039 008   . 034 016 076 216 029 141   . 015 203 124 100   . 139 193 175   . 103 170 166 087 196 068 117 164 145 054   . 142 031 114 180 191   . 199 017 085 128 000 116 154 112 185 097   . 038   .   . 149 218 194   . 048 095   . 217 013 143   . 158 028 060 011 002   . 184 155 083 162 070 082 177 004   . 067 144 134 055 165 118 167 005 043 023"
			//"089 140 019 036   .   . 077 122 137 101 021 073 062 065 132 188 205 146 224   . 042 126 212 009 035   . 056 091 078 153 098 201 046   . 053 050   .   .   . 049 108 157   . 080 173 047 179 223 063 209   . 110 052 152 020 041 150 086 187 057 171 069 202 127 037 172 147 210 214 051 189   .   . 018 102 084   . 129   . 079   . 151 109 213 014 192 160 096   .   . 219 174   .   .   .   . 094 208   . 081 131 045   . 022 074 040 012   . 125 169 176 033 006 024 075 138 061   . 168 121 027 216   . 016 001 029   . 141 215   .   .   .   . 032 076   .   . 068 166 100 193 175 124 196   . 103   . 117   . 139 090 180   .   . 191 025 145 142 054 000 085 017 114 031 182 185 149 097 048 038 194 222 116 154   . 112 218 220 095 128 195 217   . 066 083 028   .   .   . 011 184   . 119 060 143 134 067 055 082   . 070 162 023 004 118   . 167 144 177 207 106 093 130 200 088 105 005 113 123   .   . 071 064 161 003"
			//"164 090 199 025   . 031 017 054 000 085 191 114 145 180   . 038 116 222   .   . 149 112 154 095 183   . 128 218 220 048 158 060 083 195 013 148   . 002 028 217 119 011 184 066 143 165 055 167 082 162   . 004 043 177   . 067 023 118 070 207 093 200 005   . 105 106 161 135   . 123 130 190 088 064 071 021 159 137 065 140 204 073 077 122 101 062 089 132 036 019 126   . 091   .   .   . 153 078 212 056   . 009 035 156 030 080 050   . 173 046 201 053   . 107 178 049 115   . 098 221 020 052 150 086   . 041 057 179 223   .   .   . 010   . 047 171 202 069 102 189 127 172 214 104 210 147 037 051 198 018 079 206 007 160 084 111   . 026 213 129 014   . 151 186 096 022 131 163 094 174   . 181 136 058   . 219 044 208 045 081 059 138 012 040 169 121 168 033   . 120 024 075 006 176 125 008 029 141 211   . 032 072 034 001 016   .   . 027 039 099   . 203 100 068 092 087 015 193 117 170   . 166 139 103 124"
			//"168 040 024 120 006 012 061   . 121 033   . 176   . 075 059 216 027 034 029   . 039 215 211 099 008   . 072 016   . 001 103 068 087 015 124   .   . 193 117 203 139 100 196 175 092 085 180 114 164 182   .   . 191 017 000 090 031 025 054 142 185 112 128 149 183 097 154 220 194 116 218 048 038 095 222 143   .   . 155 066 148 060 002 217 013 083 158   . 011 184 167   . 118   .   .   . 004 055 082 162 177   . 134 070 165 106 071   . 088 105 200 003   . 190 135 093 113   . 161 123 137 021 204   . 019 140 159 065 073   .   .   . 062   . 101 156 056   . 146 091 042 224 030 153 078 212 035   .   . 009 080 178 107 098 173 157 046 108 049 221 053 115 201 050 133 187 152 041 110 052 047 150 020   .   . 223 086 179 057 209 202 171 102 172 037 051 147 214   .   . 104 210 018 069 198 192   . 206 160   . 213 096 186 007 111   . 151 129 109 026 163 022   . 219   . 197 081   . 136 208 094 181 045 058 074"
			//"147 172 104 189 018   . 127 171 051 214   . 069   . 210 202 084   . 186 079   . 109 151 160 026 192   . 096 111 206 007 058 219 197 081 074 181   .   . 136 022 045 044 094 163 174   . 075   . 168 059   .   . 125 061 121 040 012 120 138 006 032 215 072 039 008 029 211 141 076 027 016 001 216 099   . 092   .   . 170 175 166 068 193 203 124 087 103 015 100 196 114 191 025   .   .   . 199   . 164 182 017 142 145 054 085 097 222   . 038 183 112 194   . 048 220 185 149   . 154 116 028 143 148 011 184 066 119   . 060   .   .   . 083 217 013 070 162 167 023 118 177 067 165 004 055 082 134   .   . 207   . 135 190 161 088 130 105 064 093 123 003 113 200 071 005 122 077 140 019 021 101 204 137   .   . 073 036   . 159   . 056 156 146 224 035 205 212   .   . 091 153 078 009 126 188 046 080 178 098   . 049 133 050 107 157   . 201 221   . 108 041 187   . 223   . 010 209 152 020   . 110 150 057 063 047"
			//"082 067 004 118 207 023 177   . 144 165   . 167 134   .   . 088 123 071 106   . 113 200 161   . 105 003 005 130   . 190 089   . 062 132 101 204   .   . 137 122   . 036 019 140 021 030 078 126 212 056   . 153 188   . 205 224 146 091 156 009 049 201 133 115 046 080 098 178 053 221 157 107 173 108 050 052   . 020 179 041   . 223 152 187 047 010   . 209 086 110 069 198 189   .   .   . 104 210 147 202 127 018 037 171 214 079 186   . 084 192 151 014   . 007 206 213 109   . 160 129 136 174 181 044 094 163 045 208 219   .   .   . 197 022 074 138 059 176   . 120 061 040 033 024   . 168 169 121   . 006 029 141 001 211 216 016 008 099 032 027 076 039 215 034 072 203 193 175 196 092 124   . 117 103   . 068 100 170 139 015 182 054 031 090   . 000 164   .   . 025 199 180 142   . 191 183   . 220 154 194 185   . 222 048 218   . 112 116 149 095   .   . 011 060   . 083 195   . 028 155 184 148 119 158 013"
			//"211 029 001 016   . 034 039   . 027 076   . 215 008 141 072 092   . 166 175   . 193 068 103 139 087 124 015 100 170 196 164 114 145 182 085 031   . 017   . 054   . 025 199 090 191 194 220 112   . 128   . 048 038 149 116   . 222 218 185 095 217 060 195 002 083 066 158 155 013 028 011 184 143 119 148 043   . 144 055 067 023 167 177 070 165 134 082 162 118   . 200 088 130   .   .   . 190 135 161 005 113 064 105 093 003 140 204   . 021 062   . 101   . 019   . 122 077   . 089 137 205 188 146 091 153 224 009 078 126   .   .   . 035 156 030   . 133 201 050 157 115 080 053 107 178 098 046 221   . 108 041 179 110 063 052 086 010 057 187 020 047 152 223 150 209 171 127 172 104   . 214 102 051 147   . 069   . 210 018 202 096 213 186 079   . 129   . 014   . 111 007 206 026 151 084 197 163 208 058 074 022 081 181 094 044   . 219 136   . 045 040 138 120 176   . 169 059   . 121 075   . 012 006 168 033"
			//"058 163 094 044 045 181 131 022   . 074 174 219 197 208 081 125   . 012 040   .   . 176 168 006 169   . 059 120 075 024 211   . 008 072 076 034   .   . 027   . 099 016 001   .   . 124 170 068 103 015   . 196 092   . 117 175 166   . 203 139 054   . 182   . 145 090 164   . 085 000 025 199 191 142 031 038   . 116   . 097   . 112 149 185 194 183 154 128 218 048 060 143 011   .   .   .   . 155 158 195 002 119 083 217   . 067 023   . 043 134 167 165   . 004 055 070 177   . 082 144   . 088 071 130 190 106 064 135   .   .   .   . 105 093 003 122 132 073 204 036 077 140 101 019   . 089   . 137   . 159 224 078 153 212 188 091 035   . 156 205 030   . 126   . 056 049 115   . 107 173 053   . 221 098   . 201 157 178 108 133   .   . 150 041 010   . 063   .   . 086 110 179 057   . 052 037 172 210 147   . 171 202 102 104   .   . 069 051   . 018 079 213 111 151 084 192   . 109 129 206 007 186 026 160 014"
			//"161 106 190 130 064 071 113   . 123 003   . 200 105 135 005 021 137 204 140   .   . 073 089 159 062   . 132 036 065 019 212 126 035 056 030 146   .   . 205 156 009 091 153 224 188 053 178 201 098 133   . 107 173 115 221 080 050 157 049   .   . 223 209 152 010 041 063 179 047   . 086 110 052 057   . 198   . 051 210 172   . 069 127 171   . 037 147 202 189 104 151 084 111 129   .   . 007 206 160 096   .   . 192 213 014 163   .   . 174 197 219 074   . 094 208 022 131   .   . 136 121 125 012   .   . 040 006 075 176   .   . 061 169 138 033 032 072 215 034 016   . 029 076 001   . 211 008 027   . 099   . 170 196 103 092   . 087 139 203 117 124 193 068 166   .   . 017 090 199 191 085 031 000 164   . 114 025 180 142 182 128 185 222 097 183 116 154   .   . 218 048 220 095 112 038 083 066 155 158   . 217 195 148 184   .   . 060 028 002 119 067 070 118 167   . 134 162   . 144 055 004 023 207 082 165"
			//"160 079   . 111 026 186 109   . 129   . 084 151 192 206 096 174   . 181   .   . 131 219 058 045   .   . 081 044 208 094 168 176 169 059   . 012   .   . 121 138 006 120 024   . 125 076 141 215 211 072   . 001 216 039 027 029 034   . 032   . 203 068 015 193 087 175 103 170 124 117   .   .   . 139 166 191   . 000 180 090 031 114 017   . 085 145 164 182   . 199 112 038 218   .   .   . 048 220 154 128 149 095 183 185 194 066 148   .   . 083 060 013   . 184 155 217   .   . 158 028 144 043 023 118 004 067 207 055 167   .   .   . 134 070 165 093   . 200 071 130 113   . 003 190 135 161 105 123   . 064 140 065   .   .   . 036 062 159 122 137 101 077 073 204 132   . 042   . 153 188 030 146 205 212   . 126 091 078 009 056 133   . 050 080 046 221 098   .   . 157   . 178 108 201 173 010 041 179 063   .   . 209 150 110 086   .   . 020   . 057 172 171 189 069 198   . 202   . 051 210 104 102   . 147 214"
			//"154 097 048 218 095   . 149 185 116 194 038 112 183   . 128   . 028 148 066   . 002   . 158 119 083   . 195   . 155 184 082 167 134   . 165 023   .   . 144 070 207 118 004 067 043 003 135 200 161 005 105 190 088 113 123 106 071 130 093 064 122 073   .   . 062 140 089 065 101 137   . 019 021 159 204   .   . 205 078 224 146 126 042 156 030   . 212 056 091 153 201 173 157   .   .   . 107 178 098 133 115 108 046 049 053 041 150   . 052 010 223 047   . 110 179 187 152   . 063 020 051 198 102 189 104 172 018 210 069   .   .   . 037 171 214 213 096 151 186   . 109 079 014 007 206 160 192 129   .   . 163 208 094 058   . 044 197 045 022 136 074   .   . 181 081 138 061 040 024 125 033 012 121 168 169 176 120 075 006 059 072 032 034 029 008 027 211   .   . 016 001   . 099 215 216 087 175   . 103   . 203 015 166   . 100   . 068 117 193   . 090   . 025 114 191 145 182 017 000   . 199 031 142 164 085"
			//"103 175 196 100 139 166 193 203 117 124   . 068 087 170 015 191   . 031 090   . 017 114 164 142 145 085 182   . 180 199 154 112   .   . 194 222   . 149 116 185 095 218 048 097 038 013   . 060 158 195   . 184 143 002 028   . 148 011   . 119 070 167 162 177 134 067 082 055 165 144 118 004 043   .   .   . 064 123 135 106 071 200 113 093 003 105 161 005 130 190 073 021 036   .   . 204 019 065 089 132 077 159 062 122 101 224 146   . 188 035   . 030   . 153   . 156 042   . 212 205 221 173 050 157 107 080 108 178 201 133   .   . 046 049 053 187 209 223 150 086 152 041 047 110 179 063 010 020 052   .   .   . 104 147 198 189 037 018 171 051 214 127 069 102 202 213   . 079 007   . 014 186 129 160   . 151 111 206   . 096 081 022 181 163 197 136 058 074   . 044 094   .   . 219 174 169 040   . 168 033 138 059 012 024 120   . 176 121   . 006 029 032 016 215   . 008 072 039 027 141 001 034 099 211 076"
			//"063 041 110 086 057   . 152   . 020 047 052   . 010 179 209 198 051   . 172   .   . 069   . 018 037   . 202 189 210 104 160 151 192 096   . 186   .   . 129 213 026 111 007 079 084 074 208 219 058   .   . 094 174 131   . 163 181 044   . 045 138 176 059   . 169 040 168 075 033 121 120 024 125 006 012   .   . 027 141 029 034 215 039 032   . 008 211 072 016   . 068 092 100   .   .   . 196 170 103 015 193 139 087 203 124 090 031   . 191 145 114 085   . 199 180 054 017   . 164 000 116 038 222 218 048 097 095 220 112   .   .   . 183 185 194   . 195 060 148 011   . 066 013 184 155 158 083 028   .   . 067 055 004 082 043 118 134 207 070 144 165   . 167 023 162 093   . 106 190 088   . 071 123 161   .   . 130 135 064 005 132 122 204 140 062 137 089   .   . 036   . 065 159 073 021 035 224 078 212   . 156 056   . 153   .   . 126   . 042 009 080 049 157   . 173 046 133   . 221   . 107 050 108 098 053"
			//"212   . 153 091 009 146 042   . 205 030 188 126 035 078 056 173 221 050 080   . 115 201 098 108 046   .   .   . 178 107 063 223 010   .   . 150   . 152 020 187 057 086 110 041 052 214 210 069 147 202   . 104 198 127 051   . 102 189 171 018 213 151 096 109 192 079   . 206 014 129 111 007 084 026 186 174   . 136   . 163 181 219 131 022 074 197 058 081 044 094 176   . 120   .   . 012   .   . 168 059 061 006 169 138 033 029 034   . 216 008 215 076   . 001 141 032 039   . 211 027 117 092 166 100 196 175 139   .   . 015   .   . 087   . 124 054 182 114 031 025 017 090 085 199 180 164   . 000   . 142 097 220 048 154 038 218 183 095   . 116 194 149 112 222 128 217 002 066 184   . 013 148 028 158   . 060 011 155 119 195 162 070 023 067 134 144 082 165   . 118   .   . 207 167 043 105 106   .   .   . 093 005 071 190 130   . 200 123 113 064 140 122 036 073 021 062 132   . 137 065 019 204 159   . 101"
			//"  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .127   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 213   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 033   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 149   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 177   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 003   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   ."
			//"  . 066 184 011 119 148 002   . 028 013 143   . 083 155 195   . 144 023 067   . 177 167 082 207 134   . 162 118   . 004 161 200 105 005 003 071   .   . 123   . 064 130 190 106 088 101   . 073   . 132   . 019 021 077 137 140 204 036 122 159 156 126 056 042 035 224 212 078 030 205 091 153 188   . 146 173   .   .   . 080 050 201 115   . 053 046 098 133 157 107 223 052 086   .   .   . 110 179 063 209 152 057 010 187 047   . 102   .   . 037 069 214   . 104 210 171   .   . 147   . 129 084 186 111 007 079 026 206 151   .   .   . 192 213 014 022 081 219 181 044 131   . 074 094 208 058   .   .   . 045 040   . 024 168 125 120 169 006 138 121 033 061 176 012 059 032 039 029 001 216 076 034 027 211   . 215   . 141   . 072 015 203 166 175 087   . 103   .   . 100 196 170 139 068 092 145   . 180 164   . 054 182 031 199 025   . 114 000 017   . 097 185 218   . 038 183 128   . 116 220 048 222 095 154   ."
			//"  . 083 060 184 143 013 011 066 195   .   .   .   . 028 095 082 162 165   .   . 118 023 070 043 177   . 119 004 144 167 093 071 113 207   . 003   .   . 005 106 088 190   . 105 161 065 137 204 122 064   . 073 089 036 132 062   . 019 140 021 224   . 159 091   . 035 156 205 078 056 153 126   . 188 030 098 173 133 221 046 053 050 157 080 178 115 049 009 107 201   . 063   .   .   .   . 223 020 187 108 086 052 152 041 179 037 214   . 147 127 102 210   . 069 051 172 189   . 171 202 096 160 014 007 151 192 084 129 186   .   .   .   . 079   . 163 026 181 074 094 044 197 208 219 136 022 131 081 058 174 169 121   . 138 168 024 061 125 040 059   . 120 012   . 045 029 016 008   . 211 141 076 072 032   . 034 001 027 216 006 099 175   . 087 193 015 203   .   . 196   . 117 092 166 103 017 145 000 054   . 090 139 085 114 199   .   . 182 025 191 183 097   .   .   .   . 142 218 128 116 112 194 038 185   ."
			//"093 105 200   . 088 003 130   . 005 135 161 071 113 123 207 089 132 101 062   . 036   . 122   . 077   . 064 019 137   .   . 146 042   . 078 030   . 091 056 224   . 153 126   . 212 178 221 050 049   .   . 201   . 157 133 046 053 107 080 173 041 150 108   . 152 010 187 020 179   . 110 223 063 052 047 147   . 202 051 037 214 102 189 172 210   . 171 057 104 069 186 160 007   .   . 014 151 129 213 018 111 084 109 079 206 197 074   . 058 131 181 208   . 219 136 163 044   . 022 081 059 168 033 024 176 169 125 121 012 045   .   . 061 040 075 029 006 034 076   . 016 008 141 215 027 032 039 072   . 216 087 117 068 203 103   . 193 092 175 015 170   . 166 124 099 090 025 145 114 164 180 085   . 054   .   . 199 000 191 139 142   . 194 183   . 128 185 220   . 048 112   . 038 222   .   . 083 028 217   . 066   . 013   . 184   . 148 195 011 143 134 067 004 023 082 177 119   . 162 144 167   . 043 070 055"
			//"185 183 112 048 038 194 218   . 128 220 154 222 149 116 142 158 195   . 083   .   . 148 217   . 002 155 095 184 028 060   . 023 177 119   . 165   .   . 162 067 043 004 167 134 082 135 123 071   . 207   .   . 161 130 005 105 003 190 106 088   . 204 064   . 077 062 122 137 065 132 019 073   . 021 101 212   . 056   .   . 030 146 091 224 078 042 156 159   . 126   . 098 107   .   .   . 201 221   . 009 157 173 115 080 178 010   .   . 063 152 150 179   . 223 020 041 086   .   . 209 202 147 214 104 069 037   . 051 102   .   .   . 127 172   . 079   . 186 014 007 111 192 206 151 129   .   . 096   . 084 197 136   . 022 058 094 131 174 163 081 208   . 181 074   . 040 120 169 176 168 075 033 059   .   . 012   . 121 125 045 006 029 076 008 039 072 032   .   . 001   . 027 216 034   . 193 087 117 203 170 175   . 124 068   .   . 166   . 100 092 145 090 199 031 164 017 139   . 182 000 114 085 191 054 180"
			//"213 192 151 007 084 014 111 079 096 206 160 186 109 129 018 058   . 074 197   . 044 181 022 174 131   . 026 094   .   . 138 012 061   . 075 033   . 120 059 040 125 024   . 169 168 141 027   . 032 006   . 215 211 016 072 008   . 001 029 216 175 166   . 100 193 087   . 117 170   . 196 068 103 092 124 164   . 182   . 145 085 031 025 090 180 017 054 139 199 114 222 154 048   .   .   .   .   . 185 142 218 038 149 097 220 083   .   . 158 002 148 155   . 060 028 066 011   .   . 195 162 082 165 004 167 134 043   .   .   .   .   . 177 067 055 106 207 071 003 190 130 105 135 200 123 093   . 005   . 088 062 137 073 122 089   . 077 021   . 132 065 036   . 101 064 224 091 035   . 212 078 030 056 156   . 146 153   . 188 159 009 080   . 046 115 133 049 178   . 107 201   . 173 050 098   .   . 020 187   . 041 108 047 223 110   . 150 209   . 052 037 172 104 102 147 127 057 189 202 051 069 214 198 171 210"
			//"138 169 176 024 125 033 120   . 059 075   .   . 061 121 045 211 072 076 008   . 016 034 032   . 039   . 006 001 027 215 203 166 193 099 170 124   . 100 015   . 092 196 068   . 103 180 000 031 054 139   . 114 164 025 182 145 085 199 090 191 097 222 142 218 149   . 185 116 220 128 048 112 154 038 194 158   . 195 028 083 013   . 011 066 155 002 217 095 184 060 023 082 004   .   . 165 167 144 070 119 118   . 177 067 055 105 003   .   . 113 071 135   . 200 123 106   .   . 093 005 132 089 101   . 073 062 021 137 204 064   .   . 077 140 065 224 159 146 030 153 091 035 078   . 205 156 042 056   . 188 046 221 201 049 098 107 115 173 080   . 178 157 050 053 009 041 086 010 223 063 179 047 209 187   . 150 110 020 052 108 057   . 214 037 127   . 171 210   . 104 069 051 198 102 147 109 192 129 213   . 079   . 014 151 007   . 186 096 111 084 197 163 094   .   . 131 026   . 081 136 219 074 174 022 208"
			//"022 197 219 094 174 074 044 163 081 208   . 181 131 136 026 168 059 033 169   . 120 012 138 125 061 075 045 024 121 176   .   . 039 006 141   .   .   . 072   . 216 001 215 008 211 170 117 166   . 099   .   . 103 100 015   . 124 196 175 092 090 031 139   . 017 145 054 000   .   . 199 114 164 191 085 154   . 128 116 183 194 222 218 097 220 149 185 142 048   . 148 158 184 195   .   .   . 028 217 095 011 143 002 066   . 134 165   . 082 177 023 055   . 167 144 067 118   . 070 162   . 161 003 190 200 105 088 123   .   .   . 130 113 106 135   . 064 204 101 019 036 062 065 073 137 122 077 132   . 021 035 205 126 156 212   .   . 188 224 056 078   . 146 030 159 080 157 046 201   . 178 053 133   .   . 050   . 221 173 009 108 041 047 010 152   . 187   .   .   . 223 020 052   .   . 127 037 051 171 210 172 057 214 069 104   . 102 202 189 198 192 079 007 186   . 109 018 111 096 129 151 014 084 213 206"
			//"  .   . 201 107 173 053 157   . 133 178 098 050 115 221 009   . 209 047 010   . 086 150   . 052 152   . 108 110 020 223 171 102 127 057 210 214   .   . 202 172 198 104 069 037 147 206 129 186 213 018   . 151 160 111 096 192 014   . 079 084   . 181 026   . 131 197 022 136 208 081 094   . 058 174 074 168   . 059 121 169 033 012 120 040 075 061 138   .   . 176 034 211 001   .   .   . 215 027 032 006 016 216 039 029 141 087   .   . 103 193 166 170   . 068 117 175 100   .   . 015 182 164 085 199 114 145 191 000 031   .   .   . 017 090 180 097   .   . 194 048 218 183 220 112 116 185 149 128   . 038 083 028 060   . 158 184 002 143 066 195 155   . 148 013   . 067 118   . 167 082 055 165 162 070   . 023 004 144 043 119 207 106 003 105 113 005 093   .   . 190 200 123 088 071 161 077 062 137 122   . 140 064   . 073 019   . 204 132 036   . 035 224 153 146 212 042 159   . 056 205 126 030 188   .   ."
			//"156 035   . 153   .   . 091 224 056 078 212 146   . 205 159 098 133 053 046   . 157 050 049 173 115   . 009 107 221 201 187 150 152   . 179 047   . 086 209   . 052 110 223 010 063 210 051 102 171 057   . 069   . 189 202 037 214 104 172 198 079 186   . 111 109 192 213 129   . 096 007 151 160 084 014 058   . 081   . 197   . 181 044 163 208 131 022 026 094   . 012 168 024   .   .   . 176 121 138 045   . 125 061 040 075 008 076   . 211 039 034 141   . 215 027 029 016   . 032 072 015 103 124 196   . 087 092 117 166   .   .   . 193 175 170   . 139 031 085 199 025 145 180 114   . 054   . 182   . 191 183 116 112 185 154 048   . 038 097 128 220 218   . 194 142 066 011 083 060 158 155 013   . 217   . 148 184 028 143 095 119 067 165 134 177   . 070 055   . 004 167   . 043 023 082 113 105 123 093   . 106 207 003 200 190   . 071 005 130 088 062 140   . 204 089 077 064 036 132   .   . 101   . 122 065"
			//"  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 188   .   .   .   . 205 126   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 189   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 206   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 061   .   .   .   .   . 040   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 100   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 025   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 177 134   .   .   .   . 119   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   ."
			//"203 087 068   . 092 124 100 175 015 170 103 166 193   . 099 164   . 085 145   .   .   . 054 191 017   . 139 199 000 114 185 222 149 142 220 194   .   . 128 097 038 048 112 183 154 155 028 148 217   .   . 060 158 011   . 083 013 184 066 143   .   . 119 118 177 134 070 144 055 162 004   . 082   . 165 161   . 005 123 105 003 071 130 106 135 113 093 207 190 200   . 089 019   .   .   . 073 137 122 064 036 021 077 140 065 035 030   . 212   . 146 078   . 126 205   . 091   . 156 056 133 098 053 107 201 046 173 221 050   .   .   . 115 080   . 041 108 150 047 110 086 010 179 223 020 187 152 209   . 052 037   . 069   . 147 104 127 198 172 202 210 189 102   .   . 079 111 192 151 160   . 014 096 213   .   . 007 129 084 018 026 163 074 197 131 081 022   .   . 094 219 136 174 181 058 061 169 121 138   . 040 045 033   .   .   . 012 059   . 125 008   . 001 034 211 039 006 016 072 027 215   . 216 032 141"
			//"070   . 167 004 043 165   .   . 162 055 082 023 177 144 119 161 005   . 105   . 130 071 093 088 113   .   . 190 123 200 122 204 077 064 065 101   .   . 132 140 021 019 073 062 089 078 205 146 156 159 042   . 212 091 056 035 030 153 224 188 080 050   . 157 115 046 049 221 178   .   . 201 098 173 053 063   . 209 020   . 047 150 086 041 179 152 187 108 110 223 102 147 104   .   .   .   . 051 171 057 189 198 127 172 210 192 014   . 160 109 186 206   . 151 129 079 111   . 213 096 081 058 074 094 219 197 174 136   .   .   .   . 131 163 208 040 045 012 033 024 120 169 075 176 121   . 061 059   . 125 008 027 215 032   .   . 039 216 029 072 141 016   . 076 006 175 100 087 068 103 170 124 015   . 193 166 196 117 092 099 139 090 085 145 017 182 054   .   . 199 114 000 191 031 164 149 183 116   .   . 097 142 194 112 048   . 222   . 218 038 083 066 184 148 158 002 095   .   . 028 060 013 143   . 155"
			//"032 008 215 001 216 076 016   . 072 141   .   . 039 027 006 103 015 124 087   . 100 166 203 092 193   . 099 196 117 068 054 031 017 139   . 085   .   . 182 090 191   . 114   . 164 220 116 222 185 142   . 112 154 218 128 183 194   .   . 038 066 148 095 011 002   . 217 028   . 195 184 060 158   . 013   .   . 162 144   . 165 023 118 067   . 177 070   .   . 167 071 161 190   .   .   .   .   . 093 207 130 088   . 106 135 062 101   . 089 077 204 065   . 073 137 140 036   . 122 132 056 212   . 153 126 035 188   .   .   .   .   . 042 224 078 080   .   . 053 107   . 046 178 201 221   . 115 133   .   . 010   . 223 187 063 110   . 052 041   . 179 086 150 047 108 172   .   . 069 147 210 214 202 171   . 102 104 051 198 057 018   . 014   . 109 096 213   .   . 007   . 129 084 186 160 131 197 136 022   . 163 026 074 219 094   . 181 081 044 174 169 040 024   .   . 061 045   . 059 121 176 033 125 138 075"
			//"054 145 114 199 191 085 025   . 182 180 164 031   . 000 139 154 128 194 183   . 218   . 185   . 149   . 142 048 116 112 217 148   . 095 155 013   .   .   . 066 143 184 060 083 158 055   . 023 070 119   . 167 082 118   . 134 165 004 067 043 106 071 207 130 113 105 093 123 135 005 190 200 161 088 003   .   . 132 137 062 101 204 036   . 065 077 122 064 019 073 146   . 153   .   .   . 126 205 156 159 091 188   . 224 078 046   .   . 098   . 050 178   . 201 221   . 157   .   . 133 209 063   . 110 223 010 052 020 150   .   .   . 152   . 179 172 057 102 214 104 189   . 210 069 051 171 127 202   .   . 192 129 151 213 160 007 109 084 079 096 206 111 186 014 018 163 044 197 219 058   . 074 081 022   . 181 094 136   . 026 045 040 033 169 061 059   .   .   . 024 176 121   . 012 168 039 008 027 032   . 029   . 076   . 001   . 034 072 016 216 087 175   . 166 103 193 099   . 015 117 068 124 092 203 170"
			//"171   . 069 104 198 214 189   . 202 210   . 102 127 051 057   . 096 014 192   . 111 186 213 084 109 206 018 007 129 151 022 181 131 026 208 074   .   .   . 163   . 094 219 197 058   .   . 012   .   .   . 176   . 120 059 169 033 024 040 125 029 034 006 016 039 008 032 027 141 072 001 215 211 216 076   . 092 015 117 087 124 166 100 175   . 193 203 099 196 068 031 164 199   .   .   . 114 000 054 139 025 191   . 090 180 183 194   . 154 149 222 220   . 112 116 097 218   . 185 128 195 158   . 184 060 083 143 028 148   .   .   . 002 066 155 067 119 023 165 004   . 134 055 167 144 070 177 162 082   . 105 123 200 093 161 190 113 088 106 005 135 130 071 003 207 140 036 062 073 089 065 101   . 122   .   .   . 137   .   . 159 224 030 035   . 056   .   .   . 153 126 205 188 146 212 115 046 221 049 178 080 009 053 201 107   . 050 133 157   . 010 041 110 150   . 152 108   . 209 020 223 047 052   . 179"
			//"187 010 223 110 052 047   . 041 209 179   . 150 152 020 108 147 202 214 037   . 189 102 171 198 127   .   .   . 051 069 213 186 109 018   . 014   . 111 096 079 084   . 151 192 160   . 136 181 022 026   . 219 058 044   . 197 074   . 163 174 040 012 045   . 061 169 138 121 075 059 024 176 168 125   . 211   . 072 027 008 076   .   . 029 141 039 032 006 001   . 166 103 196   .   .   . 068 117 203 099 100 092   . 175   . 145 085   . 164   . 031 180   . 114 000   . 025   . 054 182   . 154   . 048 112 183 038 116 222   .   .   . 149 097 220   . 095 148 013 184 011 083   .   . 028 217 002 195   . 143   . 144 167 070 082 004 177 043 067 162 055   . 023 165 119 106 130   . 200 161   . 003 005 093   . 071 190 123 088   . 064 140 101   . 077 132 122 065   . 019   . 137 021 204 089 042 035   .   .   . 224 159 030 126 153   . 146 056 091 188 046 080 107 050   . 115 009 157   . 221 201 053 173 049 178"
			//"106 113 071 200 161 135 190   . 207 123 093 003 130 005 043 122 064 065 077   .   .   .   . 089   .   . 088 073 132 204 224 030 091   . 205 078   . 153 159 035 212 126 146   . 156 221 133 053 080 188   . 050 049 107 009   . 178 201 046 098 010 047 173 110 086 152 041 209 020 108 223 150 187 063 179 171   . 057 202 127 210 214 104 037 051 189 172 052 069 102   . 213 151   .   . 206   . 096 079 198 007 160 111 192 129 131 208   . 022 044 074 136   . 181 081 197 094   . 163 026 045 138 075 176 012 061 168 059   . 174   .   . 120 169   . 008 125 076 141 215 001 039 027 034 072 029 016 006   . 211 193 015 166 175 203 068 100 103 087 099 117 196 124 170 216 145 199 017 031   . 000 180 139 090   . 085 114 182 164 092 191   . 220 149 218 142 097 116   . 112 222   . 154 194 185 011 002 195 066   .   . 038   .   .   .   . 013 095 184 158 177 134 167 165 070 118 143   . 119 162 023 055 082 067 144"
			//"224 042 146 126 212 078 153 035 159 205   . 030 091 056 021 049 009 178 115   . 107   . 080 098 157 221 188 201 133 050 041 047 086 173 020 179   . 110 108 010 063 223 150 152 187 051 202 214 172 052   . 102 171 104 057 127 210   . 037 147 192   . 198 007 111 109 079 096 129   . 151 186   . 160 206   .   .   . 081 131 208 074   . 197 136   .   . 084 219 181 033 138 176   .   .   . 012 059 040 174 024 168 120 169 121 039 141   . 032 016 076 027   . 034 072 008 001   . 029 006 099 203 170 068 166 193 103 015 124   .   .   . 100 087 117 145 092 085   .   . 199 017   . 031 182 090 025   .   .   . 149 128   . 097 185   . 218 154 183 142 116 048 194   . 191 083 184   . 148 217 028 155 095 066   . 013 060 195 158 038 143 134 055 177 118 119 067 144   . 167 023 162 082 165 070 130 113 005 106 123 105 043 135   . 200   . 003 207 190 161 077 062 073 101   . 036 088 019 064 132 204 065 089 140 137"
			//"040 061 012 176 168 075 024   . 045 121 138 033 120 059 174 032   . 141   .   . 001 076 029 211 016   . 125 215 072 034 175 124 100 216 117 170   .   . 099   . 103 068 166 193 203 000 182 085 090   .   . 031 054 199 139 017 180   . 145 164 183 194 191 048 218 149 097 128 116 142 112 222 185 154 220 217   . 095 195 002 155   . 184 083   . 011 066 038   .   .   . 070 167   .   .   . 023 162   . 143 004 082   . 134 144 113 135   . 093 130   . 123   . 071   . 105 190   . 106 207 064 122   . 073 204 077   . 132 101   .   .   . 036 062   .   .   . 030 078 126   . 042 205   . 056 224 091 159   . 212 115 133 050 080 049 201 157 098 046 009 221 107 053 178 188 010 110   . 150 187 020 179 108 041   .   . 223 209 063 173 052 037 210 127 189   . 172   .   . 069 102 202 147 214 171 111 109 096 079   . 192 198 206 186 151   .   . 018   . 160 131 197 219 074 022 044 084   . 026 081 181 208 058 163 136"
			//"097 149 222   .   . 220 048   . 142 116 185 194   . 128 191 217 095 155 002   . 184 013 066 158 011   . 038 060 195 148 067 165 118 143   . 055   .   . 119 134 082 167 023 177 070 123   . 003 106 043   . 071   . 190 207 113 135 200 105 161 062 101 088 019   . 077 140 132 137 064 073 204 122 089   . 156   . 159 056 042 078 030 153 035 205 091 224 021 126 146 053 049 201   .   .   . 050   . 080 188 107 098   . 046 221 152 179   .   . 086 047 020   . 150 209 010   .   . 041 108 057 171   . 069 102 127 147   . 214   .   .   . 189 037 051 192 198 014 206 151 007 109 129 186 096 079 111 018   . 160   . 081 181 163 022 219 044 058 197 026   . 094 074 208 084 169 024 061 012 138 121 075   . 040   . 033 176 059   . 174 125 008 141 039 016 006 029   .   . 215   . 072 211 076 032 100 193 015 175   . 087 216 170 166 068   . 124 099 196 103 017 145   . 085 054 025 092   . 139 182   .   . 164 090 000"
			//"175 193 166 068 103 170 196   . 099   .   . 124 100 015   . 054 139 180 017   . 199 085 090 164 025   . 092 114 182 031 097 194 218 191 116 220   .   . 142 183 154 112 222 149 185 028 195 013 066 038   .   . 217 184 095 002 155 060 083 158 134 165 143 004 118 177 067   . 144 119 167 023 070   . 055 093   .   . 005 113 135 003 190 105 123 130 106 043 200 071 101 122 073   .   .   . 204 132   .   . 019   . 036 062 137 042 078   . 156 091 030 205   . 146 056 035 153   . 224 159 009 049 178   . 050   .   . 133 053   .   .   . 157 046 221 010 173 047 179 223 110 152 020 150 209 041 086   .   . 063 127   . 102 172 171 069 189   . 037 057 051 104 214 210 052 192 007 109 186 213 129 206 018   .   . 014 151 096 160 198 084 197 208 131 044 026 163   .   . 219 181 081 058 074 022 120 061 059 040   . 169 174 075 012 176   . 033 045 024 168   . 008 215 076   .   . 125   . 006 072 034 141 211 029 027"
			//"140 077   . 073 089 065 019   . 064 137 122 101 036 132   . 156 159 078 042   . 153 030 224   . 091   .   . 126 056 146 080 053 157 188 221 178   .   . 009   . 098 201 050 115 049 020   . 047 041 173   . 150 187 110 108 152 179 223 010 063 037 214   .   . 189 127 172   . 051 057 069 102 171 147 210 213   . 018 096 109 206 014 007 192 129 111 079 198 151 186 074 022 219   .   .   . 181 081 163 084 094 058 044 197 136   . 075   . 138 120 033 121   . 012 059 169 024   . 040   . 006 032 141 215 034 039 211 072 076   .   .   . 016 008 027 087 216 124 170 068 196 193 117 166 015 175 100 099   . 103 017 182 031 090 054 114 025   . 145 139 000   .   . 180 092 183 048 149 222 185 116 220 142 097   . 194 112 128   . 191 038 083 155 002 011   . 066   .   . 060 148 195 158 013 217 118 177 162   .   . 134   . 055 023 167   . 165 119 004 082   . 105 200 003 093 130 043   . 207 005 071 135   . 106 123"
			//"090 017 031 114 164 180 199 145 139 000   . 085 025 182 092 185 142 220   .   . 048 194 097 154 218 116 191 112 128 222 066 013 011   . 028   .   .   .   . 083   . 060 148 002   . 144 162   . 067   .   . 023 070 004 119   . 055 167 134 082 105 003 043 190 130 113   . 005 123 207 200 071 093 161 135 122 089 064 132 077 065 101 019 062 137   . 140 088 073 204 030 156 126   . 042   . 146 056   . 021 153   . 091 035 205 115   .   . 049 157 053   .   .   . 133 046 107   .   . 009 108 187 179   . 150 152   . 209 047   . 041   . 086 010 020 037 052 214 210   . 104 127 051 102 202 172 189 057 171 147 109 096 186 079 213 151 111 160   . 018 129 007 014 206 198 197 094 131 181   . 136 208 026 163   .   . 219   . 058 084   . 169 075 061   . 045   .   .   .   . 012   . 168 033 138 016 039 072 029 027 008 125 141 034 215   .   . 006 001 211 193 087 068 124   . 100 216 196 099 015 166 170 103 175 117"
			//"079 109   . 151 160 206 007 192 018   . 213 014 111 096 198 022 026 208 131   . 094   . 163 058 044 136 084 219 081 181 040 033 120 174 121 075 059   . 045 169 168 176 012 061   . 027 072 076 029 125 016 034 032 001 006 039 141   . 008 211 087 124 216 196 100 193 175 015 117 099 068 166 203 103 170 054 164 139 182   . 180 085   . 145 000 025 090 092 114 031 194 185 112   . 149 220 222 128 097 191 048 154 218 183 116 002 155 060 217 011 013   .   .   . 195 083 184 038 066 095 119 070 055 167 023 177 082 162 165 143 067   . 118 134 144 105 043 003 135 200 190 113   . 071 005   . 130 207 093 161 077 132 204 140 122 073 036 089 062 064 137 019 101 065 088 035 153   . 146 156 205 078 159 224 091 030 126 056 212 021   . 046 178 115 157 009 080   . 107 201 050 133 098 053 049 086 152 209 041 020 010 173 179   . 223   . 047 108 110 063 127 037 069 214 171   . 052 104 057 202 102 210   . 172 051"
			//"029 039 034 215 211 141 001 008 006 027   . 076 016 072 125 203 099 170   .   . 196 124 175 103 100 117 216 068 015 166 090 085 025   . 000   .   .   .   . 145   . 114 031 017   . 116 128   . 097   .   . 222 185 048 142   . 220 112 183 154 083 013 038 184 011 002   . 195 028 095 060 148 217 158 155 070 082 119 162 177 055 165 004 134 144   . 067 143 167 023 003 093 200   . 113   . 071 005   . 043 190   . 130 105 123 077   .   . 122 036 101   .   .   . 132 062 019   .   . 064 159 156 078   . 146 042   . 056 030   . 224   . 091 035 205 046 188 053 178   . 107 115 221 050 133 080 157 009 049 098 152 209 150 041 187 223 086 063   . 108 020 110 047 179 173 037 104 127 102   . 051 210 057 172   .   . 069   . 147 052   . 192 206 109   . 018   .   .   .   . 186   . 160 014 213 044 131 081 163 136 197 084 208 181 219   .   . 026 094 058 061 169 176 033   . 120 174 024 045 059 012 075 168 040 121"
			//"163 131   . 219 058 208 094   . 026 136 022 074 044 081   . 138 045 075 061   . 024 033 040   . 120   .   . 176 059 012 029 076 016 125 027 141   .   . 006   . 211 215 034 039 032 117   . 124 175 216   . 166 203 196 099 193 170 068 087 103 145 085   .   . 025 017 090   . 000 139 114 031 054 164 180 185   . 142 128 149 220 194 048 183 116 218 097 191 112 222 013 217 060   .   .   . 148 195 066 038 184 158 011 083 028   . 055   . 070 118 165 144   . 023 162 134 004   . 067   . 207 093 135 200 071 113 161 005 003   .   .   . 130 105 123 062 088 101 065 073 019 077 137 204 132 140 036 064   . 089 042 056 146 224 156 126 091   . 035 159 205   .   . 078 021 046 107 115 050 049 221 178 009 080   . 053 201 133   . 188 173 010 179 152 086   . 041   .   . 223 150 209 063 047 187 189 127 202   .   . 037   . 210 102 069   . 214 057 104 147   . 192 151 014 213 111 198   . 018 096 186 206   . 079 129"
			//"080 115 050 201 098 178 107   . 009   .   . 053 157 133   . 187 108 179 152   . 110 047 041 063 086   . 173 223 209 150 172 214 189 052 051 210   .   . 057 037 147 069 102 127 171 129 096 014 079 198   .   . 213 007 018 109 206 151 192 160 197 074 084 094 044 131 163   . 136 026 219 181 022   . 208 138   .   . 059 061 075 033 024 169 121 120 040 174 176 012 076 032 215   .   .   . 034 072   .   . 001   . 016 008 027 193 170   . 203 100 124 117   . 166 015 087 196   . 175 099 139 054 180   . 031   .   . 182 085   .   .   . 025 145 000 183 191 194 220 112 048 149 116 222 128 097 218   .   . 154 002   . 148 066 217 060 011   . 083 095 028 184 013 155 038 134 004 177 023 070 144 055 119   .   . 165 167 162 082 143 043 105 135 113 130 207 106   .   . 200 071 005 161 003 093 036 077 132 140   . 062 088 065 204 073   . 101 064 019 089   . 035 126 030   .   . 021   . 159 056 146 078 212 224 205"
			//"172 127 102   .   . 210 104   . 057 051 171 214   . 202 052 213 018 206 109   . 007 014 079 160 111   . 198 151 096 186 163 074 044 084   . 208   .   . 026 197 058 219 181 131 022 121   . 033 040 174   . 012   . 024 045 061 075 176 169 168 008 076 125 001   . 039 029 072 027 006 215 034 032 211   . 203   . 099 015 193 170 124 196 087 117 100 175 216 068 166 085 054 114   .   .   . 031   . 090 092 199 164   . 145 000 149 220   .   . 218 194 116   . 222 128 183   .   . 097 142 095 217   . 060 148 002 158   . 013   .   .   . 011 083 028 134 143 165 055 167 004 177 144 023 162 067 118 119   . 082   . 005 071 106 093 200 130 161 105 207   . 190 003 135 043 062 019 077 204 122 137 065   . 140   . 101 073 132   . 088 021 035 078 042 091 159 224   .   . 126   . 056 212 030 156 157 115 133 080   . 046 188 178 050 201   . 053 009 107 098 152 010   . 047 187 086 173   . 108 209   .   . 063 041 020"
			//"067 177 023 167 082 055 004   . 119 144 070 165 118 162 143 093   . 135   .   . 190 003 106 161 130   . 043 200 005 071 140 101 036 088 137 065   .   . 064   . 089 073 204 077 122 205 056 030 224   .   . 146 156 153 159 042 078   . 035 212 046 053 188 107 157 115 080 133 221 009 201 050 049 098 178 187   . 108 209 152 179   . 110 010   . 086 041 173   .   .   . 171 069   .   .   . 102 202   . 052 104 147   . 037 051 109 206   . 213 111   . 129   . 186   . 192 007   . 079 018 026 022   . 219 181 131   . 081 074   .   .   . 044 197   .   .   . 033 075 176   . 061 121   . 059 040 120 045   . 168 039 072 034 029 032 215 016 211 008 006 027 001 076 141 125 087 196   . 166 203 117 170 099 175   .   . 068 015 103 216 092 145 180 017 025   . 090   .   . 114 031 182 164 085 054 218 149 128 097   . 183 191 220 222 112   .   . 142   . 154 002 083 060 013 217 011 038   . 095 195 148 155 158 066 028"
			//"041 152 150 223 063 179 110 010 108 020   . 047 086 209 173 171 057 210 127   . 104   . 172 147 189 051 052 069 202 102 079 014 111 198 129 206   . 007 018 192 160 151 186 109 213 136 081 074 163 084   . 181 022 094 026 131 208   . 197 058 169   . 174 024 120 061 040 059 121   . 176 012   . 168 075   .   .   . 072 039 141 076   . 008 027   .   . 125 215 034 124 203 068   .   .   . 166 015 175 216 196 103 100 087 117 017 180   . 054 025 085 000   . 031 182 145 199   . 090 139 142 185 220 112 222 149 154 128 194   .   .   . 218 183 116 083 038 013   .   . 184 002   . 148 195 066 011   .   .   . 177 162   . 067 070   . 118 082 134 119 144 004 165   . 143 105 190   . 071 093 123 135 207 106   . 003 200 005 161 043 088 062 065 077 036 064 140 137   . 073 204 132 089 101 122 091 042 056 224 205 035 021 078   . 126   . 030 159 153 212 115 046 201 053   . 157 188 107 009 133 050 178 098 080 221"
			//"066 002 148 060 158 155 184   . 095 028 217 013 011 195 038 070 119 055 177   .   .   .   . 082   .   . 143 167 162 023 106 003 130   . 123 135   . 190 207 105 161 200 071   . 093 137 132 101 140 088   . 204 122 019 064   . 065 073 062 089 035 030 021 153 091 042 224 056 205 159 126 146 156 212 078 049   . 009 133 115 178 053 107 046 221 157 080 188 201 050   . 187 223   .   . 179   . 209 041 173 110 063 086 010 020 127 210   . 171 189 214 051   . 102 202 037 104   . 172 057 018 213 206 151 186 109 160 096   . 198   .   . 111 192   . 197 084 074 208 219 094 131 136 181 081 163 044 026   . 058 061 059 012 040 138 176 120 168 169 045 121 024 033 075 174 008 001 039 034   . 027 141 006 029   . 076 215 072 211 125 216   . 170 193 100 099 175 117   . 068 166   . 103 124 203 025 017 182 090   .   . 092   .   .   .   . 085 139 199 164 149 183 112 194 185 218 191   . 142 128 222 220 154 097 116"
			//"169 120 033 012 138 121   . 061 174 059   . 075 024 045 058 029 125 027 016   . 215 141 008 032 001   .   .   . 006 076 087 170 196 211   . 117   . 068 216 193 203   . 124 100 175   . 139 180 145 103   . 085 090 114   . 025 000   . 017 054 149 220 164   . 048 218 183 142 128 191 222 194 097 185   . 066   . 038 095 011 028   .   . 002 195 184 083 154 148   . 055 067 023   .   .   . 165 119 134 158 167 070   . 177   . 130 123   . 106   . 135 005   . 003 207   . 200   . 105 043   . 140   . 204 101 036 122 064 065   .   .   . 019 077 132   . 089 078 205 146 126 091   .   . 159 035 153 021   . 156   . 009 053 046 080 050 107 049 115 188 133   . 178 221 212 152 223   . 047 041   . 020 173 010   . 179 150 108 187   . 063 127 051   . 104 052 037 202   . 102   . 057 171 210 172 007 111   .   .   . 109 147 129 014 186   . 206 198 151 213 044 131 181 208   . 094 160 219   . 026 074 136 022 197 081"
			//"134   . 165 023 070 144 167   . 143 162   . 055 004 119 158   . 043 123 130   . 200 135 105 093 190 005 082 071 207 003 062 065 019 161 132 137   .   .   . 077   . 204 101 036 140   .   . 078   .   .   . 030   . 126 021 091 205 146 042 156 115 178 212 201 107 157 046 009 133 188 050 053 080 049 221   . 187 173 108 086 020 179 223 152   . 110 010 098 150 047 210 172 102   .   .   . 214 057 037 063 069 171   . 127 202 111 129   . 079 007 206 096   . 014 018 109 151   . 192 198 084 163   . 181 074 044 022 026 208   .   .   . 094 131 081 061 058 075 121 012   . 120 059 033 045 169 024 174 040   . 016 006 076 008 029 034 001 032 039 125 072 215 141 027 168 193 068 100 124 175 015 117   . 087   .   .   . 099   .   . 103 017 000 025   . 092   .   .   . 031 085 139 054 180 090 048 218 142 183 128 149 164 116 194 222   . 220 191 112   . 011 002 148 155   . 184 154   . 038 095 013 028 217   . 195"
			//"192 111 014 186 213 129 151   . 198 096 079 206   . 018 147 163 084 136 044   . 219   . 197   . 094   . 160 181 026 074 169 075   . 058 059 121   .   .   . 061 138 012 033 120 040 072   . 141 008 168   . 076 029 215   . 016 027 034 039 032 193 170 211 068 196 100 087 099 015 216 166 124 175 203 117   .   . 092 139 025 000 180 114   . 182 199 145 103 031 085 220   . 222   .   .   . 194 142 183 164 112 185   . 149 128 011   .   . 066   . 155 195   . 013 095   . 060   .   . 038 143 067   . 023 165 118 070 119 055   .   .   . 004   . 162 113 082 135 123 071 200   . 005 003 207 105 190 043   .   . 036 064 101 062 140 204 019 122 077 088 132 073 065 137 161 042 126 091 030 224   . 205 021 035   . 078 146 159   . 089 212 115 221 157 107 188   .   .   . 050 053 009   . 178 080 110 086 108 010   . 152   . 020   . 150   . 179 173 223 187 189 127   . 210 172 104 063   . 052 057 214 051 171 037 202"
			//"105 130 003 071 093 123 200   . 043 005   .   . 190 207 082 140 088 137 036   . 073 065 062 122 019   . 161 204 064 101 035 078 153 089   . 205   .   . 021 042 156   . 030   . 224 133 009 178 046 212   . 053 080 201 188 157 221   .   . 049 152 179 098 223 110   . 010 108   . 173 150 047 041   . 020   .   . 052 057   . 051 210 069 127   . 104 037   .   . 214 206 079 186   .   .   .   .   . 192 147 151 213   . 109 096 044 136   . 163 094 208 081   . 074 026 131 219   . 197 084 174 040   . 012 033 120 138   .   .   .   .   . 024 061 059 039   .   . 027 034   . 016 072 076 006   . 001 125   .   . 100   . 124 087 175 166   . 203 193   . 015 068 170 117 211 017   .   . 085 090 182 000 092 145   . 180 031 139 054 103 164   . 116   . 048 191 183   .   . 222   . 142 185 220 097 184 011 095 083   . 002 154 028 013 148   . 155 038 060 217 118 177 023   .   . 004 158   . 143 119 165 144 070 134 162"
			//"062   . 101 204 122 137   .   . 088 132 140 065 019 064 161 224 021   . 091   . 126 078 035 156 153   .   . 146 159 030 046 178 107 212 133 221   .   . 188 115 049 050 053 157 080 209 108 179 010 098 110   . 041 223 173 086 020 150 152 187 127 210   . 069 104 189 037 057 202   .   . 214 172 171 051 079   . 198 018   . 129 206 151 109 096 007 192 147 186 014 208 163 181   .   .   .   . 026 197 160 219 022 094 131 081 120 121   . 040 024 075 059   . 033 045 061 176   . 169 174 125 029 027 034 076 016 032 006   .   .   .   . 001 039 072 193 211 170 117 166 068 100 015 124 099   . 196 216   . 203 025 139 085 145   .   . 199 054 017 092 182 114   . 000 103 149 112 218 194 097 128 116 191   . 048 220 222 142 185 164 154 002 028 011 184 038 083   .   . 148 013 095 217 155 066 004 118 119   .   . 177 158 144 165 023   . 055   . 167 070 130 113 071 135 106 190 082   .   . 207 003 123 093   . 005"
			//"046 157 053   . 049 221 201 115 188 133 080 178 107   . 212 041   . 020 086   .   .   . 010 187 110   . 098 150 108 047 037 210 104 063 202 051   .   . 052 127 171 102 214 189 172 096 018 206 192   .   . 014 079 151   . 111 129 186 109 213   .   . 160 219 094 044 197 026 081 084 181   . 163   . 136 040   . 174 045 120 121 075 176 061 059 024 169 058 012 033   . 029 034   .   .   . 076 006 008 168 215 032 001 039 072 100 117   . 175   . 170 015   . 124 099   . 068   . 087 216 092 090 000 031 085 025 054 139 180   .   .   . 199 017   . 149 164 220 116 222 112 218 128 194 142 183 048 191   . 185 011   . 013   . 066 148 184 217 002 038 195 060 155   .   . 177 167 118 165 067   . 144 143 134   .   . 023 119 070 158 082 113 123 130 190 043 105   .   . 071 003 207 093 135 106 019 036 064 062   . 077 161 137   .   .   . 065 088   . 122 091   . 146 078 224 153 089 126 021 159 030   . 156 035 056"
			//"  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 138   .   .   .   . 045 033   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 114   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 128   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 004   .   .   .   .   . 177   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 073   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 126   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 104 189   .   .   .   . 063   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   ."
			//"087 100   . 166   .   . 068 193 216 015 175 170   . 099 211 090 092 000 025   . 114 180 145 054 199   . 103 031 139 085 183 220 048   . 128 116   . 112 191   . 185 222 194 218 097 195 095 155 083 154   . 013   . 060 038 011 028 148 002 217 177 055   . 167 004 118 134 119   . 143 023 165 067 070 144 106   . 043   . 130   . 135 200 113 005 190 105 082 071   . 065 140 204   .   .   . 101 064 062 161   . 122 019 077 132 091 205   . 224 153 078 056   . 030 159 042 126   . 035 021 188 080 221 050   . 157 049 009 178   .   .   . 107 115 133   . 098 179 020 150 223 086 209 047   . 010   . 173   . 187 189 057 214 037 172 102   . 171 127 052 202 069   . 051 063 109 151 111 014 079 096 129   . 192   . 206 186 018 213 147 160 131 136 044 094   . 197 081   . 181 074   . 022 208 163 024 120 045 169   . 061 058 121 033 012   . 075 174 176 138 016 039   . 141 029 001 168 215 125   .   . 027   . 008 072"
			//"  .   . 214 102 171 051 069   . 052 202 172 210 104 057 063   . 198 129 111   . 151 206   . 213 007   . 147 186 018 014 197 208 094 160 081 136   .   . 084 131 022 181 074 044 163 059 045 075 169 058   . 033 040 176 174 120 121   . 061 138   . 141 168   . 001 016 008 006 072 125 034   . 029 032 027 175   . 216 099 100 117 170 068 193 015 196 087   .   . 124 180 090 031   .   .   . 085 139 145 103 114 054 199 017 182 218   .   . 097 048 220 128   . 194 142 149 112   .   . 191 038 066 028 148 013 011 217 095 155   .   .   . 184 002 195 177   .   . 144 023 167 118 162 165 119 134 004 143   . 070 130 207 003   . 106 071 190 093 113 043 005   . 135 123   . 077 073   . 101 140 132 137 088 062   . 065 204 064 122 161 089 042 205 091 153 021 035   .   . 146 030 159 156 078 224 107 157 009 046   . 115 212   . 053 050   . 178 188 201   . 086 152 150 179 041 110 098   . 173 108 047 020 187   .   ."
			//"008 016 076 034 032 027 215 039 125 072   . 141 001 006 168 175 216 117 100   . 068 170 087 203 196 015 211 166 099 124   .   . 199 103 182   .   .   . 092   . 054 031 085 025 090 128 142 220   . 164   .   . 097 112 191   . 116 222 149 185 002 155 154   . 184 011 083 095   .   . 148 013 066 217 028 067   . 143 119 118 144 055 167 177 162 004 134 158 023   . 135 106 071 043   .   .   . 207 105 082 200 093 190 113   . 036 137   . 140 019 065 132   . 101 064 077 073   . 062 088   . 224 205 146 030 091 156 159   .   .   . 126 153 042 056   . 212 178 221 050 201 157 133 053 009 046 107 188   . 049 086 108 047 010 041   .   . 187 152 173 209   . 179 020 098 127 069 189 214   . 202 051 052   .   . 210   . 057 171 063 147 109 129 111 007   . 192   .   .   . 014 018 213   .   . 094 044 026 197 081 131 160 136 074 181   . 208 084 219 022 120 061 012 075   . 024 058 176 174 045 033 121 138 169 059"
			//"145 025 085 031 054 000 114   . 092 182   .   . 199 139 103 097 191 116 218   . 112 220 183   . 048   . 164 222 142 194 083 155 184 154 195 028   . 060 038   . 217 148 013   . 066 162 119 055 134 158   . 165 067 167 143 118 144 023 177 070 113 135 082 200 190   . 105 207 005 043 071 003 106 093 123 140   . 088 064 036 137   . 073 077 132 019 062 161 204 101 078 224 146   .   . 205 030 159 035 089 126   . 153 042 056 157 221   .   . 107 178 133   . 053 009 115   .   . 046 188 173 041 020   . 047 086 187 108 179 098   .   . 110 152 209 127 063 210 051 102 069 189 202   . 057 037 104 052   . 171 111 018 014 192 079 186 007 213 109   . 096 151 206 129 147 131 219 044 074 163 081 136 084 197   . 208 181 026 022 160 058   . 121 120 024   . 169 059   . 012 033 045 138 075 040 001 016 006 008   . 039   . 027 076 034   . 141 125 215 032 100 193 166   .   . 196 211   . 216 099 124 117 203 087 015"
			//"010 086 047 150 187 020 223 152 173 209 041 179 110 108 098 172   . 051 189   . 069 210 037 171 104   . 063 102   .   . 192 206 007   . 096 129   . 151 198 109 213 186   . 111 079 081 026   . 197 160   . 074 163 219 084 044   . 181 131 022 061 075   . 176 024 120   . 045 059   . 012 033 040 138 121 029   . 125   . 016 027 141 215 039 072 001 008 168 034 076 170 175 166   .   .   .   .   . 087 211 068 203 196 193 015 025   .   . 090 199 180 182   . 085 139 017 114   .   . 092 191 097 116 222 194 218 185   .   .   .   .   . 048 149 128 002 154 155 028 148 060 011 195 013 095 083   . 038   . 217 118 119 165 134 067   . 004 070   . 143 162 167   . 144 158 113 200 130   . 106 005 123 043 105   . 135 071   . 093 082 161 077   . 036 019 088 062 132   . 204 101   . 122 065 140   .   . 159 035   . 042 089 205 030 146   . 078 021   . 156 157 115 050 178 080 107 212 201 188 009 053 221 049 046 133"
			//"083 011 013 148 217 028 060   . 038 195 066 155 184 095 154 067 143   . 118   .   . 055 134   . 004 162 158 023 119 165   . 135 190 082   . 123   .   . 043 113 093 071 003 130 106 132 064 065   . 161   .   . 140 073 088 036 137 204 077 122   . 078 089   . 153 091 035 159 056 021 146 030   . 156 205 080   . 188   .   . 221 178 201 115 133 107 046 212   . 053   . 041 150   .   .   . 047 108   . 098 223 187 110 152 209 189   .   . 172 104 210 202   . 214 057 127 069   .   . 052 198 079 129 186 014 111   . 018 206   .   .   . 007 109   . 131   . 208 136 181 219 044 081 074 026   .   . 084   . 022 120 045   . 169 040 012 024 138 061 174 059   . 075 121   . 039 215 016 076 029 072 027 125   .   . 141   . 006 032 168 211 193 117 100 196 216 087   .   . 166   . 099 203 170   . 199 025 139 145 182 017   . 000 085   .   . 180   . 114 054 218 149 222 220 097 048 164   . 191 142 194 116 185 183 128"
			//"035 091 030   . 156 205 126   . 021 056 224 078 153 159 089 080 188 221 157   . 201   . 046   . 107   . 212 050 009   .   . 179 110   . 209 020   . 223 173 152   . 150 047   . 041 202 057 210 037   .   . 214   . 069 052 189 051 102 127 171 109 206 147   . 007 111 192 018 096   . 186 014 079 213 129 163   . 084 026 044 136 208 219 131 081   . 197 160 181 074 075 040 012   .   . 121 033 045 169 058 176 138 024 061 059 016 027   . 029 001 141 072   . 076 006 039 215   . 008 125 216 175 117 166 124 100 203 099 170 211   .   . 196 193 015 017 103 180 000   . 114 025 182 085 139 145 199 092   . 054 218 142 194 183 097   . 048 185 149 191 128   . 220 116 164 002 060 011 013 066 195 028   . 083   .   . 148 095 217 154 158   . 144 118   . 143 134 162   . 023 165   . 070 055   .   . 130 207 105   . 113   . 123   . 071   . 135 043 200 093 036 077 204 065 140 019 161   . 088 064 101   . 122 062 132"
			//"  . 218 194 222 185 116 112 149 191   .   .   .   . 142 164 066 038 028   .   . 060 155 083 217 184   . 154 148 095 013 134 055 004 158   . 144   .   . 143 177 070 023   . 118 067 005 207 135 105 082   . 003 106 200 043 130   . 071 113 093 077   . 161 073   . 036 062 064 132 088 204 101   . 122 137 224 156 021 159 091 205 078 126 042 056 153 035 089 146 030   . 080   .   .   .   . 053 009 046 212 201 049 107 115 133 086 020   . 041 110 179 209   . 047 108 152 223   . 010 173 052 172 051 102 214 189 171 057 210   .   .   .   . 127   . 109 147 206 129 186 151 111 096 014 018 192 007 198 079 213 044 026   . 197 163 181 094 022 131 084   . 219 208   . 160 061 176 120   . 040 059 121 174 169   . 075 012 045 138 058 168 039   . 016 001 125 008   .   . 034   . 006 032 141 029 196 100 099 087   . 193 211 117 124 166   .   . 216 068 203 025 017   .   .   .   . 103 114 092 139 085 000 054 145   ."
			//"  . 094 208 074 163 081 181   . 160 026 197   . 219 084 213   . 058 059 024   . 012 121 061 040 176   . 022 033   . 075 039 027 215 138 006 072   .   . 168   . 029 076 141 001 008 099   . 117   . 032   . 170 087 166 211 196 015 124 100 175 025 000 203 031 114 199 017 092 139 103 085 180 145   . 182 183   .   .   . 048 128 116 222   . 142 112 149 054 194 220 028 083 013   .   .   . 155 038 002 185 148 066 060 011 095   . 162   .   . 167 144 119   . 055 143 118   .   . 177   . 082 105 005 003 135 190 106 043 123   .   .   . 200 130 207 036 093 137 132 101 204   . 064 065 088 077   .   .   . 140 153   . 078 042 035 030 126 224 091 089 159 146 205 056 122 157 050 107 178 046 009 133 212 115   . 221   . 188   . 156 049 086 209 110 223   . 152   .   . 047 179 173 041 020 010 069   . 052 127   . 189 187 202 210 214   . 051 063 102   . 007 111 014   . 192 151 171   . 147 198 206 096 079 109   ."
			//"  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 148   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 118   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 064   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 102   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 181   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 045   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   ."
			//"127   . 210 214 172 202 102   . 063 057 037 051 069 052 187 192 147 096 007   . 186 129 109 079 151   .   .   . 198 206 131 136 219   .   . 081   . 181 160 044 163 074 208 094 197 045 174 121 061 022   . 075 169 012 058   . 059 033 120 040 016 027 138 034 215 001   . 125 006 168 076 141 008 029 072 087   . 211   . 196 015 117 166 100 099 068 193 032 124 170 000   . 085   .   . 182   .   . 017 203 031 090 114 025 139 048 128   . 183 112 116 142   . 220 191 218 222   . 149 164 154 083 195 013 155 184 066   .   . 185   .   . 060   . 095 118 217 144 162 165 023 004 119 055 143 177   . 158   . 067 190 043 135 113 105 003 200 106   . 082 207 071 123 005 070 036 204 019 065   . 064 132 161 077   . 137 101 088 140 093 122 091 056 153 126 089 042 159   . 030   .   . 224 205 035 201 107   .   .   . 157 156 133 178 053   . 221 212 050 080 110 086 047 020 010 223 049   . 098 173 179 209 041   . 108"
			//"115 107 178 053 080   . 050   . 212 009 046   . 201 188 156 010 098   . 110   .   . 020   . 041 223   . 049 047 173 179 127 051 069 187   . 202   .   . 063 189 172 214 210 104 037 018 198 129 109   .   . 206 192 186   . 007 096 014   . 079 044 136 213   . 219 094 131 084 026 160 074 208 197 163 081   .   . 058 174 024 059 121 012 120   . 176 061 022 033   . 027 008 076   .   .   . 141 125 039 138 034 029 215 016 006 196 015   . 087 068 117 099   . 170 216 100 166   . 193 211 103 145 182 085 180 199 090 092 000   .   .   . 114 025 139   . 054 116 128 194   . 048 142 220 191 149 112 164   .   . 184 038 155 002 083 013 060 066 011 154 095   . 028 195 185 118   . 004 055 134   . 162 158 177   .   . 165 143 067 217 070 130 005 190 200 082 113   .   . 003   . 043 106 123 105 073 019 088 077   . 036 093   . 065   .   . 137   . 204 140 153 091 030   . 035 126 122   . 089   . 078 056 224 042 159"
			//"002 184 155 013 066 195 148 011 154 095   . 028 060 038 185 134   . 162 004   . 023 144 177 067 167 119 217   . 143 055 113 123   .   . 207 005   . 071 082 130 106 003 135 190 105 064   . 137 077 093   . 065 062 204 161   . 132 101   . 140 091 205 122 146 126 153 042 021 159 089 030 078 035   .   .   . 080 212 188 107 133 221 050 157 009 201 115 156 053 178 020 010 047   .   . 209 179 173 152 049 150 041 223 086 108 104 202   . 037 069   . 057   . 210   . 189 102   . 127 063 147 192 096 014 206 007 079 198 129 171   .   . 151 111 018 044 213 136 081 074 181 094 026 208 084 131 219 160 197   .   .   . 075 061 169 033 176 040 120 058 045 012 121 059 022 016   . 001 141   . 006 072 168 039   . 027 076 125   . 138 032 100 015 196 068 211 193 099   . 124 170   .   . 117 087 114 199   . 017 139 025 203 182 180 085   . 000 103   . 090 048 218 194 116   . 112 054 222 164 191 220 128 097 149 142"
			//"152 110 179 047 041   . 150 086 098 108 010 020 223   . 049   . 063 202 104   . 102   . 127 172 069   . 187   . 052 210 109 129 151   . 018 096   .   . 147 111 079 014 206 007 192 026 084 136 131 213 219 208 197 181 160 094 081 074 044 163 120 121   .   . 176 024 061 174 045 058   . 075 169 040 059   .   . 168 125 001 072 027 034 016 006   . 039 138 076 141 117 087 124   .   .   . 170 216 193 032 166 175 068 100 099 199 182   . 145 114 000 139   . 180 092 025 031   . 017 103 164 183 128 194 220 048 097 191 116   .   .   . 112 218 142 011 185 028 195   . 148 184 095 155 038 002 060 154   .   . 004 143 055 177   . 165 167 067 118 158 119   .   . 162 217 130 071 190 135 105 207 005 082 113 200 123 003 043 106 070 093 036 132 019 073 161 077   .   . 101 065   . 140 137 062 126 153   . 042   . 091 122 056   . 030   . 205 089 146   . 107   . 053 221 046 201 156 050 212   . 178 133 080 115 009"
			//"113 190   . 003 106 005 071   . 082   . 105 123 200 043 070 062   . 132   .   . 204 137 077 140   .   . 093 101 088 065 042 205 126 122   . 056   .   . 089 091 224 030 078   . 035 009 188 221 115 156   . 178 046 050 212 107 133   . 157   . 086 020 049 150 223 110 152 173 108 098   .   .   . 041 209 037   . 063 052 104 202 051 102   . 057 069 127 187   . 210 129 192 014   .   .   . 206 198 109 171 186 079 151 111 018 094 081   .   . 219 136 026   . 208 084 044   .   . 131 160 058 169 059 033 075 024 040 174 121   .   .   . 176 120 045 016   . 027 072 076 034   . 006 141 125 039 215 168   . 029 196 216   .   .   . 124 068 175 100 211 099 166 117 015 032   . 031   . 180 145 139 182 103 017   . 000 085 092 090 203 054   . 128 048 112 164 149   .   . 194   . 191 097 116 183 060 184 038 002   .   . 185 195 155 013   .   . 154   . 066 004 118 165 144 134   . 217   . 158 143 055 162   . 177 119"
			//"193 196 170 124 175 015 166   . 211 099   . 117 068 216 032 145 103 182 199   .   . 000 017 090 114   . 203 085 092 180 149 116 112 054 142 128   .   . 164 218 097 194 220 048 183 095 038 028 002 185   . 155 083 148 154 184 195 013 011   .   . 144 217 023 167 004 177 143 119   . 165 055 134 067   . 105   . 082 043 190   . 123 071 130   . 200 113 070 003 135 137 062 101 161   .   . 065 088 077 093   .   . 073 036 064 153   .   . 035 126 205 159   . 078 021 091 146   .   . 089 212 046 133   .   . 107 080 188 221   .   . 050 201 157 009 086 049 020 209 047   . 110 108 179   . 152 223 098   . 041   . 052 210 127 037   . 069 172 189 063 057 102 051 202   .   . 186 007 206 192 018 096 147 109   . 129 014 198 079 171 213 044 081 094 219 160 131   .   . 074 208 084 163 136 197 176 024 174 061   . 120 022 059 075   .   . 121 058 012 040 001 016 076 027   . 215 138   . 168 125 141 072 029 039 006"
			//"149 048 220 194 097 128 222 218   . 142 183 116 112 191 054 083   . 195 184   .   . 028 002 066 060   . 185 013 038 155 177   . 167 217 119 162   .   . 158   . 067 165 055   .   . 207 043 123 113 070   . 135 105   . 082 190 005   . 130 106 036   . 093   . 073 019 077   . 064 161 101 065 062 140 132 035   . 089   . 153   . 205 146 091 159 126 042 122 030 078 221 046 053   .   .   .   . 188 115 156 050 080 201 157   . 110 209   . 010 223 020 108   . 179 173 086 150   . 152 098   . 037 202 214 210 104 172 052   .   .   .   . 069 189 057 111 171 129 096 014 186 007 018 206   . 109   . 147   . 079 094 084 208 131 197 074 219   . 044 160 026   . 136   . 213 120 012   . 075 169 045   . 058 061   . 121 033 174 040 022   .   . 072 001 215   . 039   .   . 076 141 125 029   . 008 068 196 216 193   . 100 032 015 170   .   . 117 211   . 175 199 025 085 000 145 114   . 031 103 092 180 182 090 017 139"
			//"039 001 141 076   . 072 034   . 168 006   . 027 215 125 138 087   . 015 196   . 166 117 193 175 068 099 032 124 216 170 017 000 114 203 139 182   . 031   . 025   . 085 180 199 145 142 191 116   . 054   . 220 183 222 164   . 128 194 218 097 011 028 185 148 060 184 002 038 095 154 013 155 083 066 195 134   . 158 143 004 162 144 023 118 119 167 177 217 165   . 123 105 003   .   .   . 135 043 113 070 071 106 200 130 207 019 132   . 062 073   . 064   . 065   . 036 204   . 077 161 089 035 056 030 078 153 224 021 205   .   .   . 126 091 159   . 156 221 133 053 050 107 009 178 188 115 201 212   . 080 110 173 179 152 010 047 223 041 086 098 108 150 020 209 049 189 102 104 210   . 057 202 063 127   . 051   . 052 172 187 171 111 096 007   . 147   . 018   . 014 206 198 079 129 192 219 094 084 131 026 044 213 081 208 074   . 136 160   . 163 024 120 033 121   . 176 022   . 058 174   . 059 040 061 045"
			//"177 004 055 165 067 162 023   . 158 119   . 144 167   .   . 105 082 005 190   . 071 123 113   . 200 207 070 003   . 135 077   . 073 093 064 132   .   . 161 036   . 101 065 019 062 159 021 205 042 122   . 078 035   . 089 153 056 030 091 224 157 221 156 050 201 107 115 188 009 212 053 178 046 080 133 010   . 098 173 110   . 020 150 086 108 223   . 049 047 179 051 037 214   .   .   . 210 052 127 187 102 172 069 189 057 007 096   . 192 151 129 018   . 206 198 111 186   . 109 147 160 197 081 074 208 094 163 084 136   .   .   . 219 044 026 120 022 121   . 033 012 024 045 075   . 061 176 058   . 040 001 125 141 039 008 076 215 029 016 168 006 034 027 072 138 100 166 196 170 087 099   . 211 193   . 117 124 216 175 032 203 025 182 199   . 103 017   .   . 085 180 092 090   . 145 112   . 191 149 142 218   . 128 220 194   . 116 164 222 097   .   . 013 028   . 060 185   . 154 038 155 195 066 002 095"
			//"109 007 206 014 079   . 186 111 147 018   . 129   . 198 171 197   . 081 094   . 181 136 131 163 219   . 213 074 084 208 061 121 176 022 045 059   .   . 058 120 040 033 075 024 169   . 125   . 039 138   .   . 008 034 168 001 072 076 016 029 100 117 032 166 068 196 193 216 099 211 124 170 087 175   . 145   .   . 092 199 182 000 031 025 139 114 017 203 085 180 116 183 194   .   .   . 220   . 149 054 222 097 112 218 142 184 195   . 083 060 028 095   . 155 038 011 148   . 002 154 158 134 162 165 055 004 067   . 144   .   .   . 167 118 119 130 070 123 005 003 071 190 207 135 043 113 200   .   . 106   . 088 065 077 062 101 073 140 036 161 064 204 137 132 093 091 146 153 078 035 159 056 089   .   . 205 030   . 224   . 156 157 133 107 201 212 115   .   . 053 178 188 080 221 046 223 110 173 152   . 086 049 209 179 047   . 020 098   . 041 104 189   . 051   . 069 187 102 063   . 210 202 172 127 057"
			//"042 153 078 030 224 056 146   . 089 159   . 205   . 021 122 046 212 133 107   . 050 221 115 080 201   . 156 053   . 178 152 020 223 049 108   .   . 150 098 086 041 047 179 110 010 057 052 051 127 187   .   . 037 102 063 104 202 214 189 172 111 129 171 186 151 007 109 198 018 147 014 206 192 079 096 197   .   . 084 094 081 136 181 044 026 219 131   . 074 208 121   . 033   .   .   . 075 174 061 022 012   . 176 120 045 001 072   . 008 215 027 006   . 141 125 016 034   . 039 168 211 087 015   . 170 196 175 216 117   .   .   . 068   . 099 025 203   . 182 085 031 199 139 180 092 017 114   .   . 090 048 191 220 149 183 194 112 097 218 164 142 222 116 128 054 011 148 184 155 083 095 195 154   .   . 028 013 038 066 185 217 118 162 004 167 158 177 119   .   . 055 143 067 144 134 200   . 043 113   . 130 070 005 135 003   . 123 082 071 106 019 036   . 137   . 073 093   . 161 088 065 132 140 077 064"
			//"061 024 075 033   . 059 012 120 058 045 169 121 176 174   . 008 168 072   .   . 034 027 039 029 215   . 138 076 125 141 193 117 068 032 099 015   . 166 211 100 175 124 170 196 087 139 092 000 017 203   . 180 145 031   . 199 182 085 025 090 218 116 054   . 112 048 149 191   . 164 194 220 183 097 128 083 066 154 038 184 195 028 148 011 095 060 002 185 013 155 144   . 165   .   .   . 055 143 177 217   . 067 167 118 119 190 005   . 105 200 123 207   . 135 043 130 071   . 113 082 161 062 132 101   . 019 140 088 137   .   .   . 073   . 064 091 122 205 056 030 146 153 159 078 021 042 126 089 035 224 107 188 178 115 046 053   . 080 157 212 009   . 221 133 156 086 150 110 179 010   . 209 098 152   . 020 047 173 041 049 187 189 202 104 069 063 127 057   . 214 210 052 172 051 037 151 007 198 109   . 111 171 096 206 014   .   . 147 186 079   . 044 074 136 197 219 213 181 160 084   . 081 163 131 026"
			//"077 019 065 101   .   . 204 036 161 064 062 137 073 088 093 035 089 056 153   . 146 205 042 224 126   . 122 030 021 078 115 221 201   . 009 133   .   .   . 157 080 053   . 107 046 108 173 020 152 049   . 179 010 150 098 110 209 047 086 041 189 051 187 102 069 104 127 052 057 063 214   .   . 172 202 192   . 147   . 007   . 129 186 111 018 151 109 171   .   . 136 197   .   .   .   . 208 084   . 213 181 163   . 044 026 024 059   . 169 176 121 045 040 075 174 120 012   . 061 058 168 008   . 076 141 001   . 125 027   .   .   .   . 016 006   .   . 117 015 124 166 196 099 170   . 193   . 211   . 175 199 092   .   . 145 085 114 090 025 103 139 031 000 182 203 218 222 048 220 183 142 128 164 149   . 116 194 191 097 054 185 011   . 184 060 154   .   .   . 013 155   . 066 028 083 167 004 143 177   . 118 217 162 055 165   . 144 158 023 067 190 130 003 123 105 200 070 071 082   .   . 005 106 113 207"
			//"118 167 144 055 134 119 165 004 217 143   . 162 023 158 066 113 070 207 200   . 003 005 130 105 071   . 067 135 082   . 036 132   . 106 088 064   .   . 093   . 062 065 137 073 077 021 089 056 091 140   . 205 042 030 122 126   . 078 153 035 107 133 224 053 050 201 157   . 188 156 178   . 115 046 009 152   . 049 098 223 108 209 047   . 173 150   . 080 179 020 202 127 210   .   .   . 051 063 189 041   .   .   .   . 052 151 018   . 109 186 096 198   . 129 147 007 014   . 111 171 213   .   .   .   . 219 197 160 081   .   .   . 181 094 084 024 163 059   . 075 033   . 174 121 058 120 012 022   . 169 215 168 027   . 039 141 034   . 001 138 125 076 072 006 040 196 124 068   . 193 216 099 032 100   . 015 170 211 087 029 175 199 139 114 031   . 025   .   . 180 000 103   . 182 017   . 112 164 218   . 048 090 142 116 220   . 128 054 194 183 060 184 155 195   . 148 097 013 185 154 028 095 083 011 038"
			//"  . 114 000 180 145 139 085 199 203 092   . 182 031 103 175 149 054 142   .   . 194   .   . 183 222   . 090 220 164 116 011 195 148 097 038 095   . 013 185 184 083 155 028 060 002 143 158 162 118 066   . 144 177 165 217 167 119 055 004 134 190 005   . 003 071 200 130 082 043 070 135 123 113 105 207 077   .   . 161 073 064 132 101 019 088 204 036 106 065 137 056 042 078   .   .   . 205 089 091 140 030 035   . 153 021 201 009   . 115 050 133 188   . 221 212 107 053   . 157 156 049 152   . 179 020 223 010 098 209   .   .   . 150 110 173 104 041 202 057 210 214 069 052 051 063 189 102   .   . 037 151 147 129 111 109 206 186 192 007 171 198 014   . 018 172 094 074 219 136 131 084 026 213 044   . 081 208 160 197 079 163 024 045 176 012 022 120 174   . 075 121 058 169 059 061 034 215 168 016   . 001 040   .   . 141   .   . 138 076 008 068 196 170 015   . 166 029 124 032 211 117 099 087 100   ."
			//"036 073 137 065 062 064 101 019 093 088 077 132 204 161 106 042 122   .   .   . 030   . 091 035 146   . 140 078 089 205 157 133   . 224 188 009   .   .   . 107 046 178 221 201   . 173 098 209 086   .   . 020 152 047 049   . 108   . 110 010   . 202 041 214 102 069 189   . 052 187   . 051   . 037 057 109   . 171   . 151 018 096 014   . 198 186 111   . 206 129 081   . 208   .   .   . 136 160 044 079 074 197 181 094 084 176 045   . 061 012 059 174   . 121 058 024 033   . 120 022 138 039 006 141 027 215 008 168 072   .   .   . 034   . 125 196 029   . 099 170 124   . 216 117 211 100   . 032   . 087 114 103   . 025   . 180 031   . 199 203 092 085 182 139   . 048 194   . 116   . 191 142 054 218   .   . 220 164 183 090   . 184 095 060 148 185   .   .   . 155 028 154   . 195 002 023 167 158 118   . 004 066 119   . 055   .   .   . 165 134 200 190 135 005 113 071 067 003 070 082 123 207 105 130 043"
			//"120 176 121 075 169   . 033   . 022 174 061 059 012 058 163 039   . 006 215   .   .   . 016 008 034   . 040 141 168 027 100 015 166 029 216 099   .   . 032 196   . 170 117 068 193   . 103 182 025 175 031 000 017   . 203 114   . 180 199 145 048 128   .   .   .   . 218 164   .   . 220 116 149 183   . 002   . 185 154 060 095 195 013   . 038 148 011   .   . 028 162 177 055   .   . 119 144 158 118 066 165 134 023 004 143 200   .   . 113 071 005 043   . 123 082 190 003   .   . 070 093 077 064 065 137 073 062 161 132 106   .   . 204 019 088 153   .   . 159 078 030   . 021 205 089 091 146 122   . 035   . 212 221 157 115   .   . 046 107   .   .   .   . 009 224 110 047 223   . 152 173   . 049 086 150 209 179 098 010   . 041 104 057 069   . 187 189   .   . 210 051 063 037 202 127 186 151 147 111   . 007 172 018   .   .   . 096 171   . 192 219 094 208 081 131 181 079   . 213   . 136 026 197 044 084"
			//"130 200 123 135 105 207 003 190 070 043 113 005 071   . 067 077 093 064 073   . 101 132 036   . 204   . 106 065 161 137   . 056 146 140 021 159   . 030 122 153 035 078 205 126 042 188 212 133 157 224   . 221 115 053 156 201 009 178 107 046   . 209 080 047 150   . 086 098 173   . 179 020 152 010 108 127   .   . 063 069 057 202   . 104 052 102 189 041 210 051 096 109 206   .   .   . 129 147   . 172 014   .   . 007 198 219 026   .   . 181   . 084   . 136   . 094   .   . 044 213 022 061   .   . 121 176   . 058 059   .   .   . 012 024 174 001 040 072 006 141 076 215   . 027 168 016 034   .   . 008 068 211 117 100 193   . 166 087 196   . 216 124 015 099   . 199 085 114 000 017 092 139 203 025   . 182 180 103 145 175 090 048 142 112 222 054 218 191   . 220 116 164 183 128   . 148 060 154 011   . 184   . 095 028 155   . 195 185 013 083 167   . 055 162 177 023 066 165 217 158 144 119 134 118 143"
			//"111 151 129 206 192 018 014   . 171 198 109   . 186 147   . 131 213 026 219   . 074 081 044 197 181   . 079 208 160 136 120 059 012 163   . 045   .   . 022 024 169 075 121 176 061 125 168 072 016 040   . 027 039 076 138 215 006 141 001 008 196 015 029 124 166 068 100 211 216 032 170 117   . 087 099 017   . 203 103 114 139 182 085 199 092   . 025   . 180   . 128 149 220   .   .   . 116 164   . 090 194 183 222   . 191 060 095   . 002 148 195   .   .   . 154 184 013   . 011 185 217   . 119 055 144 167   . 158 162   .   .   . 023 004 143   . 067   . 207   . 003 200 043 123 082 130 071 070   . 105 073 161   . 036 077 065 204 062 019 093 088 101 132 064 106 153 030 126 205 042 021 159 122 091   . 056 078 089 035 140 224 107 009 201 050 156 157   .   . 178   . 212 046 133 115 150 223 098 086   . 110 080 108 020 179   . 209 049 047 010   . 104 210   . 127 102 041   . 187 063 051 057 037 189 052"
			//"011 060 028 155 083 095 013   . 185   .   . 195 148   . 097   . 217 119 167   . 165 162   . 134 023 143 066 055 158 144 130 005 071 067 043 207   .   . 070   . 105   . 123   . 113 088   . 132   . 106   . 137 077 101 093 073 064 065 019 062 153   .   .   . 146 126 091 089 021 122   . 205 042 035 159 115   .   . 212 201 009 133 053   . 188 050 157 224 178 221   . 152 179   . 223   . 020 098 086 080 047 010 150 110 173 069 057   . 127 102 202 052   . 051 063 104 214   . 189 187 171 109 018 206 129 151 192 147 096   . 111   . 186 007   . 094 079 081 026 208 074   . 084 136 160 044 181   .   . 197 176 058 121 120   . 075 012 169 024 022 174   .   .   . 163 001 076 215 027 039 125 006 138 016   . 072   . 168   . 040 029   . 099   . 166   . 100   .   . 170 117 211 087 015 193 031 114 103 025 092 199 175   . 000 180   . 182 203 085   . 112   . 220 128   .   . 090   . 054 164 116 142 183 218 191"
			//"016 215 027   . 008 006 076   . 138 125 039 072 034 168 040 193 032 099 068   . 124 015   . 087 166 216 029 170 211   . 025 182 031 175 092 139   .   . 203 199 145 180 000   . 017 191 164   . 218 090   . 116 149 194 054 112 142 220 048 183 184 195 097 013 148 060 011 154 038 185 155 028 002 083 095 177   . 217   . 167 119 162 165 004 143 023 118 066 055 144 005 113 135   .   .   .   . 082 130 067 003 105 071 190 043 073 064   . 077 204 132 088   . 137 161 019 101   . 036 093 122 042 159 078 205 126 035 089   .   .   .   . 146 153 021 107 224 133 009 178 053 201 188 221 212 157   . 156   . 046 223 098 020 086 152 179 150 010 110 049 173 047 209 108 080 104 214 069 051 127 052 057 187 189   . 202 210   . 037 041 172   . 018 151 186 171 111   .   . 206 129 147 192 096 109   . 219 160 044 084 094 079   . 136 208   . 081 213 074 197 176 024 075 059 061 012 163   . 022 058 121   . 169 120 174"
			//"100 068 117 170   . 099 124   . 032 216 193 015 166 211 029 017 203 139 114   . 085   . 025 145 031   . 175 180 103 000 218 128 222 090 191 142   .   . 054 048   . 220 116   . 149 038 154 195   . 097   . 028 002 013 185 060 095 155 184   . 004 162   . 165 023 167 118 158   . 217 055 144 177 134 119 113   . 070 082 200   . 005 003 190 043 071 130 067 135 123   . 077 065   .   .   . 137 161 036   . 101 062 204   . 088   . 159   . 042 146   . 021   . 205   . 153 030   . 091   . 156   . 009 178 221   . 046 212 133   .   .   . 050 107   . 110 080 209 108 179 047 223 173 020   . 086 150 049   . 010 069 063 051 189 127 210   . 037 104 187 052 214   . 057 041   . 014 151 129 109 198 018 171 111   . 096   . 147 192 172 079   . 026 219   . 213 044   .   . 208 136 160 197 081 131 012 176 058 120   . 024 163 045   . 075   . 059 022 033 169 215 001 141 072 039 034 040   . 138 168   . 006 008 016 125"
			//"  .   . 116 220 183   . 194 048   . 191   . 128 222 164 090 002 185 095 060   . 013 195 011 083   . 038 097   . 154 028 118 162 023 066 143 119   . 165 217   . 134 055 144 167 177 043 082   . 130 067   . 123 113 003 070 200 207 135 190 105 019 132 106   . 204 073 036 161   .   . 065 137   . 062 064 042 035 122 089 126 159   . 030 153 021 146 091 140 078 205 133 115 178   .   .   . 221 212 157   .   . 046 050 107   .   . 108   .   . 150 209 173   . 020 098 110   .   . 086   .   . 127 057 210   .   . 037 063 202   .   .   . 102 104 052 007 172 096 018 206 014 151 198   . 147 111 186 171 109 192 219 160   . 044 131   .   . 197 094 213 084   . 081 026 079 024 033 176 121 061 174 045 022 120   . 059 075   . 169 163 040 001 006 215 034   . 016 125   . 141 027 168 008 072 039 166 068   . 100 216   . 029 099 117 170   . 015 032 124 087 114 199 180 182   . 031   . 085 203   . 000 139 145   .   ."
			//"189   . 051 210 037 057 214 104 187   . 127 202 102 063 041 109 171 018 151   . 014 096 111 192 186 198   . 206 147 129   . 081 181 079 084 026   .   . 213 094 197 208   . 219 131   . 058   . 120   .   . 121 061 033 022 176 045   .   . 169 001 072 040   .   . 215 016 168 125 138 141 027 039 008 006 193   . 032   .   . 099   . 124 196 216 166 100   . 170 117 182   . 180 203   .   . 000 103 025   . 085 145 031 199 092 112 142   . 149 222 128   .   .   . 164 048 194   . 218 054 185 002 095 155 028   . 083 154 195   .   . 013 148   . 038 004 066   . 119 055 165 167 143   . 158   .   . 217   . 134 200 082 123 130 113 135 071 105 190 070   .   . 005 207 067 019   .   . 137 077 088 064 093 036   .   . 065   . 062   . 140 153   . 126 146 122 091   .   . 078 205 089 035 056   . 050 201 212   . 188 107 224 009 221 178   . 133 156 053 046 223 110 179 209 152   . 080 047 049 098 020 108 010   . 173"
			//"157 201 221 178 046 009 053   . 156 188 115   . 050 212 224 152 049 108 223   . 047 209 086 010 150   . 080 179 098 020 189 202 102 041 052   .   . 214 187 104 037 210 051 069 127 198 147 096 111 172   . 129 109 014 171 151 018   .   . 192 094 081 079 074 181 219 044 160   . 213 208 136 131 197 026   .   . 022 058 176 045 059 033 024 174 012 120 163 075   .   .   . 141   .   .   . 027 168 016   .   . 008 034 001   . 068 099   . 193 166 015 216   . 117 211 196 124   . 100 032   . 017 139 180   .   . 145 103 182   .   .   . 031   .   .   . 090 128 142 220 194 112 191 116 164 218 222 054   .   . 060 154 028 011 002 155   . 083 184 185 038 013 195 095 097 004   .   . 144 177 143 119 217 118   . 162 055 158 134 066 067 190 207 200 071 070 130 043   .   . 123 082 105 005 113 204 073 161 036   . 019 106 064 137 065   . 132 093 101 062 126 153 078   . 042 146 140   . 122 089 205 159 035 091 021"
			//"086 223   . 179 010 108 047   . 049 173   . 209 150 098 080   . 187 057 069   . 214 202 189 037 102   . 041 210   . 051 111 096 186 172 198 018   . 014 171 007 192 206   . 151 109 084 160 081 044 079   . 136 131 074 213 219 026   . 094   . 024   .   .   . 012 176 120 058 174 022 075 121 061 169   . 039   .   . 168 215 006   . 076 001 125 034 016 040 141 027 015 193 170   .   .   . 117 211 100 029 124 087 166 196 216 114 139   . 017 031 182 092   . 000 103 199 085   . 025 203 054 149 142 220 116 112 183 164 128   .   .   . 222 048 191 184 097 195 095 155 013 060 038   . 154 011 148   .   . 083   . 158 144 118 177 055 023 134 004 217 143   .   .   . 066   . 003   . 123 113 043 207 070 130   . 005 135 082 105 067 106 019   . 073 204 093 036 088   . 065 137 161 062 132 077 146   . 089 091   . 153 140 159 205 078   . 056 122 030   . 201 107 178 133   . 050 224   . 156 212 221 009   . 157 188"
			//"044 219 136 208 197 026 074 094 213 084 131   . 181 160 079 061 022 045 176   . 033 059 120   . 012   . 163 075 058 121 016 072 034 040 125 006   .   . 138 001 008   . 027 215 039   . 211 015 100   .   . 117 193 124 032   . 099 170   .   . 199 182 175 085 031 114 025 103 092 203 180 000 017 145   . 149   . 054 164   . 142 128 194 048 191   . 218 090 220 116   . 002 155   .   .   .   . 154 011   . 013   . 148 184 038   . 119   . 177 023 162 143 134 144 158 004 165   . 118   . 070 113 207   . 123   . 105 082   .   .   .   . 071 190   . 019 106 132 064   . 101 073 088 137 161   . 204 093   . 062   . 089 205 091 042 078 146 035 153 122 021 030 056 159 140   .   . 201 221   . 188 009 156 157   .   . 178 212 046   . 080 110 108   . 150 049 086   .   . 179 020 098 010 209 152 102 069 063 189   . 104   . 057 051 210   . 202 187 214 037 151 007 206   . 109 186 172 014 171 147 129 018 192 111 198"
			//"091 126   . 078   . 159 030   . 122 021   .   . 146 089 140 115   . 009 201   . 053 133   . 046 050   . 224 178 212   . 086   . 150 080 173 108   .   . 049 110 010 179 020 223 152 052 063 202 189 041   . 051 127 214 187 069 057   .   . 037 007 096 172 014 186 151 111 147 198 171 206 129 109 192 018 131   . 213 160 219 026 081 074 094 084 181   . 079 208 136 059 061 075   .   .   . 121 058 120 163 033 169 012   . 174 215   .   . 039   .   . 125 008 027   .   . 076   .   . 138 032   . 099 170 117 068 087 211 015   .   .   . 166 196 216 199 175 182   . 180 085 114 092 000 103 025 031 203   . 145 112 164 116 218 149 220 222 183 048 054 191 194 128 142 090 184   .   . 028 002 038 095 185 011   . 195 155 154 083 097 066 004 119 167 023 217 118   .   . 055 144 158 134   . 177   . 200 082 130   . 190 067   . 123 135   . 005 070   . 105 073 019 065   .   . 204 106   . 093 161   . 064   . 036 088"
			//"007 186 096 129   .   . 206 151   . 147   . 018 014 171 037 044 079 084 181   .   . 026 094   . 074   . 192   . 213 081 024 045 033 197 058 174   .   .   . 176 061 121 059 012 120 168 138 006 001 169   . 072 016 141 040 034 125 027 215 039 068 099 008 170 124 166 196 032 211 029 117 015 100 193 216 025   . 175 203 031 092 139 180   . 103 085 199 087 000 182 142 218 116   .   .   . 128 054 048 145 220 149 194 112 164 148 038   . 011 013 095 154 002 195 185 060 155   . 184 097 066 118 143 144 162 023 177 217 119   .   .   . 165 167 158 200 134 207 043 123 135   . 082 005 070 190 003 067   . 113 204 093 132 019 036 137 101 077 073 106 161 065 064 088 105 126 078 146 056 091 089 021 140 153   . 159 205 122 042 062 035 201 188 050 053 224   .   .   . 221 133 156 115 009 157 047 150   . 110   . 223   . 173 209   .   . 108 080 179 152 102 069 051 057   . 214   . 210 041   .   . 052 127 104 063"
			//"199   . 182 000 017 092 180   .   . 103 025 139 085 203   . 218   .   . 222   . 220 142 048 149 194   . 145 116 054 128 184 095 013 183 154 038   .   . 097 060 002   . 195   . 011 158   . 119   . 083   . 162 118   . 066   . 143 144 167 177   . 207 134 135 003 071 190 070   . 067 123 005 130 113 043 036   . 106 093   .   . 064 065   .   . 101   .   . 137   . 159 091 205 140   .   . 056 122   . 062 078 042 030 126 089 050 188   . 157 053 009 212 115 133 156 201 178   . 107 224 080 086 173 020 209 150   . 049 108   .   . 179 047 223 098   . 010   .   . 051   .   . 063 202   .   . 214 041   . 127 186 171 096 007 111 129   . 109 151 172 147 206 018 198   . 219 208 181 081   . 160   . 079 094   . 026   . 213   . 192 197   . 174   . 033 163 024   .   . 121 059 022 061 045 120 076 034 138 001   . 215 169 125 072 027   . 006   .   . 039   . 068 117 099 100 124   .   . 029 032 015 216 193   . 211"
			//"110 150 209 020 152 173 179   . 080 098 086 108 047 049 046 189   . 052 102   . 210 057 104 127 214 063   . 051 187 202 007 018 014 037   . 198   . 206 172   . 109 129   . 186 111 160 213 026 094 192   . 081 044 208 079 181 084 136 219 131 176 045 197 075 033 012 024   . 058 163 121 059 120   . 174 016   . 040 138 034 125 006 141 215 168   .   . 169 027 072 099   .   .   .   .   . 015 032 196 008 170 193 124 068 211 031 092   . 025 085 139   .   .   . 203 114 180   . 199 175 090 218 191 116 128 222 149 054 142   .   .   .   .   . 164 060 183 095   .   . 155 148 154 195 185 184 013 097   . 002 023   . 162 004 118 144 165   . 167 066 158 055 119 143 083 200 135 071 005 130 082 043 067 190   . 207 123 070 113 134 105 073   . 204 101   . 019 161   . 137   . 093 077 064 036 030 146 122   . 089 126 062 021 056 205   . 159 140   . 042 050 201 221 009 157 053 035   . 224 156 133 188 115 107 212"
			//"024 012 059 121 061 174 075   .   . 058 120 045 033 022 197 016 040   .   .   . 141 006 001 039   .   . 169 027 138 072 196 099 124 008 211 216   .   . 029 068 193 117 015 166 100 103 203 139 199 087   .   . 025 180   . 031 092 000 114 017 112 142 145 220 194 222 048 054 164 090 116 128 218 149 191 011   . 097 185 148 038 095 155 060   . 013 184 183 028 195 119 118 144   .   .   . 162   . 004 083 055 177   .   . 158 071 043   . 130 003 207 082 113 005 070 200 135   . 190 067 106   .   . 137 132 204 077   . 064   .   .   . 101 073 161 126 062 159 021 205   . 146 089 056 122 153 030 140   . 042 050 156 133 107 157 221 053 115 201 224 212 178 009 188 035 223 179 150 209 086   . 173 080   .   . 108 020 049 152 046 010 069 052 102 214 041 104   .   . 051 202 187 127 057 189 014 186 171 007   .   . 037 198 096 129   .   .   . 206 109 181 219 136 026 044 074   .   . 079 213 081 084 131 094 160"
			//"196 166 015 117 193 216 170 068 029 211 100 099 124 032 008   . 175 092 031   . 180 139 199 017 085   . 087 000 203 182 048 142 194 145 164 191   .   . 090 112 149 116 128 222 218 154 185 095 184 183   . 195 011 155 097   .   . 028 060 002 167 119 083   . 165 023   . 217 158 066 144 162 118 177   . 130 113   . 070 071 043 207 135 200 082 003 190 134 123 005 064 036 137   .   .   . 132 093 019 105 065 077 101 073 161 146 021   .   . 030   .   . 042   .   . 126   .   . 153 140 224 157 188 221 133 050 115 156 009   .   .   . 053 201 212 223 046 108 173 020 179 150 098 209 049 110 047   . 086 152   . 187 202 104 189 051 214 127   . 041 063   . 057 052 010 151 206 186   .   . 147 198 172 007   . 018 129 171 109 037 192 219 084 181 074 079 094   .   . 136 081 213 131 026 044 033 012 022 024   . 176 197 174 059 121   . 045 163 075   . 034 215 027 006 016 076 169 141 040 138 072 125 039 001 168"
			//"019   . 132 137 077 088 065   .   . 161   . 064   . 093 105 091   . 021 146   .   . 159 153 042 030   . 062 205 122 056 107 009 053 035 212 188   . 178 224 201 115 221 133 050 157 098 049 108 110 046   . 209 086 179 080 150 173 020 223 152 069 057 010   . 214 102 104 187   . 041 051 202 189 127 052 111   . 172 171 186 198   . 206 151 147   . 007 037 129 096 026 044 136   .   .   . 081 213 094 192   . 131 074 219 160 012 174   . 120 033 045 058 061 059 022 176 075   . 024 163 040 016 125 027   . 034 039 138 006   .   .   . 076 215 168 068 008 099 216   . 170 166 211   . 032 196 124 029   . 193 031 203 182 199 025 000   . 017 114 175 103   . 139 092 087 112 220 222 128 218 164 191 090 048   . 142 116 054 149 145 183 060 038 148 013 097 184 154   . 028 195 185 002 095 011 165 023 217 004   . 167 083 143 162   .   . 119 066   . 177 071 200   . 207   . 003   .   . 067 070 005 043 113   . 082"
			//"001 034 072 027 039 125 141 215 040 168   .   . 076 138   . 100 029 216 166   . 170   . 196 193 124   . 008 117 032   . 199 139 085 087   . 092   .   . 175 114 017   . 182 031 025   . 054 142 048 145   . 128 218 220 090 222   . 116 112 149 060 095 183   . 013 148   . 185 154 097 028 195 011 002 038 118   . 066 217 023 143   . 055 167 158   . 004 083 144 162   . 130 123   .   . 043 005 070   . 134   .   . 003 200   . 204 088   . 036 101   . 161 077 132   . 073 065   . 019 106   . 091 021   .   . 146   . 122 159 062   .   . 030 126   . 201 035 009 188   . 178 050 212   . 156 107 053 224   . 115 150 049 209 110 086 020 047 152   . 080 098   . 108 173 046 069 210 102   . 189 063 052 041 104   . 057 051 187 127   . 037 151 198   . 014 172 007   .   . 129   . 171 109 018 111   . 181 213 094   . 219 192 084   . 136   . 026 079 208 131   . 176 121   .   . 033 197 075 163 022 059 174 061 024 058"
			//"048 222 128 116 149 191 220   . 090 164 218 142   . 054 145 011 097 038 148   . 155   . 184   . 013   . 183 028 185   . 004 119 165   . 158 143   . 055 066 167 177 144   . 023 118 082 070 207 190 134   . 005 130 135 067 071 043 123 200 113 073 064 105 065 101 204 019   . 161 106 137 132 036   . 088 091   . 140 122 146 021 159 078 126 089   . 153 062 205 056 009 157 221   .   .   . 133 156 107   . 178 115 053 201   . 150 173   .   . 047 108 098 152 209 049 223   .   . 110 080   . 189 052 051 202   . 127 187 057   .   .   . 214 069 063 151 037 018 198   . 206 186 147 096 171 007 014 172   . 109 181   . 081 094 044 136 074   . 219 079 160 208 026 084 192 176 075 012 059 120 058 174 163 024   . 045 121 022 061 197 169 215   . 034 076 040 001 168   . 027 072   . 039 006 016   . 166 032 196   . 068   . 216   . 117   . 099 029 170 193 031 114   . 139 025 085 087   . 175 203 182 092 017 199 103"
			//"107 050 133   . 115   . 178 201 224 212   . 009 053 156 035 086   . 173 150   . 179 108 110 152 047 098 046 020 049   . 104 057 214 010 063 052   .   . 041 069 127 051 202 102   . 147 171 018 007 037   . 096 111 206 172 186 198 129 151 109   . 026 192 208 074 181 094 213 160   .   . 081 044 131 084 120   .   . 022 012 174 045 075 176 058 033 024 197 121   . 006 016 027   .   .   . 072 138 001 169 141 039 076 215 168 166 216   .   . 124 099 211 193 015 032 068   .   . 196 029 175 025 092 000 182 031 017 203 139   .   .   . 085 114 103   . 145 142 191 116 220 222 164 128 054 048 194   .   . 149 148 185 195 184   .   . 013 002 060 097 154 155 095 038   . 167 055 023 162 118 158 143 066 004   . 119 144 217 177 083   . 200 043 071 003 067 190   .   . 123 005 070 113 207 130   . 204 093 019 161 073 105 088 132 137   . 064 106   . 077 146 126 205 159   . 030 062 078 140   . 056   . 042 153 089"
			//"190 071 005 123 113 043 135   . 067 082 130 207 003 070   . 036 106   . 204   . 065 064 019   . 101   .   . 137 093 132 153 159   . 062 089 021   .   . 140 126 042   . 056   . 091 212 156 009 107 035   . 133   . 178 224   . 188 221 201 115 223 108 046 179 047 150 110 049 098 080 020 209 086 152 173 189   . 041 187 102 052 057 210 069 063 214 104 010 051 202 018   . 129   .   .   . 096   . 007 037 206   . 014 151 147   . 084   . 044 074 026 160 131 081 213 219 208   . 094   . 163 120 174   . 059 012 061   . 045   .   .   . 033   . 058 215 169 006 125 027 141 034 168 072 138 001 076 040   . 039 166 032 015 196 100 117 124 193 068 029 211 170 099 216 008 114 180 031 182   . 103 092   . 199   . 139 000 203 017 087 145   . 191   . 194 090 048   .   . 116 128 054   . 142 218 013 148 185   .   . 060   . 038 195 028   . 095   . 155 002   . 167 144 119 118 165 083   . 066 217 162 143 177 004 158"
			//"184 148 195 028 002 038 155 060 097 154 011 095 013 185 183 118 066 143 023   . 055 119 004 177   . 158 083 144   . 162 190 207 003 134 082 043   . 135 067 200   . 123 005 071 130 161 093   . 019 105   . 132 036 065 106 204 088 137   . 077 126 159 062   . 030 146 153 122 089 140 205 056 091 042 021 157   .   . 156 050 188 009 178   . 212 053   . 035   . 133 108 086 020   .   .   . 209 049 110 046 179 152 047 223 098 102 052   . 189 214 057 063 127 202 187 069 210   . 104 041 172 111 198 129 096 186 109 171 018   .   .   . 014 151 147 219   . 026   . 136 208   . 160 081 213 094 074   .   . 131 012 022 059 024 120 121 033 061 176 163 058   . 045 174 197 215   . 034 072 016 168 125 040 001   . 006 027   . 039 169 008 068 216 166   . 029 196 211   . 117 015 032 193 099 100 085   . 203 199 103   . 087 092 182 000   . 139 175 180 017 222 112 116 142 218 194 145 220 090 054 128 191 149 048 164"
			//"104 102 202 051 127 052 210   . 041 063   . 057 214 187 010   . 172   . 186   . 206 018 007 109 014 147 037 129 171 096 094   . 074 192 160 084   . 208 079 219 131 136 081 181 044 058   . 045 024 197   . 059 120 075 163 012 174 121 176 061   . 006 169 141   . 034 001   . 168 040 027   . 016 039 125 100   . 029 032 166   . 099 170 068 211 124   . 008 117 015 139 025 000   .   .   . 182 203 199 087 180 017   . 114 103   . 191   . 218 194 142 164 149 128 054 112 220   . 048   . 097 011   . 028 195 148 002 185 095   .   .   . 013 060 154 167 083 119   . 144 055 023 158 162   . 004 165 066   . 177 071 070 005   . 130 123 003   . 200 067   . 135 207 043   . 073 065 204 132 036 161 088 106 019   . 064 137 093   . 105 062 126 021 146 030 140 153 089   . 205 056 122 042   . 091 053 050 156 107 212 201 035 188 133 221   . 009   . 178   . 150 223 020 108   . 047 046   . 080 049 209 173 152 110 098"
			//"  . 146 056 205 042 021 078   . 140 089   .   . 030 122 062 157   . 188 050   . 178 009 107 115 053   . 035 221 156   .   .   . 047 046   . 173   .   . 080 223 152 020 209   . 086 063 187 057 104 010 214 202 189   . 041 102 052   . 069 127 151 018 037   . 014 186 007 171 147 172 129 096 111 109 198   .   . 079 213 181 084 026 208 219 160 074 094 192 136 081 045   .   .   .   .   . 059 022   .   .   . 061 033 176   . 034 125   . 016 076 006 168 039 072 138 215 141   . 001 040   . 100 216 117   .   .   . 032 099   .   .   .   .   . 211 114 087 139 092 000 180 031 103 182 203 199 085 175   .   . 222 054 128 048 218 116 194 149 112 090 164   . 142 191 145 060 155   . 195 011 154   . 097 184 013 095 028 185 002 183 083   . 143 023 165 066 004   .   . 144   . 217 177   .   .   . 071 070 190   . 200 134 043 005 123   . 207 067   . 113 204 073 137   .   . 101 105   . 106 093 132 088 077 019   ."
			//"004 023   . 144 177 143 055 167 066 158   . 119 165 217 083 130 067   . 071   . 135 207 190 113 003   .   . 123 070 005 019 064 101 105   . 088   .   . 106 073 077 137 132 204 036 089   . 159   . 062   . 056 091 078 140 146 021 205 126 042 201   .   . 178 053 050   . 156 212 224 221 133 157 115 188 086   . 080   . 150   . 108 179 223 098 047 110 046 020 209 057 189 051   .   .   . 202 187 104 010 210   .   . 069 063 186 198   . 111 014 018 147 109 096 171 151 206   . 007 172 079 044   .   . 081 181 131 213 026   .   .   . 074 219 160 176 197 045 174 121 075 012 058 059   . 024   . 163   . 061 034 138 072 001 016 027 076 039   . 040 168 141   .   . 169 068 170 166 015 100 211 216 029 196   . 099   . 032   . 008 087 114 092 031 085 175 199   .   . 000   . 203 017 139 025 194 222 054   .   . 112 145 191 128 116   . 142   . 220 149 148 060 028 095   . 013 183 155 097 185 195 038   . 184 154"
			//"094 181 081   . 131 084 208   . 079   . 044 026 074 213 192 120   . 174 012   . 075 045 024 061 033   . 197 121 022 059 001   . 076 169 168 125   .   . 040 215 039 027 072 034   . 211 032 099 196 008   . 015 100 170 029 166 216 117   . 193 114 139 087 180 085 031   . 203 103 175 000   .   . 017 092 218   . 090 054 222 191 142 220   . 164 194 048 145 116 128 095 011 028   .   . 038 195 185 184 183 155 002 013 060 154 023   .   . 118 165 119 158 177 162 217 167 055   .   . 066 067 130 043 123 005 071 113 070 207 134   .   . 003 200 082 073 105 064 088 137 065   . 161 132 093 019 101 106   . 077 146 122   .   . 091 205 030 042   . 140 089 078 159 021 062 201   . 050 133 157 212 188 224 107   . 009 221 156 115 035   . 223 173 150 047 080 110   .   . 020 209 049 152   . 086 214 102 187 104   . 069 010 052 202 051   . 057 041   . 127 186 151 129 018 111   . 037   . 172 171 096   . 109 007 147"
			//"176 033 045 059 120   . 121 012 197 022 024   . 075   . 131   . 169 168 076   . 027 125 215 016 141   . 061 072 040 006 068 216 170 039 032   .   .   . 008 166 100 015 099   . 196   . 175 092 114 193   . 139 199 000 087 085 103 182 031 025 222 191 017   . 220 194 112 090 054 145 128 142 048 218 164   .   . 183 097 013 154   . 028 148 185 155 060 149 195 095   . 004 162   .   . 158 119 066   . 002 144 118 055 023 217 003 082   .   . 135 043 070   . 207 067 071   .   . 200 134 105 019 161 132 064 101   . 106 088 113   .   . 065 204   . 146 077 021 089 056 205 030 122   . 140 126 078 062   .   . 053 224 009 201 107 133 178 157 050 035 156   . 188 212 042 150 020 047 108 110 049 098 046 223   . 173 209 080 086   . 152   . 063 214 210 010 069   .   .   . 057 041 189 052 104 206 014 172 151   . 186 127 147 018 096   . 198 037 129   . 074   . 081   . 094 208 109 136 192   . 026 160 044 219 213"
			//"069 214 057 202 189 063 051 102 010 187 104 052 210 041 152 007 037 147 014   . 129 198 151 111 206 171 127 096 172   . 219 084 208   . 213 160   .   . 192 181 044 081 026 074 094 022 163 174 176   . 075 045 024 121 197   . 058 059 012 120 034   .   . 027 141 076 215 040 138 169   .   . 001 016 168   .   . 008 029 124 211   .   .   .   . 170 068 039 015 099 092 199 182   .   .   . 139 175 114 193 000 025 180   . 203 194 164   . 048   . 191 054 218 142 090   . 116   . 112 145 183   . 154 195 095 013 011 097 038   .   .   . 155 148 185 023 002 143 158 162   .   .   .   . 066 167 055 083   .   . 003 067 207   .   . 005 135 130 071 134 070 123   .   . 177 204 137 101 064   . 093 161 105 073 065   . 132 106 036 113 077 146 089 030 078 062 126   .   . 056 159   . 091 021 153   . 053 224 201 156 050 042 212 009 133   . 188 035 221 157 047 150 209 173 110 179 115 020 046 080 108 098 086 223 049"
			//"200 003 207 005 130   . 123   . 134   . 190 043   . 067 177 019 105 161 101   . 137 088 073 036 065   . 113 132 106 064 126   . 078 077 122 089   . 205   . 146 091 056 159 030 153 156 224 188 201 042   . 009 107 221 035 053 212 133 050 157   . 173 115 020 179 047 223 080 049 046 209 108 110 086 098 104   .   . 041 214 063 052 051 102 187 210 069 152 202 057 198 007 096   .   .   . 018 172 151 127   . 111 206 186 171 074 160   . 094 208 084   . 044   . 079 181 136   . 219 192 197 024 058 059   . 033 120 163 174   .   .   . 075 012 022 034 061 125 168 072 027 076 138 006 040 215 141   .   . 016 124 029 099 068 196 015 170 100 166 008 032 117 216 211   . 031 000 085 139 199 203 103 087 114   . 092 182 175 025 193 017 222 164 194 220 145   . 054   . 128 142 090 218   . 048 155 013 097 060   . 148 149 154 095 195   . 038 183 028 011 165 023   . 143 004   . 002   . 083   . 119 158 118 167 217"
			//"167   .   . 162 118 158 144   .   . 217   . 143 055 066 002 190 134 082 003   . 123   . 200 130 135   . 177 005 067 207 073 088 065 113 093 161   .   . 105 204 036   . 064 101 019 122 140 021   . 077   . 159 153 205 062 030   . 056 146 091 050 188 042 221 178 053 201 224 156 035 133 009 107 157 212 110 086 046 080 047   . 173 020 150 049 179 223 115 209 108 052   . 202   .   .   . 057 041 069 152 051   . 210   . 187 014 147   . 007 206 198 171 111 018 172 186 129   . 151 037 192   . 160   . 026 074 044 079 084   .   .   . 208   . 213 012 131 174 058 059 121 033 022 045   . 176 075 197 024 120 076 040 006 215 001 072 141 016 034 169 138 027 125 168 061 166 117 124   . 196 032 211 008 068   . 216   . 029 100 039 193 031 103   . 180 087 114   .   . 182 139 175 025 092 199 220 194 090 112   . 222 017 164   . 128   . 191 145 116 218 013 148 195 038   . 155   .   . 183 097 095 154   .   . 185"
			//"  . 124   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 116   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 144   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 091   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 051   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 213   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 215   ."
			//"215 076 006 072 016 168   .   . 169 138   . 125 141 040 061 196 008 211 124   . 117 216 068 100 170   . 039 015 029 099 114 092 180 193 203 103   .   . 087 031 025 182 139 085 199 054 090 191 112 017   . 142 048 116 145   . 164 128 222 218 148 038 149   . 155 013 060 097 185   .   . 095 184 011 154 004 118 083 066 165 158 143 144 023 217 055 167 002 162 119   . 190 005   .   . 082 207 067 200 177 123   . 135 071 070   . 161   . 019 065 088 093 036 064 106 204 137   . 073   . 062 153 089   . 159 030 091 140 021 077   .   . 078 146   . 050 042 188 212 133 221 053 156 009 224 201 178 035 107 157 047 080 108 223   .   . 179 086 150 046 049   . 173 098 115 102 051 214 057   . 187 063 010 069   . 052 202 041 189 152 127 186 147 014 206 037 151   .   . 096 018 172 111 198 007 208 074 079 219   . 181 109 160 026 081   . 084 192 136 044 033 012 059 174   . 075 131   .   . 163 045 058 120 176 022"
			//"060   . 095 195 011 154 028 148   . 185 184 038 155 097 149 004 083 158 165   . 144 143 167 118 055 217 002 162 066 119 200 043 135 177 070 082   . 123 134 071 130 005 207 003   .   . 106   . 073 113   . 064 019   . 105 101 161 132 204 036 146 021 077 205   . 030 126 140 122 062 056 159 153 091 089 107   . 035 224 053   .   . 221 050 156 178 201 042 133 009   . 110 209   .   .   . 108 080 223 115   . 086 179   . 049 214 063   . 104 210 052 187 189 057 041 102 051   . 069 010 037   . 147 096   . 014 111 172 198   .   .   . 206 186   . 181 109 084 160 081 136 074 213   .   . 219 208 192   . 044 033 163 045 176 024 059 075 120 012 197   . 121 174 058 131 034 027 076 006 001 138   . 169 215   . 125 072   . 016   .   . 166 211 124 170 008 068 032   . 015 099 029 100 216 196 180 085 175 114 203 031 193 103 139 182   . 092 087 000 025 194 222 128 191 048 220   . 116 145 090 142 164 218   . 054"
			//"  . 053 009 133 157 212 221 050 035   .   . 188 178 224 042   . 046 098 047   . 020 173 223 086 179 049 115 209 080 108 069 052 210 152 187 063   .   . 010 102 189 202 057 214 104 171 172 198 151 127   . 018   . 129 037   . 147 096 186 111 181   . 109   . 208 074 219 079 213 192 081 026 094 044 160 024   . 197 163 033   . 174 121 012 022 075 176 131 059 045 125 001 072   .   . 168 006 040 215 061 027 016 141 034 138 124 211   . 196 170 216 032   . 099 029 166 117   . 068 008 087 199 103 182 139 085 025 175 092 193   .   . 180 031 203 222 017 191 164 128 116 194 054 142   . 112 220 145   . 218 013 097 095 060 184 195 155 011 148 183 185   . 038   . 149 023 144 165 119   . 217 158   . 167   . 143 162 066 118 002 177 071 082 003 135 134 200   .   . 005 207 067 130 043 190 065 101 106 073 093 204 113 161 064 132   . 088 105 137   . 030 146 056 021   .   . 077 205 062 140 159 089 091 126   ."
			//"126 030 159 056 091 089 205 146 062 122 153 021 078 140 077 107 035 212   .   . 221 188 201 157   .   . 042 133 224 009 223 173 179 115 049 098   .   . 046   . 086 209 108 047 110 187 041 052 069 152   . 057 104 051 010 214 063 202 102 189 186 198 127   . 206 014 151 172 171 037 096 018 007 111 147   .   . 192 079 074 160 084 136 181 213 208 219 109 081 026 174 024 059   .   .   . 045 163 176 131 121 120 075   . 022   . 168   . 001 141   . 138 016 006   . 034 027   . 215   . 008   . 211 015 099 124 100 029 216   .   .   . 170 166 032 031 193 092 103 182 000 085 203 139 175 114 180 087   .   . 194 090 142 112 048 128 220 218 222 145 054   . 191 164 017 148 028 013 095 184 185 154 183 060   . 038 195 097 011 149 002 023 158 165 055   . 167   .   . 162 119 066 118 143 004 135 003 067 200   .   . 177 082 207 005   .   . 134 123 130 101 204 132 088 019 065 113 137 105 106 064 161 036 073 093"
			//"223 047 108 209 086 098 020   . 046 049 110   . 179 080 115 104   . 063 214   . 051 052 069   . 210   . 152   . 041 057   . 198 206 127 171 147   . 129 037 186 111   . 018 014 007 213 079 084 219 109   . 026 094 136 192 074 160 081 181 044 012 174 131 121 075 033   . 163 022   . 059   . 024 120 058 001   . 169 040   . 168 125 027 034 138 141 215   . 072 006 216 196 015   .   .   . 099 029   . 039 117 100 170 166 032 085 103   . 199   . 092 203 025 139 175   . 000   . 114 087 145 048 164 128 142 194   . 090 191   .   .   . 220 222 054 148 149   . 154 195 028 013 185 095 097   . 155 183   . 011 165 066 119   . 004   . 055 118   . 083 217 144 143 158 002 071 123 003 207 190 070 082 134 200   . 043 005 067 130 177 113 204 161   . 065 105 073 093   . 132 064 106 036 088   . 078 030   . 126   . 146   . 089 159 056   . 021 062   . 091 053 050 133   . 107 178 042   . 035 224 009 212 157 201 156"
			//"114 085 139 182 025 103 000   . 087 203   .   . 180 175 193 048 145 164   .   . 116 191 112 218 220   . 017 128 090 142 060 038 155 149 185 154   .   . 183 148 011 195 095 013 184 217 066 143 167 002   . 119 004 144   . 165   . 162 023 118 071 043 177 123 135 003 200 067 070 134 005 207 190 130 082 019   . 105 106 101 161 088   . 204 093 065   . 113   .   . 021 153 056 062   .   . 159 140 126 077 205 091   . 146 122 053 212   . 107   . 188   . 157   . 224   . 221   . 201 035 046 110   . 209 108 047 086 080 173   .   . 020 179 150 049   .   . 052   . 202 051 214   . 057 041 069 210 010   . 189 014 172 018 151 007 096 206 111 186 037 171 129 198 147 127 181 136 074   . 094   . 160 192 219   . 084 081 079 044 109 131 012 058 033 075 197 176   .   . 059 045 163 120 174 024 141 076 040 215   . 034 061 168 006 072   .   . 169 027 016 124 166 015   .   . 170 039   . 008 029 099 211 100 068 032"
			//"151 014 018 096 111 147 129   . 037 171 007 198 206 172 127 094 192   . 074   .   . 084   . 044 208   . 109 081 079 026 176 174 075 131   . 058   . 121   . 012 120 059 045 033 024 138 040 125 215 061   . 006 001 027 169 076 168 072   .   . 166 216 039 117 170 124 068 029 032 008   . 099 196 100   . 199   . 087   . 085   . 092 000   . 203 180   . 193 182 139 191 048 128   .   .   . 142 090 112 017 116 218 220 222 054 013   .   . 184 155 038 185 011 095 097 148 028   .   . 183 083 004 158 162 119 165 118 066 143   .   .   . 055 023 217 071 177 043   . 005 123   . 070 207   . 200   . 134   . 130   . 106 064 073   . 132 065 036 204 105 093 137 088 161 113   .   . 030 159 153 122 089 062 126   . 021 056 140 091 077 042 050 212 053 178 035   . 156   . 133   . 224 157 188 107 179 047 080 223   . 150 115   . 108   .   . 173   . 020 086 214 102 202 052 104 210 152   . 010 041 057 063 189 069 187"
			//"  . 194 142 128 218 164 116   . 145 054 048 191 220 090 017 184 183 154 013   . 028   . 060 011 155   . 149 195 097 095 167 143 055 002   . 158   .   . 083 023   . 162   . 165 004 070 067 043   . 177   . 207 190 123 134 003 082 005 071 130 204 088 113 137   . 101 073 106   . 105 132 064 019 036 161   .   .   . 140 030   . 021 205   . 122 078   .   .   . 159 188 107 133   .   .   . 009 224 201 042   . 157 178 050 156 047 098   . 110 179   . 049 086 108   . 150 020   . 223 046 010 104 063 202   . 214 189 041 052   .   .   . 210 102 187 186   .   .   . 096 129   . 171 018   . 151 206   .   .   . 074 079 026 219 094 081   . 044 181 192   . 136 084 160 109 012 121 033 045 024 022 058 197 176   . 174   . 163 120 131 061 034   . 076   . 169 215   .   . 072   . 040 016 125 001 170 124 029 068   . 166 039 211   . 015   . 216 008 117 100 085 031 182 092 199 180 193   . 087 175 139 103 025 114   ."
			//"073   . 064 132 036 161 137 204 105 093   . 088 065 106   . 153 062 089   .   . 205 021 126 091 078   . 077 056 140 159 201 188   . 042 156 212   .   . 035   .   . 133 009 053 107 049 080 173 223 115   . 108 110 020 046 047 098   .   . 086 102 052 152 051 210 214 069 041 187   . 202   .   . 189 063 007   . 037   . 014   . 198 129 186 171 206 151 127 096 018 084 094 081   .   .   . 026 079 219 109 136 044 208 181 213 033 058   . 024 075 174 022 120 045 163 012 121   . 176 197 169 001 168 072 006 076 016 040 125   .   .   . 141 034 138 166 039 216 211 015 117 124 032 099   . 068   . 008   . 100 085 175   .   . 199   . 180 025 031 087 203 000 092 103 193 222   .   . 142 048 054 164 145 112   . 191 128 090 218 017 149 148 154 013   .   . 060   .   . 195 095 097   . 038 184 055 165 066 167   . 023 002 158 119 162   .   . 083 144 118   . 071 005 043   . 135 177 123 134 067 207 082 130   . 070"
			//"  . 074 026 081 044   . 136   . 192 213   . 084 208 079 109 024 197   . 033   . 121 174 176 120 075   .   . 059 163 045 215 125 141 061 138 168   . 027 169 034 016 072 006 076 001 032 029 216 068 039   . 099 196 117 008   . 211 015 166 100 031 092 193 000 180 085 114 175 203 087 182   .   . 025 103 048 218 145   . 194 164 191 116 222 054 220 112 017 128 142 038 184 195   .   .   . 095 097 060   .   . 011 155   . 185   . 158   . 004 055 143 217 118 119 066 023 144   . 167   . 134   . 082 005   .   . 130 067 043   .   .   . 135 071 070 204 113 088 161 132 137 101 093 064 106 073   . 105 019 036 030 140   .   . 153 056 078 091 146 062 122 205 021 089 077 050 221 053 009   . 156 212 035 201   . 188 133 224 157 042 115 150 098 047 179 046 223 049   . 209 108 080 086 173 110 210 214 041   .   . 102 152 063 057 202   . 052   . 051 189 014 186 096 198   . 206 127   . 037   . 018 147 111 151   ."
			//"186 206 198 018 007 171 096   . 127   .   . 147 129 037 189 219 109 213   .   . 081 160   . 094   .   .   . 026 192 084 012 058 121 044 163 022   . 059 131 033 024 045 174 075 176 040 169 168 034 120   . 125 215 072 061 141   .   . 076 001 124   . 016 015 117 170 166 008   . 039   . 216 068 196 032   . 199 193 087 180 203 103 182 085 175   . 031 100   . 092 164 112 142   .   .   . 191   . 222 025 128 048 116 194 090 155 185   . 060 028 154 097 184 038 183 013 195   . 148 149 002 167 217 119 143 055 004   . 158   .   .   . 144 165 066 003   . 082 070   . 005 135 067 043 134 071 123 177 200   . 065 105 088 204   . 064   . 019 101 113 106 132 161   . 130 030 056   .   . 126 140 122 077 146   . 089 159 062 153 036 091 053 156 178 221 042 050 224   . 009 188 035 107 212 201 020 179 046   .   .   . 157   . 173 108   .   . 115 209 110 210 214 057 063   .   . 086   . 152 010 052 187 104 102 041"
			//"204 065 088 064 019 093 132   . 113 106   . 161 137 105 130 126 077 122 078   . 056 089 146 153 205 140 036 159 062   .   . 212 221 091 224 156   . 133 042 053 107 009 188 178 201 080 046 098 150   .   . 173 223 209 115 179 049 108 047 110 214 063 086   .   . 210 102 010 041 152 057 052 069   . 187 151   . 127 037   . 171 147 096 014 172 129 186   . 018 198 160 219 026 109   .   . 084 192   . 111 081   . 136 074 079 075 022   .   . 121 058 163 024 174 197 033   .   . 012 131 061 215 138   . 125 141   . 169 168   .   . 072 027 076 040 124 016   . 032 099 015 170 029 216 008   . 117 039   . 196 180   . 092 031 114 139 000 199 085 193   .   . 103 203 100 194 128 220 191 112 090 054 017 222   .   . 142 145 048 025 218 013 185 155 028 149 148 097   . 095 038 183 184 154   .   . 055 083 023 066 165 011 217 143 119   . 158 002 162 004 135 003 207 082   . 123 118   . 177 134 043 070 190 071 067"
			//"181 208 084 026 094 213 081 074 109 079   . 160 136 192 111 176 131 022 075   . 059 058 012 024 121   . 044 045 197 174 034 168   . 120 040 138   .   . 061 076 001 006 125 141   . 029 008 211 166   .   . 216 068   . 039 170 032 099 124 196 085 103 100 182 000 180 031 087 175   . 139 092 114 199 203 112   .   . 145 220 054 164 128 194 090 116 222 025 142 191 154 060 095   .   .   . 038 183 148 218 195 184 028 013 097 055   .   . 167 144 158 066 004 143 083 165 162   .   . 002 177 200 070 207 043 135 190 134 082   .   .   . 123 003 067 101 130 161 093 064 132 065 106 088 105 204 137   .   . 019 078 062 021 146 126   . 205 153 030 077 140 056 089 122 036 053 133 178 188 201 224   . 042 050   .   . 009 035 107 091   . 047 049 179 020 115 150   .   . 108 173 046   . 098 223 051 210 010 102   . 214 086 187 052 057   . 063 152 202 104 206 014 018 147   . 129 189 096 127 037 198 171 007 186 172"
			//"034   . 125 006 001 138 072   . 061 040 215   . 027   . 120 068 039 032 170   . 015   . 166   . 117 029 016 099 008 216 031 103   . 100 175 203   .   . 193 085   .   . 092   . 114 090 145   . 222 025   . 191 112   . 017 220 054 142 194   . 013 154   . 195   . 155 148 183 097 149 095   . 060 184   . 167   . 002 083 055 217   . 162 165 066 144 023 011 119 143 082 200 207   .   .   . 043   . 071 118 005 190 123 003 067 065 093   . 073 137 161   . 019   . 105 101 132   . 204 113 077 126 122 159 021 078 153   . 089   .   .   . 205 030 140 053 091 212 156 009 133 178 224   . 035 050 221 042   . 107   . 046 173   . 223 108 020 110 047 115   . 209   . 049 157   . 202 210 052 069 041   . 152 102   . 063 057   . 104 086 189   . 171   .   . 127 186   .   . 018 198 037   . 147 151 136 208 192 181 079 074   . 213   . 026   . 160 109 081 094 075   . 045   . 176 121 044   . 131 197 174 022 024   . 163"
			//"150 179   . 108 110   . 209   .   .   .   . 098 020   . 157 069   . 187 210   . 202 063 102 104   .   . 086   . 010 052 186 147 129 189 172 171   .   . 127 014 007 018 198 206 151 079 192   . 181 111   . 084   . 081 109 208 213 026 074 094 033 058 044 059 121 075 012 197 163 131 045   . 176 024 022 215   . 061 169 141 138   .   . 076   . 027 034 120 006 125   . 068 099   .   .   . 216 008 166 016 015 196 117 124 029 180   .   . 114 000 103 175 199 092 087 085 182   .   . 193 017 112 054 142 191 220 048 145 164   .   .   . 116 194   . 013 218 154 185 095   . 155   .   . 183 148 028 149   . 184 055 083 143   . 167 119 144 004 165 002 066 162 158 217 011 003 005 135 043 200 067 070   . 071   . 082 207   . 190 118 130 101 093 065 137 113 204   .   . 064 088 105 019 161 073 205 078   . 146   .   . 036 122 021 159   . 089 077   . 153 178   . 009 212   .   .   .   . 042   . 188 156   . 050 224"
			//"146 078   . 159 153 122 056 030 077 140 126 089 205 062 036 201 042 156 178   . 133 212 050 107 221   . 091 009 035 188 150   . 020 157 080 049   .   . 115 047 110 108   . 179 223 041   . 063   . 086   . 052 069 202 152 210 187 057   . 104 014 147 189   . 129 206 186   .   . 127 018 198 151 007 171 219   .   . 192 208   . 160 081 074 079 136 181 111 026 084 058 176   .   .   .   . 174 197 012 044 059 024 121 033 163 141   .   . 215 027 168 040 001 125 169 076 072   .   . 061 039 068 032 099 216 170 196 008 211   .   .   .   . 124 029 085 100 103 203 139 182 180 175 092   . 031 000   .   . 199 220 145 191 222 112 142   .   . 194 017 090   . 164 054 025 013   . 155 038 060 097 185 149 148   . 154   . 183   . 218 011 165   . 055 144 002 023   .   . 119 143 083 004   . 167 123 135 134 071   . 003 118 070 043 207   . 082 177 005 190 065 101 064 161 073 137 130 132 113 105 088 093   . 204 106"
			//"148   . 038   . 184 185   . 013 149 097 060 154 028 183 218 167 002 217   .   . 162 158 023 004 144   . 011 119 083 143 071 082 123 118 067   .   .   . 177 003 190 207 043   . 200 106   . 161 204 130   . 088 073 132 113 065 093 064 101 019 030 089 036 056 205   . 146 062   . 077 159 021 126 153 122 201   . 042 035 178 156 212 133   . 224 221 050 091 009 188 098 223 108   .   .   .   . 046 150 157 209 110 020 047 080 210 187   . 069 051 063 041 104 052 010 214 202   . 102 152 127 151 171 018 198 206 007 037   .   .   .   . 129 014 172 074 111 160 213 026 081   . 079 084 192 181 136 109   . 094 075 197 174 012 176 045   . 024 033   . 163 059 058 022 044 076 072 141 125 215 040 138 061 034   . 168 006 169   . 120 016   . 032 170 117 039 166   .   .   . 216 008 196 211 068 000 180 087 031   . 085 100 203 092 139   .   . 193 182 199 220 194 142 164 112 116 025 128   . 145 191   . 048   . 090"
			//"071 135 043 207 190 070 005   . 177   . 200 082 123 134 118 073 113 093 065   . 132 161 204 019 137   . 130 064 105 088 146 089 205 036 140 122   .   . 077 030 153 159 021 078 126 224 035 212 050 091   .   . 201 133 042   . 156   . 053 107   . 098 157 209 020 179   . 046 080 115 108 173   . 110 049 069   . 152 010   . 187 063 202 214 041 051 102 086 057   .   . 151 018   . 206 171   . 037 186 189 096 007 129 014 172 208 213   . 219 136 160 079 094 084 192 074 081   . 181 109 131 176 022 045 174 075 024 197   . 044 012   . 121 033   .   . 120 168 138 006 072 141 040 125 169   . 027 061   . 001 170 008   . 166 068 099 117 196   . 039 029 015 211 032   . 085 182   . 092   . 175 203 193   .   . 103 139 087 199 100 025 194 054 220 116 017 222   .   . 142 191 145 048 164 112 028 155 183 148   . 013 218 185 038 095   . 154 149 195 184 055 165 119 158 167   . 011   . 002 083 143 217 004 023 066"
			//"222   . 191 142 048 054   .   . 017 090   . 164 116 145 025 060 149 185   .   . 195 154   . 184 028   . 218   . 183 038 023 158 144 011 066 217   . 162 002 165 004 119 143 055 167   .   . 082 071 118   .   . 200   . 177 135 070 207   . 190 101 161 130 132 137 065 204 105   . 113   . 088 073 019 093 126   . 077 062 078 122 089 056 030   . 205 146 036   . 021   . 201   .   .   .   . 188 035 050 091 133 107   . 053 224   . 049   . 223 020 098 080   . 173 046 047 209   . 150   . 152 069   . 057 052 210 104 010 063   .   .   .   . 214   . 014   . 147 171 018   . 206 172 198 037 186 129 127   . 007 208 192 084 181   . 026   . 094 074 109 079 081 160 213 111 033   . 075 174 176 163   . 131   .   . 058 045 197   .   . 120 076 138 141 027 061 034 040   . 006 125 169 001 168 215 117 170   . 166   . 124 016   . 216 099   .   . 039 015 196 180 085 139 103   . 000 100   .   . 087 092 203 199   . 175"
			//"031 180   . 139 199 203 182   . 193   .   . 103 000 087 100 112 017 054 220   . 128 164 222 048 116   .   . 142 145 191 148 154 028 218 097   .   .   . 149 013 184 095 038 155   . 066 083 158 023 011   . 143   . 162 002 055   . 119 165 004 003 082 118   . 123 135   . 134 067 177 207 043 200 190 070 073   . 113 105 065 093 161 132 101 106 137 204 130 064   . 089   . 159   .   .   . 021 062 146 036 056 153 205   . 140 178 156   . 201 221 212 224 107 188 035 053 133   . 050 042 115   . 049 108 173 179 110 046 098   .   .   . 020   . 080   . 086 063 187 057 202 210 041 052 010 102 051 152   . 104 206 037 198 186 151 018 129 007   . 127 172   . 147 171 189 074 081 208   . 219 079 213   . 181   . 160 026 192 094 111   . 033 022 075 121 131 012   .   .   . 174 197 024 058 176 027 141 169   .   . 076 120 138 125 006   . 168 061 072 001 170 124 099 211   .   . 016   . 039 008 216 032   . 166 029"
			//"012 075   . 045 024   . 059 033 131 163 176   . 121 197 044   . 061 138 141   . 072 168 034 001 027 040 120 006 169 125   . 211 117 016 029 032   .   . 039 124 196 099   . 170 068 175 087   . 031   .   .   . 114 182 193 180 203 139 085 199 194 164 025 128 116 220 222 145 090 017 142 191 112 048 054 060   .   . 183 155 185 154   . 013 097 028 148 218 095 038 158 167   .   .   .   . 143 083 023 011 162 004 144 165 066 135 070   . 200 123 082 067   . 043 134 003 005   . 071 177 113 073 093 064 088 065 019 105 161   .   .   .   . 101 106 030 036 089 122 159 056 078   . 021 062 146 205   .   . 153 178 035 188 050 201 009 221 107 053 042 224 133 212 156 091 047 209 179 173 223 080 049 115   .   .   . 108   . 110 157 086 214   . 210 051 152 102   .   . 057 052 010 104 063   . 129 206 037 186 172 014 189 171 198 018   . 147 127 096   . 208 074 026   . 219 136 111 081 109   . 084 213   . 181 079"
			//"050   . 188   .   . 156 133 053 042 224 201 212 221 035 091 223 115 049 179   . 209 098 150 110 020   .   . 108 046   . 102   . 051 086 041 187   . 202 152 214 104   . 052 210 069 172 037 147 186 189   . 198 151 096 127 206 171 018 014 007   . 160 111 081 136 208 181 192 079 109 026 084   . 094 213 176   . 131 197 075   . 058 059 033 163 121 012 044 045 174 168   . 006   .   .   . 125 169 034 120 072 001 027 076   . 170 032   . 068 117 211 029 196 216 008 124 015   . 166 039   . 114 203 139 092 180 199 087 103   .   .   . 000   . 175 194 025 164 054 142 128 220 090 191   . 222 116 017   . 048 155 183   . 148 060 095 028 184 013 149 097 195 154 185   . 165 162 055 143 167 066 217 002 023   . 158 119 083 004 011 118 003 070   . 123 177 071 067   . 207 043 134 190   . 200   . 065 105   .   . 101 130 093 088 064   . 161 113 132 019 078 030 159 089 126 205 036 056 077 062   .   . 153   . 140"
			//"102 210 052 057 104 187 202   .   . 041   . 063 051 010 086   . 127 171   .   .   . 147 186 007 129   . 189 018   . 198 181 160   . 111 079 213   . 081 109 074 094 026 084 208 219 163 197 058 012 044   . 174 176 059 131 075 022 045 033 024 076 168 120   . 027 141 034 169 040 061 006 125 215 001 138 068   . 039 008 170 032   . 015 124 029 117 166 016 099 216 103 114   .   .   .   . 092   . 031 100 182 199 000 085   . 220 054   . 112 116 164 090 048 191 145 194 128   . 222 017   . 060 185 095 038 155 184   . 154   .   .   .   . 013 097 165 011 158 217 119 162 055 066   . 083 023 144 002   . 004 135 134 043 071 200 207 123 190 003 177 067   . 082 070 118 101 132 065 088 073 106 093 113 204   . 161 064 105 019 130 036 030 122 078 205 077 146 140   . 159 021 062   . 089 126 221   . 035 050   . 053 091 156 188   .   .   . 042 133   . 179 047 108 098   . 020   .   . 115 046 173 049 110 150 080"
			//"023 055 143 119 004 217 162   . 002   . 167 158 144 083 011 200 177 070 135   . 005 082   . 190 123   . 118 207 134 043 204 161 137 130 106 093   . 132   . 101   . 064 088 065 073 140   . 089 146 036 205 021 126   . 077 078 122 159 030 153 053 212 091 133 221 178 050 035 224 042 009 188 201 107 156 223   .   . 046 179 049   . 209   . 080 020 150 157   . 173   . 069   .   .   .   . 052 010 102 086 202 104 051 214 041 206 171   . 151 129 147 172 007 198 037 014 096   . 186 127 109 219 213 026 084 208 094 192 160   .   .   .   . 074   . 033   . 058 022 045 059   . 163   . 197 012 121   .   . 024 141 169 125 034 215 006 027 001 076 061 040 072 168 138 120 124 015 170 216 068 029   . 039 166 117 211 099 008   . 016 100 085 203 180   . 193   . 175   . 139 092 087 199 103 114 116 220 145 222   . 194 025   . 191 142   . 164 017 128 048 155 013 095 154 060   . 218   . 149 183 038 185 184 148 097"
			//"166 170 216 099 196 032 015 124 039   . 068 211 117 008 016   . 193 203   .   . 182 103 031 199 000   . 100 139 087 092 222 164 116 025 090 054   .   . 017 194 048 142 191 220   . 097 183 154 148 218   . 038 060 195 149   . 185 095 013 184 165 158 011   . 144 055 023 083 066 002 119 143 167 004 217   .   . 177 134 135 070 082 005 003 067 123 071 118 207 043 161 073   .   .   . 093 088 105 204 130 132 019 137 101 106   . 122   . 126   . 089 140 153 021 062   . 056   . 146   . 042 201 156 009 188 178 107 035 212 091   .   .   . 053 224 047 157 098 049 108 209 179 080 173 046 150 020 115   .   . 210 010 052 102 069 057 051 104 214 152 041   . 063 187 086 014 096 206 198   . 172 171 127 186   . 147 018 037 007 189   . 074 213 208 136 109 181   .   . 026 084 192 094 160 219 121 075 197 012   . 033 044 022 174 045   .   . 131 059   . 141 076 006 168 215   . 120 072 061 169 125 138 001 034 040"
			//"101 137 161 088 073 106 064 065 130 105   . 093 132 113 190 146 036 140 205   . 159 122 030 126 056   .   . 021 077   . 053   . 133 153 035 224   . 009 091 178 201 188 212 221 050   . 115 049 047 107   . 098 150 108 157 020 080 173 179 223 210 187   . 057 202 051 214 152 010 086 052 063 102 069 041 186 151 189 127 129 172 171 018 206   . 096   . 104   . 147 213 181 084   .   .   . 160 109 074 007 026 219 081 208 192 121 163   . 012 059   .   . 176   .   . 075 045   . 033 044 120 034 040 125 168 027 215 061 138   .   .   . 072 141 169 170   . 032   . 216   . 117 008 211 039 124 015 016 166 068 000 193 103 085 031 092 182 114 180 100 087 139   . 175 196 220 142 116 164 222 145 090 025 194   . 054 191 017 112   . 048 155 097 028 195 218 013 183   . 038 154 149 060   . 148   . 144 002   .   . 055 184 066 158 143   . 217 011 119 167 123 135 043 070   . 005 004 207 118 177 082 067 200 003 134"
			//"047 020 098 173 223 080 108   . 157 046 150 049 209 115   . 102 086 041 051   . 057   . 214 069 202   . 110 052 152 063 014 171 096 104 037 172   .   . 189 206 151 198 147   . 186   . 109   . 074   .   . 160 181 026 111 136   . 084 208 219 075 022 094 045 059 121   . 131 197 044 174 058 012 176 163 034   .   . 061 027 040 138 006 141 169   .   .   . 125 168 032 166   .   .   .   . 211   . 124 001 099 068 015 170 008   . 175   . 031 182 203 087 114 103 193 180 139   . 085   . 025 222 090 191 164 116 112   . 054   .   .   .   . 220 145 155 048   .   .   . 095 028 183 154 149 013 195   .   . 060 144 002 158 165 023 143 162 167   . 011 083 119 217 066 184 135 207 123   . 071 134 067 118 003   .   . 043   . 200   . 190   . 106 137 132 130 101   .   . 088 161 113 073 093 204 056 205 077 030   . 078 019 140   . 021   . 122 036 159 126   . 178 188 156 050 133 153   . 091 042 212 224 201 053 035"
			//"014 129 147 198 151 172 018 206 189 037 186 171 096 127 104 181   . 079 136   . 026 213 074 219 081 192 007 084 109 160 033 022 059 094 197 163   .   . 044 075 176 174 058   . 012 169 061 138 076 024   . 168 034 006   . 027 040   .   . 215 170 032 001 099 015 117 124 039 008 016 216 211 166 068 029 031 114 100 193 000 175 203 139 180 087 182 085 196   . 103   . 222 191   .   . 090 164 017 194 199 142 112 128   . 145 028 097   . 148 195 185 183 060 154 149 155 095   . 013 218 011   . 066 143 158 144 167 002 217 184   .   . 162 055   . 135   . 070 067 043 207 123 134 082 177 003 005 118 071 200 137 113 161 101 204 088 132 073 065 130 105 064 093 106 190 078   .   . 089 146   . 140 036 030   . 122 021 077 126 019 153   . 224 221 133 091 053   .   . 188 212 042 201 156 050 209 020 115 047 046 179 107 080 098 173   . 049 157   . 223 051 210 052 187 102 202 110 057 086 152 063 041 069 214 010"
			//"030 205 089 021   .   . 159 078 036 062 146 122 056   . 019 050 091 224 221   . 009   . 053 201 133 035 153 188   . 212   . 049 209 107   . 080   .   . 157 179 223 173 098 020 150   . 152 187 214 110   . 063 102   . 086   . 041 052   . 069 206 171 104 018 096 129 014 127   . 189 198 147 186 151   . 181   . 111 109 136 079   . 026 208 192 081 074 007   . 160 022 012 174   .   .   .   . 131 033 094 045 176 059   . 197 027 040   . 034 072 138 169 215 168 061 141 006   . 076 120 016   . 029 216 211 117 068 039   .   .   .   . 015 170 008 180   . 203 175 092 139 000 087   . 193 085 182 100   . 114   . 017 164 194 222 191   . 112 220 025 145 142 054 090 199 155   . 028 154   . 183   . 218 013   . 185 038 149 060   . 184 055 066 144 162 011 165   .   . 143   . 002 167 217   . 005   . 177 003 134 135 004 067   . 043   . 070 118 207 200 137   . 088 093 204 132 190 064 130   .   . 106 073 101 105"
			//"033 121 058 174 176 163 045   . 044 197 012   . 059 131 094 034 120 040 027   . 006 138 076   . 072   . 024 125 061 168 124 032 015 001 008 029   .   . 016 170   . 216 211 117 166 087 193 203 085 196 182 103 031 139 100 000 175   . 180 114 220 054 199   .   . 116 194 017   . 025 191 164 222 112 090   .   . 218 149 028 097 185 095 155 183 195 013 048 038 154 217 023 143   .   . 066 158   . 165 184 119   . 162 055 083 123 067   . 071 005 070 134 200 082 177 135 207   . 003 118 130 204 106   . 161 137 073   . 093 190   .   . 132 065 105 078 019 122 140 021 159 205 062 089 077 030 056 036   .   . 221 042 212 053 050 188   . 201 178 091   .   . 156 224 153 179 108   . 098 150 046 080 157 047 209 049 173 115 223 107 110 210 041 051   . 086 214   .   . 052 063 152 069 187 102 096 129 127 014   . 206   . 172 147 198   . 171 189 018 151 136 208 084   . 181 081 007   . 111 109 160 079 219 074 192"
			//"165 144   . 143 167 066 119   . 011 083   . 217   . 002   . 071 118 067 123   . 207 070 003 200 005   .   . 043 177 082 101 093 132 190 105 106   .   . 130 065 073 088   . 137 204 062 077 122 030   .   . 089 146 159 036 205   . 021   . 126 178   . 153   . 133 221   .   . 035 091 188 212 050 201 224 150   . 157 115 020 080 049 108 179 046 209 047 107 173 098 187 102 052   .   .   . 063 152 214 110   . 069 202   . 010   . 172   . 186 096 171 037   . 147 127 206 018   . 014   . 111   . 079 084   . 136 219 109 213   .   .   . 081 208 192 075 094 022 163 174 045 121 197 058 131 033 059 044   . 176 027 061 168 076 034 125 072   .   . 120 169   . 138   . 024 170   . 117   . 166 008 029 016 124   .   . 216 039 068 001 196 180   . 000 182 100 085   .   . 092 103 193 114 203 031 128 116 017   .   . 220 199 090 164 191   . 054 025 142 112   . 155   . 185   . 195 048   . 218 149 154 097   . 013 183"
			//"076 027 168 125   . 040 006   . 120 169   .   . 072 061 024   . 016   . 117   . 099 032 124   . 015   . 001 216 039 211 085 203 182 196 087   .   . 139 100 180 114 092   . 000 031 145 017 054 194 199   . 164 222 142 025 116 090   .   . 112 155 185 048 095 195 028 013 149 183 218 038 154 148 060 097 023   .   . 002 144 066 217 119 055 083 162 165 184 143 158 070 071   . 118   .   . 082 177   . 004 207 200 005 135 134 137   .   . 204 132 093 105 073 161 113 065 064   .   . 130 036 146 140 021 089 205   . 077 122   .   . 159   . 078 062 178 153 156 224 188 009 221 035 212 042 053 133   .   . 201 020 115 098 047 150 173 209 223 179 157 046 108 049 080 107 210   .   . 063 102 010 041 086 214   . 187 052 152 069 110 104 206   . 129 096 189 014 037   .   . 147 127 151 171 186 081 136 109 074   . 208   . 079 160 084   . 213   . 026   . 121 075 174   .   . 059 094   . 044 131   . 163 176 033 197"
			//"  . 116   . 191 112 090 142 220 025   .   . 054 128 017 199 148 218 097 028   . 095 185   . 060 195   . 048 038 149 154 165 217 162 184 083 066   . 119 011 055 167 143   . 144 023 134 177 070 003 004   . 082 071 207 118 123   . 043 135 200 065 093   . 064 132 137   . 113   . 130   .   . 204 073 106 146   . 036 077 205 140 122 159 078 062 056 030 019 021 089 156 050 188   .   .   . 212 042 053 153 009 201 133   . 035 020 080   . 150   . 049   . 223   . 115   . 108   . 047 157 086   . 041 052 063 051 069 152 187   .   .   . 202 210 010 206 104 171 172 198 018 129 037 147 127 014 096 189   . 151 136 109 160   .   . 084   . 219   . 111 192 026   . 079 007 075 045 121   . 012 197 163 044 033   . 022 174 131 176 094 024 141   . 027 072 120 076 169   . 125 168 061 215 138 034 015 117 039 124   . 170 001   . 211 216   . 032 016 099 068 000 180 092 203   .   . 196 139 100 193 103 175   . 085   ."
			//"214 051   . 052 069 041 057 210 086   . 102 187 202 152   . 186 189 172 129   . 018   . 014   .   . 037 104   . 127   . 074 213 081 007 192 079   .   . 111 208   . 084 160 136 181 197 131 022 033 094   .   . 012 045 044 121 163 174 075 176 141 138 024   . 072 027 076 061 169 120 125 168 034 215 040 166   . 016 039 117   . 032 099 170 008 015 124 001 216 211 203 031 092   .   . 175   . 193 085 196 139 114 182 180 087   . 090   . 222 128 054 145 112 164 017 220 142   . 194   . 218 148 097 038 154 028 060 149   . 048   .   . 195 155 183 055 184 217 066 143 119 144 083 158   . 165 162 011   . 167 123 177 082 003 071 043 005 200 135 118 134   . 070 067 004 065 064 137 161 204 105 106 130   .   . 093 088 113 073 190 019 078 140 205   . 036 030   .   . 021 089 077 126 122 146   . 221   . 053 035   .   . 224   . 188   . 156 091 009 201   . 179 173 049 150   . 107 108 157 115 098 080   . 047 046"
			//"074 136 160   . 219 079 026   . 111 192 181   . 081 109 007 012 044 163   .   . 045 022 033 176 059   . 094 174 131 058 076   . 072 024   . 040   . 006 120 141 215 125 168   . 034 008 039 032 124 001   .   . 166 099 016 117 029 216 170 068 180 203 196 139 182 000 085 193 087 100 092 103 031 114 175   .   . 025 017 116 090 054 142 220 145   . 194 199 191 164 185 148   .   .   . 097 154 149 013 048   . 060 195 155 183 144 066   .   . 162 217 083 167 158 002 055   .   . 165 011 118 071 067 043   . 123 200 177 070 004   .   .   . 135 134 065 190 093 106   . 064 137 105 161 113 101 132 130   .   . 205 077 089 030 146 021 056 126 078 036 062 159 122 140 019 178 009 221 212 050 035 224 091   .   . 156 188 042 201 153 107   . 080 020 209 157 047 046   . 173   . 115 223   . 150 202 051 152 214   . 210 110 041 063 052   .   . 086 057 069 129 206 198   . 186 096 104   . 189 127 147   . 151 014 037"
			//"  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 132   .   .   .   . 161   .   .   .   .   .   .   . 159   .   .   .   .   .   .   .   .   .   .   . 153   .   .   .   .   .   .   .   . 178   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 202   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 081   .   . 219   .   . 208   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 125   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   . 139   .   .   .   .   .   .   .   . 203   .   .   .   .   .   .   .   .   .   .   . 145   .   .   .   .   .   .   . 195   .   .   .   . 155   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   ."
			//"013 028 154   . 060 097 095   . 218   .   . 185 195 149 048 023 011 066 144   . 119 217 165   . 162 083 184 143   . 158 003 070 005   .   . 067   . 207 118 135 200   . 082 123 071 105 113 093 101 190   . 161 204   . 130 137 106 088 065 073 078 122 019 159 056   . 030   . 062 036 021 089 146 126 140 050   . 091 042 221 224 156 009 178 035 133 053   . 188   . 049 150 173   .   .   . 098 115 047 107 108 223 209 179 046 051 041   . 102 202 187 010 069 063 152 210 057   . 214 086 189 186 172 198 147 129 151 127 171   .   .   . 096 206 037   . 007   . 079 084 026 136 192 160 109 074 081 111   . 219 121 131 058 033 012 174 059   . 075   . 197 045 022 163 094 141 006 027 168 034 169   . 120 076   . 138 125 061 215 024 001 170 029   . 015 016 124 008   . 216   .   . 068 032 166 182   . 193 085 087 180   . 175 103 092   . 203 100 139 114 116 220 191 054   .   . 199   . 025 017 164   . 112 194 145"
			//"085   . 103 092   . 175 139 180 100 087   . 203 182 193 196 222 025 090   .   . 142 054 194 112 128   . 199 191 017 164   . 185 195 048   . 097   .   .   . 155 060 038 154   . 148 083 002 217 165 184   . 158 023 119 011   . 066 143 055 167 135   . 004   . 005 123 003   . 134   . 043 082 071 200 067 204   . 130 113 137 106 093 064 065 105 132 101 190 088 161   . 146 021 036   .   . 089 077 030 019 159 126   . 078 062 221 224   . 050   . 156   .   .   . 042   . 009   . 053 091 157 150   . 173 098 020 223 115 049   .   . 108 209 179   . 210 110 187 041 052 057 051 010 063 152 214 202 086   . 069 129 127 147 014 186   . 096   . 206 189 037   . 171   . 104 208 026 136 160   . 192 079 111 074   . 213 084 109 219 007 094   . 163 121 059 044   .   .   . 174   . 131 176 022   . 072 027 061 076   . 141 024 040 168 125   .   . 120 006 215 117 170 216 032   . 015 001 099 016 039   . 029 068   . 008"
			//"  .   . 212 188   . 224 009   .   . 035 050 156 133 042 153 150 157 080 020   . 108 049 047   .   .   . 107 173 115   . 214 187 202 110 010 041   .   . 086 210 069 052 063   . 102 037 127 171   .   .   . 147 186 018 189 129 172 198 206 151 208 213 007   . 081 136 074 109   . 111 084 160 181 219 079 012   . 044 131 121 163 022 045 075 197 059   . 094 174 058 138 034 125   .   .   .   . 061   . 024 006 215 072 141 169   . 029   . 166 015 032 008 068 211 039 170 099   . 124   . 100 031 175 092 103 000   . 193   .   .   .   . 182 180 087 220 199 054   . 191 142 116 145 164 017 194 128 025   . 112 028 149 154 013 148 038   . 060 155 218 183   . 185 097 048 055 119 144 158 023 083 066 011 165   .   .   . 002 167 184 004   . 067 123 005 118 003   .   . 043 082 177 200 070 071   . 137 113 101   .   .   . 106 161 088   . 093 130 064 073 205 078 021 122 146 056   .   . 036 077   . 140 126   .   ."
			"124   . 211 216 068 029   .   . 016 008 166 032 015   . 001 031 100   . 000   .   . 203   . 114 182   . 196 092 193 103 194   . 128 199 145 090   .   . 025 220 112 191 164 116 222 183 149 185 013 048   . 154 148 095 218 028 097 038 155 060 055 217 184   . 162 144 165 002   . 011 143 158 023 167 066 071   . 118 177 123 067   . 207 135 134 005 003 004   . 082 093 204 088   .   . 106 161 113 101 190 064 073 132 065 105 205 140   . 146 056 122 062 126 089 077 078 159   . 030 036 091 050 224 188 212 221 201 042 156 153   .   . 133 178 035 179   . 049 080 173 108 020 046   . 115 047 209 157   . 223 051 152 063 214 102 052   . 069 210 086 010   . 187 041 110 206 018 129 147 186 037 172 189 014   . 171 198 127 151 104 007 208 079 136 081 111 074   .   . 084 160 109 219   . 181 059 121 131 033   . 075 094   . 058   .   . 022   . 045 176 027   . 125 138 034 072 024   .   . 061 168 040 215   . 169"
		});

		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);

		const int solveUsingTheseAlgos =
			0
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix3>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix4>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix5>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrix6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_1>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_2>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_3>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_5>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_6>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_7>()
			| PuzzlesSudokuTester::getAlgoFlag<FastestBruteForceSudokuMatrix_14>()
			////| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks2>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks3>()
			//| PuzzlesSudokuTester::getAlgoFlag<SudokuMatrixDancingLinks4>()
			//| PuzzlesSudokuTester::getAlgoFlag<FastestDancingLinksSudokuMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<FsssSolver>()
			////| PuzzlesSudokuTester::getAlgoFlag<JSolveMatrix>()
			////| PuzzlesSudokuTester::getAlgoFlag<bb_sudoku_Matrix>()
			;

		unsigned int numSolutions = 1;
		unsigned int dimension = 225;

		//const string libraryFilePath("../../../test/reports/Sudoku/universalMultiDimensionalSudokuPuzzlesLibrary.csv");
		//PuzzlesSudokuTester::readExistngLibrary(libraryFilePath, solveLibraryPuzzlesAgain, numSolutions, algoFlag);

		for (unsigned int i = 0; i < examples.size(); ++i)
		{
			int iterations = 0;
			long long timeRequiredToGeneratePuzzle = 0;
			SudokuPuzzleUtils::getLogger() << "\n\nPuzzle No. " << i + 1;
			PuzzlesSudokuTester::solveSudokuPuzzleUsingDifferentAlgo(SudokuPuzzleUtils::convertToVectorOfVector(examples[i], dimension, 0), numSolutions, solveUsingTheseAlgos, false);
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationIterations = iterations;
			//SudokuPuzzleUniversalDatabase::getCurrentEntry()->second.m_puzzleGenerationTime = timeRequiredToGeneratePuzzle;
		}

		//SudokuPuzzleUniversalDatabase::writeToLibrary();

		SudokuPuzzleUtils::getLogger() << "\nDebugging is over. Press any key to proceed...";
		cin.get();
	}

	// ====================================================================================

	void testPuzzlesSudokuSolver()
	{
		std::srand(std::time(0)); //use current time as seed for random generator
		SudokuPuzzleUtils::getLogger().setPrintOnScreen(true);

		// Debugging special cases
		//PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_1();
		//PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_2();
		//PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_3();
		//PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_4();
		//PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_5(); // sanity test
		PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_6();
		//PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_7();
		//PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_8(); //49x49
		//PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_9(); //100x100
		PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_10(); //100x100
		//PuzzlesSudokuTester::debugSpecialPuzzleOrAlgorithm_11(); //225x225

		//PuzzlesSudokuTester::testSudokuPuzzlesWithDimension9();
		//PuzzlesSudokuTester::exhaustiveTestSudokuPuzzlesWithDimension9();
		// Try Sudoku of different dimension
		
		const int numPuzzlesToGenerate = 5;
		const int numSolutionsToFind = 1;
		const bool solveLibraryPuzzlesAgain = false;

		//PuzzlesSudokuTester::testSudokuPuzzlesOfAllDimensions(numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		
		//PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions({9}, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		//PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(4, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain); // testSudokuPuzzlesOfDifferentDimensions(dimension, numPuzzlesToGenerate, numSolutions, solveLibraryPuzzlesAgain)
		//PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(9, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		//PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(16, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		//PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(25, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		//PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(36, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		/*
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(49, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(64, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(81, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(100, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(121, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(144, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(169, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(196, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(225, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(256, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(289, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(324, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(361, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		PuzzlesSudokuTester::testSudokuPuzzlesOfDifferentDimensions(400, numPuzzlesToGenerate, numSolutionsToFind, solveLibraryPuzzlesAgain);
		*/
	}
}
