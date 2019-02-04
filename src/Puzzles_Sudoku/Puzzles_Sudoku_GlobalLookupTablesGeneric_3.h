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
#include <cmath>
using namespace std;

namespace mm {

	class GlobalLookupTablesGeneric_3
	{
	public:
		typedef unsigned long long DEGIT_TYPE;
		static constexpr const unsigned int SIZE_OF_BYTE = 8;
		static constexpr const unsigned int BITS_IN_ONE_DIGIT = sizeof(DEGIT_TYPE) * SIZE_OF_BYTE;

		static const GlobalLookupTablesGeneric_3 globalLookupTablesGeneric_3;
		//static const GlobalLookupTablesGeneric_3<size>& getGlobalLookupTable()
		//{
		//	static GlobalLookupTablesGeneric_3<size> singletonObject;
		//	return singletonObject;
		//}

		GlobalLookupTablesGeneric_3();
		//unsigned int getCountOfPerfectSquares(unsigned int num);

		//unsigned int maxDim;
		//vector<unsigned short> sizeToIndexHash; // Size = (maxDim + 1) * sizeof(unsigned short) = 401 * 2 bytes 
												//= 802 bytes
		//vector<BigIntegerBitCollection> initialbits; // Size = perfect quares below (maxDim + 1) * sizeof(BigIntegerBitCollection) = 20 * average size 5 bytes
													 //= 100 bytes
		//static constexpr const unsigned int numDigits = size / (BITS_IN_ONE_DIGIT - 1) + 1;
		//StaticBitSet_1<size> initialbits_1;
		//StaticBitSet_2<size> initialbits_2;
		//StaticBitSet_3<size> initialbits_3;

		//array<unsigned int, size + 1> bitPositionToDigitIndexHash; // Size = (maxDim + 1) * sizeof(unsigned int) = 401 * 4 bytes 
														  //= 1604 bytes ~ 2 KB
		
		array<DEGIT_TYPE, BITS_IN_ONE_DIGIT + 1u> bitMask32; // Size = 33 * sizeof(unsigned int) = 33 * 4 bytes 
										//= 132 bytes
		array<DEGIT_TYPE, BITS_IN_ONE_DIGIT + 1u> bitMask32Inv; // Size = 33 * sizeof(unsigned int) = 33 * 4 bytes 
										   //= 132 bytes
		//array<StaticBitSet_1<size>, size + 1> bitMask_1; // Size = (maxDim + 1) * sizeof(BigIntegerBitCollection) = 401 * average size 5 bytes 
													 //= 2005 bytes ~ 2 KB
		//array<StaticBitSet_2<size>, size + 1> bitMask_2;
		//array<StaticBitSet_3<size>, size + 1> bitMask_3;

		//used 1 time in processPendingAssignmentsOneByOne() -- can be removed if we have look up table for peers of an element
		//used 2 times in processPendingAssignmentsAllAtOnce() 
		//array<array<unsigned int, size>, size> cellToBoxIndex; // cellToBoxIndex[boxSize][row][column] 
															   //Size = box size range from 2 to 20 * (perfect quare from 4 to 400 * perfect square from 4 to 400)
															   //     = 21 * (perfect quare from 4 to 400 * perfect square from 4 to 400)
															   //Actual Size = 4*4 + 9*9 + ... + 400*400 integers = 722,665 integers 
															   //            = 2890660 bytes ~ 2.8 MB

															   //used 2 times in processPendingAssignmentsOneByOne()
															   //used 2 times in searchForHiddenSingles()
															   //used 2 times in searchForHiddenSingles2()
															   //used 2 times in searchForNakedPair()
		//array<std::pair<unsigned int, unsigned int>, size> boxIndexToBoxStart;			// boxIndexToBoxStart[boxSize][boxIndex].rowStart & boxIndexToBoxStart[boxSize][boxIndex].columnStart
																							//Size = 21 * perfect quare from 4 to 400 = 
																							//Actual Size = 4 + 9 + ... + 400 integers = 2869 integers = 2869 * 4 bytes 
																							//            = 11476 bytes ~ 11 KB

																							//NOT used anywhere
																							//vector< vector< vector< std::pair<unsigned int, unsigned int>>>> cellToBoxStart;	// cellToBoxIndex[boxSize][row][column].rowStart & cellToBoxIndex[boxSize][row][column].columnStart

		array<unsigned int, 1u << 8> countONbits8; //Size = 256 * sizeof(char) 
											//= 256 bytes
		//array<unsigned int, 1 << 16> countONbits16; //Size = 65536 * sizeof(char) 
											 //= 65536 bytes ~ 65 KB
		array<unsigned int, 1u << 8> whichBitIsON8; //Size = 256 * sizeof(char) 
											 //= 256 bytes
		//array<unsigned int, 1 << 16> whichBitIsON16; //Size = 65536 * sizeof(char) 
											  //= 65536 bytes ~ 65 KB

											  //vector<BigIntegerBitCollection> valueToBitPositionHash;
	};

	//unsigned int GlobalLookupTablesGeneric_3::getCountOfPerfectSquares(unsigned int num)
	//{
	//	unsigned int retVal = 1;
	//	for (; retVal * retVal <= num; ++retVal);
	//	return retVal - 1;
	//}
}
