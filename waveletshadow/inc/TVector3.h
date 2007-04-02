#pragma once

#include "basic.h"

/** \brief Class that defines a three-component vector
*
*       This class define a three-component vector and
*       the mathematical operations that involve it.
*
*       This class has nothing to do with the std::vector that
*       it a template to store diffrent types of object inside
*
*
*/
class TVector3
	{
	public:
		// Constructors
		TVector3()
			: iX(0)
			, iY(0)
			, iZ(0) {};
		TVector3( const TVector3& aV ) : iX(aV.iX), iY(aV.iY), iZ(aV.iZ)
			{};
		TVector3( float aX, float aY, float aZ )
			: iX(aX)
			, iY(aY)
			, iZ(aZ) {};
		TVector3( unsigned int aColor, float aX, float aY, float aZ )
			: iX(aX)
			, iY(aY)
			, iZ(aZ) {};

		void set( float aX, float aY, float aZ ) { iX=aX; iY=aY; iZ=aZ; };
		void set( const TVector3& aVector ) { iX=aVector.iX; iY=aVector.iY; iZ=aVector.iZ; };

		// Destructor
		~TVector3(){};

		TVector3 normalize();
		TVector3 normalize() const;
		float length() const;

		TVector3 cross(const TVector3&) const;
		TVector3 cross(const TVector3&, const TVector3&) const;

		float dot(const TVector3&) const;
		float dot(const TVector3&, const TVector3&) const;

		TVector3 vertex2Vector( const TVector3& aSecondVertex ) const;
		TVector3 sphericalToVector(float aTheta, float aPhi  ) const;

		//PUBLIC OPERATORS
		TVector3 operator-(){ return TVector3(-iX, -iY, -iZ); };

		//assignments
		TVector3& operator+=( const TVector3& aV ){ iX+=aV.iX; iY+=aV.iY; iZ+=aV.iZ; return *this; };
		TVector3& operator-=( const TVector3& aV ){ iX-=aV.iX; iY-=aV.iY; iZ-=aV.iZ; return *this; };
		TVector3& operator*=( float aVal ){ iX*=aVal; iY*=aVal; iZ*=aVal; return *this; };
		TVector3& operator/=( float aVal ){ iX/=aVal; iY/=aVal; iZ/=aVal; return *this; };

		//math operators
		TVector3 operator+( const TVector3& aV2 ) { return TVector3(*this)+=aV2; };
		TVector3 operator-( const TVector3& aV2 ) { return TVector3(*this)-=aV2; };
		TVector3 operator*( float aVal ) { return TVector3(*this)*=aVal; };
		TVector3 operator/( float aVal ) { return TVector3(*this)/=aVal; };

		bool operator==( const TVector3& aV ) { return ( (iX==aV.iX)&&(iY==aV.iY)&&(iZ==aV.iZ) )? true:false; };
		bool operator!=( const TVector3& aV ) { return !(*this==aV); };
		bool operator<( const TVector3& aV ) { return ( (iX<aV.iX)&&(iY<aV.iY)&&(iZ<aV.iZ) )? true:false; };
		bool operator>( const TVector3& aV ) { return ( (iX>aV.iX)&&(iY>aV.iY)&&(iZ>aV.iZ) )? true:false; };
		bool operator<( const float aV ) { return ( (iX<aV)&&(iY<aV)&&(iZ<aV) )? true:false; };
		bool operator>( const float aV ) { return ( (iX>aV)&&(iY>aV)&&(iZ>aV) )? true:false; };

	public:
		float iX; ///< The x-component of the vector
		float iY; ///< The y-component of the vector
		float iZ; ///< The z-component of the vector
	};






//INLINE FUNCTIONS

/// Method that normalizes the vector to unit length
inline
TVector3 TVector3::normalize()
	{
	float length = this->length();

	//Make sure its not too short
	if(0.0 == length) length=1;

	//return the normalized vector
	return TVector3
		(
		this->iX / length
		,this->iY / length
		,this->iZ / length
		);
	}


