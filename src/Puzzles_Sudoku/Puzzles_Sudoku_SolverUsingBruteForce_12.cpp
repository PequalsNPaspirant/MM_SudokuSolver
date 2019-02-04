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
//#include <vector>
#include <array>
#include <set>
#include <string>
#include <cmath>
using namespace std;

#include <windows.h>

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_12.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"
#include "Timer/Timer.h"

/*
Algorithm in detail:

Dimension of grid = n
Dimension of box = sqrt(n)

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

- Initialising the grid with given hints:
	- Initializing one by one - Assign a value val to any cell c:
	  (If d initial clues are provided then
	   d * ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1)) BigInt & operations)
		- Update the bits for naked singles
		  ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1) BigInt & operations)
			- Change the value of current cell/Remove the the possibilities of other values in current cell than the choosen value val 
			  (1 BigInt = operation)
			- Propagate the constraint to the row, column and box to which this cell belongs to i.e. remove the possibility of value val 
			  from other cells in the same row, column and box
			  ((n - 1) BigInt & operations for row to check if bit is ON, if yes, then BigInt & operation to make bit OFF, 
			   (n - 1) BigInt & operations for column to check if bit is ON, if yes, then BigInt & operation to make bit OFF, 
			   (n - 2 * sqrt(n) + 1) BigInt & operations for box to check if bit is ON, if yes, then BigInt & operation to make bit OFF)
			  Check if only one bit is ON here to find naked single.
			  ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1) operations to count the ON bits and same number of (int == 1) operations)
		- Update the counters for hidden singles
		  (3 * (1 + p + ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1)) int - operations for each of d clues)
			- Set counter = 1 at the position where value val is assigned to. Do it for each row, column and box counters. (3 int = operations)
				Reduce the counters by 1 for the positions = all ON bits in the current cell. Say we found p such positions. Do it for each row, column and box counters. 
				(loop of n iterations => n BigInt & operations to check if bit is ON, if yes, then 3 int - operations
				i.e. 3 * p int - operations)
			- While propagating constraint (refer line 40 in this file), if the bit at val is ON, then reduce the counter by 1
			  ((n - 1) BigInt & operations for row to check if bit is ON, if yes, then int - operation to reduce counter for column and box, 
			   (n - 1) BigInt & operations for column to check if bit is ON, if yes, then int - operation to reduce counter for row and box, 
			   (n - 2 * sqrt(n) + 1) BigInt & operations for box to check if bit is ON, if yes, then int - operation to reduce counter for row and column)
	- Initializing all in one go (this can increase efficiency of Propagation of the constraint above at line 40 in this file):
		- Update the bits for naked singles
		  (total 3 * (d + n*n) BigInt bitwise operations)
			- Loop through all n*n cells and Prepare the initializer mask
			  (If d initial clues are provided then d * (one BigInt | operation each for row, column and box)
			  i.e. 3 * d BigInt | operations)
			- Loop through all n*n cells and Apply masks for initial structure of naked singles for entire grid
			  (2 BigInt | operations to combine masks for row, column and box,
			   1 BigInt & operation to remove bits from naked single bits
			   i.e. 3 * n * n BigInt bitwise operations)
			  Check if only one bit is ON here to find naked single.
			  (n*n operations to count the ON bits and same number of (int == 1) operations)
		- Update the counters for hidden singles
		  (d * 3*p + 1 + n*n * 3*p)
			- For all d clues, while removing all ON bits other than val, update counters for all such ON bits (let's say we found p such ON positions) 
			  for row, column and box in which this cell falls
			  (for each row, column and box, p int - operations
			   i.e. total d * 3*p int - operations)
			- Before removing bits at line 68, keep track of all ON bits. Lets say it is bitsToBeRemoved
			  (requires one BigInt & operation)
			- Reduce the counters by 1 for the positions = all ON bits in the above bitsToBeRemoved. Say we found p such positions. 
			  Do it for each row, column and box counters.
			  (total operations for n*n cells = n*n * 3*p)
	- Which is better? Initializing d initial clues one by one OR Initializing all clues in one go
	  d * ((n - 1) + (n - 1) + (n - 2 * sqrt(n) + 1)) = 3 * (d + n*n)
	  d * (3n - 1 - 2 * sqrt(n))                      = 3 * (d + n*n)
	  d * (3n - 1 - 2 * sqrt(n)) - 3d                 = n*n
	  d * (3n - 4 - 2 * sqrt(n))                      = n*n
	  d                                               = n*n / (3n - 4 - 2 * sqrt(n))
	  d                                               ~ n*n / 3n
	  d                                               ~ n / 3
	  Thus when d ~ n / 3, the number of operations required in both cases are same
	  OR we can say, when d ~< n / 3, the one by one assignment would be preferable
	  Actual values of d below which one by one assignment would be preferable
	  n		d = n*n / (3n - 4 - 2 * sqrt(n)) 
	  --------------------------------------
	  4		4 * 4 / (3 * 4 - 4 - 2 * 2) = 16 / (12 - 4 - 4) = 16 / 4 = 4
	  9		9 * 9 / (3 * 9 - 4 - 2 * 3) = 81 / (27 - 4 - 6) = 81 / 17 = 4.76
	  16	16 * 16 / (3 * 16 - 4 - 2 * 4) = 256 / (48 - 4 - 8) = 256 / 36 = 7.11
- Find hidden single
  (total row, column and box = 3 * n
   total 3n * (2*n + 1) BigInt bitwise operations ~ 6*n*n)
	- Loop through all cells in each row, column and box, 2 BigInt & and | operations to find atLeatOnes and moreThanOnce masks for each cell
	  (for each row, column and box = 2 * n BigInt bitwise operations)
	- onlyOnes = atLeatOnes & ~moreThanOnce to find hidden singles
- Find locked candidate



*/

namespace mm {

	bool FastestBruteForceSudokuMatrix_6::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		unsigned int size = dataIn.size();
		switch (size)
		{
		case 4:
		{
			FastestBruteForceSudokuMatrixTemplate_6<4> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 9:
		{
			FastestBruteForceSudokuMatrixTemplate_6<9> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 16:
		{
			FastestBruteForceSudokuMatrixTemplate_6<16> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 25:
		{
			FastestBruteForceSudokuMatrixTemplate_6<25> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 36:
		{
			FastestBruteForceSudokuMatrixTemplate_6<36> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 49:
		{
			FastestBruteForceSudokuMatrixTemplate_6<49> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 64:
		{
			FastestBruteForceSudokuMatrixTemplate_6<64> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 81:
		{
			FastestBruteForceSudokuMatrixTemplate_6<81> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 100:
		{
			FastestBruteForceSudokuMatrixTemplate_6<100> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		default:
			SudokuPuzzleUtils::getLogger(ConsoleTextColour::Red) << "\n\nERROR: Unsupported dimension: " << size << "\n";
			SudokuPuzzleUtils::getLogger().setConsoleTextColour(ConsoleTextColour::BrightWhite);
		}
		
		return false;
	}

} // namespace mm
