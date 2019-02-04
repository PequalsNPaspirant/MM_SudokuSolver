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

namespace mm {

	//Forward declaraton
	struct SudokuPuzzleBasicStats;

	class SudokuMatrixDancingLinks2
	{
		struct DancingLinksColumnNode;

		struct DancingLinksNode
		{
			DancingLinksNode(DancingLinksColumnNode* columnHeader = nullptr)
			{
				L = R = U = D = this;
				C = columnHeader;
			}

			void hookRight(DancingLinksNode* nodeIn)
			{
				nodeIn->R = this->R;
				nodeIn->L = this;
				
				this->R = nodeIn;
				nodeIn->R->L = nodeIn;
			}

			void hookDown(DancingLinksNode* nodeIn)
			{
				nodeIn->D = this->D;
				nodeIn->U = this;

				this->D = nodeIn;
				nodeIn->D->U = nodeIn;

				nodeIn->C->size += 1;
			}

			void unlinkLR()
			{
				this->L->R = this->R;
				this->R->L = this->L;
				++m_countLinkUpdates;
			}

			void relinkLR()
			{
				this->L->R = this->R->L = this;
				++m_countLinkUpdates;
			}

			void unlinkUD()
			{
				this->U->D = this->D;
				this->D->U = this->U;
				this->C->size -= 1;
				++m_countLinkUpdates;
			}

			void relinkUD()
			{
				this->U->D = this->D->U = this;
				this->C->size += 1;
				++m_countLinkUpdates;
			}

			DancingLinksNode* L;
			DancingLinksNode* R;
			DancingLinksNode* U;
			DancingLinksNode* D;
			DancingLinksColumnNode* C;
		};

		struct DancingLinksColumnNode : public DancingLinksNode
		{
			DancingLinksColumnNode(int indexIn)
				: size(0), index(indexIn)
			{
				this->C = nullptr;
			}

			int size;
			//string name;
			int index;
		};

		int m_size;
		//bool m_findAllSolutions;
		DancingLinksColumnNode* m_masterHeader;
		vector<DancingLinksNode*> m_solutionNodes;
		//vector< vector<int> > m_solvedSudoku;

	public:
		static bool solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats);

		static vector< vector<bool> > getInitialGrid(int size);
		SudokuMatrixDancingLinks2(const vector< vector<int> >& dataIn, bool findAllSolutions = false);
		~SudokuMatrixDancingLinks2();
		bool solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions);
		bool search(int k, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions);
		void cover(DancingLinksColumnNode* columnNode);
		void uncover(DancingLinksColumnNode* columnNode);

		int getPerformanceParameter()
		{
			return m_countLinkUpdates;
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

		bool m_isValid;
		static unsigned long long m_countLinkUpdates;
		static unsigned long long m_countValuesTried;
		static int m_nodesLeaked;
	};


}
