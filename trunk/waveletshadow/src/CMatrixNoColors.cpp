#pragma once
#include <vector>
#include "CMatrixNoColors.h"

CMatrixNoColors::CMatrixNoColors()
{
	iRows=0;
	iCols=0;
}

CMatrixNoColors::CMatrixNoColors(int aRows, int aCols)
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
//CMatrixNoColors::CMatrixNoColors(TVector3* aMatrix, int aRows, int aCols)
//{
//	iRows=aRows;
//	iCols=aCols;
//
//	//create empty Matrix
//	std::vector<TVector3> r;
//	r.resize(aCols);
//	for(int rows=0; rows<aRows; rows++)
//	{
//		iMatrix.push_back(r);
//	}
//
//
//	for(int rows=0; rows<aRows; rows++)
//	{
//		for(int cols=0; cols<aCols; cols++)
//		{
//			iMatrix.at(rows).at(cols)=*(aMatrix + rows*cols+ cols);
//		}
//	}
//
//}

CMatrixNoColors::CMatrixNoColors(float* aMatrix, int aRows, int aCols)
{
	iMatrix.clear();

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
			float a(*(aMatrix+rows*aCols+cols));
			(iMatrix.at(rows).at(cols)) = a;
		}
	}
	iRows=aRows;
	iCols=aCols;
}

CMatrixNoColors::~CMatrixNoColors()
{
	for(int rows=0;rows<iRows;rows++)
		iMatrix.at(rows).clear();
	iMatrix.clear();
}

//CMatrixNoColors* CMatrixNoColors::operator=(float *aMatrix, int aRows, int aCols)
//{
//	CMatrixNoColors newMatrix(aRows, aCols);
//	for(int rows=0; rows<aRows;rows++)
//		for(int cols=0; cols<aCols; cols++)
//		{
//			newMatrix.iMatrix.at(rows).at(cols)
//		}
//}
CMatrixNoColors * CMatrixNoColors::transpose()
{
	CMatrixNoColors *transpose=new CMatrixNoColors(iCols,iRows);
	for(int rows=0;rows<iRows; rows++)
		for(int cols=0; cols<iCols; cols++)
		{
			transpose->iMatrix.at(cols).at(rows)=iMatrix.at(rows).at(cols);
		}
		return transpose;
}

CMatrixNoColors* CMatrixNoColors::crop(int aRow1,int aRow2,int aCol1, int aCol2 )
{
	int newR=aRow2-aRow1;
	int newC=aCol2-aCol1;
	CMatrixNoColors *cropped=new CMatrixNoColors(newR, newC);
	//printf("\naR1=%d, aR2=%d, aC1=%d, aC2=%d, R=%d, C=%d",aRow1,aRow2,aCol1,aCol2,newR,newC);
	//cropped->print();
	printf("\ncropping.....");

	printf("\noriginal matrix\n");
	this->print();

	for (int i=0,endI=newR; i<endI; i++)
	{
		for (int j=0, endJ=newC; j<endJ; j++)
		{
		//printf("\n rows=%d, cols=%d, rows1= %d, cols1= %d",i,j,aRow1+i,aCol1+j);
		cropped->iMatrix.at(i).at(j) = iMatrix.at(aRow1+i).at(aCol1+j);
		}
	}

	
	printf("\n cropped Matrix is :");
	printf("\naR1=%d, aR2=%d, aC1=%d, aC2=%d, R=%d, C=%d\n",aRow1,aRow2,aCol1,aCol2,newR,newC);
	cropped->print();
		printf("\n............cropped!");
	return cropped;
}

void CMatrixNoColors::substitute(CMatrixNoColors *aMatrix, int aRow1, int aRow2, int aCol1, int aCol2)
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

void CMatrixNoColors::operator /(float a)
{
	a=a*1.0;
//	printf("\n ....division");

	for(int rows=0;rows<iRows; rows++)
	{
		printf("\n");
		for(int cols=0; cols<iCols; cols++)
		{
//			printf("(%f)", iMatrix.at(rows).at(cols));
			iMatrix.at(rows).at(cols)/=a;
//			printf("%f    ", iMatrix.at(rows).at(cols));
		}
	}
//	printf("\ndivision finished");
}
void CMatrixNoColors::operator * (float a)
{

	for(int rows=0;rows<iRows; rows++)
	{
		for(int cols=0; cols<iCols; cols++)
		{
			iMatrix.at(rows).at(cols)*=a;
		}
	}
}
void CMatrixNoColors::print()
{
	printf("\n Matrix without colors\n");
	for(int rows=0;rows<iRows; rows++)
	{
		for(int cols=0; cols<iCols; cols++)
		{
			printf("%f\t",iMatrix.at(rows).at(cols));
		}
		printf("\n");
	}

}

float* CMatrixNoColors::returnFloat()
{
	float *p=new float[iRows*iCols];
	for (int rows=0;rows<iRows;rows++)
	{
		for (int cols=0; cols<iCols;cols++)
		{
			(*(p+rows*iCols+cols))=iMatrix.at(rows).at(cols);
		}
	}
	
	return p;
}