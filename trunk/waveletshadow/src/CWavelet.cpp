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
		iWavelet->print();
	else
		iWaveletNoColors->print();

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
		iWavelet->print();
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
		iWaveletNoColors->print();

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
	TSquareHashTable::iterator it = iParentSum.find( aS );

	//item already exists
	if( it != iParentSum.end( ) )
	{
		return (*it).second;
	}
	//calculate new item
	else
	{
		float value;

		//the first item
		if( 0 == aS.l && 0 == aS.x && 0 == aS.y )
		{
			value = 0.0f; //TODO: return the scaling coefficient F000, the scaling coefficient
		}
		//calculate the detail coefficients
		else
		{
			TSquare s(aS.l - 1, aS.x/2, aS.y/2);
			float qx = aS.x-(2.0f*s.x);
			float qy = aS.y-(2.0f*s.y);

			value = ComputeParentSum( aF, s );

			float sum(0.0f);
			for( int m=1; m<4; m++ )
			{
				sum += F( m, s.l, s.x, s.y ) * sign(m, qx, qy);
			}

			//combine the result
			value += pow( 2.0f, s.l ) * sum;
		}
		pair<TSquare,float> item = make_pair( aS, value );
		iParentSum.insert( item );

		return value;
	}
}

//pre-compute the children sums
void CWavelet::ComputeChildrenSums( CWavelet aG, CWavelet aH )
{
	//levels
	for(int l = 0; l<10; l++)
	{
		//Positions
		for(int x = 0; x<10; x++)
		{
			for(int y = 0; y<10; y++)
			{
				//m
				for(int m = 1; m<4; m++)
				{
					//Both Wavelets are non-zero
					if ( aG.F(m,l,x,y) != 0 &&  aH.F(m,l,x,y) != 0 )
					{
						float c = aG.F(m,l,x,y) * aH.F(m,l,x,y);
						TSquare s(l,x,y);

						//sum all the levels
						while( s.l >= 0 )
						{
							TSquareHashTable::iterator it = iChildSum.find( s );

							//exists already
							if( it != iChildSum.end( ) )
							{
								(*it).second = c + (*it).second;
							}
							//create new item
							else
							{
								pair<TSquare,float> item = make_pair(s,c);
								iChildSum.insert( item );
							}
							s.l -= 1;
							s.x /= 2;
							s.y /= 2;
						}
					}
				}
			}
		}
	}
}

//The wavelet product
float CWavelet::Product( CWavelet aG, CWavelet aH, TSquare aS, int aM)
{
	float twoToL = pow( 2.0f, aS.l );

	//1. all wavelets at the same square but of different type
	int m1 = ( (aM+1)%4 ) + 1;
	int m2 = ( (aM+2)%4 ) + 1;
	float c1 =  twoToL * 
		( 
		( aG.F( m1, aS.l,aS.x,aS.y ) * aH.F( m2, aS.l,aS.x,aS.y ) ) 
		+ ( aG.F( m2, aS.l,aS.x,aS.y ) * aH.F( m1, aS.l,aS.x,aS.y ) ) 
		);

	//2. Product of identical wavelets at strictly finer levels
	float c2 =
		twoToL * sign( aM, 0 ,0) * (*iChildSum.find( TSquare( aS.l+1, 2*aS.x,   2*aS.y   ) )).second
		+ twoToL * sign( aM, 1 ,0) * (*iChildSum.find( TSquare( aS.l+1, 2*aS.x+1, 2*aS.y   ) )).second
		+ twoToL * sign( aM, 0 ,1) * (*iChildSum.find( TSquare( aS.l+1, 2*aS.x,   2*aS.y+1 ) )).second
		+ twoToL * sign( aM, 1 ,1) * (*iChildSum.find( TSquare( aS.l+1, 2*aS.x+1, 2*aS.y+1 ) )).second;

	//3. One identical wavelet and the rest at coarser levels
	float c3 =    ( aG.F( aM, aS.l,aS.x,aS.y )*ComputeParentSum(aH, aS) ) 
		+ ( aH.F( aM, aS.l,aS.x,aS.y )*ComputeParentSum(aG, aS) );

	//return the sum of the contributions
	return (c1 + c2 +c3);
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
