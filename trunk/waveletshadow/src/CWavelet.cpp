#include <math.h>

#include "CWavelet.h"

CWavelet::CWavelet(void)
	{
	}

CWavelet::~CWavelet(void)
	{
	}

//Pre-compute the sums for product calculation
float CWavelet::ComputeParentSum( CWavelet aF, TSquare aS)
	{
	THashTable::iterator it = iParentSum.find( aS );

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
							THashTable::iterator it = iChildSum.find( s );

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