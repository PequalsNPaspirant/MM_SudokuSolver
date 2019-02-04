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

#include "Puzzles_Sudoku/Puzzles_Sudoku_Utils.h"

namespace mm {

	//Forward declaraton
	struct SudokuPuzzleBasicStats;

	class FastestDancingLinksSudokuMatrix
	{
	//public:
		//struct DancingLinksColumnNode;

		struct DancingLinksNode
		{
			DancingLinksNode(DancingLinksNode* columnHeader = nullptr, int indexIn = 0)
				: size(0), index(indexIn)
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

			DancingLinksNode* L;
			DancingLinksNode* R;
			DancingLinksNode* U;
			DancingLinksNode* D;
			//DancingLinksColumnNode* C;
			DancingLinksNode* C;
			int size;
			int index;
		};

		/*
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
		};*/

		int m_size;
		static DancingLinksNode* m_masterHeader;
		static vector<DancingLinksNode*> m_rowsHeaders;

		vector<DancingLinksNode*> m_solutionNodes;
		vector<DancingLinksNode*> m_initiallyRemovedRows;
		//vector< vector<int> > m_solvedSudoku;

	public:
		static bool solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats);

		static void initialize(int size);
		static DancingLinksNode* getNewNode(DancingLinksNode* columnHeader = nullptr, int indexIn = 0);

		FastestDancingLinksSudokuMatrix(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats, bool findAllSolutions = false);
		~FastestDancingLinksSudokuMatrix();
		bool solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions);
		bool search(int k, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions);
		void cover(DancingLinksNode* columnNode);
		void uncover(DancingLinksNode* columnNode);

		static DancingLinksNode* m_memoryPool;
		static int m_memoryPoolIndex;

		bool m_isValid;
		SudokuPuzzleBasicStats& m_stats;

	private:
		void unlinkLR(DancingLinksNode* node)
		{
			node->L->R = node->R;
			node->R->L = node->L;
			++m_stats.m_cellsTriedOrLinkUpdates;
		}

		void relinkLR(DancingLinksNode* node)
		{
			node->L->R = node->R->L = node;
			++m_stats.m_cellsTriedOrLinkUpdates;
		}

		void unlinkUD(DancingLinksNode* node)
		{
			node->U->D = node->D;
			node->D->U = node->U;
			node->C->size -= 1;
			++m_stats.m_cellsTriedOrLinkUpdates;
		}

		void relinkUD(DancingLinksNode* node)
		{
			node->U->D = node->D->U = node;
			node->C->size += 1;
			++m_stats.m_cellsTriedOrLinkUpdates;
		}
	};


}
