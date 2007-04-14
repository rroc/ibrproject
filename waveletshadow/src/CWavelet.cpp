#include <math.h>

#include "CWavelet.h"

CWavelet::CWavelet( CMatrixNoColors* aMatrix ,int aRows, int aCols)
{
	iWaveletNoColors = aMatrix;
	iWavelet         = NULL;
	iRows            = aRows;
	iCols            = aCols;
	withColors       = false;
	decomposed       = false;
	recomposed       = false;
}

CWavelet::CWavelet( CMatrix* aMatrix ,int aRows, int aCols)
{
	iWavelet         = aMatrix;
	iWaveletNoColors = NULL;
	iRows            = aRows;
	iCols            = aCols;
	withColors       = true;
	decomposed       = false;
	recomposed       = false;
}
CWavelet::~CWavelet(void)
{
	iRows            = 0;
	iCols            = 0;
	withColors       = false;
	iWavelet         = NULL;
	iWaveletNoColors = NULL;
	decomposed       = false;
	recomposed       = false;

}
CMatrix* CWavelet::nonStandardDeconstructionStep(CMatrix *aMatrix)
{
	CMatrix *temp= new CMatrix(aMatrix->iRows,aMatrix->iCols);
	int limit=aMatrix->iCols/2;
	//printf("\nlimit=%d, iCols=%d",limit,aMatrix->iCols);

	//printf("\n decomposing the cropped.....\n result is \n");
	for(int i=0; i<limit;i++)
	{
		temp->iMatrix.at(0).at(i)       = ( aMatrix->iMatrix.at(0).at(2*i) + aMatrix->iMatrix.at(0).at(2*i+1) ) / (sqrt(2.0));
		temp->iMatrix.at(0).at(limit+i) = ( aMatrix->iMatrix.at(0).at(2*i) - aMatrix->iMatrix.at(0).at(2*i+1) ) / (sqrt(2.0));
	}
	aMatrix->~CMatrix();



	/*temp->print();
	printf("\n.............decomposed!");*/
	return temp;
}
CMatrix* CWavelet::nonStandardReconstructionStep(CMatrix *aMatrix)
{
	CMatrix *temp= new CMatrix(aMatrix->iRows,aMatrix->iCols);

	int limit=aMatrix->iCols/2;
	for (int i=0;i<limit;i++)
	{
		temp->iMatrix.at(0).at(2*i)   = ( aMatrix->iMatrix.at(0).at(i) + aMatrix->iMatrix.at(0).at(limit+i) )/sqrt(2.0);
		temp->iMatrix.at(0).at(2*i+1) = ( aMatrix->iMatrix.at(0).at(i) - aMatrix->iMatrix.at(0).at(limit+i) )/sqrt(2.0);
	}


	aMatrix->~CMatrix();
	return temp;
}

CMatrixNoColors* CWavelet::nonStandardDeconstructionStep(CMatrixNoColors *aMatrix)
{
	CMatrixNoColors *temp= new CMatrixNoColors(aMatrix->iRows,aMatrix->iCols);
	int limit=aMatrix->iCols/2;
	//printf("\nlimit=%d, iCols=%d",limit,aMatrix->iCols);

	//printf("\n decomposing the cropped.....\n result is \n");
	for(int i=0; i<limit;i++)
	{
		temp->iMatrix.at(0).at(i)       = ( aMatrix->iMatrix.at(0).at(2*i) + aMatrix->iMatrix.at(0).at(2*i+1) ) / (sqrt(2.0));
		temp->iMatrix.at(0).at(limit+i) = ( aMatrix->iMatrix.at(0).at(2*i) - aMatrix->iMatrix.at(0).at(2*i+1) ) / (sqrt(2.0));
	}
	aMatrix->~CMatrixNoColors();


	
	/*temp->print();
	printf("\n.............decomposed!");*/
	return temp;
}
CMatrixNoColors* CWavelet::nonStandardReconstructionStep(CMatrixNoColors *aMatrix)
{
	CMatrixNoColors *temp= new CMatrixNoColors(aMatrix->iRows,aMatrix->iCols);

	int limit=aMatrix->iCols/2;
	for (int i=0;i<limit;i++)
	{
		temp->iMatrix.at(0).at(2*i)   = ( aMatrix->iMatrix.at(0).at(i) + aMatrix->iMatrix.at(0).at(limit+i) )/sqrt(2.0);
		temp->iMatrix.at(0).at(2*i+1) = ( aMatrix->iMatrix.at(0).at(i) - aMatrix->iMatrix.at(0).at(limit+i) )/sqrt(2.0);
	}


	aMatrix->~CMatrixNoColors();
	return temp;
}

