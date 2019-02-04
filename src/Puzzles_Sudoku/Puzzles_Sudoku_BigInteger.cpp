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


#include <unordered_map>
#include <iostream>
#include <limits>
using namespace std;

#include "Puzzles_Sudoku/Puzzles_Sudoku_BigInteger.h"
#include "Assert/MyAssert.h"

namespace mm {

	const unsigned int BigIntegerBitCollection::SIZE_OF_BYTE; // Initialized inside constructor
	const unsigned int BigIntegerBitCollection::BITS_IN_ONE_DIGIT;
	//vector< BigIntegerBitCollection > Cell3::bitMask = { 0b1, 0b10, 0b100, 0b1000, 0b10000, 0b100000, 0b1000000, 0b10000000, 0b100000000 };
	vector< BigIntegerBitCollection > BigIntegerBitCollection::bitMask(0);
	void BigIntegerBitCollection::prepareBitMask(const int size)
	{
		const int oldSize = bitMask.size();
		if (oldSize < size)
		{
			bitMask.resize(size, BigIntegerBitCollection());
			for (int i = oldSize; i < size; ++i)
			{
				BigIntegerBitCollection temp;
				temp.resize(i / BITS_IN_ONE_DIGIT + 1, 0);
				temp.setnthBitON(i + 1);
				//Cell3::bitMask.push_back(std::move(temp));
				bitMask[i] = std::move(temp);
			}
		}
	}

	//Constructors
	BigIntegerBitCollection::BigIntegerBitCollection()
		: m_digits(0)
	{

	}

	BigIntegerBitCollection::BigIntegerBitCollection(const DEGIT_TYPE& digitValue)
		: m_digits(1, digitValue)
	{

	}

	BigIntegerBitCollection::BigIntegerBitCollection(const int numDigits, const DEGIT_TYPE& digitValue)
		: m_digits(numDigits, digitValue)
	{

	}

	//Copy constructor
	BigIntegerBitCollection::BigIntegerBitCollection(const BigIntegerBitCollection& copy)
		: m_digits(copy.m_digits)
	{
	}

	//Move constructor
	BigIntegerBitCollection::BigIntegerBitCollection(BigIntegerBitCollection&& copy)
		: m_digits(0)
	{
		m_digits.swap(copy.m_digits);
	}

	//Assignment operator
	BigIntegerBitCollection& BigIntegerBitCollection::operator=(const BigIntegerBitCollection& copy)
	{
		if (m_digits.size() == copy.m_digits.size())
			m_digits.assign(copy.m_digits.begin(), copy.m_digits.end());
		else
			m_digits = copy.m_digits;

		return *this;
	}

	//Move assignment operator
	BigIntegerBitCollection& BigIntegerBitCollection::operator=(BigIntegerBitCollection&& copy)
	{
		m_digits.swap(copy.m_digits);
		return *this;
	}


	void BigIntegerBitCollection::resize(const int numDigits, const DEGIT_TYPE& digitValue /*= 0*/)
	{
		m_digits.resize(numDigits, digitValue);
	}

	const BigIntegerBitCollection operator&(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs)
	{
		BigIntegerBitCollection result(lhs);
		result &= rhs;
		return result;
	}

	void BigIntegerBitCollection::operator&=(const BigIntegerBitCollection& rhs)
	{
		unsigned int lhsIndex = m_digits.size();
		unsigned int rhsIndex = rhs.m_digits.size();
		// lhsIndex >= rhsIndex at this place
		for (; lhsIndex > 0 && rhsIndex > 0;)
			m_digits[--lhsIndex] &= rhs.m_digits[--rhsIndex];

		//Mark the remaining leftmost bits zero
		for (; lhsIndex > 0;)
			m_digits[--lhsIndex] = 0;
	}

	const BigIntegerBitCollection operator|(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs)
	{
		BigIntegerBitCollection result(lhs);
		result |= rhs;
		return result;
	}

	void BigIntegerBitCollection::operator|=(const BigIntegerBitCollection& rhs)
	{
		// we need to resize lhs if it's size is less
		if (m_digits.size() < rhs.m_digits.size())
		{
			BigIntegerBitCollection temp(*this);
			m_digits.resize(rhs.m_digits.size(), 0);
			int startIndex = m_digits.size() - temp.m_digits.size() - 1;
			for (DEGIT_TYPE num : temp.m_digits)
				m_digits[++startIndex] = num;
		}

		unsigned int lhsIndex = m_digits.size();
		unsigned int rhsIndex = rhs.m_digits.size();
		// lhsIndex >= rhsIndex at this place
		for (; lhsIndex > 0 && rhsIndex > 0;)
			m_digits[--lhsIndex] |= rhs.m_digits[--rhsIndex];
	}

	const BigIntegerBitCollection operator^(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs)
	{
		BigIntegerBitCollection result(lhs);
		result ^= rhs;
		return result;
	}

