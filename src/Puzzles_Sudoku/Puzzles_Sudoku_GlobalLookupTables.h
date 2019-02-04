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
#include <cmath>
using namespace std;

#include "Puzzles_Sudoku/Puzzles_Sudoku_BigInteger.h"

namespace mm {

#define MAX_DIM 400
#define MAX_BOX_DIM 20

	// Global/Hardcoded lookup tables for fast performance
	class GlobalLookupTables
	{
	public:
		GlobalLookupTables();		
		unsigned int getCountOfPerfectSquares(unsigned int num);

		unsigned int maxDim;
		vector<unsigned short> sizeToIndexHash; // Size = (maxDim + 1) * sizeof(unsigned short) = 401 * 2 bytes 
		//= 802 bytes
		vector<BigIntegerBitCollection> initialbits; // Size = perfect quares below (maxDim + 1) * sizeof(BigIntegerBitCollection) = 20 * average size 5 bytes
		//= 100 bytes
		vector<unsigned int> bitPositionToDigitIndexHash; // Size = (maxDim + 1) * sizeof(unsigned int) = 401 * 4 bytes 
		//= 1604 bytes ~ 2 KB
		vector<BigIntegerBitCollection> bitMask; // Size = (maxDim + 1) * sizeof(BigIntegerBitCollection) = 401 * average size 5 bytes 
		//= 2005 bytes ~ 2 KB
		vector<unsigned int> bitMask32; // Size = 33 * sizeof(unsigned int) = 33 * 4 bytes 
		//= 132 bytes
		vector<unsigned int> bitMask32Inv; // Size = 33 * sizeof(unsigned int) = 33 * 4 bytes 
		//= 132 bytes

		//used 1 time in processPendingAssignmentsOneByOne() -- can be removed if we have look up table for peers of an element
		//used 2 times in processPendingAssignmentsAllAtOnce() 
		vector< vector< vector<unsigned int>>> cellToBoxIndex; // cellToBoxIndex[boxSize][row][column] 
		//Size = box size range from 2 to 20 * (perfect quare from 4 to 400 * perfect square from 4 to 400)
		//     = 21 * (perfect quare from 4 to 400 * perfect square from 4 to 400)
		//Actual Size = 4*4 + 9*9 + ... + 400*400 integers = 722,665 integers 
		//            = 2890660 bytes ~ 2.8 MB

		//used 2 times in processPendingAssignmentsOneByOne()
		//used 2 times in searchForHiddenSingles()
		//used 2 times in searchForHiddenSingles2()
		//used 2 times in searchForNakedPair()
		vector< vector< std::pair<unsigned int, unsigned int>>> boxIndexToBoxStart;			// boxIndexToBoxStart[boxSize][boxIndex].rowStart & boxIndexToBoxStart[boxSize][boxIndex].columnStart
		//Size = 21 * perfect quare from 4 to 400 = 
		//Actual Size = 4 + 9 + ... + 400 integers = 2869 integers = 2869 * 4 bytes 
		//            = 11476 bytes ~ 11 KB

		//NOT used anywhere
		//vector< vector< vector< std::pair<unsigned int, unsigned int>>>> cellToBoxStart;	// cellToBoxIndex[boxSize][row][column].rowStart & cellToBoxIndex[boxSize][row][column].columnStart
		
		vector<unsigned char> countONbits8; //Size = 256 * sizeof(char) 
		//= 256 bytes
		vector<unsigned char> countONbits16; //Size = 65536 * sizeof(char) 
		//= 65536 bytes ~ 65 KB
		vector<unsigned char> whichBitIsON8; //Size = 256 * sizeof(char) 
		//= 256 bytes
		vector<unsigned char> whichBitIsON16; //Size = 65536 * sizeof(char) 
		//= 65536 bytes ~ 65 KB

		//vector<BigIntegerBitCollection> valueToBitPositionHash;

		//static constexpr const unsigned int SIZE_OF_BYTE = 8;
		//static constexpr const unsigned int BITS_IN_ONE_DIGIT = sizeof(unsigned int) * SIZE_OF_BYTE;
	};

	extern const GlobalLookupTables globalLookupTables;
}
