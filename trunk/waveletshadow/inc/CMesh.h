#pragma once

#include "TTriangle.h"
#include "TVector3.h"
#include "TColorRGBA.h"
#include "THashtable.h"

/** \brief Main 3D graphic object class
*
* This class is the hub of the object hierarchy. It holds the geometry, color and lighting data for a
* 3D object, and it handles the culling, shading calculations.
*
* If you want to implement you own object it should inherit from this class. Look at CIcosahedron.h and
* CIcosahedron.cpp how to implement you own object.
*
* Calculations that is performed on TMatrix4 , TVector3 or Vertix4f is done in these classes.
*
*
*/
class CMesh {
	public:
		// Constructor
		CMesh();
		CMesh( const CMesh& aMesh );

		// Destructor
		virtual ~CMesh();

		// functions
		void clearMesh();

		void calculateFaceNormals();
		void calculateVertexNormals();

		void setSolidColor(float aR, float aG, float aB);
		virtual void randomColors();
		void randomColors( TColorRGBA& aColorMult, float aBase );

	public:
		std::string     iName;
		TColorRGBA		iMaterialColor;

		TVector3		iMin;
		TVector3		iMax;

		vector<TVector3>  iVertices; /// vertex coordinates
		vector<TTriangle> iTriangles; ///  triangle list
		vector<TVector3>  iFaceNormals;   /// normals
		vector<TVector3>  iVertexNormals; /// normals

		vector<TColorRGBA> iFaceColors; ///  face colors for each triangle

		vector<TColorRGBA> iVertexColors; ///  face colors for each triangle


		//per mesh we store (KSamplingResolution*KSamplingResolution*6) floats
		vector< vector<float> >	iVisibilityCoefficients;
		vector< TIntHashTable >	iVisibilityHash;
		vector< TIntHashTable > iWaveletHash;

		int iTextureId;
	};
