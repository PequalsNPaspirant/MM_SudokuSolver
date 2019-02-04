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

#include "Puzzles_Sudoku/Puzzles_Sudoku_GlobalLookupTablesGeneric_3.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"
#include "Assert/MyAssert.h"
#include "Timer/Timer.h"

namespace mm {

	const GlobalLookupTablesGeneric_3 GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3;

	GlobalLookupTablesGeneric_3::GlobalLookupTablesGeneric_3()
		//: maxDim(MAX_DIM),
		//sizeToIndexHash(maxDim + 1, -1),
		//initialbits(getCountOfPerfectSquares(maxDim + 1), BigIntegerBitCollection()),
		//bitPositionToDigitIndexHash(maxDim + 1, -1),
		//bitMask(maxDim + 1, BigIntegerBitCollection()),
		//bitMask32(33, -1),
		//bitMask32Inv(33, -1),
		////valueToBitPositionHash(0, BigIntegerBitCollection()),
		//cellToBoxIndex(0),
		//boxIndexToBoxStart(0),
		////cellToBoxStart(0),  // TODO: remove if not required
		//countONbits8(1 << 8, 0),
		//countONbits16(1 << 16, 0),
		//whichBitIsON8(1 << 8, 0),
		//whichBitIsON16(1 << 16, 0)
	{
		//int value = 1;
		//for (unsigned int i = 0; i < initialbits.size(); ++i, ++value)
		//{
		//	unsigned int square = value * value;
		//	sizeToIndexHash[square] = i;
		//	initialbits[i].resize(square / GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT + 1, 0);
		//	initialbits[i].setAllRightmostBitsON(square);
		//}

		//This is definition of: void StaticBitSet_1<size>::setAllRightmostBitsON(const int count)
		//unsigned int localSize = size;
		//for (unsigned int i = initialbits.m_digits.size(); i > 0 && localSize > 0; localSize -= BITS_IN_ONE_DIGIT)
		//{
		//	//if (localSize >= BITS_IN_ONE_DIGIT)
		//	//	//initialbits.m_digits[--i] = numeric_limits<DEGIT_TYPE>::max();
		//	//	initialbits.m_digits[--i] = ((1u << (BITS_IN_ONE_DIGIT - 1)) - 1u) << 1u + 1u;
		//	//else
		//	//	initialbits.m_digits[--i] = (1u << localSize) - 1u;
		//}

		//unsigned int value = 0;
		//for (unsigned int i = 1; i < bitPositionToDigitIndexHash.size(); ++i)
		//{
		//	bitPositionToDigitIndexHash[i] = value;

		//	if (i % GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT == 0)
		//		++value;
		//}

		for (unsigned int i = 1; i < BITS_IN_ONE_DIGIT + 1; ++i)
		{
			bitMask32[i] = 1u << (i - 1);
		}

		for (unsigned int i = 1; i < BITS_IN_ONE_DIGIT + 1; ++i)
		{
			bitMask32Inv[i] = ~bitMask32[i];
		}

		//for (unsigned int i = 1; i < bitMask_1.size(); ++i)
		//{
		//	//BigIntegerBitCollection temp;
		//	//temp.resize((i - 1) / GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT + 1, 0);
		//	//temp.setnthBitON(i);
		//	//bitMask[i] = std::move(temp);
		//	bitMask_1[i].resetAllDigits();
		//	//bitMask[i].m_digits[bitPositionToDigitIndexHash[i]] = bitMask32[i % BITS_IN_ONE_DIGIT];
		//	bitMask_1[i].setBitON(i);
		//}

		//for (unsigned int i = 1; i < bitMask_2.size(); ++i)
		//{
		//	bitMask_2[i].resetAllDigits();
		//	bitMask_2[i].setBitON(i);
		//}

		//for (unsigned int i = 1; i < bitMask_3.size(); ++i)
		//{
		//	bitMask_3[i].resetAllDigits();
		//	bitMask_3[i].setBitON(i);
		//}

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

		//cellToBoxIndex.push_back(vector< vector<unsigned int>>(0, vector<unsigned int>(0, 0))); // Index = 0
		//cellToBoxIndex.push_back(vector< vector<unsigned int>>(0, vector<unsigned int>(0, 0))); // Index = 1
		//unsigned int dim;
		//unsigned int boxSize = sqrt(size);
		//for (unsigned int boxSize = 2; (dim = boxSize * boxSize) <= maxDim; ++boxSize)
		//{
		//cellToBoxIndex.push_back(vector< vector<unsigned int>>(dim, vector<unsigned int>(dim, 0)));
		//for (unsigned int i = 0; i < size; ++i)
		//	for (unsigned int j = 0; j < size; ++j)
		//		cellToBoxIndex[i][j] = (i / boxSize) * boxSize + (j / boxSize);
		//}


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

		//boxIndexToBoxStart.push_back(vector< std::pair<unsigned int, unsigned int>>(0)); // Index = 0
		//boxIndexToBoxStart.push_back(vector< std::pair<unsigned int, unsigned int>>(0)); // Index = 1
		//for (unsigned int boxSize = 2; (dim = boxSize * boxSize) <= maxDim; ++boxSize)
		//{
		//boxIndexToBoxStart.push_back(vector< std::pair<unsigned int, unsigned int>>(dim, std::pair<unsigned int, unsigned int>(0, 0)));
		//for (unsigned int boxIndex = 0; boxIndex < size; ++boxIndex)
		//{
		//	boxIndexToBoxStart[boxIndex].first = (boxIndex / boxSize) * boxSize;
		//	boxIndexToBoxStart[boxIndex].second = (boxIndex % boxSize) * boxSize;
		//}
		//}

		countONbits8.fill(0);
		for (unsigned int i = 1; i < countONbits8.size(); ++i)
		{
			for (unsigned int bitPosition = 0; bitPosition < 8; ++bitPosition)
				if (i & (1 << bitPosition))
					countONbits8[i] += 1;
		}

		//countONbits16.fill(0);
		//for (unsigned int i = 1; i < countONbits16.size(); ++i)
		//{
		//	for (unsigned int bitPosition = 0; bitPosition < 16; ++bitPosition)
		//		if (i & (1 << bitPosition))
		//			countONbits16[i] += 1;
		//}

		whichBitIsON8.fill(0);
		for (unsigned int i = 1, bitPosition = 1; i < whichBitIsON8.size(); i *= 2u, ++bitPosition)
		{
			//If it is power of 2, then only one bit is ON
			//if (i & (i - 1) == 0)
			//	for (unsigned int bitPosition = 0; bitPosition < 8; ++bitPosition)
			//		if (i & (1 << bitPosition))
			//		{
			//			whichBitIsON8[i] = bitPosition + 1;
			//			break;
			//		}
			whichBitIsON8[i] = bitPosition;
		}

		//whichBitIsON16.fill(0);
		//for (unsigned int i = 1, bitPosition = 1; i < whichBitIsON16.size(); i *= 2, ++bitPosition)
		//{
		//	//If it is power of 2, then only one bit is ON
		//	whichBitIsON16[i] = bitPosition;
		//}

		//initialbits.resetAllDigits(((1u << (BITS_IN_ONE_DIGIT - 1)) - 1u) << 1u + 1u);
		//for (unsigned int i = localSize + 1; i <= numDigits * BITS_IN_ONE_DIGIT; ++i)
		//	initialbits.setBitOFF(i);
		//initialbits_1.resetAllDigits();
		//for (unsigned int i = 1; i <= localSize; ++i)
		//	initialbits_1.setBitON(i);

		//initialbits_2.resetAllDigits();
		//for (unsigned int i = 1; i <= localSize; ++i)
		//	initialbits_2.setBitON(i);

		//initialbits_3.resetAllDigits();
		//for (unsigned int i = 1; i <= localSize; ++i)
		//	initialbits_3.setBitON(i);
	}


}
