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
#include "Puzzles_Sudoku/Puzzles_Sudoku_GlobalLookupTablesGeneric_3.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_GlobalLookupTablesTemplate_3.h"

namespace mm {

	template<unsigned int numberOfDigits, unsigned int digitIndex>
	class StaticBitSetMethods
	{
	public:
		typedef array<GlobalLookupTablesGeneric_3::DEGIT_TYPE, numberOfDigits> ArrayType;

		static void assignment(ArrayType& lhs, const ArrayType& rhs)
		{
			lhs[digitIndex] = rhs[digitIndex];
			StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::assignment(lhs, rhs);
		}

		static void bitwiseAND(ArrayType& lhs, const ArrayType& rhs)
		{
			lhs[digitIndex] &= rhs[digitIndex];
			StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::bitwiseAND(lhs, rhs);
		}

		static void bitwiseOR(ArrayType& lhs, const ArrayType& rhs)
		{
			lhs[digitIndex] |= rhs[digitIndex];
			StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::bitwiseOR(lhs, rhs);
		}

		static void bitwiseXOR(ArrayType& lhs, const ArrayType& rhs)
		{
			lhs[digitIndex] ^= rhs[digitIndex];
			StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::bitwiseXOR(lhs, rhs);
		}

		static bool isEqual(const ArrayType& lhs, const ArrayType& rhs)
		{
			return (lhs[digitIndex] == rhs[digitIndex])
				&& StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::isEqual(lhs, rhs);
		}

		static void resetAllDigits(ArrayType& lhs, const GlobalLookupTablesGeneric_3::DEGIT_TYPE& digitValue = 0)
		{
			lhs[digitIndex] = digitValue;
			StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::resetAllDigits(lhs, digitValue);
		}

		static bool isZero(const ArrayType& lhs)
		{
			return (lhs[digitIndex] == 0)
				&& StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::isZero(lhs);
		}

		static bool isOnlyOneBitON(const ArrayType& lhs)
		{
			bool trueForThisDigit = ((lhs[digitIndex] & (lhs[digitIndex] - 1ULL)) == 0);
			bool remainingDigitsZero = StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::isZero(lhs);
			bool onlyOneBitOnInRemainingDigits = StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::isOnlyOneBitON(lhs);
			return (trueForThisDigit && remainingDigitsZero)
				|| ((lhs[digitIndex] == 0) && onlyOneBitOnInRemainingDigits);
		}
		
		static bool hasAllBitsON(const ArrayType& lhs, const ArrayType& mask)
		{
			return ((lhs[digitIndex] & mask[digitIndex]) == mask[digitIndex])
				&& StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::hasAllBitsON(lhs, mask);
		}

		static bool hasAtLeastOneBitON(const ArrayType& lhs, const ArrayType& mask)
		{
			return ((lhs[digitIndex] & mask[digitIndex]) > 0)
				&& StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::hasAllBitsON(lhs, mask);
		}

		static void setAllBitsOFF(ArrayType& lhs, const ArrayType& mask)
		{
			lhs[digitIndex] &= ~mask[digitIndex];
			StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::setAllBitsOFF(lhs, mask);
		}

		static void setAllBitsON(ArrayType& lhs, const ArrayType& mask)
		{
			lhs[digitIndex] |= mask[digitIndex];
			StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::setAllBitsON(lhs, mask);
		}
		
