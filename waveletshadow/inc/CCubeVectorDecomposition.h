#pragma once
#include "TVector3.h"

//USAGE:
//TVector3 m1[4] = {	TVector3(9,9,9)
//					,TVector3(7,7,7)
//					,TVector3(3,3,3)
//					,TVector3(5,5,5)
//					};
//TVector3 *data = &m1[0];

//CCubeVectorDecomposition d( data, 4 );
//printf("[%f, %f, %f]\n[%f, %f, %f]\n[%f, %f, %f]\n[%f, %f, %f]\n"
//	, (*(d.iData)).iX, (*(d.iData)).iY, (*(d.iData)).iZ, (*(d.iData+1)).iX, (*(d.iData+1)).iY, (*(d.iData+1)).iZ, (*(d.iData+2)).iX, (*(d.iData+2)).iY, (*(d.iData+2)).iZ, (*(d.iData+3)).iX, (*(d.iData+3)).iY, (*(d.iData+3)).iZ );


class CCubeVectorDecomposition
	{
	public:
		CCubeVectorDecomposition( TVector3* aCubeMap, const int aFaceSize );
		~CCubeVectorDecomposition();

	private:
		void decompositionStep( int aStepMax );
		void normalize();

	public:
		TVector3* iData;
		TVector3* iDataTemp;
	};
