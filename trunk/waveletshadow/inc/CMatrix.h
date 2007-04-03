#pragma once
#include<vector>
class CMatrix
{
	public:
		CMatrix(int aRows, int aCols);
		CMatrix(float* aMatrix, int aRows, int aCols);
		~CMatrix();

		CMatrix * transpose();
		void print();
		//CMatrix* operator=(float *aMatrix, int aRows, int aCols);


		CMatrix* crop(int aRow1,int aRow2,int aCol1, int aCol2 );
		void substitute(CMatrix *aMatrix, int aRow1, int aRow2, int aCol1, int aCol2);

	private:
		int iRows;
		int iCols;
    private:
		std::vector< std::vector<float> > iMatrix;
};