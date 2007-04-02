#include "basic.h"
#include "CMesh.h"
#include "TColorRGBA.h"

// **** Constructors
CMesh::CMesh()
	: iName("noname")
	, iMaterialColor( 1.0, 1.0, 1.0, 1.0 )
	{
	//empty on purpose
	}

CMesh::CMesh( const CMesh& aMesh )
	{
	iMin = aMesh.iMin;
	iMax = aMesh.iMax;
	iMaterialColor = aMesh.iMaterialColor;
	iName = aMesh.iName;
	iTextureId = aMesh.iTextureId;
	iVertices = aMesh.iVertices;
	iTriangles = aMesh.iTriangles;
	iFaceNormals = aMesh.iFaceNormals;
	iVertexNormals = aMesh.iVertexNormals;
	iFaceColors = aMesh.iFaceColors;
	}

// **** Destructor
CMesh::~CMesh()
	{
	this->clearMesh();
	}

// **** Public methods
void CMesh::clearMesh()
	{
	iVertices.clear();
	iTriangles.clear();
	iFaceNormals.clear();
	iVertexNormals.clear();
	iFaceColors.clear();
	}

/** \brief Method that calculate the face normal
*
*
*	This method shall calculate the face normal by a cross product of the two edges and
*	store the result in the iFaceNormals vector.<br>
*	Remember you have the TVector3::cros(TVector3) and the Vector::cros(TVector3, TVector3) method.
*
*	Remember that if there is a result in the iFaceNormals vector you might want to clear it.
*
*
*/
void CMesh::calculateFaceNormals()
	{
	this->iFaceNormals.clear();

	TVector3 vx1, vx2, vx3;
	for( int i=0, j=static_cast<int>(iTriangles.size()); i<j; i++)
		{
		//get points
		vx1 = this->iVertices.at( (this->iTriangles.at( i ).iV1) );
		vx2 = this->iVertices.at( (this->iTriangles.at( i ).iV2) );
		vx3 = this->iVertices.at( (this->iTriangles.at( i ).iV3) );

		//iTriangles have been defined counter-clockwise
		vx1 = vx2.vertex2Vector( vx1 ); //v2->v1
		vx2 = vx3.vertex2Vector( vx2 ); //v3->v2

		//Calculate cross product to find the normal vector
		vx1 = vx1.cross( vx2 );
		vx1 = vx1.normalize(); 		//Normalize it to length 1

		//update to the normals list
		this->iFaceNormals.push_back( vx1 );
		}
	}

/** \brief Method that calculate the vertex normals
*
*
*
*/
void CMesh::calculateVertexNormals()
	{
	if( iFaceNormals.size()<1)
		{
		calculateFaceNormals();
		}

	vector< pair<TVector3,int> > anl; //Added Normals List
	anl.resize( iVertices.size(), make_pair<TVector3,int>( TVector3(), 0) );

	int a,b,c;
	std::pair<TVector3,int> av1, av2, av3;

	for( int i=0, j=static_cast<int>(iTriangles.size()); i<j; i++)
		{
		//get vertex indices for current triangle
		a=this->iTriangles.at(i).iV1;
		b=this->iTriangles.at(i).iV2;
		c=this->iTriangles.at(i).iV3;

		//get the stored values for normals
		av1 = anl.at(a);
		av2 = anl.at(b);
		av3 = anl.at(c);

		//add new face normals to corresponding vertices
		anl.at(a) = make_pair<TVector3,int>( av1.first + this->iFaceNormals.at(i), av1.second+1 );
		anl.at(b) = make_pair<TVector3,int>( av2.first + this->iFaceNormals.at(i), av2.second+1 );
		anl.at(c) = make_pair<TVector3,int>( av3.first + this->iFaceNormals.at(i), av3.second+1 );
		}

	TVector3 normal;
	//create vertex normals by averaging the face normals connected to the vertex
	this->iVertexNormals.clear();
	for( int i=0, j=static_cast<int>(anl.size()); i<j; i++)
		{
		normal.set( anl.at(i).first ); // / anl.at(i).second );
		//divide combined vectors by their amount
		this->iVertexNormals.push_back( normal.normalize() );
		}
	}


/** \brief Method that calculate random color for each face
*
*	This method should calculate a random color for a face and
*	store it in the faceColor vector. Use the TColorRGBA::randomColors()
*	method. <br>
*	You only want to do these calculations once, but the method might be called
*	every time a new frame is calculated. There is a method size that might
*	could be usefull for you.
*
*/
void CMesh::randomColors()
	{
	iFaceColors.clear();
	if(0 == iFaceColors.size() )
		{
//		srand( clock() );
		TColorRGBA a;
		for( int i=0, j=static_cast<int>(iTriangles.size()); i<j; i++)
			{
			this->iFaceColors.push_back( a.randomColors() );
			}
		}
	}

/** \brief Method that calculate random colors for the color you specified
*/
void CMesh::randomColors( TColorRGBA& aColorMult, float aBase )
	{
	iFaceColors.clear();
	if(0 == iFaceColors.size() )
		{
//		srand( clock() );
		TColorRGBA a;
		TColorRGBA baseColor = aColorMult*aBase;
		for( int i=0, j=static_cast<int>(iTriangles.size()); i<j; i++)
			{
			this->iFaceColors.push_back( (aColorMult*(a.randomColors())+baseColor) );
			}
		}
	}

/** \brief Method that set the same color for all faces
*
*	Method that set the same color for all faces
*
* \param _r the value of the red
* \param _g the value of the green
* \param _b the value of the blue value
*
*/
void CMesh::setSolidColor(float aR, float aG, float aB)
	{
	if(this->iFaceColors.size() > 0)
		{
		this->iFaceColors.clear();
		}
	TColorRGBA c(aR,aG,aB);
	for(int i = 0; i < static_cast<int>(this->iTriangles.size()); i++)
		{
		this->iFaceColors.push_back(c);
		}
	}


