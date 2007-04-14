#pragma once
#include<vector>
#include "THashtable.h"
class CMatrixNoColors
{
	public:
		CMatrixNoColors();
		CMatrixNoColors(int aRows, int aCols);
		CMatrixNoColors(float* aMatrix, int aRows, int aCols);
		CMatrixNoColors(TIntHashTable *aMatrix, int aRows, int aCols, int aFace);//implemented for a cubemap, 'NOT' generic hashtable constructor
		
		~CMatrixNoColors();

		CMatrixNoColors * transpose();
		void print();
		void operator /(float a);
		void operator *(float a);
		


		CMatrixNoColors* crop(int aRow1,int aRow2,int aCol1, int aCol2 );
		void substitute(CMatrixNoColors *aMatrix, int aRow1, int aRow2, int aCol1, int aCol2);

		float* returnFloat();


		int iRows;
		int iCols;
   
		std::vector< std::vector<float> > iMatrix;
};