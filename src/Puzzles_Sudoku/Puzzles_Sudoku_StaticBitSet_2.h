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

	template<unsigned int size>
	class StaticBitSet_2
	{
	public:
		typedef unsigned int DEGIT_TYPE;
		static constexpr const unsigned int SIZE_OF_BYTE = 8;

	public:
		static constexpr const unsigned int BITS_IN_ONE_DIGIT = sizeof(DEGIT_TYPE) * SIZE_OF_BYTE;

	public:
		static constexpr const unsigned int numDigits = size / (BITS_IN_ONE_DIGIT - 1) + 1;
		//Constructors
		StaticBitSet_2();

	public:
		//Copy constructor
		StaticBitSet_2(const StaticBitSet_2<size>& copy);
		//Assignment operator
		StaticBitSet_2<size>& operator=(const StaticBitSet_2<size>& copy);

		void operator&=(const StaticBitSet_2<size>& rhs);
		void operator|=(const StaticBitSet_2<size>& rhs);
		void operator^=(const StaticBitSet_2<size>& rhs);
		template<unsigned int sizeBits>
		friend bool operator==(const StaticBitSet_2<sizeBits>& lhs, const StaticBitSet_2<sizeBits>& rhs);
		template<unsigned int sizeBits>
		friend bool operator!=(const StaticBitSet_2<sizeBits>& lhs, const StaticBitSet_2<sizeBits>& rhs);
		void resetAllDigits(const DEGIT_TYPE& digitValue = 0);
		bool isZero() const;
		bool isOnlyOneBitON() const;
		bool isBitON(const unsigned int& index) const;
		bool hasAllBitsON(const StaticBitSet_2<size>& subSet) const;
		bool hasAtLeastOneBitON(const StaticBitSet_2<size>& mask) const;
		void setBitOFF(const unsigned int& index);
		void setAllBitsOFF(const StaticBitSet_2<size>& mask);
		void setBitON(const unsigned int& index);
		void setAllBitsON(const StaticBitSet_2<size>& mask);

		array<DEGIT_TYPE, numDigits> m_digits;

	public:
		const unsigned int getCountOfONBits() const;
	private:
		const unsigned int getCountOfONBits_v1() const;
		const unsigned int getCountOfONBits_v2() const;
		const unsigned int getCountOfONBits_v3() const;
		const unsigned int getCountOfONBits_v4() const;

	public:
		const unsigned int getOnBitPosition() const;
	private:
		const unsigned int getOnBitPosition_v1() const;
		const unsigned int getOnBitPosition_v1a(const unsigned int num) const;
		const unsigned int getOnBitPosition_v1b(const unsigned int num) const;
		const unsigned int getOnBitPosition_v1c(const unsigned int num) const;
		const unsigned int getOnBitPosition_v1d(const unsigned int num) const;
		const unsigned int getOnBitPosition_v2() const;
		const unsigned int getOnBitPosition_v3() const;
		const unsigned int getOnBitPosition_v4() const;
	};

	//Constructors
	template<unsigned int size>
	StaticBitSet_2<size>::StaticBitSet_2()
	{
	}

	//Copy constructor
	template<unsigned int size>
	StaticBitSet_2<size>::StaticBitSet_2(const StaticBitSet_2<size>& copy)
		: m_digits(copy.m_digits)
	{
	}

	//Assignment operator
	template<unsigned int size>
	StaticBitSet_2<size>& StaticBitSet_2<size>::operator=(const StaticBitSet_2<size>& copy)
	{
		m_digits = copy.m_digits;
		return *this;
	}

	template<unsigned int size>
	void StaticBitSet_2<size>::operator&=(const StaticBitSet_2<size>& rhs)
	{
		for (unsigned int index = 0; index < m_digits.size(); ++index)
			m_digits[index] &= rhs.m_digits[index];
	}

	template<unsigned int size>
	void StaticBitSet_2<size>::operator|=(const StaticBitSet_2<size>& rhs)
	{
		for (unsigned int index = 0; index < m_digits.size(); ++index)
			m_digits[index] |= rhs.m_digits[index];
	}

	template<unsigned int size>
	void StaticBitSet_2<size>::operator^=(const StaticBitSet_2<size>& rhs)
	{
		for (unsigned int index = 0; index < m_digits.size(); ++index)
			m_digits[index] ^= rhs.m_digits[index];
	}

	template<unsigned int sizeBits>
	bool operator==(const StaticBitSet_2<sizeBits>& lhs, const StaticBitSet_2<sizeBits>& rhs)
	{
		for (unsigned int index = 0; index < lhs.m_digits.size(); ++index)
			if (lhs.m_digits[index] != rhs.m_digits[index])
				return false;

		return true;
	}

	template<unsigned int sizeBits>
	bool operator!=(const StaticBitSet_2<sizeBits>& lhs, const StaticBitSet_2<sizeBits>& rhs)
	{
		return !(lhs == rhs);
	}

	template<unsigned int size>
	bool StaticBitSet_2<size>::isOnlyOneBitON() const
	{
		unsigned int bitCount = 0;
		for (unsigned int index = 0; index < m_digits.size(); ++index)
		{
			if (m_digits[index] == 0) continue;
			if ((m_digits[index] & (m_digits[index] - 1)) != 0 || ++bitCount != 1)
				return false;
		}

		return true;
	}

	template<unsigned int size>
	void StaticBitSet_2<size>::resetAllDigits(const DEGIT_TYPE& digitValue /*= 0*/)
	{
		for (size_t i = m_digits.size(); i > 0;)
			m_digits[--i] = digitValue;
	}

	template<unsigned int size>
	bool StaticBitSet_2<size>::isZero() const
	{
		for (size_t i = m_digits.size(); i > 0;)
			if (m_digits[--i] != 0)
				return false;

		return true;
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getCountOfONBits() const
	{
		return getCountOfONBits_v4();
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getCountOfONBits_v1() const
	{
		unsigned int count = 0;
		for (unsigned int value = 1; value <= size; ++value)
			if (isBitON(value))
				++count;

		return count;
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getCountOfONBits_v2() const
	{
		//const GlobalLookupTablesTemplate<size>& globalLookupTables = GlobalLookupTablesTemplate<size>::getGlobalLookupTable();
		unsigned int count = 0;
		for (unsigned int i = 0; i < m_digits.size(); ++i)
			count += (globalLookupTables.countONbits8[m_digits[i] & 0b11111111]
				+ globalLookupTables.countONbits8[(m_digits[i] >> 8) & 0b11111111]
				+ globalLookupTables.countONbits8[(m_digits[i] >> 16) & 0b11111111]
				+ globalLookupTables.countONbits8[(m_digits[i] >> 24) & 0b11111111]);

		return count;
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getCountOfONBits_v3() const
	{
		const GlobalLookupTablesTemplate<size>& globalLookupTables = GlobalLookupTablesTemplate<size>::globalLookupTablesTemplate;
		unsigned int count = 0;
		for (unsigned int i = 0; i < m_digits.size(); ++i)
			count += (globalLookupTables.countONbits16[m_digits[i] & 0b1111111111111111]
				+ globalLookupTables.countONbits16[(m_digits[i] >> 16) & 0b1111111111111111]);

		return count;
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getCountOfONBits_v4() const
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
	const unsigned int StaticBitSet_2<size>::getOnBitPosition() const
	{
		return getOnBitPosition_v4();
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getOnBitPosition_v1() const
	{
		int i = m_digits.size();
		if (i == 1)
			return getOnBitPosition_v1d(m_digits[0]);

		int carryPositions = 0;
		for (; i > 0 && m_digits[--i] == 0; ++carryPositions);
		return carryPositions * BITS_IN_ONE_DIGIT + getOnBitPosition_v1d(bitSet.m_digits[i]);
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getOnBitPosition_v1a(const unsigned int num) const
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

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getOnBitPosition_v1b(const unsigned int num) const
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

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getOnBitPosition_v1c(const unsigned int num) const
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
	const unsigned int StaticBitSet_2<size>::getOnBitPosition_v1d(const unsigned int num) const
	{
		return static_cast<unsigned int>(log2(num)) + 1;
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getOnBitPosition_v2() const
	{
		const GlobalLookupTablesTemplate<size>& globalLookupTables = GlobalLookupTablesTemplate<size>::globalLookupTablesTemplate;
		int i = m_digits.size();
		if (i == 1)
			return globalLookupTables.whichBitIsON16[m_digits[0]];

		int carryPositions = 0;
		for (; i > 0 && m_digits[--i] == 0; ++carryPositions);

		unsigned int temp = globalLookupTables.whichBitIsON8[m_digits[i] & 0b1111111111111111];
		unsigned int count = temp;
		if (temp == 0)
		{
			temp = globalLookupTables.whichBitIsON8[(m_digits[i] >> 16) & 0b1111111111111111];
			count = 16 + temp;
		}
		return carryPositions * StaticBitSet_2<size>::BITS_IN_ONE_DIGIT + count;
	}

	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getOnBitPosition_v3() const
	{
		unsigned int i = 0;
		unsigned int digitPosition = 0;
		for (; i < m_digits.size() && m_digits[i] == 0; ++digitPosition, ++i);
		unsigned int currentDigit = m_digits[i];
		unsigned int localBytePosition = 0;
		for (; (currentDigit & 0b11111111) == 0; ++localBytePosition, currentDigit = currentDigit >> 8);

		const GlobalLookupTablesTemplate<size>& globalLookupTables = GlobalLookupTablesTemplate<size>::globalLookupTablesTemplate;
		return digitPosition * StaticBitSet_2<size>::BITS_IN_ONE_DIGIT
			+ localBytePosition * 8
			+ globalLookupTables.whichBitIsON8[currentDigit & 0b11111111];
	}

	//The above implementation can be rewritten as following to avoid branching
	template<unsigned int size>
	const unsigned int StaticBitSet_2<size>::getOnBitPosition_v4() const
	{
		unsigned int i = 0;
		unsigned int carryPositions = 0;
		for (; i < m_digits.size() && m_digits[i] == 0; ++carryPositions, ++i);

		const GlobalLookupTablesTemplate<size>& globalLookupTables = GlobalLookupTablesTemplate<size>::globalLookupTablesTemplate;
		unsigned int currentDigit = m_digits[i];
		unsigned int temp1 = globalLookupTables.whichBitIsON8[currentDigit & 0b11111111];
		unsigned int temp2 = globalLookupTables.whichBitIsON8[(currentDigit >> 8) & 0b11111111];
		unsigned int temp3 = globalLookupTables.whichBitIsON8[(currentDigit >> 16) & 0b11111111];
		unsigned int temp4 = globalLookupTables.whichBitIsON8[(currentDigit >> 24) & 0b11111111];

		static unsigned int multiplier[9] = { 0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u };
		return carryPositions * StaticBitSet_2<size>::BITS_IN_ONE_DIGIT
			+ temp1
			+ (8 * multiplier[temp2] + temp2)
			+ (16 * multiplier[temp3] + temp3)
			+ (24 * multiplier[temp4] + temp4);
	}

	// Index range [1, infinity)
	template<unsigned int size>
	bool StaticBitSet_2<size>::isBitON(const unsigned int& index) const
	{
		const GlobalLookupTablesTemplate<size>& globalLookupTables = GlobalLookupTablesTemplate<size>::globalLookupTablesTemplate;
		return m_digits[globalLookupTables.bitPositionToDigitIndexHash[index]] & globalLookupTables.bitMask32[((index - 1u) % BITS_IN_ONE_DIGIT) + 1]; // remainder after division by power of 2, so it is fast
	}

	template<unsigned int size>
	bool StaticBitSet_2<size>::hasAllBitsON(const StaticBitSet_2<size>& mask) const
	{
		for (unsigned int i = 0; i < mask.m_digits.size(); ++i)
			if ((m_digits[i] & mask.m_digits[i]) != mask.m_digits[i])
				return false;

		return true;
	}

	template<unsigned int size>
	bool StaticBitSet_2<size>::hasAtLeastOneBitON(const StaticBitSet_2<size>& mask) const
	{
		for (unsigned int i = 0; i < mask.m_digits.size(); ++i)
			if (/*mask.m_digits[i] != 0 &&*/ (m_digits[i] & mask.m_digits[i]) == 0)
				return false;

		return true;
	}

	// Index range [1, infinity)
	template<unsigned int size>
	void StaticBitSet_2<size>::setBitOFF(const unsigned int& index)
	{
		const GlobalLookupTablesTemplate<size>& globalLookupTables = GlobalLookupTablesTemplate<size>::globalLookupTablesTemplate;
		m_digits[globalLookupTables.bitPositionToDigitIndexHash[index]] &= globalLookupTables.bitMask32Inv[((index - 1u) % BITS_IN_ONE_DIGIT) + 1];
	}

	template<unsigned int size>
	void StaticBitSet_2<size>::setAllBitsOFF(const StaticBitSet_2<size>& mask)
	{
		for (unsigned int i = 0; i < mask.m_digits.size(); ++i)
			m_digits[i] &= ~mask.m_digits[i];
	}

	// Index range [1, infinity)
	template<unsigned int size>
	void StaticBitSet_2<size>::setBitON(const unsigned int& index)
	{
		//index        = 1 2 3 4...31 32 33 34 35 36...63 64 65 66 67...
		//bit position = 1 2 3 4...31 32  1  2  3  4...31 32  1  2  3...
		const GlobalLookupTablesTemplate<size>& globalLookupTables = GlobalLookupTablesTemplate<size>::globalLookupTablesTemplate;
		m_digits[globalLookupTables.bitPositionToDigitIndexHash[index]] |= globalLookupTables.bitMask32[((index - 1u) % BITS_IN_ONE_DIGIT) + 1];
	}

	template<unsigned int size>
	void StaticBitSet_2<size>::setAllBitsON(const StaticBitSet_2<size>& mask)
	{
		for (unsigned int i = 0; i < mask.m_digits.size(); ++i)
			m_digits[i] |= mask.m_digits[i];
	}

	
	template<>
	class StaticBitSet_2<9>
	{
	public:
		typedef unsigned int DEGIT_TYPE;
		static constexpr const unsigned int SIZE_OF_BYTE = 8;

	public:
		static constexpr const unsigned int BITS_IN_ONE_DIGIT = sizeof(DEGIT_TYPE) * SIZE_OF_BYTE;

	public:
		static constexpr const unsigned int numDigits = 9 / (BITS_IN_ONE_DIGIT - 1) + 1;
		//Constructors
		StaticBitSet_2()
			: m_digits(0)
		{
		}

	public:
		//Copy constructor
		StaticBitSet_2(const StaticBitSet_2<9>& copy) { m_digits = copy.m_digits; }
		//Assignment operator
		StaticBitSet_2<9>& operator=(const StaticBitSet_2<9>& copy) 
		{ 
			m_digits = copy.m_digits; 
			return *this;
		}

		void operator&=(const StaticBitSet_2<9>& rhs) { m_digits &= rhs.m_digits; }
		void operator|=(const StaticBitSet_2<9>& rhs) { m_digits |= rhs.m_digits; }
		void operator^=(const StaticBitSet_2<9>& rhs) { m_digits ^= rhs.m_digits; }
		
		void resetAllDigits(const DEGIT_TYPE& digitValue = 0) { m_digits = digitValue; }
		bool isZero() const { return m_digits == 0; }
		bool isOnlyOneBitON() const { return (m_digits & (m_digits - 1u)) == 0; }
		bool isBitON(const unsigned int& index) const { return (m_digits & (1u << (index - 1u))) > 0;  }
		bool hasAllBitsON(const StaticBitSet_2<9>& subSet) const { return (m_digits & subSet.m_digits) == subSet.m_digits; }
		bool hasAtLeastOneBitON(const StaticBitSet_2<9>& mask) const { return (m_digits & mask.m_digits) > 0; }
		void setBitOFF(const unsigned int& index) { m_digits &= ~(1u << (index - 1u)); }
		void setAllBitsOFF(const StaticBitSet_2<9>& mask) { m_digits &= ~mask.m_digits; }
		void setBitON(const unsigned int& index) { m_digits |= (1u << (index - 1u)); }
		void setAllBitsON(const StaticBitSet_2<9>& mask) { m_digits |= mask.m_digits; }

		DEGIT_TYPE m_digits;

	public:
		const unsigned int getCountOfONBits() const
		{
			unsigned int temp = m_digits;
			temp = temp - ((temp >> 1) & 0x55555555);
			temp = (temp & 0x33333333) + ((temp >> 2) & 0x33333333);
			return (((temp + (temp >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;

			//return (globalLookupTables.countONbits8[m_digits[i] & 0b11111111]
			//	+ globalLookupTables.countONbits8[(m_digits[i] >> 8) & 0b11111111]
			//	+ globalLookupTables.countONbits8[(m_digits[i] >> 16) & 0b11111111]
			//	+ globalLookupTables.countONbits8[(m_digits[i] >> 24) & 0b11111111]);

			//unsigned int count = 0;
			//for (unsigned int value = 1; value <= 9; ++value)
			//	if (isBitON(value))
			//		++count;
			//return count;
		}
	private:
		//const unsigned int getCountOfONBits_v1() const;
		//const unsigned int getCountOfONBits_v2() const;
		//const unsigned int getCountOfONBits_v3() const;
		//const unsigned int getCountOfONBits_v4() const;

	public:
		const unsigned int getOnBitPosition() const
		{
			return static_cast<unsigned int>(log2(m_digits)) + 1;

			//switch (m_digits)
			//{
			//case 1: return 1;
			//case 2: return 2;
			//case 4: return 3;
			//case 8: return 4;
			//case 16: return 5;
			//case 32: return 6;
			//case 64: return 7;
			//case 128: return 8;
			//case 256: return 9;
			//}

			//return 0;
		}
	private:
		//const unsigned int getOnBitPosition_v1() const;
		//const unsigned int getOnBitPosition_v1a(const unsigned int num) const;
		//const unsigned int getOnBitPosition_v1b(const unsigned int num) const;
		//const unsigned int getOnBitPosition_v1c(const unsigned int num) const;
		//const unsigned int getOnBitPosition_v1d(const unsigned int num) const;
		//const unsigned int getOnBitPosition_v2() const;
		//const unsigned int getOnBitPosition_v3() const;
		//const unsigned int getOnBitPosition_v4() const;
	public:

		friend bool operator==(const StaticBitSet_2<9>& lhs, const StaticBitSet_2<9>& rhs)
		{
			return lhs.m_digits == rhs.m_digits;
		}

		friend bool operator!=(const StaticBitSet_2<9>& lhs, const StaticBitSet_2<9>& rhs)
		{
			return lhs.m_digits != rhs.m_digits;
		}
	};
}
