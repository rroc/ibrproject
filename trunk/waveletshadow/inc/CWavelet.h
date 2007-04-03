#pragma once

#include "basic.h"
#include "THashtable.h"

class CWavelet
	{
	public:
		CWavelet(void);
		~CWavelet(void);

		void Decompose();
		void Reconstruct();

	private:
		void DecomposeStep();
		void ReconstructStep();

		float ComputeParentSum( CWavelet aF, TSquare aS);
		void ComputeChildrenSums( CWavelet aG, CWavelet aH );

		float Product( CWavelet aG, CWavelet aH, TSquare aS, int aM);

		float F(int aM, int aL, int aX, int aY );
		int sign( int aM, int aX, int aY );

	private:

		THashTable iParentSum;
		THashTable iChildSum;
	};

