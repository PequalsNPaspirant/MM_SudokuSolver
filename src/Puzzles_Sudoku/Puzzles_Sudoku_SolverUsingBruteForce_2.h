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


#include <vector>
using namespace std;

namespace mm {

	class Cell2
	{
	public:
		Cell2();
		Cell2(const Cell2& obj);
		bool setFalse(int index);

		int m_value;
		vector<bool> m_isPossible;
		int m_isPossibleCount;
		bool originalClue;
	};


	class SudokuMatrix2
	{
	public:
		static bool solve(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut);

		SudokuMatrix2(const vector< vector<int> >& dataIn);
		bool propagateConstraints(int row, int column, vector< vector<Cell2> >& dataIn);
		void printColourful(const vector< vector<Cell2> >& dataIn);
		bool solve(vector< vector<int> >& solutionOut);
		bool executeStep(const vector< vector<Cell2> >& dataIn, vector< vector<int> >& solutionOut);
		void copyDataFromTo(const vector< vector<Cell2> >& from, vector< vector<Cell2> >& to);
		bool validateIntermediateSteps(const vector< vector<Cell2> >& dataIn);

		int getPerformanceParameter()
		{
			return m_countCellsTried;
		}

		int getValuesTried()
		{
			return m_countValuesTried;
		}

		int getNumRecursiveCalls()
		{
			return 0;
		}

		int getMemoryLeaked()
		{
			return 0;
		}

		vector< vector<Cell2> > m_data;
		bool m_isValid;
		unsigned long long m_countCellsTried;
		unsigned long long m_countValuesTried;
	};

}
