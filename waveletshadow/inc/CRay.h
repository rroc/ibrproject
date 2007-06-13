
#ifndef CRay_H
#define CRay_H
#include "TVector3.h"
class CRay
	{
	public:
		CRay();
		CRay(TVector3 aStart, TVector3 aDirection);
		CRay( TVector3 aStart, const unsigned int aCubeId, const float aU, const float aV );

		~CRay();

		void Set(TVector3 aStart, TVector3 aDirection);
		//bool IntersectTriangle( TVector3* aTriangle[3] );
		bool IntersectTriangle( TVector3* aTriangle[3], TVector3* aPlaneNormal ) ;
		bool IntersectBoundingBoxAABB( TVector3 *aMin, TVector3 *aMax );

	public:
		TVector3 iStartPoint;			//Source of ray
		TVector3 iDirection;			//Direction (normalised)
	};

#endif