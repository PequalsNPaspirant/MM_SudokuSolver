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
#include <set>
using namespace std;

namespace mm {

	class Cell_0;

	class Comparator
	{
	public:
		const vector< vector<Cell_0> >& m_refData;
		Comparator(const vector< vector<Cell_0> >& dataIn);
		bool operator()(const std::pair<int, int>& left, const std::pair<int, int>& right);

	};

	class Cell_0
	{
	public:
		Cell_0();
		Cell_0(const Cell_0& obj);
		bool setFalse(int index);

		int m_value;
		bool isEmpty;
		vector<bool> m_isPossible;
		int m_isPossibleCount;
		bool originalClue;
	};


	class SudokuMatrix_0
	{
	public:
		static void solve(const vector< vector<int> >& dataIn, vector< vector<int> >& solutionOut);

		SudokuMatrix_0(const vector< vector<int> >& dataIn);
		bool propagateConstraints(int row, int column, vector< vector<Cell_0> >& dataIn);
		void print(const vector< vector<Cell_0> >& dataIn);
		void printColourful(const vector< vector<Cell_0> >& dataIn);
		void execute(vector< vector<int> >& solutionOut);
		bool executeStep(multiset<std::pair<int, int>, Comparator>::iterator itFrom, multiset<std::pair<int, int>, Comparator>::iterator itTo, const vector< vector<Cell_0> >& dataIn, vector< vector<int> >& solutionOut);
		bool validate(const vector< vector<Cell_0> >& dataIn);
		bool validateIntermediateSteps(const vector< vector<Cell_0> >& dataIn);
		bool validateSingleCell(int row, int column, const vector< vector<Cell_0> >& dataIn);
		bool validateSingleCell2(int row, int column, const vector< vector<Cell_0> >& dataIn);
		void copyDataFromTo(const vector< vector<Cell_0> >& from, vector< vector<Cell_0> >& to);

		vector< vector<Cell_0> > m_data;
		multiset<std::pair<int, int>, Comparator> m_emptyCells;

		int getPerformanceParameter()
		{
			//return m_countCellsTried;
			return 0;
		}

		int getValuesTried()
		{
			//return m_countValuesTried;
			return 0;
		}

		int getNumRecursiveCalls()
		{
			return 0;
		}

		int getMemoryLeaked()
		{
			return 0;
		}
	};

}