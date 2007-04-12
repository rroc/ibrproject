#pragma once
#include <vector>
#include "CMatrix.h"

CMatrix::CMatrix()
{
	iRows=0;
	iCols=0;
}

CMatrix::CMatrix(int aRows, int aCols)
{

	std::vector<TVector3> r;
	r.resize(aCols);
	for(int rows=0; rows<aRows; rows++)
	{
		iMatrix.push_back(r);
	}
	iRows=aRows;
	iCols=aCols;
}
CMatrix::CMatrix(TVector3* aMatrix, int aRows, int aCols)
{
	iRows=aRows;
	iCols=aCols;

	//create empty Matrix
	std::vector<TVector3> r;
	r.resize(aCols);
	for(int rows=0; rows<aRows; rows++)
	{
		iMatrix.push_back(r);
	}


	for(int rows=0; rows<aRows; rows++)
	{
		for(int cols=0; cols<aCols; cols++)
		{
			iMatrix.at(rows).at(cols)=*(aMatrix+rows*aCols+cols);
		}
	}

}

//CMatrix::CMatrix(float* aMatrix, int aRows, int aCols)
//{
//	
//	std::vector<TVector3> r;
//	r.resize(aCols);
//	for(int rows=0; rows<aRows; rows++)
//	{
//		iMatrix.push_back(r);
//	}
//	for(int rows=0; rows<aRows; rows++)
//	{
//		for(int cols=0; cols<aCols; cols++)
//		{
//			float x=0;
//			float y=0;
//			float z=0;
//
//			for(int color=0;color<3;color++)
//			{
//				x=*(aMatrix+ rows*cols*3+cols*3+1);
//				y=*(aMatrix+ rows*cols*3+cols*3+2);
//				z=*(aMatrix+ rows*cols*3+cols*3+3);
//			}
//
//			TVector3 a(x,y,z);
//			(iMatrix.at(rows).at(cols)) = a;
//		}
//	}
//	iRows=aRows;
//	iCols=aCols;
//}

CMatrix::~CMatrix()
{
	for(int rows=0;rows<iRows;rows++)
		iMatrix.at(rows).clear();
	iMatrix.clear();
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
	for (int i=0,endI=newR; i<endI; i++)
	{
		for (int j=0, endJ=newC; j<endJ; j++)
		{
			//printf("\n rows=%d, cols=%d, rows1= %d, cols1= %d",i,j,aRow1+i,aCol1+j);
			cropped->iMatrix.at(i).at(j) = iMatrix.at(aRow1+i).at(aCol1+j);
		}
	}

	return cropped;

	//int newR=aRow2-aRow1;
	//int newC=aCol2-aCol1;
	//CMatrix *cropped=new CMatrix(newR, newC);
	//for(int rows=0, rows1=aRow1; rows<newR, rows1<aRow2; rows++,rows1++)
	//	for(int cols=0, cols1=aCol1; cols<newC, cols1<aCol2; cols++,cols1++)
	//	{
	//		cropped->iMatrix.at(rows).at(cols)=iMatrix.at(rows1).at(cols1);
	//	}
	//	return cropped;
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

void CMatrix::operator /(float a)
{
	a=a*1.0;
	for(int rows=0;rows<iRows; rows++)
	{
		for(int cols=0; cols<iCols; cols++)
		{
			iMatrix.at(rows).at(cols)/=a;
		}
	}
}


void CMatrix::operator *(float a)
{

	for(int rows=0;rows<iRows; rows++)
	{
		for(int cols=0; cols<iCols; cols++)
		{
			iMatrix.at(rows).at(cols)*=a;
		}
	}
}

void CMatrix::print()
{
	std::cout<<"\n not implemented yet";
	for(int rows=0;rows<iRows; rows++)
	{
		for(int cols=0; cols<iCols; cols++)
		{
			printf("[ ");
			iMatrix.at(rows).at(cols).print();
			printf(" ]      ");
		}
		printf("\n");
	}

}

float* CMatrix::returnFloat()
{
	float *p=new float[iRows*iCols*3];
	for (int rows=0;rows<iRows;rows++)
	{
		for (int cols=0; cols<iCols;cols++)
		{
			( *(p+rows*iCols*3+cols*3) )  = iMatrix.at(rows).at(cols).iX;
			( *(p+rows*iCols*3+cols*3+1) )= iMatrix.at(rows).at(cols).iY;
			( *(p+rows*iCols*3+cols*3+2) )= iMatrix.at(rows).at(cols).iZ;
		}
	}

	return p;
}