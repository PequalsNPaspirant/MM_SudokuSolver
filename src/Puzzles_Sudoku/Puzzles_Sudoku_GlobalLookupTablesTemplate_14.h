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

#include <array>
//#include <cmath>
#undef max
using namespace std;

#include "Puzzles_Sudoku/Puzzles_Sudoku_GlobalLookupTablesGeneric_14.h"
//#include "Puzzles_Sudoku/Puzzles_Sudoku_StaticBitSet_1.h"
//#include "Puzzles_Sudoku/Puzzles_Sudoku_StaticBitSet_2.h"
//#include "Puzzles_Sudoku/Puzzles_Sudoku_StaticBitSet_3.h"

namespace mm {

	template<unsigned int size>
	class GlobalLookupTablesTemplate_14
	{
	public:
		//typedef unsigned int DEGIT_TYPE;
		//static constexpr const unsigned int SIZE_OF_BYTE = 8;
		//static constexpr const unsigned int BITS_IN_ONE_DIGIT = sizeof(DEGIT_TYPE) * SIZE_OF_BYTE;

		static const GlobalLookupTablesTemplate_14<size> globalLookupTablesTemplate_14;
		//static const GlobalLookupTablesTemplate_14<size>& getGlobalLookupTable()
		//{
		//	static GlobalLookupTablesTemplate_14<size> singletonObject;
		//	return singletonObject;
		//}

		GlobalLookupTablesTemplate_14();
		//unsigned int getCountOfPerfectSquares(unsigned int num);

		//unsigned int maxDim;
		//vector<unsigned short> sizeToIndexHash; // Size = (maxDim + 1) * sizeof(unsigned short) = 401 * 2 bytes 
												//= 802 bytes
		//vector<BigIntegerBitCollection> initialbits; // Size = perfect quares below (maxDim + 1) * sizeof(BigIntegerBitCollection) = 20 * average size 5 bytes
													 //= 100 bytes
		
		static constexpr const unsigned int boxSize = getCompileTimeSquareRoot(size);
		static constexpr const unsigned int totalCells = (size * size);
		static constexpr const unsigned int numDigits = size / (GlobalLookupTablesGeneric_14::BITS_IN_ONE_DIGIT + 1u) + 1u;
		static constexpr const unsigned int optimumClues = (size * size) / (3 * size - 4 - 2 * boxSize);
		template<const unsigned int percentage>
		static constexpr const unsigned int getPercentCells()
		{
			return totalCells * percentage / 100;
		}
		//StaticBitSet_1<size> initialbits_1;
		//StaticBitSet_2<size> initialbits_2;
		//StaticBitSet_3<size> initialbits_3;
		array<GlobalLookupTablesGeneric_14::DEGIT_TYPE, numDigits> initialbits_14;

		array<unsigned int, size + 1u> bitPositionToDigitIndexHash; // Size = (maxDim + 1) * sizeof(unsigned int) = 401 * 4 bytes 
														  //= 1604 bytes ~ 2 KB
		
		//array<DEGIT_TYPE, BITS_IN_ONE_DIGIT + 1> bitMask32; // Size = 33 * sizeof(unsigned int) = 33 * 4 bytes 
										//= 132 bytes
		//array<DEGIT_TYPE, BITS_IN_ONE_DIGIT + 1> bitMask32Inv; // Size = 33 * sizeof(unsigned int) = 33 * 4 bytes 
										   //= 132 bytes
		//array<StaticBitSet_1<size>, size + 1> bitMask_1; // Size = (maxDim + 1) * sizeof(BigIntegerBitCollection) = 401 * average size 5 bytes 
													 //= 2005 bytes ~ 2 KB
		//array<StaticBitSet_2<size>, size + 1> bitMask_2;
		//array<StaticBitSet_3<size>, size + 1> bitMask_3;
		array<array<GlobalLookupTablesGeneric_14::DEGIT_TYPE, numDigits>, size + 1> bitMask_3;

