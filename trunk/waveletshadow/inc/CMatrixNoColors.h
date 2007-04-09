#pragma once
#include<vector>
class CMatrixNoColors
{
	public:
		CMatrixNoColors();
		CMatrixNoColors(int aRows, int aCols);
		CMatrixNoColors(float* aMatrix, int aRows, int aCols);
		//CMatrixNoColors(TVector3 *aMatrix, int aRows, int aCols);
		
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