#pragma once
#include<vector>
#include "TVector3.h"
class CMatrix
{
	public:
		CMatrix();
		CMatrix(int aRows, int aCols);
		//CMatrix(float* aMatrix, int aRows, int aCols);
		CMatrix(TVector3 *aMatrix, int aRows, int aCols);
		
		~CMatrix();

		CMatrix * transpose();
		void print();
		//CMatrix* operator=(float *aMatrix, int aRows, int aCols);
		void operator /(float a);
		void operator *(float a);
		


		CMatrix* crop(int aRow1,int aRow2,int aCol1, int aCol2 );
		void substitute(CMatrix *aMatrix, int aRow1, int aRow2, int aCol1, int aCol2);
		float* returnFloat();


		int iRows;
		int iCols;
   
		std::vector< std::vector<TVector3> > iMatrix;
};