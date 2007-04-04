#pragma once
#include <vector>
#include "CMatrix.h"

CMatrix::CMatrix(int aRows, int aCols)
{

	std::vector<float> r;
	r.resize(aCols,0.0);
	for(int rows=0; rows<aRows; rows++)
	{
		iMatrix.push_back(r);
	}
	iRows=aRows;
	iCols=aCols;
}

CMatrix::CMatrix(float* aMatrix, int aRows, int aCols)
{
	std::vector<float> r;
	r.resize(aCols,0.0);
	for(int rows=0; rows<aRows; rows++)
	{
		iMatrix.push_back(r);
	}
	for(int rows=0; rows<aRows; rows++)
	{
		for(int cols=0; cols<aCols; cols++)
		{
			float a = *(aMatrix + rows*aCols + cols);
			(iMatrix.at(rows).at(cols)) = a;
		}
	}
	iRows=aRows;
	iCols=aCols;
}

CMatrix::~CMatrix()
{
	for(int rows=0;rows<iRows;rows++)
		iMatrix.at(rows).clear();
}

//CMatrix* CMatrix::operator=(float *aMatrix, int aRows, int aCols)
//{
//	CMatrix newMatrix(aRows, aCols);
//	for(int rows=0; rows<aRows;rows++)
//		for(int cols=0; cols<aCols; cols++)
//		{
//			newMatrix.iMatrix.at(rows).at(cols)
//		}
//}
CMatrix * CMatrix::transpose()
{
	CMatrix *transpose=new CMatrix(iCols,iRows);
	for(int rows=0;rows<iRows; rows++)
		for(int cols=0; cols<iCols; cols++)
		{
			transpose->iMatrix.at(cols).at(rows)=iMatrix.at(rows).at(cols);
		}
		return transpose;
}

CMatrix* CMatrix::crop(int aRow1,int aRow2,int aCol1, int aCol2 )
{
	int newR=aRow2-aRow1;
	int newC=aCol2-aCol1;
	CMatrix *cropped=new CMatrix(newR, newC);
	for(int rows=0, rows1=aRow1; rows<newR, rows1<aRow2; rows++,rows1++)
		for(int cols=0, cols1=aCol1; cols<newC, cols1<aCol2; cols++,cols1++)
		{
			cropped->iMatrix.at(rows).at(cols)=iMatrix.at(rows1).at(cols1);
		}
	return cropped;
}

void CMatrix::substitute(CMatrix *aMatrix, int aRow1, int aRow2, int aCol1, int aCol2)
{
	int R=aRow2-aRow1;
	int C=aCol2-aCol1;
	for(int rows=aRow1, r=0; rows<aRow2, r<R; rows++, r++)
	{
		for(int cols=aCol1,c=0; cols<aCol2, c<C; cols++,c++)
		{
			//printf("\n%d",cols);
			//iMatrix[rows][cols]= aMatrix->iMatrix[rows][cols];
			iMatrix.at(rows).at(cols)= aMatrix->iMatrix.at(r).at(c);
		}
	}
}

void CMatrix::print()
{
	for(int rows=0;rows<iRows; rows++)
	{
		for(int cols=0; cols<iCols; cols++)
		{
			printf("%f\t",iMatrix.at(rows).at(cols));
		}
		printf("\n");
	}

}