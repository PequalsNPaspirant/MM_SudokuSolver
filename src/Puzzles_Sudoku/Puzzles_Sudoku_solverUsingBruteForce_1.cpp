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

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingBruteForce_1.h"

namespace mm {

		Possible::Possible() : _b(9, true) {}
		bool   Possible::is_on(int i) const { return _b[i - 1]; }
		int    Possible::count()      const { return std::count(_b.begin(), _b.end(), true); }
		void   Possible::eliminate(int i) { _b[i - 1] = false; }
		int    Possible::val()        const {
			auto it = find(_b.begin(), _b.end(), true);
			return (it != _b.end() ? 1 + (it - _b.begin()) : -1);
		}

	string Possible::str(int width) const {
		string s(width, ' ');
		int k = 0;
		for (int i = 1; i <= 9; i++) {
			if (is_on(i)) s[k++] = '0' + i;
		}
		return s;
	}

	unsigned long long Sudoku::m_countCellsTried = 0;
	unsigned long long Sudoku::m_countValuesTried = 0;

	Possible Sudoku::possible(int k) const { return _cells[k]; }

	bool Sudoku::is_solved() const {
		for (size_t k = 0; k < _cells.size(); k++) {
			if (_cells[k].count() != 1) {
				return false;
			}
		}
		return true;
	}

	void Sudoku::write(ostream& o) const {
		int width = 1;
		for (size_t k = 0; k < _cells.size(); k++) {
			width = max(width, 1 + _cells[k].count());
		}
		const string sep(3 * width, '-');
		for (int i = 0; i < 9; i++) {
			if (i == 3 || i == 6) {
				o << sep << "+-" << sep << "+" << sep << endl;
			}
			for (int j = 0; j < 9; j++) {
				if (j == 3 || j == 6) o << "| ";
				o << _cells[i * 9 + j].str(width);
			}
			o << endl;
		}
	}

	void Sudoku::convertToVectorOfVector(vector< vector<int> >& dataOut)
	{
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				dataOut[i][j] = _cells[i * 9 + j].val();
	}

	vector< vector<int> >
		Sudoku::_group(27), Sudoku::_neighbors(81), Sudoku::_groups_of(81);

	void Sudoku::init() {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				const int k = i * 9 + j;
				const int x[3] = { i, 9 + j, 18 + (i / 3) * 3 + j / 3 };
				for (int g = 0; g < 3; g++) {
					_group[x[g]].push_back(k);
					_groups_of[k].push_back(x[g]);
				}
			}
		}
		for (size_t k = 0; k < _neighbors.size(); k++) {
			for (size_t x = 0; x < _groups_of[k].size(); x++) {
				for (int j = 0; j < 9; j++) {
					size_t k2 = _group[_groups_of[k][x]][j];
					if (k2 != k) _neighbors[k].push_back(k2);
				}
			}
		}
	}

	bool Sudoku::assign(int k, int val) {

		//SudokuPuzzleUtils::getLogger() << "\nassign: " << k << ", " << val;
		for (int i = 1; i <= 9; i++) {
			if (i != val) {
				if (!eliminate(k, i)) return false;
			}
		}
		return true;
	}

	bool Sudoku::eliminate(int k, int val) {
		if (!_cells[k].is_on(val)) {
			return true;
		}
		_cells[k].eliminate(val);
		const int N = _cells[k].count();
		if (N == 0) {
			return false;
		}
		else if (N == 1) {
			const int v = _cells[k].val();
			for (size_t i = 0; i < _neighbors[k].size(); i++) {
				if (!eliminate(_neighbors[k][i], v)) return false;
			}
		}
		for (size_t i = 0; i < _groups_of[k].size(); i++) {
			const int x = _groups_of[k][i];
			int n = 0, ks;
			for (int j = 0; j < 9; j++) {
				const int p = _group[x][j];
				if (_cells[p].is_on(val)) {
					n++, ks = p;
				}
			}
			if (n == 0) {
				return false;
			}
			else if (n == 1) {
				if (!assign(ks, val)) {
					return false;
				}
			}
		}
		return true;
	}

	int Sudoku::least_count() const {
		int k = -1, min = 10;
		for (size_t i = 0; i < _cells.size(); ++i) {
			const int m = _cells[i].count();
			if (m > 1 && (k == -1 || m < min)) {
				min = m, k = i;
			}
		}
		return k;
	}

	Sudoku::Sudoku(string s)
		: _cells(81)
	{
		m_countCellsTried = 0;
		m_countValuesTried = 0;

		int k = 0;
		for (size_t i = 0; i < s.size(); i++) {
			if (s[i] >= '1' && s[i] <= '9') {
				if (!assign(k, s[i] - '0')) {
					cerr << "error" << endl;
					return;
				}
				k++;
			}
			else if (s[i] == '0' || s[i] == '.') {
				k++;
			}
		}
	}

	unique_ptr<Sudoku> solve(unique_ptr<Sudoku> S) {
		if (S == nullptr || S->is_solved()) {
			return S;
		}
		++Sudoku::m_countCellsTried;
		int k = S->least_count();
		//SudokuPuzzleUtils::getLogger() << "\nTrying cell[ " << k << " ]";

		Possible p = S->possible(k);
		for (int i = 1; i <= 9; i++) {
			if (p.is_on(i)) {
				++Sudoku::m_countValuesTried;
				//SudokuPuzzleUtils::getLogger() << "\nTrying Value: " << i + 1;
				unique_ptr<Sudoku> S1(new Sudoku(*S));
				if (S1->assign(k, i)) {
					if (auto S2 = solve(std::move(S1))) {
						return S2;
					}
				}
			}
		}
		return{};
	}

}
