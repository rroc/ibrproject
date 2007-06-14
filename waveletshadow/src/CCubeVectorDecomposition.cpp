#include "basic.h"
#include "CCubeVectorDecomposition.h"

static const float K1BySqrt2( 1.0f / sqrtf(2.0f) );

//Do the wavelet decomposition
CCubeVectorDecomposition::CCubeVectorDecomposition( TVector3* aCubeMap, const int aCubeSize )
	{
	float oneByNormalizeScale = 1.0f / sqrtf( static_cast<float>(aCubeSize));

	//printf("CV: %d, %f, %f\n",aCubeSize,iNormalizeScale, oneByNormalizeScale);
	int h = aCubeSize;

	iData     = new TVector3[aCubeSize];
	iDataTemp = new TVector3[aCubeSize];

	//NORMALIZE
	for(int i=0; i<aCubeSize; i++)
		{
		*(iData+i) = *(aCubeMap+i) * oneByNormalizeScale;
		}
//	printf("[%f, %f, %f, %f]\n", *(iData), *(iData+1), *(iData+2), *(iData+3) );

	//Decompose
	while( 0 < h )
		{
		//printf(".");
		decompositionStep( h );
		h /= 2;
		}
	//printf("\ndecompose ok...");

	//free the temp buffer
	if( NULL != this->iDataTemp)
		{
		delete this->iDataTemp;
		this->iDataTemp = NULL;
		}
	//printf("done\n");
	}

CCubeVectorDecomposition::~CCubeVectorDecomposition()
	{
	if( this->iData != NULL )
		{
		delete this->iData;
		this->iData = NULL;
		}
	}

void CCubeVectorDecomposition::decompositionStep( int aStepMax )
	{
	//Compose one step of wavelet
	int i2;
	for(int i=0, endI = aStepMax/2; i<endI; i++ )
		{
		i2 = i*2;
		*(iDataTemp+i)			= ( *(iData+i2) + *(iData+i2+1) ) * K1BySqrt2; 
		*(iDataTemp + endI + i) = ( *(iData+i2) - *(iData+i2+1) ) * K1BySqrt2;

		//printf("(%d/%d) - [%f, %f, %f, %f]\n", i, endI, *(iDataTemp), *(iDataTemp+1), *(iDataTemp+2), *(iDataTemp+3) );

		}
	//Update data
	for(int i=0; i<aStepMax; i++)
		{
		*(iData+i) = *(iDataTemp+i);
		}
	}
