#pragma once

#include "Basic.h"
#include "TTriangle.h"
#include "TVector3.h"
//#include "TMatrix4.h"
#include "CMesh.h"

class CBox : public CMesh
	{
	public:
		CBox();
		CBox( float aWidth, float aHeight );
		CBox( float aWidth, float aHeight, float aDepth );

		~CBox();

	private:
		void init( float aWidth, float aHeight, float aDepth );
	};
