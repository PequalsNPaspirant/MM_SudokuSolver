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
#include <cmath>
using namespace std;

#include "Puzzles_Sudoku/Puzzles_Sudoku_SolverUsingMicrosoftZ3Solver_1.h"
#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"

#include "C++/z3++.h"
using namespace z3;

namespace mm {

	bool Microsoft_z3_solver_1::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
        context c;

        // nxn matrix of integer variables 
        int dim = dataIn.size();
        expr_vector x(c);
        for (unsigned i = 0; i < dim; ++i)
            for (unsigned j = 0; j < dim; ++j) {
                std::stringstream x_name;

                x_name << "x_" << i << '_' << j;
                x.push_back(c.int_const(x_name.str().c_str()));
            }

        solver s(c);

        // each cell contains a value in {1, ..., n}
        for (unsigned i = 0; i < dim; ++i)
            for (unsigned j = 0; j < dim; ++j) {
                s.add(x[i * dim + j] >= 1 && x[i * dim + j] <= dim);
            }

        // each row contains a digit at most once
        for (unsigned i = 0; i < dim; ++i) {
            expr_vector t(c);
            for (unsigned j = 0; j < dim; ++j)
                t.push_back(x[i * dim + j]);
            s.add(distinct(t));
        }

        // each column contains a digit at most once
        for (unsigned j = 0; j < dim; ++j) {
            expr_vector t(c);
            for (unsigned i = 0; i < dim; ++i)
                t.push_back(x[i * dim + j]);
            s.add(distinct(t));
        }

        // each 3x3 square contains a digit at most once
        for (unsigned i0 = 0; i0 < 3; i0++) {
            for (unsigned j0 = 0; j0 < 3; j0++) {
                expr_vector t(c);
                for (unsigned i = 0; i < 3; i++)
                    for (unsigned j = 0; j < 3; j++)
                        t.push_back(x[(i0 * 3 + i) * dim + j0 * 3 + j]);
                s.add(distinct(t));
            }
        }

        // sudoku instance, we use '0' for empty cells
        //int instance[9][9] = { {0,0,0,0,9,4,0,3,0},
        //                      {0,0,0,5,1,0,0,0,7},
        //                      {0,8,9,0,0,0,0,4,0},
        //                      {0,0,0,0,0,0,2,0,8},
        //                      {0,6,0,2,0,1,0,5,0},
        //                      {1,0,2,0,0,0,0,0,0},
        //                      {0,7,0,0,0,0,5,2,0},
        //                      {9,0,0,0,6,5,0,0,0},
        //                      {0,4,0,9,7,0,0,0,0} };

        for (unsigned i = 0; i < dim; i++)
            for (unsigned j = 0; j < dim; j++)
                if (dataIn[i][j] != 0)
                    s.add(x[i * dim + j] == dataIn[i][j]);

        s.check();
        //std::cout << s.check() << std::endl;
        //std::cout << s << std::endl;

        vector<vector<int>> solution(dim, vector<int>(dim, 0));
        model m = s.get_model();
        for (unsigned i = 0; i < dim; ++i) {
            for (unsigned j = 0; j < dim; ++j)
                solution[i][j] = m.eval(x[i * dim + j]);
            //std::cout << '\n';
        }
        solutionSetsOut.push_back(std::move(solution));

		return true;
	}

	

}
