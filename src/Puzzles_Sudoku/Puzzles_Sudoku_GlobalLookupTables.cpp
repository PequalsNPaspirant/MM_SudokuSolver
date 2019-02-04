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
#include <array>
#include <set>
#include <string>
#include <cmath>
using namespace std;

#include <windows.h>

#include "Puzzles_Sudoku/Puzzles_Sudoku_GlobalLookupTables.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"
#include "Timer/Timer.h"

namespace mm {

	const GlobalLookupTables globalLookupTables;

	//Note: bit index starts with 1. bit index of rightmost bit is 1, not 0

	GlobalLookupTables::GlobalLookupTables()
		: maxDim(MAX_DIM),
		sizeToIndexHash(maxDim + 1, 0),
		initialbits(getCountOfPerfectSquares(maxDim + 1), BigIntegerBitCollection()),
		bitPositionToDigitIndexHash(maxDim + 1, -1),
		bitMask(maxDim + 1, BigIntegerBitCollection()),
		bitMask32(33, -1),
		bitMask32Inv(33, -1),
		//valueToBitPositionHash(0, BigIntegerBitCollection()),
		cellToBoxIndex(0),
		boxIndexToBoxStart(0),
		//cellToBoxStart(0),  // TODO: remove if not required
		countONbits8(1 << 8, 0),
		countONbits16(1 << 16, 0),
		whichBitIsON8(1 << 8, 0),
		whichBitIsON16(1 << 16, 0)
	{
		int boxDim = 1;
		for (unsigned int i = 0; i < initialbits.size(); ++i, ++boxDim)
		{
			unsigned int dimension = boxDim * boxDim;
			sizeToIndexHash[dimension] = i;
			initialbits[i].resize(BigIntegerBitCollection::getNumDigits(dimension), 0);
			initialbits[i].setAllRightmostBitsON(dimension);
		}

		int value = 0;
		for (unsigned int i = 1; i < bitPositionToDigitIndexHash.size(); ++i)
		{
			bitPositionToDigitIndexHash[i] = value;

			if (i % BigIntegerBitCollection::BITS_IN_ONE_DIGIT == 0)
				++value;
		}

		for (unsigned int dimension = 1; dimension <= maxDim; ++dimension)
		{
			BigIntegerBitCollection temp;
			temp.resize(BigIntegerBitCollection::getNumDigits(dimension), 0);
			temp.setnthBitON(dimension);
			bitMask[dimension] = std::move(temp);
		}

		for (unsigned int i = 1; i < 33; ++i)
		{
			bitMask32[i] = 1u << (i - 1);
		}

		for (unsigned int i = 1; i < 33; ++i)
		{
			bitMask32Inv[i] = ~bitMask32[i];
		}

		/*

		. . . | . . . | 9 . .
		. . . | . . . | . . .
		. 2 . | 9 1 . | . 7 .
		------+-------+------
		. 8 . | . . . | . 4 7
		. . . | . . . | . 2 8
		. . . | . . 9 | . . .
		------+-------+------
		. . . | . . 3 | 7 . .
		5 9 . | . 7 . | . 1 .
		. 7 . | . . . | . . .

		*/

		cellToBoxIndex.push_back(vector< vector<unsigned int>>(0, vector<unsigned int>(0, 0))); // Index = 0
		cellToBoxIndex.push_back(vector< vector<unsigned int>>(0, vector<unsigned int>(0, 0))); // Index = 1
		unsigned int dim;
		for (unsigned int boxSize = 2; (dim = boxSize * boxSize) <= maxDim; ++boxSize)
		{
			cellToBoxIndex.push_back(vector< vector<unsigned int>>(dim, vector<unsigned int>(dim, 0)));
			for (unsigned int i = 0; i < dim; ++i)
				for (unsigned int j = 0; j < dim; ++j)
					cellToBoxIndex[boxSize][i][j] = (i / boxSize) * boxSize + (j / boxSize);
		}

		/*
		boxIndex:

		Cells:
row:	0 1 2 | 3 4 5 | 6 7 8
Col   0	0 . . | 1 . . | 2 . .
	  1	. . . | . . . | . . .
	  2	. . . | . . . | . . .
		------+-------+------
	  3	3 . . | 4 . . | 5 . .
	  4	. . . | . . . | . . .
	  5	. . . | . . . | . . .
		------+-------+------
	  6	6 . . | 7 . . | 8 . .
	  7	. . . | . . . | . . .
	  8	. . . | . . . | . . .

		*/

		boxIndexToBoxStart.push_back(vector< std::pair<unsigned int, unsigned int>>(0)); // Index = 0
		boxIndexToBoxStart.push_back(vector< std::pair<unsigned int, unsigned int>>(0)); // Index = 1
		for (unsigned int boxSize = 2; (dim = boxSize * boxSize) <= maxDim; ++boxSize)
		{
			boxIndexToBoxStart.push_back(vector< std::pair<unsigned int, unsigned int>>(dim, std::pair<unsigned int, unsigned int>(0, 0)));
			for (unsigned int boxIndex = 0; boxIndex < dim; ++boxIndex)
			{
				boxIndexToBoxStart[boxSize][boxIndex].first = (boxIndex / boxSize) * boxSize;
				boxIndexToBoxStart[boxSize][boxIndex].second = (boxIndex % boxSize) * boxSize;
			}
		}

		for (unsigned int i = 1; i < countONbits8.size(); ++i)
		{
			for (unsigned int value = 0; value < 8; ++value)
				if (i & (1 << value))
					countONbits8[i] += 1;
		}

		for (unsigned int i = 1; i < countONbits16.size(); ++i)
		{
			for (unsigned int value = 0; value < 16; ++value)
				if (i & (1 << value))
					countONbits16[i] += 1;
		}

		for (unsigned int i = 1; i < whichBitIsON8.size(); ++i)
		{
			whichBitIsON8[i] = BigIntegerBitCollection(i).getOnBitPosition();
		}

		for (unsigned int i = 1; i < whichBitIsON16.size(); ++i)
		{
			whichBitIsON16[i] = BigIntegerBitCollection(i).getOnBitPosition();
		}
	}

	unsigned int GlobalLookupTables::getCountOfPerfectSquares(unsigned int num)
	{
		unsigned int retVal = 1;
		for (; retVal * retVal <= num; ++retVal);
		return retVal - 1;
	}

}