		//used 1 time in processPendingAssignmentsOneByOne() -- can be removed if we have look up table for peers of an element
		//used 2 times in processPendingAssignmentsAllAtOnce() 
		array<array<unsigned int, size>, size> cellToBoxIndex; // cellToBoxIndex[boxSize][row][column] 
															   //Size = box size range from 2 to 20 * (perfect quare from 4 to 400 * perfect square from 4 to 400)
															   //     = 21 * (perfect quare from 4 to 400 * perfect square from 4 to 400)
															   //Actual Size = 4*4 + 9*9 + ... + 400*400 integers = 722,665 integers 
															   //            = 2890660 bytes ~ 2.8 MB

															   //used 2 times in processPendingAssignmentsOneByOne()
															   //used 2 times in searchForHiddenSingles()
															   //used 2 times in searchForHiddenSingles2()
															   //used 2 times in searchForNakedPair()
		array<std::pair<unsigned int, unsigned int>, size> boxIndexToBoxStart;			// boxIndexToBoxStart[boxSize][boxIndex].rowStart & boxIndexToBoxStart[boxSize][boxIndex].columnStart
																							//Size = 21 * perfect quare from 4 to 400 = 
																							//Actual Size = 4 + 9 + ... + 400 integers = 2869 integers = 2869 * 4 bytes 
																							//            = 11476 bytes ~ 11 KB

																							//NOT used anywhere
																							//vector< vector< vector< std::pair<unsigned int, unsigned int>>>> cellToBoxStart;	// cellToBoxIndex[boxSize][row][column].rowStart & cellToBoxIndex[boxSize][row][column].columnStart

		//array<unsigned int, 1 << 8> countONbits8; //Size = 256 * sizeof(char) 
											//= 256 bytes
		//array<unsigned int, 1 << 16> countONbits16; //Size = 65536 * sizeof(char) 
											 //= 65536 bytes ~ 65 KB
		//array<unsigned int, 1 << 8> whichBitIsON8; //Size = 256 * sizeof(char) 
											 //= 256 bytes
		//array<unsigned int, 1 << 16> whichBitIsON16; //Size = 65536 * sizeof(char) 
											  //= 65536 bytes ~ 65 KB

											  //vector<BigIntegerBitCollection> valueToBitPositionHash;
	};

	template<unsigned int size>
	const GlobalLookupTablesTemplate_14<size> GlobalLookupTablesTemplate_14<size>::globalLookupTablesTemplate_14;

	template<unsigned int size>
	GlobalLookupTablesTemplate_14<size>::GlobalLookupTablesTemplate_14()
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
		//	initialbits[i].resize(square / GlobalLookupTablesTemplate_14::BITS_IN_ONE_DIGIT + 1, 0);
		//	initialbits[i].setAllRightmostBitsON(square);
		//}

		//This is definition of: void StaticBitSet_1<size>::setAllRightmostBitsON(const int count)
		unsigned int localSize = size;
		//for (unsigned int i = initialbits.m_digits.size(); i > 0 && localSize > 0; localSize -= BITS_IN_ONE_DIGIT)
		//{
		//	//if (localSize >= BITS_IN_ONE_DIGIT)
		//	//	//initialbits.m_digits[--i] = numeric_limits<DEGIT_TYPE>::max();
		//	//	initialbits.m_digits[--i] = ((1u << (BITS_IN_ONE_DIGIT - 1)) - 1u) << 1u + 1u;
		//	//else
		//	//	initialbits.m_digits[--i] = (1u << localSize) - 1u;
		//}

		//initialbits_14.fill(0);
		//unsigned int index = 0;
		//unsigned int bitShift = 0;
		//for (unsigned int i = 0; i < localSize; ++i)
		//{
		//	//initialbits_14.setBitON(i);
		//	initialbits_14[index] |= (1ULL << bitShift);
		//	if (i == GlobalLookupTablesGeneric_14::BITS_IN_ONE_DIGIT)
		//	{
		//		bitShift = 0;
		//		++index;
		//	}
		//	else
		//		++bitShift;
		//}

		initialbits_14.fill(std::numeric_limits<GlobalLookupTablesGeneric_14::DEGIT_TYPE>::max());
		initialbits_14[numDigits - 1] = (1ULL << (localSize % GlobalLookupTablesGeneric_14::BITS_IN_ONE_DIGIT)) - 1ULL;

