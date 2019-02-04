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
using namespace std;

#include "Assert/MyAssert.h"
//#include "Puzzles_Sudoku/Puzzles_Sudoku_GlobalLookupTables.h"

namespace mm {

	class BigIntegerBitCollection
	{
	public:
		typedef unsigned int DEGIT_TYPE;
		static constexpr const unsigned int SIZE_OF_BYTE = 8;
		static constexpr const unsigned int BITS_IN_ONE_DIGIT = sizeof(DEGIT_TYPE) * SIZE_OF_BYTE;
		static const unsigned int getNumDigits(const unsigned int dimension)
		{
			//dimension = 1 2 3 4...9...16...25...31 32 33...36...49...63 64 65...81...95 96 97...100
			//numDigits = 1 1 1 1...1... 1... 1... 1  1  2... 2... 2... 2  2  3... 3... 3  3  4...  4
			return ((dimension - 1u) / BITS_IN_ONE_DIGIT + 1);
			//OR
			//return (
			//			(dimension / BITS_IN_ONE_DIGIT) // One digit for each group of 32 dimensions/bits
			//			+ ((dimension % BITS_IN_ONE_DIGIT) > 0 ? 1 : 0) // One extra digit if there is additional bits unable to form a complete gruop of 32 bits
			//		); 
		}

		static const unsigned int getBitPositionInCurrentDigit(const unsigned int bitPosition)
		{
			return (bitPosition - 1u) % BITS_IN_ONE_DIGIT + 1;
		}

	public:
		//Constructors
		BigIntegerBitCollection();
		explicit BigIntegerBitCollection(const DEGIT_TYPE& digitValue);
		BigIntegerBitCollection(const int numDigits, const DEGIT_TYPE& digitValue);

		//Copy constructor
		BigIntegerBitCollection(const BigIntegerBitCollection& copy);
		//Move constructor
		BigIntegerBitCollection(BigIntegerBitCollection&& copy);
		//Assignment operator
		BigIntegerBitCollection& operator=(const BigIntegerBitCollection& copy);
		//Move assignment operator
		BigIntegerBitCollection& operator=(BigIntegerBitCollection&& copy);

		void resize(const int numDigits, const DEGIT_TYPE& digitValue = 0);
		friend const BigIntegerBitCollection operator&(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs);
		void operator&=(const BigIntegerBitCollection& rhs);
		friend const BigIntegerBitCollection operator|(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs);
		void operator|=(const BigIntegerBitCollection& rhs);
		friend const BigIntegerBitCollection operator^(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs);
		void operator^=(const BigIntegerBitCollection& rhs);
		const BigIntegerBitCollection operator~() const;
		const BigIntegerBitCollection operator<<(const unsigned int shift) const;
		void setnthBitON(const unsigned int position);
		void setnthBitOFF(const unsigned int position);
		bool isnthBitON(const unsigned int position) const;
		//bool isnthBitOFF(const unsigned int position) const; //A separate function is not required. Make use of !isnthBitON(index)
		friend bool operator<(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs);
		friend bool operator<=(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs);
		friend bool operator>(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs);
		friend bool operator>=(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs);
		friend bool operator==(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs);
		friend bool operator!=(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs);
		int getOnBitPosition() const;
		bool isOnlyOneBitON();
		void setAllRightmostBitsON(const int size);
		void resetAllDigits(const DEGIT_TYPE& digitValue = 0);
		bool isZero();

		void printAllDigits();

		static void prepareBitMask(const int size);
		static vector<BigIntegerBitCollection> bitMask;

		vector<DEGIT_TYPE>& getDigits()
		{
			return m_digits;
		}

		const vector<DEGIT_TYPE>& getDigits() const
		{
			return m_digits;
		}

	private:
		vector<DEGIT_TYPE> m_digits;

		size_t skipAllZerosAtLeft() const;
		const unsigned int getOnBitPosition_1(const unsigned int num) const;
		const unsigned int getOnBitPosition_2(const unsigned int num) const;
		const unsigned int getOnBitPosition_3(const unsigned int num) const;
		const unsigned int getOnBitPosition_4(const unsigned int num) const;
	};

}