		static unsigned int getCountOfONBits(const ArrayType& lhs)
		{
			//Assumes that only one bit is ON
			//Gives correct results ONLY if only one bit is ON
			GlobalLookupTablesGeneric_3::DEGIT_TYPE currentDigit = lhs[digitIndex];
			return (
				GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[currentDigit & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 8) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 16) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 24) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 32) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 40) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 48) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 56) & 0b11111111]
				+ StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::getCountOfONBits(lhs)
				);
		}

		static unsigned int getOnBitPosition(const ArrayType& lhs)
		{
			//Assumes that only one bit is ON
			//Gives correct results ONLY if only one bit is ON
			GlobalLookupTablesGeneric_3::DEGIT_TYPE currentDigit = lhs[digitIndex];
			unsigned int temp1 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[currentDigit & 0b11111111];
			unsigned int temp2 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 8) & 0b11111111];
			unsigned int temp3 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 16) & 0b11111111];
			unsigned int temp4 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 24) & 0b11111111];
			unsigned int temp5 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 32) & 0b11111111];
			unsigned int temp6 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 40) & 0b11111111];
			unsigned int temp7 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 48) & 0b11111111];
			unsigned int temp8 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 56) & 0b11111111];

			//static unsigned int multiplier[9] = { 0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u };
			//return carryPositions * GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT
			//	+ temp1
			//	+ (8 * multiplier[temp2] + temp2)
			//	+ (16 * multiplier[temp3] + temp3)
			//	+ (24 * multiplier[temp4] + temp4);

			return (
				temp1
				+ (temp2 == 0 ? 0 : 8 + temp2)
				+ (temp3 == 0 ? 0 : 16 + temp3)
				+ (temp4 == 0 ? 0 : 24 + temp4)
				+ (temp5 == 0 ? 0 : 32 + temp5)
				+ (temp6 == 0 ? 0 : 40 + temp6)
				+ (temp7 == 0 ? 0 : 48 + temp7)
				+ (temp8 == 0 ? 0 : 56 + temp8)
				+ StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::getOnBitPosition(lhs)
				);
		}

		static void keepOnlyRightmostBitON(ArrayType& lhs)
		{
			GlobalLookupTablesGeneric_3::DEGIT_TYPE currentDigit = lhs[digitIndex];
			if (currentDigit != 0)
			{
				lhs[digitIndex] = (currentDigit & -currentDigit);
				//lhs[digitIndex] = ((currentDigit & (currentDigit - 1u)) ^ currentDigit);
				StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::resetAllDigits(lhs);
			}
			else
				StaticBitSetMethods<numberOfDigits, digitIndex - 1u>::keepOnlyRightmostBitON(lhs);
		}
		
	};

	template<unsigned int numberOfDigits>
	class StaticBitSetMethods<numberOfDigits, 0>
	{
	public:
		typedef array<GlobalLookupTablesGeneric_3::DEGIT_TYPE, numberOfDigits> ArrayType;

		static void assignment(ArrayType& lhs, const ArrayType& rhs)
		{
			lhs[0] = rhs[0];
		}

		static void bitwiseAND(ArrayType& lhs, const ArrayType& rhs)
		{
			lhs[0] &= rhs[0];
		}

		static void bitwiseOR(ArrayType& lhs, const ArrayType& rhs)
		{
			lhs[0] |= rhs[0];
		}

		static void bitwiseXOR(ArrayType& lhs, const ArrayType& rhs)
		{
			lhs[0] ^= rhs[0];
		}

		static bool isEqual(const ArrayType& lhs, const ArrayType& rhs)
		{
			return (lhs[0] == rhs[0]);
		}

		static void resetAllDigits(ArrayType& lhs, const GlobalLookupTablesGeneric_3::DEGIT_TYPE& digitValue = 0)
		{
			lhs[0] = digitValue;
		}

		static bool isZero(const ArrayType& lhs)
		{
			return (lhs[0] == 0);
		}

		static bool isOnlyOneBitON(const ArrayType& lhs)
		{
			return ((lhs[0] & (lhs[0] - 1u)) == 0);
		}

		static bool hasAllBitsON(const ArrayType& lhs, const ArrayType& mask)
		{
			return ((lhs[0] & mask[0]) == mask[0]);
		}

		static bool hasAtLeastOneBitON(const ArrayType& lhs, const ArrayType& mask)
		{
			return ((lhs[0] & mask[0]) > 0);
		}

		static void setAllBitsOFF(ArrayType& lhs, const ArrayType& mask)
		{
			lhs[0] &= ~mask[0];
		}

		static void setAllBitsON(ArrayType& lhs, const ArrayType& mask)
		{
			lhs[0] |= mask[0];
		}

		static unsigned int getCountOfONBits(const ArrayType& lhs)
		{
			//Assumes that only one bit is ON
			//Gives correct results ONLY if only one bit is ON
			GlobalLookupTablesGeneric_3::DEGIT_TYPE currentDigit = lhs[0];
			return (
				GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[currentDigit & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 8) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 16) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 24) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 32) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 40) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 48) & 0b11111111]
				+ GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.countONbits8[(currentDigit >> 56) & 0b11111111]
				);
		}

		static unsigned int getOnBitPosition(const ArrayType& lhs)
		{
			//Assumes that only one bit is ON
			//Gives correct results ONLY if only one bit is ON
			GlobalLookupTablesGeneric_3::DEGIT_TYPE currentDigit = lhs[0];
			unsigned int temp1 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[currentDigit & 0b11111111];
			unsigned int temp2 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 8) & 0b11111111];
			unsigned int temp3 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 16) & 0b11111111];
			unsigned int temp4 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 24) & 0b11111111];
			unsigned int temp5 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 32) & 0b11111111];
			unsigned int temp6 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 40) & 0b11111111];
			unsigned int temp7 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 48) & 0b11111111];
			unsigned int temp8 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 56) & 0b11111111];

			//static unsigned int multiplier[9] = { 0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u };
			//return carryPositions * GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT
			//	+ temp1
			//	+ (8 * multiplier[temp2] + temp2)
			//	+ (16 * multiplier[temp3] + temp3)
			//	+ (24 * multiplier[temp4] + temp4);

			return (
				temp1
				+ (temp2 == 0 ? 0 : 8 + temp2)
				+ (temp3 == 0 ? 0 : 16 + temp3)
				+ (temp4 == 0 ? 0 : 24 + temp4)
				+ (temp5 == 0 ? 0 : 32 + temp5)
				+ (temp6 == 0 ? 0 : 40 + temp6)
				+ (temp7 == 0 ? 0 : 48 + temp7)
				+ (temp8 == 0 ? 0 : 56 + temp8)
				);
		}

		static void keepOnlyRightmostBitON(ArrayType& lhs)
		{
			GlobalLookupTablesGeneric_3::DEGIT_TYPE currentDigit = lhs[0];
			//if (currentDigit != 0)
			//{
				lhs[0] = (currentDigit & -currentDigit);
				//lhs[0] = ((currentDigit & (currentDigit - 1u)) ^ currentDigit);
			//}
		}
	};

	template<unsigned int size>
	class StaticBitSet_3
	{
	public:
		typedef array<GlobalLookupTablesGeneric_3::DEGIT_TYPE, GlobalLookupTablesTemplate_3<size>::numDigits> ArrayType;
		//Constructors
		StaticBitSet_3();
		//StaticBitSet_3(const ArrayType& input);

	public:
		//Copy constructor
		StaticBitSet_3(const StaticBitSet_3<size>& copy);
		//Assignment operator
		StaticBitSet_3<size>& operator=(const StaticBitSet_3<size>& rhs);
		StaticBitSet_3<size>& operator=(const ArrayType& rhs);

		void operator&=(const StaticBitSet_3<size>& rhs);
		void operator|=(const StaticBitSet_3<size>& rhs);
		void operator^=(const StaticBitSet_3<size>& rhs);
		template<unsigned int sizeBits>
		friend bool operator==(const StaticBitSet_3<sizeBits>& lhs, const StaticBitSet_3<sizeBits>& rhs);
		template<unsigned int sizeBits>
		friend bool operator!=(const StaticBitSet_3<sizeBits>& lhs, const StaticBitSet_3<sizeBits>& rhs);
		template<unsigned int sizeBits>
		friend bool operator!=(const StaticBitSet_3<sizeBits>& lhs, const array<GlobalLookupTablesGeneric_3::DEGIT_TYPE, GlobalLookupTablesTemplate_3<sizeBits>::numDigits>& rhs);
		void resetAllDigits(const GlobalLookupTablesGeneric_3::DEGIT_TYPE& digitValue = 0);
		bool isZero() const;
		bool isOnlyOneBitON() const;
		bool isBitON(const unsigned int& index) const;
		bool hasAllBitsON(const StaticBitSet_3<size>& subSet) const;
		bool hasAtLeastOneBitON(const StaticBitSet_3<size>& mask) const;
		void setBitOFF(const unsigned int& index);
		void setAllBitsOFF(const StaticBitSet_3<size>& mask);
		void setBitON(const unsigned int& index);
		void setAllBitsON(const StaticBitSet_3<size>& mask);
		void keepOnlyRightmostBitON();

	private:
		ArrayType m_digits;

	public:
		const unsigned int getCountOfONBits() const;
	private:
		const unsigned int getCountOfONBits_v1() const;
		const unsigned int getCountOfONBits_v2() const;
		//const unsigned int getCountOfONBits_v3() const;
		const unsigned int getCountOfONBits_v4() const;

	public:
		const unsigned int getOnBitPosition() const;
	private:
		const unsigned int getOnBitPosition_v1() const;
		//const unsigned int getOnBitPosition_v1a(const unsigned int num) const;
		//const unsigned int getOnBitPosition_v1b(const unsigned int num) const;
		const unsigned int getOnBitPosition_v1c(const unsigned int num) const;
		const unsigned int getOnBitPosition_v1d(const unsigned int num) const;
		//const unsigned int getOnBitPosition_v2() const;
		const unsigned int getOnBitPosition_v3() const;
		const unsigned int getOnBitPosition_v4() const;
	};

	//Constructors
	template<unsigned int size>
	StaticBitSet_3<size>::StaticBitSet_3()
	{
	}

	//template<unsigned int size>
	//StaticBitSet_3<size>::StaticBitSet_3(const ArrayType& input)
	//	:m_digits(input)
	//{
	//}

	//Copy constructor
	template<unsigned int size>
	StaticBitSet_3<size>::StaticBitSet_3(const StaticBitSet_3<size>& copy)
		: m_digits(copy.m_digits)
	{
	}

	//Assignment operator
	template<unsigned int size>
	StaticBitSet_3<size>& StaticBitSet_3<size>::operator=(const StaticBitSet_3<size>& rhs)
	{
		//m_digits = copy.m_digits;
		StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::assignment(m_digits, rhs.m_digits);
		return *this;
	}

	template<unsigned int size>
	StaticBitSet_3<size>& StaticBitSet_3<size>::operator=(const ArrayType& rhs)
	{
		//m_digits = rhs;
		StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::assignment(m_digits, rhs);
		return *this;
	}

	template<unsigned int size>
	void StaticBitSet_3<size>::operator&=(const StaticBitSet_3<size>& rhs)
	{
		//for (unsigned int index = 0; index < m_digits.size(); ++index)
		//	m_digits[index] &= rhs.m_digits[index];
		StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::bitwiseAND(m_digits, rhs.m_digits);
	}

	template<unsigned int size>
	void StaticBitSet_3<size>::operator|=(const StaticBitSet_3<size>& rhs)
	{
		//for (unsigned int index = 0; index < m_digits.size(); ++index)
		//	m_digits[index] |= rhs.m_digits[index];
		StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::bitwiseOR(m_digits, rhs.m_digits);
	}

	template<unsigned int size>
	void StaticBitSet_3<size>::operator^=(const StaticBitSet_3<size>& rhs)
	{
		//for (unsigned int index = 0; index < m_digits.size(); ++index)
		//	m_digits[index] ^= rhs.m_digits[index];
		StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::bitwiseXOR(m_digits, rhs.m_digits);
	}

	template<unsigned int sizeBits>
	bool operator==(const StaticBitSet_3<sizeBits>& lhs, const StaticBitSet_3<sizeBits>& rhs)
	{
		//for (unsigned int index = 0; index < lhs.m_digits.size(); ++index)
		//	if (lhs.m_digits[index] != rhs.m_digits[index])
		//		return false;

		//return true;

		return StaticBitSetMethods<GlobalLookupTablesTemplate_3<sizeBits>::numDigits, GlobalLookupTablesTemplate_3<sizeBits>::numDigits - 1u>::isEqual(lhs.m_digits, rhs.m_digits);
	}

	template<unsigned int sizeBits>
	bool operator!=(const StaticBitSet_3<sizeBits>& lhs, const StaticBitSet_3<sizeBits>& rhs)
	{
		return !(lhs == rhs);
	}

	template<unsigned int sizeBits>
	bool operator!=(const StaticBitSet_3<sizeBits>& lhs, const array<GlobalLookupTablesGeneric_3::DEGIT_TYPE, GlobalLookupTablesTemplate_3<sizeBits>::numDigits>& rhs)
	{
		//return lhs.m_digits != rhs;
		return !StaticBitSetMethods<GlobalLookupTablesTemplate_3<sizeBits>::numDigits, GlobalLookupTablesTemplate_3<sizeBits>::numDigits - 1u>::isEqual(lhs.m_digits, rhs);
	}

	template<unsigned int size>
	bool StaticBitSet_3<size>::isOnlyOneBitON() const
	{
		//unsigned int bitCount = 0;
		//for (unsigned int index = 0; index < m_digits.size(); ++index)
		//{
		//	if (m_digits[index] == 0) continue;
		//	if ((m_digits[index] & (m_digits[index] - 1)) != 0 || ++bitCount != 1)
		//		return false;
		//}

		//return true;

		return StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::isOnlyOneBitON(m_digits);
	}

	template<unsigned int size>
	void StaticBitSet_3<size>::resetAllDigits(const GlobalLookupTablesGeneric_3::DEGIT_TYPE& digitValue /*= 0*/)
	{
		//for (size_t i = m_digits.size(); i > 0;)
		//	m_digits[--i] = digitValue;
		StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::resetAllDigits(m_digits, digitValue);
	}

	template<unsigned int size>
	bool StaticBitSet_3<size>::isZero() const
	{
		//for (size_t i = m_digits.size(); i > 0;)
		//	if (m_digits[--i] != 0)
		//		return false;

		//return true;
		return StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::isZero(m_digits);
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getCountOfONBits() const
	{
		//return getCountOfONBits_v4();
		return StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::getCountOfONBits(m_digits);
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getCountOfONBits_v1() const
	{
		unsigned int count = 0;
		for (unsigned int value = 1; value <= size; ++value)
			if (isBitON(value))
				++count;

		return count;
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getCountOfONBits_v2() const
	{
		//const GlobalLookupTablesTemplate_3<size>& globalLookupTables = GlobalLookupTablesTemplate_3<size>::getGlobalLookupTable();
		unsigned int count = 0;
		for (unsigned int i = 0; i < m_digits.size(); ++i)
			count += (globalLookupTables.countONbits8[m_digits[i] & 0b11111111]
				+ globalLookupTables.countONbits8[(m_digits[i] >> 8) & 0b11111111]
				+ globalLookupTables.countONbits8[(m_digits[i] >> 16) & 0b11111111]
				+ globalLookupTables.countONbits8[(m_digits[i] >> 24) & 0b11111111]);

		return count;
	}

	//template<unsigned int size>
	//const unsigned int StaticBitSet_3<size>::getCountOfONBits_v3() const
	//{
	//	const GlobalLookupTablesTemplate_3<size>& globalLookupTables = GlobalLookupTablesTemplate_3<size>::globalLookupTablesTemplate_3;
	//	unsigned int count = 0;
	//	for (unsigned int i = 0; i < m_digits.size(); ++i)
	//		count += (globalLookupTables.countONbits16[m_digits[i] & 0b1111111111111111]
	//			+ globalLookupTables.countONbits16[(m_digits[i] >> 16) & 0b1111111111111111]);

	//	return count;
	//}

	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getCountOfONBits_v4() const
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < m_digits.size(); ++i)
		{
			unsigned int temp = m_digits[i];
			temp = temp - ((temp >> 1) & 0x55555555);
			temp = (temp & 0x33333333) + ((temp >> 2) & 0x33333333);
			count += (((temp + (temp >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
		}

		return count;
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getOnBitPosition() const
	{
		//return getOnBitPosition_v4();
		return StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::getOnBitPosition(m_digits);
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getOnBitPosition_v1() const
	{
		int i = m_digits.size();
		if (i == 1)
			return getOnBitPosition_v1d(m_digits[0]);

		int carryPositions = 0;
		for (; i > 0 && m_digits[--i] == 0; ++carryPositions);
		return carryPositions * GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT + getOnBitPosition_v1d(bitSet.m_digits[i]);
	}

	//template<unsigned int size>
	//const unsigned int StaticBitSet_3<size>::getOnBitPosition_v1a(const unsigned int num) const
	//{
	//	switch (num)
	//	{
	//	case 1: return 1;
	//	case 2: return 2;
	//	case 4: return 3;
	//	case 8: return 4;
	//	case 16: return 5;
	//	case 32: return 6;
	//	case 64: return 7;
	//	case 128: return 8;
	//	case 256: return 9;
	//	}

	//	return -1;
	//}

	//template<unsigned int size>
	//const unsigned int StaticBitSet_3<size>::getOnBitPosition_v1b(const unsigned int num) const
	//{
	//	// Max 3 comparisons needed in worst case due to binary search
	//	if (num < 32u) // 1 or 2 or 4 or 8 or 16
	//	{
	//		if (num < 8u) // 1 or 2 or 4
	//		{
	//			if (num != 4u) // 1 or 2
	//				return num;
	//			else // 4
	//				return 3u;
	//		}
	//		else // 8 or 16
	//		{
	//			if (num == 8u) // 8
	//				return 4u;
	//			else // 16
	//				return 5u;
	//		}
	//	}
	//	else // 32 or 64 or 128 or 256
	//	{
	//		if (num < 128u) // 32 or 64
	//		{
	//			if (num == 32u) // 32
	//				return 6u;
	//			else // 64
	//				return 7u;
	//		}
	//		else // 128 or 256
	//		{
	//			if (num == 128u) // 128
	//				return 8u;
	//			else // 256
	//				return 9u;
	//		}
	//	}
	//}

	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getOnBitPosition_v1c(const unsigned int num) const
	{
		static unordered_map<unsigned int, unsigned int> valueOnBitPositionHashMap(0);

		if (valueOnBitPositionHashMap.empty())
		{
			valueOnBitPositionHashMap.insert({
				{ 1u << 0, 1 },
				{ 1u << 1, 2 },
				{ 1u << 2, 3 },
				{ 1u << 3, 4 },
				{ 1u << 4, 5 },
				{ 1u << 5, 6 },
				{ 1u << 6, 7 },
				{ 1u << 7, 8 },
				{ 1u << 8, 9 },
				{ 1u << 9, 10 },
				{ 1u << 10, 11 },
				{ 1u << 11, 12 },
				{ 1u << 12, 13 },
				{ 1u << 13, 14 },
				{ 1u << 14, 15 },
				{ 1u << 15, 16 },
				{ 1u << 16, 17 },
				{ 1u << 17, 18 },
				{ 1u << 18, 19 },
				{ 1u << 19, 20 },
				{ 1u << 20, 21 },
				{ 1u << 21, 22 },
				{ 1u << 22, 23 },
				{ 1u << 23, 24 },
				{ 1u << 24, 25 },
				{ 1u << 25, 26 },
				{ 1u << 26, 27 },
				{ 1u << 27, 28 },
				{ 1u << 28, 29 },
				{ 1u << 29, 30 },
				{ 1u << 30, 31 },
				{ 1u << 31, 32 }
			});
		}

		unordered_map<unsigned int, unsigned int>::iterator it = valueOnBitPositionHashMap.find(num);
		if (it == valueOnBitPositionHashMap.end())
			MyAssert::myRunTimeAssert(false, "Bit position value entry missing from map");

		return it->second;
	}

	// This is fastest implementation of all from v1a to v1d
	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getOnBitPosition_v1d(const unsigned int num) const
	{
		return static_cast<unsigned int>(log2(num)) + 1;
	}

	//template<unsigned int size>
	//const unsigned int StaticBitSet_3<size>::getOnBitPosition_v2() const
	//{
	//	const GlobalLookupTablesTemplate_3<size>& globalLookupTables = GlobalLookupTablesTemplate_3<size>::globalLookupTablesTemplate_3;
	//	int i = m_digits.size();
	//	if (i == 1)
	//		return globalLookupTables.whichBitIsON16[m_digits[0]];

	//	int carryPositions = 0;
	//	for (; i > 0 && m_digits[--i] == 0; ++carryPositions);

	//	unsigned int temp = globalLookupTables.whichBitIsON8[m_digits[i] & 0b1111111111111111];
	//	unsigned int count = temp;
	//	if (temp == 0)
	//	{
	//		temp = globalLookupTables.whichBitIsON8[(m_digits[i] >> 16) & 0b1111111111111111];
	//		count = 16 + temp;
	//	}
	//	return carryPositions * GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT + count;
	//}

	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getOnBitPosition_v3() const
	{
		unsigned int i = m_digits.size();
		unsigned int digitPosition = 0;
		for (; i > 0 && m_digits[--i] == 0; ++digitPosition);
		unsigned int currentDigit = m_digits[i];
		unsigned int localBytePosition = 0;
		for (; (currentDigit & 0b11111111) == 0; ++localBytePosition, currentDigit = currentDigit >> 8);

		const GlobalLookupTablesTemplate_3<size>& globalLookupTables = GlobalLookupTablesTemplate_3<size>::globalLookupTablesTemplate_3;
		return digitPosition * GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT
			+ localBytePosition * 8
			+ globalLookupTables.whichBitIsON8[currentDigit & 0b11111111];
	}

	//The above implementation can be rewritten as following to avoid branching
	template<unsigned int size>
	const unsigned int StaticBitSet_3<size>::getOnBitPosition_v4() const
	{
		unsigned int i = m_digits.size();
		unsigned int carryPositions = 0;
		for (; i > 0 && m_digits[--i] == 0; ++carryPositions);

		const GlobalLookupTablesTemplate_3<size>& globalLookupTables = GlobalLookupTablesTemplate_3<size>::globalLookupTablesTemplate_3;
		unsigned int currentDigit = m_digits[i];
		unsigned int temp1 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[currentDigit & 0b11111111];
		unsigned int temp2 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 8) & 0b11111111];
		unsigned int temp3 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 16) & 0b11111111];
		unsigned int temp4 = GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.whichBitIsON8[(currentDigit >> 24) & 0b11111111];

		//static unsigned int multiplier[9] = { 0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u };
		//return carryPositions * GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT
		//	+ temp1
		//	+ (8 * multiplier[temp2] + temp2)
		//	+ (16 * multiplier[temp3] + temp3)
		//	+ (24 * multiplier[temp4] + temp4);

		return carryPositions * GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT
			+ temp1
			+ (temp2 == 0 ? 0 : 8 + temp2)
			+ (temp3 == 0 ? 0 : 16 + temp3)
			+ (temp4 == 0 ? 0 : 24 + temp4);
	}

	// Index range [1, infinity)
	template<unsigned int size>
	bool StaticBitSet_3<size>::isBitON(const unsigned int& index) const
	{
		const GlobalLookupTablesTemplate_3<size>& globalLookupTables = GlobalLookupTablesTemplate_3<size>::globalLookupTablesTemplate_3;
		return (m_digits[globalLookupTables.bitPositionToDigitIndexHash[index]] 
			//& GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.bitMask32[index % GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT]) > 0; // remainder after division by power of 2, so it is fast
			& GlobalLookupTablesGeneric_3::globalLookupTablesGeneric_3.bitMask32[index & (GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT - 1u)]) > 0;
	}

	template<unsigned int size>
	bool StaticBitSet_3<size>::hasAllBitsON(const StaticBitSet_3<size>& mask) const
	{
		//for (unsigned int i = 0; i < mask.m_digits.size(); ++i)
		//	if ((m_digits[i] & mask.m_digits[i]) != mask.m_digits[i])
		//		return false;

		//return true;
		return StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::hasAllBitsON(m_digits, mask.m_digits);
	}

	template<unsigned int size>
	bool StaticBitSet_3<size>::hasAtLeastOneBitON(const StaticBitSet_3<size>& mask) const
	{
		//for (unsigned int i = 0; i < mask.m_digits.size(); ++i)
		//	if (/*mask.m_digits[i] != 0 &&*/ (m_digits[i] & mask.m_digits[i]) == 0)
		//		return false;

		//return true;
		return StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::hasAtLeastOneBitON(m_digits, mask.m_digits);
	}

	// Index range [1, infinity)
	template<unsigned int size>
	void StaticBitSet_3<size>::setBitOFF(const unsigned int& index)
	{
		const GlobalLookupTablesTemplate_3<size>& globalLookupTables = GlobalLookupTablesTemplate_3<size>::globalLookupTablesTemplate_3;
		m_digits[globalLookupTables.bitPositionToDigitIndexHash[index]] &= 
			globalLookupTables.bitMask32Inv[index & (GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT - 1u)];
	}

	template<unsigned int size>
	void StaticBitSet_3<size>::setAllBitsOFF(const StaticBitSet_3<size>& mask)
	{
		//for (unsigned int i = 0; i < mask.m_digits.size(); ++i)
		//	m_digits[i] &= ~mask.m_digits[i];
		return StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::setAllBitsOFF(m_digits, mask.m_digits);
	}

	// Index range [1, infinity)
	template<unsigned int size>
	void StaticBitSet_3<size>::setBitON(const unsigned int& index)
	{
		const GlobalLookupTablesTemplate_3<size>& globalLookupTables = GlobalLookupTablesTemplate_3<size>::globalLookupTablesTemplate_3;
		m_digits[globalLookupTables.bitPositionToDigitIndexHash[index]] |= 
			globalLookupTables.bitMask32[index & (GlobalLookupTablesGeneric_3::BITS_IN_ONE_DIGIT - 1u)];
	}

	template<unsigned int size>
	void StaticBitSet_3<size>::setAllBitsON(const StaticBitSet_3<size>& mask)
	{
		//for (unsigned int i = 0; i < mask.m_digits.size(); ++i)
		//	m_digits[i] |= mask.m_digits[i];
		return StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::setAllBitsON(m_digits, mask.m_digits);
	}
	
	template<unsigned int size>
	void StaticBitSet_3<size>::keepOnlyRightmostBitON()
	{
		StaticBitSetMethods<GlobalLookupTablesTemplate_3<size>::numDigits, GlobalLookupTablesTemplate_3<size>::numDigits - 1u>::keepOnlyRightmostBitON(m_digits);
	}
}
