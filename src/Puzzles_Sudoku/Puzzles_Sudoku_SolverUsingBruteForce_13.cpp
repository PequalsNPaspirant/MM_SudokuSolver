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

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_13.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"
#include "Timer/Timer.h"

namespace mm {

	bool FastestBruteForceSudokuMatrix_7::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		unsigned int size = dataIn.size();
		switch (size)
		{
		case 4:
		{
			FastestBruteForceSudokuMatrixTemplate_7<4> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
			break;

		case 9:
		{
			FastestBruteForceSudokuMatrixTemplate_7<9> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
			break;

		case 16:
		{
			FastestBruteForceSudokuMatrixTemplate_7<16> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 25:
		{
			FastestBruteForceSudokuMatrixTemplate_7<25> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 36:
		{
			FastestBruteForceSudokuMatrixTemplate_7<36> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 49:
		{
			FastestBruteForceSudokuMatrixTemplate_7<49> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 64:
		{
			FastestBruteForceSudokuMatrixTemplate_7<64> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 81:
		{
			FastestBruteForceSudokuMatrixTemplate_7<81> obj(dataIn, stats);
			return obj.solve(solutionSetsOut, numSolutions);
		}
		break;

		case 100:
		{
			FastestBruteForceSudokuMatrixTemplate_7<100> obj(dataIn, stats);
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