void CWavelet::nonStandardDeconstruction()
{
	if(withColors)
	{
		int g=iCols;

		//printf("\n original Matrix before decomposition..\n");
		//iWavelet->print();

		iWavelet->operator / (static_cast<float>(g));
		//		printf("\ng=%f,iCols=%d",g,iCols);

		//printf("\nnormalized..\n");
		//iWavelet->print();


		while (g>=17)
		{
			for(int row=0;row<g; row++)
			{
				/*printf("\n----------");
				printf("\ng=%f, iCols=%d", g, iCols);
				printf("\nrow =%d, row+1 = %d, g-1=%d",row,row+1, (g));*/
				CMatrix* cropped = iWavelet->crop(row,row+1,0, (g));
				CMatrix* decomposed= nonStandardDeconstructionStep(cropped);
				iWavelet->substitute(decomposed,row,row+1,0, (g));				
			}

			for(int cols=0;cols<g;cols++)
			{
				CMatrix* cropped= iWavelet->crop(0, (g),cols,cols+1);
				CMatrix *transposed= cropped->transpose();
				CMatrix* decomposed= nonStandardDeconstructionStep(transposed);
				CMatrix* actual_decomposed= decomposed->transpose();
				iWavelet->substitute(actual_decomposed, 0, (g),cols,cols+1);
			}
			g=g/2;
			//printf("\ng=%f,iCols=%d",g,iCols);
		}
	}
	else
	{
		int g=iCols;
		
		//printf("\n original Matrix before decomposition..\n");
		//iWaveletNoColors->print();

		iWaveletNoColors->operator / (static_cast<float>(g));
//		printf("\ng=%f,iCols=%d",g,iCols);

		//printf("\nnormalized..\n");
		//iWaveletNoColors->print();

		
		while (g>=2)
		{
			for(int row=0;row<g; row++)
			{
				/*printf("\n----------");
				printf("\ng=%f, iCols=%d", g, iCols);
				printf("\nrow =%d, row+1 = %d, g-1=%d",row,row+1, (g));*/
				CMatrixNoColors* cropped = iWaveletNoColors->crop(row,row+1,0, (g));
				CMatrixNoColors* decomposed= nonStandardDeconstructionStep(cropped);
				iWaveletNoColors->substitute(decomposed,row,row+1,0, (g));				
			}

			for(int cols=0;cols<g;cols++)
			{
				CMatrixNoColors* cropped= iWaveletNoColors->crop(0, (g),cols,cols+1);
				CMatrixNoColors *transposed= cropped->transpose();
				CMatrixNoColors* decomposed= nonStandardDeconstructionStep(transposed);
				CMatrixNoColors* actual_decomposed= decomposed->transpose();
				iWaveletNoColors->substitute(actual_decomposed, 0, (g),cols,cols+1);
			}
			g=g/2;
				//printf("\ng=%f,iCols=%d",g,iCols);
		}
	}
		//printf("\n after decomposition....\n");
		//iWaveletNoColors->print();
		//printf("\n-------------------\n");
	
	this->decomposed=true;
	this->recomposed=false;
}

void CWavelet::print()
{
printf("\nwith Colors= %d\ndeconstructed= %d\nreconstructed =%d\n ",withColors,decomposed, recomposed);
if(withColors)
	{
	iWavelet->print();
	}

else
	{
	iWaveletNoColors->print();
	}
}

void CWavelet::nonStandardReconstruction()
{
	if(withColors)
	{
		int g=2;
		while(g<=iCols)
		{
			for(int col=0; col<g; col++)
			{
				CMatrix *cropped=iWavelet->crop(0,g,col,col+1);
				CMatrix *transposed=cropped->transpose();
				CMatrix *reconstructed=nonStandardReconstructionStep(transposed);
				CMatrix *actual_reconstructed=reconstructed->transpose();
				iWavelet->substitute(actual_reconstructed, 0, g,col,col+1);
			}
			for (int row=0;row<g;row++)
			{
				CMatrix *cropped=iWavelet->crop(row,row+1,0,g);
				//CMatrix *transposed=cropped->transpose();
				CMatrix *reconstructed=nonStandardReconstructionStep(cropped);
				//CMatrix *actual_reconstructed=reconstructed->transpose();
				iWavelet->substitute(reconstructed, row,row+1,0,g);


			}
			g*=2;

		}
		iWavelet->operator *(iCols);

	}
	else
	{
	int g=2;
	while(g<=iCols)
	{
		for(int col=0; col<g; col++)
		{
			CMatrixNoColors *cropped=iWaveletNoColors->crop(0,g,col,col+1);
			CMatrixNoColors *transposed=cropped->transpose();
			CMatrixNoColors *reconstructed=nonStandardReconstructionStep(transposed);
			CMatrixNoColors *actual_reconstructed=reconstructed->transpose();
			iWaveletNoColors->substitute(actual_reconstructed, 0, g,col,col+1);
		}
		for (int row=0;row<g;row++)
		{
			CMatrixNoColors *cropped=iWaveletNoColors->crop(row,row+1,0,g);
			//CMatrixNoColors *transposed=cropped->transpose();
			CMatrixNoColors *reconstructed=nonStandardReconstructionStep(cropped);
			//CMatrixNoColors *actual_reconstructed=reconstructed->transpose();
			iWaveletNoColors->substitute(reconstructed, row,row+1,0,g);


		}
		g*=2;
	
	}
	iWaveletNoColors->operator *(iCols);
	}	

	this->decomposed=false;
	this->recomposed=true;

}