	void BigIntegerBitCollection::operator^=(const BigIntegerBitCollection& rhs)
	{
		// we need to resize lhs if it's size is less
		if (m_digits.size() < rhs.m_digits.size())
		{
			BigIntegerBitCollection temp(*this);
			m_digits.resize(rhs.m_digits.size(), 0);
			int startIndex = m_digits.size() - temp.m_digits.size() - 1;
			for (DEGIT_TYPE num : temp.m_digits)
				m_digits[++startIndex] = num;
		}

		unsigned int lhsIndex = m_digits.size();
		unsigned int rhsIndex = rhs.m_digits.size();
		// lhsIndex >= rhsIndex at this place
		for (; lhsIndex > 0 && rhsIndex > 0;)
			m_digits[--lhsIndex] ^= rhs.m_digits[--rhsIndex];

		//Mark the remaining leftmost bits  // No need to handle it: n ^ 0 = n
		//for (; lhsIndex > 0;)
		//	m_digits[--lhsIndex] ^= 0;
	}

	const BigIntegerBitCollection BigIntegerBitCollection::operator~() const
	{
		BigIntegerBitCollection result(*this);
		for (DEGIT_TYPE& num : result.m_digits)
			num = ~num;

		return result;
	}

	const BigIntegerBitCollection BigIntegerBitCollection::operator<<(const unsigned int shift) const
	{
		BigIntegerBitCollection result;
		result.resize(m_digits.size(), 0);
		int localShift = shift % BITS_IN_ONE_DIGIT;
		int digitShift = shift / BITS_IN_ONE_DIGIT;
		int carry = 0;
		for (size_t i = digitShift; i < m_digits.size(); ++i)
		{
			result.m_digits[i] |= carry;
			result.m_digits[i] = m_digits[i - digitShift] << localShift;
			carry = m_digits[i + digitShift] >> (sizeof(DEGIT_TYPE) - localShift);
		}

		return result;
	}

	void BigIntegerBitCollection::setnthBitON(const unsigned int position)
	{
		//int index = m_digits.size() - 1 - (position - 1u) / BITS_IN_ONE_DIGIT;
		int index = (position - 1u) / BITS_IN_ONE_DIGIT;
		//int localPosition = (position - 1) % BITS_IN_ONE_DIGIT;
		int localPosition = (position - 1u) - (index * BITS_IN_ONE_DIGIT);
		m_digits[index] |= (1u << localPosition);

		//*this |= BigIntegerBitCollection::bitMask[position - 1u];
	}

	void BigIntegerBitCollection::setnthBitOFF(const unsigned int position)
	{
		//int index = m_digits.size() - 1 - (position - 1u) / BITS_IN_ONE_DIGIT;
		int index = (position - 1u) / BITS_IN_ONE_DIGIT;
		//int localPosition = (position - 1) % BITS_IN_ONE_DIGIT;
		int localPosition = (position - 1u) - (index * BITS_IN_ONE_DIGIT);		
		m_digits[index] &= ~(1u << localPosition);
	}

	bool BigIntegerBitCollection::isnthBitON(const unsigned int position) const
	{
		//return (m_isPossible & SudokuPuzzleUtils::bitMask[index]) > BigIntegerBitCollection(0);

		//int index = m_digits.size() - 1 - (position - 1u) / BITS_IN_ONE_DIGIT;
		int index = (position - 1u) / BITS_IN_ONE_DIGIT;
		//int localPosition = (position - 1) % BITS_IN_ONE_DIGIT;
		int localPosition = (position - 1u) - (index * BITS_IN_ONE_DIGIT);
		return (m_digits[index] & (1u << localPosition)) > 0;
	}

	/*
	bool BigIntegerBitCollection::isnthBitOFF(const unsigned int position) const
	{
		int index = m_digits.size() - 1 - (position - 1u) / BITS_IN_ONE_DIGIT;
		//int localPosition = (position - 1) % BITS_IN_ONE_DIGIT;
		int localPosition = (position - 1u) - (index * BITS_IN_ONE_DIGIT);
		return (m_digits[index] & (1u << localPosition)) == 0;
	}
	*/

	bool operator<(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs)
	{
		size_t lhsStart = lhs.skipAllZerosAtLeft();
		size_t rhsStart = rhs.skipAllZerosAtLeft();

		if ((lhs.m_digits.size() - lhsStart) != (rhs.m_digits.size() - rhsStart))
			return (lhs.m_digits.size() - lhsStart) < (rhs.m_digits.size() - rhsStart);

		for (; lhsStart < lhs.m_digits.size(); ++lhsStart, ++rhsStart)
			if (lhs.m_digits[lhsStart] > rhs.m_digits[rhsStart])
				return false;
			else if (lhs.m_digits[lhsStart] < rhs.m_digits[rhsStart])
				return true;

		return false;
	}

