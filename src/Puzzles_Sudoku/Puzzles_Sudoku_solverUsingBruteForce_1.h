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
#include <algorithm>
#include <memory>
#include <string>
using namespace std;

namespace mm {

	class Possible 
	{
		vector<bool> _b;
	public:
		Possible();
		bool   is_on(int i) const;
		int    count()      const;
		void   eliminate(int i);
		int    val()        const;
		string str(int wth) const;
	};

	class Sudoku 
	{
		vector<Possible> _cells;
		static vector< vector<int> > _group, _neighbors, _groups_of;

		bool     eliminate(int k, int val);

	public:
		static bool solve(const string& s)
		{
			Sudoku obj(s);
			//TODO: Write definition using existing functions
			//obj.solve();
			return obj.is_solved();
		}

		Sudoku(string s);
		static void init();

		Possible possible(int k) const;
		bool     is_solved() const;
		bool     assign(int k, int val);
		int      least_count() const;
		void     write(ostream& o) const;
		void	 convertToVectorOfVector(vector< vector<int> >& dataOut);

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

		static unsigned long long m_countCellsTried;
		static unsigned long long m_countValuesTried;
	};

	unique_ptr<Sudoku> solve(unique_ptr<Sudoku> S);

}