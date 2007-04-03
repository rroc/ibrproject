#pragma once

#include "basic.h"
#include "THashtable.h"

class CWavelet
	{
	public:
		CWavelet(void);
		~CWavelet(void);

		float * Decompose(float* aMatrix, int aRows, int aCols);
		float * Reconstruct(float* aWavelet, int aRows, int aCols);

	private:
		float* DecomposeStep();
		float* ReconstructStep();
		float* transpose(float* aMatrix);

		float ComputeParentSum( CWavelet aF, TSquare aS);
		void ComputeChildrenSums( CWavelet aG, CWavelet aH );

		float Product( CWavelet aG, CWavelet aH, TSquare aS, int aM);

		float F(int aM, int aL, int aX, int aY );
		int sign( int aM, int aX, int aY );

	private:
		float *iMatrix;
		float *iWavelet;
		int iRows;
		int iCols;

		THashTable iParentSum;
		THashTable iChildSum;
	};

