#include "basic.h"
#include "CCubeReconstruction.h"

static const float K1BySqrt2( 1.0f / sqrtf(2.0f) );

//Do the wavelet decomposition
CCubeReconstruction::CCubeReconstruction( float* aDecomposedCubeMap, const int aCubeSize )
	{
	float deNormalizeScale = sqrtf( static_cast<float>(aCubeSize) );

	//printf("CU: %d, %f, %f\n",aCubeSize,iNormalizeScale,i1ByNormalizeScale);
	int h = 2;

	iData     = new float[aCubeSize];
	iDataTemp = new float[aCubeSize];

	//PREPARE DATA
	for(int i=0; i<aCubeSize; i++)
		{
		*(iData+i) = *(aDecomposedCubeMap+i);
		}

	//Decompose
	while( h <= aCubeSize )
		{
		reconstructionStep( h );
		h *= 2;
		}

	//NORMALIZE
	for(int i=0; i<aCubeSize; i++)
		{
		*(iData+i) *= deNormalizeScale;
		}

	//free the temp buffer
	if( NULL != iDataTemp)
		{
		delete iDataTemp;
		}
	}

CCubeReconstruction::~CCubeReconstruction()
	{
	if( this->iData != NULL )
		{
		delete this->iData;
		this->iData = NULL;
		}
	}

void CCubeReconstruction::reconstructionStep( int aStepMax )
	{
	int i2;
	for(int i=0, endI = aStepMax/2; i<endI; i++ )
		{
		i2 = i*2;
		*(iDataTemp+i2)		= ( *(iData+i) + *(iData+endI+i) ) * K1BySqrt2; 
		*(iDataTemp+i2+1)	= ( *(iData+i) - *(iData+endI+i) ) * K1BySqrt2;
		//printf("(%d/%d) - [%f, %f, %f, %f]\n", i, endI, *(iDataTemp), *(iDataTemp+1), *(iDataTemp+2), *(iDataTemp+3) );
		}
	//Update data
	for(int i=0; i<aStepMax; i++)
		{
		*(iData+i) = *(iDataTemp+i);
		}
	}