/// Method that normalizes the vector to unit length
inline
TVector3 TVector3::normalize() const
	{
	float length = this->length();

	//Make sure its not too short
	if(0.0 == length) length=1;

	//return the normalized vector
	return TVector3
		(
		this->iX / length
		,this->iY / length
		,this->iZ / length
		);
	}

/** \brief Cross(vector) product of two vectors (this . _v1)
*       \param _v1 the vector that this should be crossed with
*       \return The result as a TVector3
*
*       | i  j  k  |
*   | aX aY aZ | = (aY*bZ - aZ*bY)i - (aX*bZ - aZ*bX)j + (aX*bY - aY*bX)k
*   | bX bY bZ |
*/
inline
TVector3 TVector3::cross(const TVector3& _v1) const
	{
	return TVector3
		(
		this->iY * _v1.iZ  -  this->iZ * _v1.iY        //X
		,this->iZ * _v1.iX  -  this->iX * _v1.iZ        //Y
		,this->iX * _v1.iY  -  this->iY * _v1.iX        //Z
		);
	}
/** \brief Cross(vector) product of two vectors
*       Method that produces the cross product between two vectors, given as arguments
*       \param _v1 the first vector for the cross product
*       \param _v2 the second vector for the cross product
*       \return The result as a TVector3 : |vector| =A cross B = A*B*sin(AB), angle(vector) = 90° to the plane AB
*/
inline
TVector3 TVector3::cross(const TVector3& _v1, const TVector3& _v2) const
	{
	// (i x i) = (j x j) = (k x k) = 0
	// (i x j) =k; (j x k) =i;  (k x i) =j;
	// AxB = (Ay*Bz-Az*By)i + (Az*Bx-Ax*Bz)j + (Ax*By-Ay*Bx)k
	return  TVector3(
		_v1.iY * _v2.iZ  -  _v1.iZ * _v2.iY   //(Ay*Bz-Az*By)i = X
		, _v1.iZ * _v2.iX  -  _v1.iX * _v2.iZ   //(Az*Bx-Ax*Bz)j= Y
		, _v1.iX * _v2.iY  -  _v1.iY * _v2.iX   //(Ax*By-Ay*Bx)k= Z
		);
	}

/** \brief Dot (scalar) product between this and the vector given as argument
*
*       Method that calculates the dot (scalar) product between this vector and
*       the vector given as the input argument.
*
*       \param _v1 the second vector for the dot product
*       \return The result as a float : A dot B = |A|*|B| * cos(ß)
*
*/
inline
float TVector3::dot(const TVector3& _v1) const
	{
	return (  this->iX * _v1.iX
		+ this->iY * _v1.iY
		+ this->iZ * _v1.iZ
		);
	}

/** \brief Dot (scalar) product of two vectors
*
*       Method that produces the dot product between two vectors, given as arguments
*       \param _v1 the first vector for the dot product
*       \param _v2 the second vector for the dot product
*
*       \return The result as a float
*
*/
inline
float TVector3::dot(const TVector3& _v1, const TVector3& _v2) const
	{
	return (
		_v1.iX * _v2.iX
		+ _v1.iY * _v2.iY
		+ _v1.iZ * _v2.iZ
		);
	}

/// Length of the vector (absolute value)
inline
float TVector3::length() const
	{
	return sqrt( iX*iX + iY*iY + iZ*iZ );
	}


// Returns TVector3 created from two Vertices: this -> second
inline
TVector3 TVector3::vertex2Vector( const TVector3& aSecondVertex ) const
	{
	//vector from A to B = b-a
	return TVector3( aSecondVertex.iX-this->iX, aSecondVertex.iY-this->iY, aSecondVertex.iZ-this->iZ );
	}

inline
TVector3 TVector3::sphericalToVector(float aTheta, float aPhi  ) const
	{
	float sinPhi	= sin( aPhi );
	float cosPhi	= cos( aPhi );
	float sinTheta	= sin( aTheta );
	float cosTheta	= cos( aTheta );
	return ( TVector3( sinTheta*cosPhi, sinTheta*sinPhi, cosTheta ).normalize() );
//	return TVector3(sin(aTheta)*cos(aPhi), sin(aTheta)*sin(aPhi), cos(aTheta));
	}