//CMatrixNoColors* CWavelet::standardDeconstructionStep(CMatrixNoColors *aMatrix)
//{
//	
//}
void CWavelet::standardDeconstruction()
{
	if (withColors)
	{
		iWavelet->operator /(sqrt(iCols*1.0));
		for (int row=0;row<iRows;row++)
		{
			int columns=iCols;
			while (columns>=2)
			{
				CMatrix *cropped=iWavelet->crop(row,row+1,0,columns);
				CMatrix *decomposed=nonStandardDeconstructionStep(cropped);
				iWavelet->substitute(decomposed,row,row+1,0,columns);
				columns/=2;
			}
		}
		//iWavelet->print();
	}
	else
	{
		iWaveletNoColors->operator /(sqrt(iCols*1.0));
		for (int row=0;row<iRows;row++)
		{
			int columns=iCols;
			while (columns>=2)
			{
				CMatrixNoColors *cropped=iWaveletNoColors->crop(row,row+1,0,columns);
				CMatrixNoColors *decomposed=nonStandardDeconstructionStep(cropped);
				iWaveletNoColors->substitute(decomposed,row,row+1,0,columns);
				columns/=2;
			}
		}
		//iWaveletNoColors->print();

	}
	
	decomposed=true;
	recomposed=false;
}

void CWavelet::standardReconstruction()
{
	if (withColors)
	{
		for (int row=0;row<iRows;row++)
		{
			int columns=2;
			while(columns<=iCols)
			{
				CMatrix *cropped= iWavelet->crop(row,row+1,0,columns);
				CMatrix *reconstructed=nonStandardReconstructionStep(cropped);
				iWavelet->substitute(reconstructed,row,row+1,0,columns);
				columns*=2;
			}
		}
		float temp=sqrt((float)iCols);
		iWavelet->operator * ( temp );
	} 
	else
	{
	for (int row=0;row<iRows;row++)
	{
		int columns=2;
		while(columns<=iCols)
		{
			CMatrixNoColors *cropped= iWaveletNoColors->crop(row,row+1,0,columns);
			CMatrixNoColors *reconstructed=nonStandardReconstructionStep(cropped);
			iWaveletNoColors->substitute(reconstructed,row,row+1,0,columns);
			columns*=2;
		}
	}
	float temp=sqrt((float)iCols);
	iWaveletNoColors->operator * ( temp );
	}
	decomposed=false;
	recomposed=true;
}

float* CWavelet::returnScaledFloat()
{
	if(withColors)
	{
		return iWavelet->returnScaledFloat();
	}
	else return NULL;
	/*
	else
		return iWaveletNoColors->returnScaledFloat();*/
}


float* CWavelet::returnFloat()
{
	if(withColors)
	{
		return iWavelet->returnFloat();
	}
	else
		return iWaveletNoColors->returnFloat();
}


//Pre-compute the sums for product calculation
float CWavelet::ComputeParentSum( CWavelet aF, TSquare aS)
{
return 0;
}

//pre-compute the children sums
void CWavelet::ComputeChildrenSums( CWavelet aG, CWavelet aH )
{
}

//The wavelet product
float CWavelet::Product( CWavelet aG, CWavelet aH, TSquare aS, int aM)
{
	return (0);
}

//Return a wavelet according to the parameters
float CWavelet::F(int aM, int aL, int aX, int aY )
{
	return 0.0f;
}

//return the sign of a wavelet
int CWavelet::sign( int aM, int aX, int aY )
{
	return 1;
}
