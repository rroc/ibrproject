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

		void nonStandardDeconstruction();
		void nonStandardReconstruction();
		void standardDeconstruction();
		void standardReconstruction();
		void print();
		

		//non-standard steps- with/without colors
		CMatrix* nonStandardDeconstructionStep(CMatrix *aMatrix);
		CMatrix* nonStandardReconstructionStep(CMatrix *aMatrix);
		CMatrixNoColors* nonStandardDeconstructionStep(CMatrixNoColors *aMatrix);
		CMatrixNoColors* nonStandardReconstructionStep(CMatrixNoColors *aMatrix);

		//standard steps- with/without colors
		/*CMatrix* standardDeconstructionStep(CMatrix *aMatrix);
		CMatrix* standardReconstructionStep(CMatrix *aMatrix);
		CMatrixNoColors* standardDeconstructionStep(CMatrixNoColors *aMatrix);
		CMatrixNoColors* standardReconstructionStep(CMatrixNoColors *aMatrix);*/

		

		float* returnFloat();

	private:	
		float ComputeParentSum( CWavelet aF, TSquare aS);
		void ComputeChildrenSums( CWavelet aG, CWavelet aH );

		float Product( CWavelet aG, CWavelet aH, TSquare aS, int aM);

		float F(int aM, int aL, int aX, int aY );
		int sign( int aM, int aX, int aY );


	public:
		CMatrix *iWavelet;
		CMatrixNoColors *iWaveletNoColors;
	private:		
		int iRows;
		int iCols;

		THashTable iParentSum;
		THashTable iChildSum;
	public:
		bool withColors;
		bool decomposed;
		bool recomposed;
	};

