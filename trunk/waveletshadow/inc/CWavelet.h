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

		void ComputeParentSum( CWavelet aF, TSquare aS);
		void ComputeChildrenSums( CWavelet aG, CWavelet aH );

		void Product( TSquare aS, int aM);

	private:

		THashTable iParentSum;
		THashTable iChildSum;
	};

