#include "CRay.h"
#include "TVector3.h"

static const float KIntersectionAngle( 1.99*M_PI );

CRay::CRay()
	{
	}

CRay::CRay(TVector3 aStart, TVector3 aDirection)
	: iStartPoint(aStart)
	{
	iDirection=aDirection.normalize();
	}

CRay::CRay( TVector3 aStart, const unsigned int aCubeId, const float aU, const float aV )
	: iStartPoint(aStart)
	{
	switch( aCubeId )
		{
		//up
		case 0:
			iDirection.iX = ( aU - 0.5 ) * 2.0;
			iDirection.iY = 1.0;
			iDirection.iZ = ( aV - 0.5 ) * (-2.0);
			break;
		//left
		case 1:
			iDirection.iX = -1.0;
			iDirection.iY = ( aV - 0.5 ) * (-2.0);
			iDirection.iZ = ( aU - 0.5 ) * (-2.0);
			break;
		//forward
		case 2:
			iDirection.iX = ( aU - 0.5 ) * (2.0);
			iDirection.iY = ( aV - 0.5 ) * (-2.0);
			iDirection.iZ = -1.0;
			break;
		//right
		case 3:
			iDirection.iX = 1.0;
			iDirection.iY = ( aV - 0.5 ) * (-2.0);
			iDirection.iZ = ( aU - 0.5 ) * (2.0);
			break;
		//down
		case 4:
			iDirection.iX = ( aU - 0.5 ) * (2.0);
			iDirection.iY = -1.0;
			iDirection.iZ = ( aV - 0.5 ) * (2.0);
			break;
		//backward
		case 5:
			iDirection.iX = ( aU - 0.5 ) * (2.0);
			iDirection.iY = ( aV - 0.5 ) * (2.0);
			iDirection.iZ = 1.0;
			break;
		default:
			break;
		}
	iDirection = iDirection.normalize();
	}

CRay::~CRay()
	{}

void CRay::Set(TVector3 aStart, TVector3 aDirection)
	{
	iStartPoint=aStart;
	iDirection=aDirection.normalize();
	}

bool CRay::IntersectTriangle2( TVector3* aTriangle[3], TVector3* aPlaneNormal ) 
	{
//	TVector3 edge1 = aTriangle[0]->vertex2Vector( *aTriangle[1] );
//	TVector3 edge2 = aTriangle[1]->vertex2Vector( *aTriangle[2] );
//	TVector3 planeNormal = edge1.cross(edge2);

	//Check the ray and plane are not parallel
	float dot=iDirection.dot( *aPlaneNormal );

	if(dot==0.0f)
		return false;

	//Get the ray parameter where it meets the plane
	float planeIntercept=-aPlaneNormal->dot(*aTriangle[0]);
	float t = -iStartPoint.dot( *aPlaneNormal ) - planeIntercept;
	t/=dot;

	//If this parameter is non-positive, no intersection with this triangle
	if(t<=0.0f)
		return false;

	//Calculate the point of intersection of ray and plane
	TVector3 intersectionPoint = iStartPoint + iDirection*t;

	//See if this is inside the triangle
	double angle=0.0;

	TVector3 vA;
	TVector3 vB;
	//Loop through vertices
	for(int j=0; j<3; ++j)
		{
		//Calculate the vector from this vertex to the intersection point
		vA.set( *aTriangle[j]-intersectionPoint );

		//Calculate the vector from the next vertex to the point
		vB.set( *aTriangle[(j+1) % 3]-intersectionPoint );

		//Get the angle between these vectors
		angle += acos( vA.dot( vB )/( vA.length() * vB.length() ) );
		}

	//If the sum of the angles is greater than 2*PI, then the point is inside
	if(angle >= KIntersectionAngle )
		{
		//Return the intersection parameter if requested
//		if(intersectionT)
//			*intersectionT=t;
		return true;
		}

	return false;
	}


bool CRay::IntersectTriangle( TVector3* aTriangle[3] )
	{
	TVector3 edge1 = (*aTriangle[1])-(*aTriangle[0]);
	TVector3 edge2 = (*aTriangle[2])-(*aTriangle[1]);

	TVector3 pvec( iDirection.cross( edge2 ) );

	//if close to 0 ray is parallel
	float det=edge1.dot( pvec );

	//to account for poor float precision use
	//so called epsilon value of 0.0001 instead
	//of comparing to exactly 0
	if( ( det < 0.0001f ) && (det > -0.0001f) )
		{
		return false;
		}

	//distance to plane, <0 means ray behind the plane
	TVector3 tvec = iStartPoint - (*aTriangle[0]);
	float u= tvec.dot( pvec );
	if( u<0.0 || u>det )
		return false;

	TVector3 qvec( tvec.cross( edge1 ) );
	float v=iDirection.dot( qvec );
	if(v<0.0||u+v >det)
		return false;

	return true;
	}