	bool operator<=(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs)
	{
		//return *this < rhs || *this == rhs;
		return !(lhs > rhs);
	}

	bool operator>(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs)
	{
		return rhs < lhs;
	}

	bool operator>=(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs)
	{
		//return *this > rhs || *this == rhs;
		return !(lhs < rhs);
	}

	bool operator==(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs)
	{
		size_t lhsStart = lhs.skipAllZerosAtLeft();
		size_t rhsStart = rhs.skipAllZerosAtLeft();

		if ((lhs.m_digits.size() - lhsStart) != (rhs.m_digits.size() - rhsStart))
			return false;

		for (; lhsStart < lhs.m_digits.size(); ++lhsStart, ++rhsStart)
			if (lhs.m_digits[lhsStart] != rhs.m_digits[rhsStart])
				return false;

		return true;
	}

	bool operator!=(const BigIntegerBitCollection& lhs, const BigIntegerBitCollection& rhs)
	{
		return !(lhs == rhs);
	}

	int BigIntegerBitCollection::getOnBitPosition() const
	{
		unsigned int numDigits = m_digits.size();
		if(numDigits == 1)
			return getOnBitPosition_4(m_digits[0]);

		unsigned int carryPositions = 0;
		unsigned int digitIndex = 0;
		for (; digitIndex < numDigits && m_digits[digitIndex] == 0; ++carryPositions, ++digitIndex);
		return carryPositions * BITS_IN_ONE_DIGIT + getOnBitPosition_4(m_digits[digitIndex]);
	}

	bool BigIntegerBitCollection::isOnlyOneBitON()
	{
		unsigned int numNonZeroDigits = 0;
		unsigned int nonZeroDigitIndex = 0;
		for (unsigned int i = 0; i < m_digits.size(); ++i)
			if (m_digits[i] != 0)
			{
				++numNonZeroDigits;
				nonZeroDigitIndex = i;
			}

		return (numNonZeroDigits == 1) && ((m_digits[nonZeroDigitIndex] & (m_digits[nonZeroDigitIndex] - 1)) == 0);
	}

	void BigIntegerBitCollection::setAllRightmostBitsON(const int size)
	{
		unsigned int localSize = size, i = 0;
		for (; i < m_digits.size() && localSize >= BITS_IN_ONE_DIGIT; localSize -= BITS_IN_ONE_DIGIT, ++i)
			m_digits[i] = numeric_limits<DEGIT_TYPE>::max();

		if(localSize > 0)
			m_digits[i] = (1 << localSize) - 1;
	}

	void BigIntegerBitCollection::resetAllDigits(const DEGIT_TYPE& digitValue /*= 0*/)
	{
		for (size_t i = 0; i < m_digits.size(); ++i)
			m_digits[i] = digitValue;
	}

	bool BigIntegerBitCollection::isZero()
	{
		for (size_t i = 0; i < m_digits.size(); ++i)
			if (m_digits[i] != 0)
				return false;

		return true;
	}

	size_t BigIntegerBitCollection::skipAllZerosAtLeft() const
	{
		size_t i = 0;
		for (; i < m_digits.size() && m_digits[i] == 0; ++i);
		return i;
	}

	const unsigned int BigIntegerBitCollection::getOnBitPosition_1(const unsigned int num) const
	{
		switch (num)
		{
		case 1: return 1;
		case 2: return 2;
		case 4: return 3;
		case 8: return 4;
		case 16: return 5;
		case 32: return 6;
		case 64: return 7;
		case 128: return 8;
		case 256: return 9;
		}

		return -1;
	}

	const unsigned int BigIntegerBitCollection::getOnBitPosition_2(const unsigned int num) const
	{
		// Max 3 comparisons needed in worst case due to binary search
		if (num < 32u) // 1 or 2 or 4 or 8 or 16
		{
			if (num < 8u) // 1 or 2 or 4
			{
				if (num != 4u) // 1 or 2
					return num;
				else // 4
					return 3u;
			}
			else // 8 or 16
			{
				if (num == 8u) // 8
					return 4u;
				else // 16
					return 5u;
			}
		}
		else // 32 or 64 or 128 or 256
		{
			if (num < 128u) // 32 or 64
			{
				if (num == 32u) // 32
					return 6u;
				else // 64
					return 7u;
			}
			else // 128 or 256
			{
				if (num == 128u) // 128
					return 8u;
				else // 256
					return 9u;
			}
		}
	}

	const unsigned int BigIntegerBitCollection::getOnBitPosition_3(const unsigned int num) const
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

	// This is fastest implementation of all
	const unsigned int BigIntegerBitCollection::getOnBitPosition_4(const unsigned int num) const
	{
		return static_cast<unsigned int>(log2(num)) + 1;
	}

	void BigIntegerBitCollection::printAllDigits()
	{
		for (size_t i = 0; i < m_digits.size(); ++i)
			cout << m_digits[i] << ", ";
	}

}
