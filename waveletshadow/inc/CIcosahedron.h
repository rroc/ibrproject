#ifndef ICOSA1_H
#define ICOSA1_H

#include "Basic.h"
//#include "TTriangle.h"
#include "TVector3.h"
//#include "TMatrix4.h"
#include "CMesh.h"


/** \brief Class that defines a icosahedron
*
* Class that defines an icosahedron
* data is entered into
*	- vertices
*	- triangles
*	- faceVisibility (true)
*
* These are inherited  from the CMesh object.
*
*
* If you want to know more about icosahedron take a look at:
*
*  http://mathworld.wolfram.com/CIcosahedron.html
*
*
*/
class CIcosahedron : public CMesh
	{
	public:
		CIcosahedron();
		CIcosahedron( float aScale );
	private:
		void init( float s );
	};

#endif




