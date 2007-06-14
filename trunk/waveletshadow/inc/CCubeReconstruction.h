#pragma once

//USAGE:
//float m1[4]= {9,7,3,5};
//float *data = &m1[0];
//float* data = new float[32*32*6];
//printf("[%f, %f, %f, %f]\n", *(p), *(p+1), *(p+2), *(p+3) );
//printf("Start\n");
//CCubeReconstruction d( data, 32*32*6 );
//printf("Done\n");
//printf("[%f, %f, %f, %f]\n", *(d.iData), *(d.iData+1), *(d.iData+2), *(d.iData+3) );


class CCubeReconstruction
	{
	public:
		CCubeReconstruction( float* aCubeMap, const int aFaceSize );
		~CCubeReconstruction();

	private:
		void reconstructionStep( int aStepMax );
		void normalize();

	public:
		float* iData;
		float* iDataTemp;
	};





//ALL WAVELET TESTS
////DECOMPOSE NO COLOR
//float m1[4]= {9,7,3,5};
//float *data = &m1[0];
////float* data = new float[32*32*6];
//printf("[%f, %f, %f, %f]\n", *(data), *(data+1), *(data+2), *(data+3) );
//printf("Start\n");
////CCubeDecomposition dm( data, 32*32*6 );
//CCubeDecomposition dm( data, 4 );
//printf("Done\n");
//printf("[%f, %f, %f, %f]\n", *(dm.iData), *(dm.iData+1), *(dm.iData+2), *(dm.iData+3) );

//CCubeReconstruction rm( dm.iData, 4);
//printf("Done\n");
//printf("[%f, %f, %f, %f]\n", *(rm.iData), *(rm.iData+1), *(rm.iData+2), *(rm.iData+3) );

////DECOMPOSE COLOR
//TVector3 c1[4] = {	TVector3(9,9,9)
//					,TVector3(7,7,7)
//					,TVector3(3,3,3)
//					,TVector3(5,5,5)
//					};
//TVector3 *cdata = &c1[0];
//CCubeVectorDecomposition d( cdata, 4 );
//printf("\n[%f, %f, %f]\n[%f, %f, %f]\n[%f, %f, %f]\n[%f, %f, %f]\n\n"
//	, (*(d.iData)).iX, (*(d.iData)).iY, (*(d.iData)).iZ, (*(d.iData+1)).iX, (*(d.iData+1)).iY, (*(d.iData+1)).iZ, (*(d.iData+2)).iX, (*(d.iData+2)).iY, (*(d.iData+2)).iZ, (*(d.iData+3)).iX, (*(d.iData+3)).iY, (*(d.iData+3)).iZ );

//printf("\nORIGINAL:\n");
//TVector3 sum0(0,0,0);
//for( int i=0; i<4; i++)
//	{
//	sum0 += (c1[i] * m1[i]); 
//	}
//printf("Sum0: [%f, %f, %f]\n", sum0.iX, sum0.iY, sum0.iZ );

//printf("\nDECOMPOSED:\n");
//TVector3 sum1(0,0,0);
//for( int i=0; i<4; i++)
//	{
//	sum1 += *(d.iData+i) * *(dm.iData+i); 
//	}
//sum1 *= sqrtf(4.0f)*sqrtf(4.0f);
//printf("Sum1: [%f, %f, %f]\n", sum1.iX, sum1.iY, sum1.iZ );


//printf("\nDECOMPOSED TEST:\n");
//float sum3(0);
//for( int i=0; i<4; i++)
//	{
//	sum3 += *(dm.iData+i) * *(dm.iData+i); 
//	}
////printf("Sum3: %f\n", sum3 );
//sum3 *= ( sqrtf(4.0f)*sqrtf(4.0f) );
//printf("Sum3 scaled: %f\n", sum3 );


