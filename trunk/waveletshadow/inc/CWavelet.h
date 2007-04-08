#pragma once

#include "basic.h"
#include "THashtable.h"
#include "CMatrix.h"
#include "CMatrixNoColors.h"

class CWavelet
	{
	public:
		CWavelet();
		CWavelet( CMatrixNoColors* aMatrix ,int aRows, int aCols);
		CWavelet( CMatrix* aMatrix ,int aRows, int aCols);

		~CWavelet();

		void Decompose();
		void Reconstruct();
		void print();
		

	
		CMatrix* DecompositionStep(CMatrix *aMatrix);
		CMatrix* ReconstructionStep(CMatrix *aMatrix);
		CMatrixNoColors* DecompositionStep(CMatrixNoColors *aMatrix);
		CMatrixNoColors* ReconstructionStep(CMatrixNoColors *aMatrix);

	private:	
		float ComputeParentSum( CWavelet aF, TSquare aS);
		void ComputeChildrenSums( CWavelet aG, CWavelet aH );

		float Product( CWavelet aG, CWavelet aH, TSquare aS, int aM);

		float F(int aM, int aL, int aX, int aY );
		int sign( int aM, int aX, int aY );

	private:
		CMatrix *iWavelet;
		CMatrixNoColors *iWaveletNoColors;
		
		int iRows;
		int iCols;

		THashTable iParentSum;
		THashTable iChildSum;
	public:
		bool withColors;
		bool decomposed;
		bool recomposed;
	};

