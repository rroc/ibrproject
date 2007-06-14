#pragma once

//USAGE:
//float m1[4]= {9,7,3,5};
//float *data = &m1[0];
//float* data = new float[32*32*6];
//printf("[%f, %f, %f, %f]\n", *(p), *(p+1), *(p+2), *(p+3) );
//printf("Start\n");
//CCubeDecomposition d( data, 32*32*6 );
//printf("Done\n");
//printf("[%f, %f, %f, %f]\n", *(d.iData), *(d.iData+1), *(d.iData+2), *(d.iData+3) );


class CCubeDecomposition
	{
	public:
		CCubeDecomposition( float* aCubeMap, const int aFaceSize );
		~CCubeDecomposition();

	private:
		void decompositionStep( int aStepMax );
		void normalize();

	public:
		float* iData;
		float* iDataTemp;
	};
