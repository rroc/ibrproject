#include "CIcosahedron.h"


CIcosahedron::CIcosahedron()
	{
	this->init( 1 );
	}

CIcosahedron::CIcosahedron(float aScale )
	{
	this->init( aScale );
	}

void CIcosahedron::init( float s  ){

	this->clearMesh();
	float p = ((1.0 + sqrt(5.0))/2.0)*s;

	TVector3 v0(s,0.0,p);
	this->iVertices.push_back(v0);
	TVector3 v1(-s,0.0,p);
	this->iVertices.push_back(v1);
	TVector3 v2(s,0.0,-p);
	this->iVertices.push_back(v2);
	TVector3 v3(-s,0.0,-p);
	this->iVertices.push_back(v3);
	TVector3 v4(0.0,p,s);
	this->iVertices.push_back(v4);
	TVector3 v5(0,-p,s);
	this->iVertices.push_back(v5);
	TVector3 v6(0,p,-s);
	this->iVertices.push_back(v6);
	TVector3 v7(0.0,-p,-s);
	this->iVertices.push_back(v7);
	TVector3 v8(p,s,0.0);
	this->iVertices.push_back(v8);
	TVector3 v9(-p,s,0.0);
	this->iVertices.push_back(v9);
	TVector3 v10(p,-s,0.0);
	this->iVertices.push_back(v10);
	TVector3 v11(-p,-s,0.0);
	this->iVertices.push_back(v11);


	//Triangles
	//0
	TTriangle t0(0,4,1);
	this->iTriangles.push_back(t0);

	//1
	TTriangle t1(0,1,5);
	this->iTriangles.push_back(t1);

	//2
	TTriangle t2(0,5,10);
	this->iTriangles.push_back(t2);

	//3
	TTriangle t3(0,10,8);
	this->iTriangles.push_back(t3);

	//4
	TTriangle t4(0,8,4);
	this->iTriangles.push_back(t4);

	//5
	TTriangle t5(4,8,6);
	this->iTriangles.push_back(t5);

	//6
	TTriangle t6(4,6,9);
	this->iTriangles.push_back(t6);

	//7
	TTriangle t7(4,9,1);
	this->iTriangles.push_back(t7);

	//8
	TTriangle t8(1,9,11);
	this->iTriangles.push_back(t8);

	//9
	TTriangle t9(1,11,5);
	this->iTriangles.push_back(t9);

	//10
	TTriangle t10(2,7,3);
	this->iTriangles.push_back(t10);

	//11
	TTriangle t11(2,3,6);
	this->iTriangles.push_back(t11);

	//12
	TTriangle t12(2,6,8);
	this->iTriangles.push_back(t12);

	//13
	TTriangle t13(2,8,10);
	this->iTriangles.push_back(t13);

	//14
	TTriangle t14(2,10,7);
	this->iTriangles.push_back(t14);

	//15
	TTriangle t15(7,10,5);
	this->iTriangles.push_back(t15);

	//16
	TTriangle t16(7,5,11);
	this->iTriangles.push_back(t16);

	//17
	TTriangle t17(7,11,3);
	this->iTriangles.push_back(t17);

	//18
	TTriangle t18(3,11,9);
	this->iTriangles.push_back(t18);

	//19
	TTriangle t19(3,9,6);
	this->iTriangles.push_back(t19);
	}


