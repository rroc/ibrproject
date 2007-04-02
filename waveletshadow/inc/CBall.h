#ifndef CBALL_H
#define CBALL_H

#include "Basic.h"
#include "TTriangle.h"
#include "TVector3.h"
//#include "TMatrix4.h"
#include "CMesh.h"


/** \brief Class that defines a ball
*
* Class that defines a ball
* data is entered into
*	- vertices
*	- triangles
*	- faceVisibility (true)
*
* These are inherited  from the CMesh object.
*
*
*/
class CBall : public CMesh
	{
	public:
		CBall();
		CBall( int aRowSize );
		CBall( int aRowSize, float aScale );
		~CBall();
		void randomColors(); //virtual function

	private:
		void init( int aRowSize, float aScale );
	private:
		int iRowSize;
	};

#endif

