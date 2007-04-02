#include "CBox.h"

CBox::CBox()
	{
	init( 1.0, 1.0, 1.0 );
	}

CBox::~CBox()
	{
	}

CBox::CBox( float aWidth, float aHeight )
	{
	init( aWidth, aHeight, aWidth );
	}

CBox::CBox( float aWidth, float aHeight, float aDepth )
	{
	init( aWidth, aHeight, aDepth );
	}

void CBox::init( float aWidth, float aHeight, float aDepth )
	{
/*
	float width = aWidth/2.0;
	float height = aHeight/2.0;
	float depth = aDepth/2.0;

	static const TColor32PSP white( 0xff, 0xff, 0xff );

	TVertexDataContainer container;
	container.iColor = white.GetPspColor();

	this->clearMesh();

	//0
	container.iVertex = TVector3 ( -width, -height, -depth);
	this->iVertexList.push_back( container );
	//1
	container.iVertex = TVector3 ( width, -height, -depth);
	this->iVertexList.push_back( container );
	//2
	container.iVertex = TVector3 ( -width, height, -depth);
	this->iVertexList.push_back( container );
	//3
	container.iVertex = TVector3 ( width, height, -depth);
	this->iVertexList.push_back( container );


	//4
	container.iVertex = TVector3 ( -width, -height,depth);
	this->iVertexList.push_back( container );
	//5
	container.iVertex = TVector3 ( width, -height,depth);
	this->iVertexList.push_back( container );
	//6
	container.iVertex = TVector3 ( width, height, depth);
	this->iVertexList.push_back( container );
	//7
	container.iVertex = TVector3 ( -width, height, depth);
	this->iVertexList.push_back( container );

	//Triangles
#ifdef _CW
	TTriangle t0(3,2,0); this->iTriangles.push_back(t0);
	TTriangle t1(3,0,1); this->iTriangles.push_back(t1);

	TTriangle t2(1,0,4); this->iTriangles.push_back(t2);
	TTriangle t3(1,4,5); this->iTriangles.push_back(t3);

	TTriangle t4(4,6,5); this->iTriangles.push_back(t4);
	TTriangle t5(4,7,6); this->iTriangles.push_back(t5);

	TTriangle t6(6,7,2); this->iTriangles.push_back(t6);
	TTriangle t7(6,2,3); this->iTriangles.push_back(t7);

	TTriangle t8(6,1,5); this->iTriangles.push_back(t8);
	TTriangle t9(6,3,1); this->iTriangles.push_back(t9);

	TTriangle t10(4,2,7); this->iTriangles.push_back(t10);
	TTriangle t11(4,0,2); this->iTriangles.push_back(t11);
#else
	TTriangle t0(0,2,3); this->iTriangles.push_back(t0);
	TTriangle t1(1,0,3); this->iTriangles.push_back(t1);

	TTriangle t2(4,0,1); this->iTriangles.push_back(t2);
	TTriangle t3(5,4,1); this->iTriangles.push_back(t3);

	TTriangle t4(5,6,4); this->iTriangles.push_back(t4);
	TTriangle t5(6,7,4); this->iTriangles.push_back(t5);

	TTriangle t6(2,7,6); this->iTriangles.push_back(t6);
	TTriangle t7(3,2,6); this->iTriangles.push_back(t7);

	TTriangle t8(5,1,6); this->iTriangles.push_back(t8);
	TTriangle t9(1,3,6); this->iTriangles.push_back(t9);

	TTriangle t10(7,2,4); this->iTriangles.push_back(t10);
	TTriangle t11(2,0,4); this->iTriangles.push_back(t11);
#endif

	//STRIPS
	this->iTriStripped = true;
	this->iTriStrip.push_back( 4 );
	this->iTriStrip.push_back( 3 );
	this->iTriStrip.push_back( 7 );
	this->iTriStrip.push_back( 8 );
	this->iTriStrip.push_back( 5 );
	this->iTriStrip.push_back( 3 );
	this->iTriStrip.push_back( 1 );
	this->iTriStrip.push_back( 4 );
	this->iTriStrip.push_back( 2 );
	this->iTriStrip.push_back( 7 );
	this->iTriStrip.push_back( 6 );
	this->iTriStrip.push_back( 5 );
	this->iTriStrip.push_back( 2 );
	this->iTriStrip.push_back( 1 );
*/
	}