bool CRay::IntersectBoundingBoxAABB( TVector3 *aMin, TVector3 *aMax )
	{
	//If the source of the ray is within the box, there must be an intersection
	if(
		   aMin->iX <= iStartPoint.iX && aMax->iX >= iStartPoint.iX
		&& aMin->iY <= iStartPoint.iY && aMax->iY >= iStartPoint.iY
		&& aMin->iZ <= iStartPoint.iZ && aMax->iZ >= iStartPoint.iZ
		)
		{
		return true;
		}

	//RAY STARTS FROM OUTSIDE the BOX:
	//Keep track of the largest tNear and smallest tFar
	TVector3 tNear( -99999.0f,-99999.0f,-99999.0f );
	TVector3 tFar(   99999.0f, 99999.0f, 99999.0f );

	TVector3 t1;
	TVector3 t2;

	//CHECK X
	//If the ray is parallel to the x plane
	if(iDirection.iX == 0.0f )
		{
		//If the ray origin is not between the x extents of the box, return false
		if( aMin->iX <= iStartPoint.iX && aMax->iX >= iStartPoint.iX )
			return false;
		}
	else
		{
		//The ray is not parallel to the plane
		//Compute the intersection distances
		t1.iX =( aMin->iX - iStartPoint.iX )/ iDirection.iX;
		t2.iX =( aMax->iX - iStartPoint.iX )/ iDirection.iX;

		//Make sure t1 is the distance to the intersection with the near plane
		if(t1.iX>t2.iX)
			{
			float temp=t1.iX;
			t1.iX=t2.iX;
			t2.iX=temp;
			}

		//Update tNear and tFar
		if(t1.iX>tNear.iX)
			tNear.iX=t1.iX;

		if(t2.iX<tFar.iX)
			tFar.iX=t2.iX;
		}

	//CHECK Y
	//If the ray is parallel to the y plane
	if(iDirection.iY == 0.0f )
		{
		//If the ray origin is not between the x extents of the box, return false
		if( aMin->iY <= iStartPoint.iY && aMax->iY >= iStartPoint.iY )
			return false;
		}
	else
		{
		//The ray is not parallel to the plane
		//Compute the intersection distances
		t1.iY =( aMin->iY - iStartPoint.iY )/ iDirection.iY;
		t2.iY =( aMax->iY - iStartPoint.iY )/ iDirection.iY;

		//Make sure t1 is the distance to the intersection with the near plane
		if(t1.iY>t2.iY)
			{
			float temp=t1.iY;
			t1.iY=t2.iY;
			t2.iY=temp;
			}

		//Update tNear and tFar
		if(t1.iY>tNear.iY)
			tNear.iY=t1.iY;

		if(t2.iY<tFar.iY)
			tFar.iY=t2.iY;
		}

	//CHECK Z
	//If the ray is parallel to the z plane
	if(iDirection.iZ == 0.0f )
		{
		//If the ray origin is not between the x extents of the box, return false
		if( aMin->iZ <= iStartPoint.iZ && aMax->iZ >= iStartPoint.iZ )
			return false;
		}
	else
		{
		//The ray is not parallel to the plane
		//Compute the intersection distances
		t1.iZ =( aMin->iZ - iStartPoint.iZ )/ iDirection.iZ;
		t2.iZ =( aMax->iZ - iStartPoint.iZ )/ iDirection.iZ;

		//Make sure t1 is the distance to the intersection with the near plane
		if(t1.iZ>t2.iZ)
			{
			float temp=t1.iZ;
			t1.iZ=t2.iZ;
			t2.iZ=temp;
			}

		//Update tNear and tFar
		if(t1.iZ>tNear.iZ)
			tNear.iZ=t1.iZ;

		if(t2.iZ<tFar.iZ)
			tFar.iZ=t2.iZ;
		}

	//CHECK INTERSECTIONS:
	//If tNear>tFar, no intersection
	if( (tNear.iX>tFar.iX) || (tNear.iY>tFar.iY) || (tNear.iZ>tFar.iZ) )
		return false;

	//If tFar<0, box is behind ray
	if(tFar.iX<0.0 || tFar.iY<0.0 || tFar.iZ<0.0 )
		return false;

	return true;
	}