		unsigned int value = 0;
		for (unsigned int i = 1u; i < bitPositionToDigitIndexHash.size(); ++i)
		{
			bitPositionToDigitIndexHash[i] = value;
			if (i % GlobalLookupTablesGeneric_14::BITS_IN_ONE_DIGIT == 0)
				++value;
		}

		//for (unsigned int i = 1; i < BITS_IN_ONE_DIGIT + 1; ++i)
		//{
		//	bitMask32[i] = 1u << (i - 1);
		//}

		//for (unsigned int i = 1; i < BITS_IN_ONE_DIGIT + 1; ++i)
		//{
		//	bitMask32Inv[i] = ~bitMask32[i];
		//}

		//for (unsigned int i = 1; i < bitMask_1.size(); ++i)
		//{
		//	//BigIntegerBitCollection temp;
		//	//temp.resize((i - 1) / GlobalLookupTablesTemplate_14::BITS_IN_ONE_DIGIT + 1, 0);
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

		bitMask_3[0].fill(0);
		for (unsigned int i = 1; i < bitMask_3.size(); ++i)
		{
			bitMask_3[i].fill(0);
			bitMask_3[i][bitPositionToDigitIndexHash[i]] |= (1ULL << ((i - 1u) % GlobalLookupTablesGeneric_14::BITS_IN_ONE_DIGIT));
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

		//cellToBoxIndex.push_back(vector< vector<unsigned int>>(0, vector<unsigned int>(0, 0))); // Index = 0
		//cellToBoxIndex.push_back(vector< vector<unsigned int>>(0, vector<unsigned int>(0, 0))); // Index = 1
		//unsigned int dim;
		unsigned int boxSize = sqrt(size);
		//for (unsigned int boxSize = 2; (dim = boxSize * boxSize) <= maxDim; ++boxSize)
		//{
			//cellToBoxIndex.push_back(vector< vector<unsigned int>>(dim, vector<unsigned int>(dim, 0)));
			for (unsigned int i = 0; i < size; ++i)
				for (unsigned int j = 0; j < size; ++j)
					cellToBoxIndex[i][j] = (i / boxSize) * boxSize + (j / boxSize);
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
			for (unsigned int boxIndex = 0; boxIndex < size; ++boxIndex)
			{
				boxIndexToBoxStart[boxIndex].first = (boxIndex / boxSize) * boxSize;
				boxIndexToBoxStart[boxIndex].second = (boxIndex % boxSize) * boxSize;
			}
		//}
		
		//countONbits8.fill(0);
		//for (unsigned int i = 1; i < countONbits8.size(); ++i)
		//{
		//	for (unsigned int bitPosition = 0; bitPosition < 8; ++bitPosition)
		//		if (i & (1 << bitPosition))
		//			countONbits8[i] += 1;
		//}

		//countONbits16.fill(0);
		//for (unsigned int i = 1; i < countONbits16.size(); ++i)
		//{
		//	for (unsigned int bitPosition = 0; bitPosition < 16; ++bitPosition)
		//		if (i & (1 << bitPosition))
		//			countONbits16[i] += 1;
		//}

		//whichBitIsON8.fill(0);
		//for (unsigned int i = 1, bitPosition = 1; i < whichBitIsON8.size(); i *= 2, ++bitPosition)
		//{
		//	//If it is power of 2, then only one bit is ON
		//	//if (i & (i - 1) == 0)
		//	//	for (unsigned int bitPosition = 0; bitPosition < 8; ++bitPosition)
		//	//		if (i & (1 << bitPosition))
		//	//		{
		//	//			whichBitIsON8[i] = bitPosition + 1;
		//	//			break;
		//	//		}
		//	whichBitIsON8[i] = bitPosition;
		//}

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
	}

	//unsigned int GlobalLookupTablesTemplate_14::getCountOfPerfectSquares(unsigned int num)
	//{
	//	unsigned int retVal = 1;
	//	for (; retVal * retVal <= num; ++retVal);
	//	return retVal - 1;
	//}
}
