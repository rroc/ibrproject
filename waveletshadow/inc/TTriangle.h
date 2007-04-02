#ifndef TRIANGLE1_H
#define TRIANGLE1_H

#include "Basic.h"

/** \brief Class that defines a triangle
*
*	This class define a triangle polygon and is used
*	in the triangles vector in the CMesh object.
*
*	The corners is orded in counter-clockwise from the front side
*
*/
class TTriangle
	{
	public:
		// Constructors
		TTriangle(): iV1(-1), iV2(-1), iV3(-1) {};
		TTriangle( int aV1, int aV2, int aV3): iV1(aV1), iV2(aV2), iV3(aV3) {};
		//inline void reverse();

		// Destructor
		~TTriangle(void){};

	public:
		///brief vertex indices
		int iV1, iV2, iV3;
//		int iN1, iN2, iN3;
	};

//INLINE FUNCTIONS
/// method that reverses the vertex order of the triangle
/*
inline
void TTriangle::reverse(void)
	{
	int _v = this->iV1;
	this->iV1 = iV2;
	this->iV2 = _v;
	}
*/
#endif
