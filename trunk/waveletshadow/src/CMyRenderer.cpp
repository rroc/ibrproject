//INCLUDES
#include <fstream>
#include "CMyRenderer.h"
#include "Texture.h"
#include "CMeshLoader.h"
#include "CObjLoader.h"
#include <ctime>
#include "CMatrixNoColors.h"
#include "CWavelet.h"
#ifdef USE_OPENMP
#include <omp.h>
#endif

//INIT STATIC DATA
CMyRenderer* CMyRenderer::iCurrentRenderer = 0;

static const float KEpsilon( 0.0001 );

//static const float	KObjectScale = 0.2f;
//static const string KObjectName = "monster"; //"testscene" (1.0); "monster" (0.2); "gt16k" (0.0045); //"simple4" (0.0045);
static const float	KObjectScale = 0.0035f;
static const string KObjectName = "simple4"; //"dragonflyfine_plate"(0.15), "testscene" (1.0); "monster" (0.2); "gt16k" (0.0045); //"simple4_2" (0.0035);
static const string KObjectFileName = KObjectName+".obj";

static const string KDataFileName	  = KObjectName + "_coefficients.bin";
static const string KHashDataFileName = KObjectName + "_hash_coefficients.bin";
static const string KWaveletDataFileName = KObjectName + "_wavelet_coefficients.bin";

static const float KLightVectorSize( 3.0 );

static const float KCubeMapSize(3.0f);

static const float vertices[8][3] = {

	{-KCubeMapSize,  KCubeMapSize, -KCubeMapSize}, // l t f
	{-KCubeMapSize, -KCubeMapSize, -KCubeMapSize}, // l b f
	{ KCubeMapSize, -KCubeMapSize, -KCubeMapSize}, // r b f
	{ KCubeMapSize,  KCubeMapSize, -KCubeMapSize}, // r t f

	{-KCubeMapSize,  KCubeMapSize,  KCubeMapSize}, // l t b
	{-KCubeMapSize, -KCubeMapSize,  KCubeMapSize}, // l b b
	{ KCubeMapSize, -KCubeMapSize,  KCubeMapSize}, // r b b
	{ KCubeMapSize,  KCubeMapSize,  KCubeMapSize}, // r t b
	};

//static const float norms[6][3] = 
//	{
//	{ 1, 0, 0}, 
//	{-1, 0, 0}, 
//	{ 0, 1, 0}, 
//	{ 0,-1, 0}, 
//	{ 0, 0,-1}, 
//	{ 0, 0, 1} 
//	};

static const int faces[6][4] = {

	{3, 2, 6, 7}, // right
	{4, 5, 1, 0}, // left
	{4, 0, 3, 7}, // top
	{1, 5, 6, 2}, // bottom
	{7, 6, 5, 4}, // back
	{0, 1, 2, 3}, // front
	};

//CONSTRUCTORS
//
//Default constructor
//Constructor width the screen size defined
CMyRenderer::CMyRenderer( const int aWidth, const int aHeight )
: iScreenHeight( aHeight )
, iScreenWidth( aWidth )
, iFrame(0)
, iTime(0)
, iTimebase(0)
, iLightingModelName("shadowed")

, iObjectRotationAngle(0)
, iObjectRotationAxis(0,0,0)
, iObjectRotationFinished( false )

, iLightRotationAngle(0)
, iLightRotationAxis(0,0,0)

, iCubeMapVertex(0)
, iWireFrame(GL_TRIANGLES)
, iVisualDecomposition( false )
, iScale(1.0f)
	{
	InitMain();
	}

//
//DESTRUCTOR releases the used heap
//
CMyRenderer::~CMyRenderer()
	{
	for(int i=0, j=iMeshList.size(); i<j; i++)
		{
		if( NULL != iMeshList.at(i) )
			{
			delete iMeshList.at(i);
			}
		}
	for(int i=0, j=iSceneGraph.size(); i<j; i++)
		{
		if( NULL != iSceneGraph.at(i) )
			{
			delete iSceneGraph.at(i);
			}
		}

	if( NULL != iLightProbe) 
		delete[] iLightProbe;
	if( NULL != iTransformedLightProbe) 
		delete[] iTransformedLightProbe;
	CMyRenderer::iCurrentRenderer = 0;
	}


void CMyRenderer::LoadTextures()
	{
	LoadTGATexture("church_roof.tga" );
	LoadTGATexture("church_left.tga" );
	LoadTGATexture("church_front.tga");
	LoadTGATexture("church_right.tga");
	LoadTGATexture("church_floor.tga");
	LoadTGATexture("church_back.tga" );
	}

//
/// Init function for the constructors
void CMyRenderer::InitMain()
	{
#ifdef USE_OPENMP
	omp_set_num_threads( 3 );
#endif

	//Prepare the navigation xform
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) &iObjectRotationXForm );
	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) &iObjectRotationXFormInv );
	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) &iLightRotationXForm );
	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) &iLightRotationXFormInv );

	glMatrixMode( GL_VIEWPORT );
	glViewport(0, 0, (GLsizei)iScreenWidth, (GLsizei)iScreenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLdouble)iScreenWidth/(GLdouble)iScreenHeight, 0.01, 1000);

	//	glOrtho( -1, 1, -1, 1, 0.01, 1000 );

	glMatrixMode(GL_MODELVIEW);

	//Construct the scenegraph
	CreateScene();

	LoadTextures();

	InitLights();

	//initialize cubemapvector samples
	iNumberOfSamples = InitializeSamplingData();

	PrecomputedRadianceTransfer();

	InitVertexMap();
	InitHashTables();

	//	SavePRTHashData();

	//	iLightVector = iLightData->GetLightVector();
	//glLightfv( GL_LIGHT0, GL_POSITION, reinterpret_cast<GLfloat*>(&iLightVector) );
	//iLightVector *= KLightVectorSize; // for drawing the vector!
	}


//From cube build vector map
TVector3 CMyRenderer::CubeToVector( int aCube, float aU, float aV )
	{
	switch(aCube)
		{
		case 0: //roof
			return( TVector3( 
				   (aU-0.5)*2.0f
				,  1.0f
				, -(aV-0.5)*2.0f 
				)//.normalize() 
				);
			break;
		case 1: //left
			return( TVector3( 
				  -1.0f
				, -(aV-0.5)*2.0f
				, -(aU-0.5)*2.0f 
				)//.normalize()
				);
			break;
		case 2: //front 
			return( TVector3( 
				   (aU-0.5)*2.0f
				, -(aV-0.5)*2.0f
				, -1.0f 
				)//.normalize()
				);
			break;
		case 3: //right
			return( TVector3( 
				   1.0f
				, -(aV-0.5)*2.0f
				,  (aU-0.5)*2.0f 
				)//.normalize()
				);
			break;
		case 4: //floor
			return( TVector3( 
				   (aU-0.5)*2.0f
				, -1.0f
				,  (aV-0.5)*2.0f 
				)//.normalize()
				);
			break;
		case 5: //back
			return( TVector3( 
				   (aU-0.5)*2.0f
				,  (aV-0.5)*2.0f
				,  1.0f 
				)//.normalize()
				);
			break;

		default:
			printf("ERROR (invalid cube size)\n");
			//no other options
			break;
		}
	}

//CODE FOR TESTING:
//printf("\nTEST:\n");
//TVector3 vec = renderer->CubeToVector(0, 0.1, 0.8);
//printf("[%f %f %f]\n", vec.iX, vec.iY, vec.iZ );	
//TVector3 result = renderer->VectorToCube( vec );
//printf("[%f %f %f]\n\n", result.iX, result.iY, result.iZ );	
TVector3 CMyRenderer::VectorToCube( const TVector3& aVector)
	{

//------------
//FRONT (0,0,-1)
if( ( aVector.iZ < 0 ) && ( aVector.iZ <= -fabs( aVector.iX ) ) &&  ( aVector.iZ <= -fabs( aVector.iY ) ) )
	{
	return ( TVector3(
		2 //cube id
		, 0.5 -  0.5 * aVector.iX/aVector.iZ
		, 0.5 +  0.5 * aVector.iY/aVector.iZ
		) 
		);
	}
//BACK (0,0,1)
else if( ( aVector.iZ >= 0 ) && ( aVector.iZ >= fabs( aVector.iX ) ) &&  ( aVector.iZ >= fabs( aVector.iY ) ) )
	{
	return ( TVector3(
		5 //cube id
		,  0.5 +  0.5 * aVector.iX/aVector.iZ
		,  0.5 +  0.5 * aVector.iY/aVector.iZ
		) 
		);
	}
//FLOOR
else if( ( aVector.iY <= 0 ) && ( aVector.iY <= -fabs( aVector.iX ) ) &&  ( aVector.iY <= -fabs( aVector.iZ ) ) )
	{
	return ( TVector3(
		4 //cube id
		, 0.5 -  0.5 * aVector.iX/aVector.iY
		, 0.5 -  0.5 * aVector.iZ/aVector.iY
		) 
		);
	}
	//ROOF
else if( ( aVector.iY >= 0 ) && ( aVector.iY >= fabs( aVector.iX ) ) &&  ( aVector.iY >= fabs( aVector.iZ ) ) )
	{
	return ( TVector3(
		0 //cube id
		, 0.5 +  0.5 * aVector.iX/aVector.iY
		, 0.5 -  0.5 * aVector.iZ/aVector.iY
		) 
		);
	}
//LEFT
else if( ( aVector.iX <= 0 ) && ( aVector.iX <= -fabs( aVector.iY ) ) &&  ( aVector.iX <= -fabs( aVector.iZ ) ) )
	{
	return ( TVector3(
		1 //cube id
		,  0.5 +  0.5 * aVector.iZ/aVector.iX
		,  0.5 +  0.5 * aVector.iY/aVector.iX
		) 
		);
	}

//RIGHT
else if( ( aVector.iX >= 0 ) && ( aVector.iX >= fabs( aVector.iY ) ) &&  ( aVector.iX >= fabs( aVector.iZ ) ) )
	{
	return ( TVector3(
		3 //cube id
		, 0.5 +  0.5 * aVector.iZ/aVector.iX
		, 0.5 -  0.5 * aVector.iY/aVector.iX
		) 
		);
	}
#ifdef _DEBUG
	else
		{
		printf("invalid vector for cubemap!");
		exit(-1);
		}
#endif // _DEBUG
	}

//////////////////////////////////////////////////////////////////////////
// Sampling data vectors for each cube face
// data is formed:
// foreach face in cubemap
//		foreach v coordinate in face
//			foreach u coordinate in face
//				calculate a corresponding vector
//
// ie. the vector data is stored as follows:
//
// (f0,u0,v0),(f0,u1,v0),(f0,u2,v0), ... 
// (f0,u0,v1),(f0,u1,v1),(f0,u2,v1), ... 
// ...
// (f1,u0,v0),(f1,u1,v0),(f1,u2,v0), ...
// (f1,u0,v1),(f1,u1,v1),(f1,u2,v1), ... 
//...
//////////////////////////////////////////////////////////////////////////
int CMyRenderer::InitializeSamplingData()
	{
	printf("Initializing sampling coefficients...");
	iSampleData.clear();

	int index(0);
	float diff = 1.0f/(KSamplingResolution-1);
	
	for (int cube=0; cube<6; cube++)
		{
		for(float v=0; v<=1.0f; v += diff)
			{
			for(float u=0; u<=1.0f; u+=diff )
				{
				iSampleData.push_back( (CubeToVector(cube, u, v)) );//.normalize() );		
				//printf("%d\t%f\t%f - [%f, %f, %f]\n", cube, u, v, iSampleData.back().iX, iSampleData.back().iY, iSampleData.back().iZ);
				index++;
				}
			}
		}
	printf("READY, with %d coefficients.\n\n", index);
	return index;
	}

void CMyRenderer::CalculateTransformedLightProbe()
	{
	TVector3 vec(0,0,0);
	TVector3 cubeCoords; //( face, u, v)

	static const int vDiff = KSamplingResolution*(KSamplingResolution-1);

	float angle = iLightRotationAngle; //0.0f;

	//NOTE: OPENGL matrices are column major
	//Y 
	//float M[4][4] =  {
	//	 {cos(angle),0,sin(angle),0}
	//	,{0,1,0,0}
	//	,{-sin(angle),0,cos(angle),0}
	//	,{0,0,0,1}
	//	};

	////X 
	//float M[4][4] =  {
	//	{1,0,0,0}
	//	,{0,cos(angle),-sin(angle),0}
	//	,{0,sin(angle),cos(angle),0}
	//	,{0,0,0,1}
	//	};

	////Z 
	//float M[4][4] =  {
	//	{cos(angle),-sin(angle),0,0}
	//	,{sin(angle),cos(angle),0,0}
	//	,{0,0,1,0}
	//	,{0,0,0,1}
	//	};

	//float M[4][4] =  {
	//	 {iLightRotationXFormInv[0][0], iLightRotationXFormInv[0][1], iLightRotationXFormInv[0][2], iLightRotationXFormInv[0][3]}
	//	,{iLightRotationXFormInv[1][0], iLightRotationXFormInv[1][1], iLightRotationXFormInv[1][2], iLightRotationXFormInv[1][3]}
	//	,{iLightRotationXFormInv[2][0], iLightRotationXFormInv[2][1], iLightRotationXFormInv[2][2], iLightRotationXFormInv[2][3]}
	//	,{iLightRotationXFormInv[3][0], iLightRotationXFormInv[3][1], iLightRotationXFormInv[3][2], iLightRotationXFormInv[3][3]}
	//	};

	float M[4][4] =  {
		{iLightRotationXForm[0][0], iLightRotationXForm[0][1], iLightRotationXForm[0][2], iLightRotationXForm[0][3]}
		,{iLightRotationXForm[1][0], iLightRotationXForm[1][1], iLightRotationXForm[1][2], iLightRotationXForm[1][3]}
		,{iLightRotationXForm[2][0], iLightRotationXForm[2][1], iLightRotationXForm[2][2], iLightRotationXForm[2][3]}
		,{iLightRotationXForm[3][0], iLightRotationXForm[3][1], iLightRotationXForm[3][2], iLightRotationXForm[3][3]}
		};


	//browse the cubemap vectors
	for (int i=0, endI=iSampleData.size(); i<endI; i++)
		{
		//transform the sample with the light transformation matrix
		//vec = MultMatrixVect( (float*)iLightRotationXFormInv, iSampleData.at(i) ).normalize();
		vec = MultMatrixVect( (float*)M, iSampleData.at(i) ).normalize();

		//VISUALIZE THE VECTORS
		//*(iTransformedLightProbe+i) = ((vec+1.0f)/2.0f);

		//printf("[%f %f %f] -> ", iSampleData.at(i).iX, iSampleData.at(i).iY, iSampleData.at(i).iZ );
		//printf("[%f %f %f]\t", vec.iX, vec.iY, vec.iZ );

		//vec = iSampleData.at(i).normalize();
		//get cube: face#,u,v
		vec = VectorToCube(vec);
		//Visualize the UV coordinates
		//*(iTransformedLightProbe+i) = TVector3(1.0f, vec.iY, vec.iZ );


		//get new color for the vector
		//int offset = static_cast<int>( floorf( vec.iX*KSamplingFaceCoefficients ) + floorf( vec.iY*KSamplingResolution ) + floorf( vec.iZ*vDiff ) );
		//printf("FACE: %f (%f, %f) \t-> OFFSET: %d/%d\n", vec.iX, vec.iY, vec.iZ, offset, KSamplingTotalCoefficients );

		// *(iTransformedLightProbe + static_cast<int>( floorf( vec.iX*KSamplingFaceCoefficients + vec.iY*KSamplingResolution + vec.iZ*vDiff ) ) ) 
		//	 = *(iLightProbe + i );

		//find transformed probe pixel from lightprobe
		//---------------------------------------------
		////transform the sample with the light transformation matrix
		//v.set( MultMatrixVect( (float*) M, iSampleData.at(i) ) );
		//
		////get cube: face#,u,v
		//v.set( VectorToCube(v) );

		////int offset = static_cast<int>( v.iX*KSamplingFaceCoefficients + v.iY*KSamplingResolution + v.iZ*vDiff );
		////printf("FACE: %f (%f, %f) \t-> OFFSET: %d/%d\n", v.iX, v.iY, v.iZ, offset, KSamplingTotalCoefficients );

		//get new color for the vector
		int face = static_cast<int>(vec.iX*KSamplingFaceCoefficients); 
		//int face = static_cast<int>(2*KSamplingFaceCoefficients); 
		int u = static_cast<int>(vec.iY*KSamplingResolution);
		int v = static_cast<int>(vec.iZ*KSamplingResolution);
		if(u>31) u=31;
		if(v>31) v=31;
		*(iTransformedLightProbe+i) =*(iLightProbe + face + u + v*KSamplingResolution); //*(iLightProbe + static_cast<int>( floorf( vec.iX*KSamplingFaceCoefficients + vec.iY*KSamplingResolution + vec.iZ*vDiff ) )  );
		}

	LightProbeWaveletHash();
	}


/** \brief Method that creates basic solar system
*
*   Setting up the sun, earth and moon
*/
void CMyRenderer::CreateScene()
	{
	//Load meshes from a file
	printf("Loading Meshes...\n");

	//OBJ files
	CObjLoader* loader = new CObjLoader();
	iObjectCount = loader->LoadObj( KObjectFileName );

	////3DS
	//CMeshLoader* loader = new CMeshLoader();
	//iCarObjectCount = loader->Load3Ds( "simple3.3ds", 0.004 );

	printf("Adding %d objects\n", iObjectCount );
	for( int i=0; i<iObjectCount; i++ )
		{
		iMeshList.push_back( loader->GetMesh( i, KObjectScale ) );
		}
	printf("\n");

	//Transparent objects are rendered separately
	//	iCarObjectCount -= transparentCount;

	//Free the loader
	delete loader;
	loader = NULL;

	//	system("pause");
	//	printf("GOT: vertices: %d, norms:%d triangles:%d\n", iMeshList.at(0)->iNumVertices, iMeshList.at(0)->iVertexNormals.size(), iMeshList.at(0)->iTriangles.size() );


	//ADD OTHER OBJECTS
	//-----------------
	//	printf("Create Objects...\n");
	//	iMeshList.push_back( new CBall( 10, 0.4 ) );
	//iMeshList.push_back( new CIcosahedron( 0.2 ) );
	//iMeshList.push_back( new CIcosahedron( 0.3 ) );
	//iMeshList.push_back( new CIcosahedron( 0.2 ) );
	//iMeshList.push_back( new CBall( 8, 0.7 ) );



	//--------------------------
	// BUILD THE SCENEGRAPH:
	//--------------------------
	printf("Build Scenegraph..\n");
	//BASE NODE:
	iScene = new CSceneNode();
	CSceneNode* currentNode = iScene;

	//INITIAL ROTATION & translation
	iSceneRotation = new CSceneRotation( TVector3(0.0f, 0.0f, 0.0f) );

	//AT THE MOMENT THESE ARE OUT OF THE SCENE
	//	currentNode = currentNode->addChild( new CSceneTranslation( TVector3(0.0, 0.0, -4.0) ) );
	//	currentNode = currentNode->addChild( iSceneRotation );

	//Add model parts:
	for( int i=0; i<iObjectCount; i++ )
		{
		currentNode = currentNode->addChild( new CSceneMesh( iMeshList.at(i) ) );
		}

	//-----------------------------------------
	//Apply transformation for the whole scene
	printf("Create Scenegraph...\n");
	iSceneGraph.clear();
	iSceneGraphTransparent.clear();
	glLoadIdentity();
	ApplySceneTransformations( iScene );
	glLoadIdentity();

	//CALCULATE NORMALS AND COLOR FOR ALL
	//------------------------------------
	printf("Normals and Colors...\n");
	iObjectsInScene=0;
	iVerticesInScene=0;
	for(int i=0, j=iSceneGraph.size(); i<j; i++)
		{
		iObjectsInScene++;
		iVerticesInScene += iSceneGraph.at(i)->iNumVertices;
		iSceneGraph.at(i)->calculateFaceNormals();
		}

	printf("Objects in scene: %d\n Vertices in Scene: %d\n", iObjectsInScene, iVerticesInScene );
	printf("Scene OK.\n");
	}

void CMyRenderer::InitLights()
	{
	/*	//CREATE THE LIGHT
	GLfloat light_ambient[]= { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light_diffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light_specular[]={ 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);

	GLfloat coord[]={ 1.0f, 0.0f, 0.0f };
	glLightfv ( GL_LIGHT0, GL_POSITION, coord );

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	*/
	/*texture properties*/
	//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	//	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0, 0.0, 0.0, 0);
	}

// Method that updates the scene at idle time
void CMyRenderer::UpdateScene()
	{
	glutPostRedisplay();
	}


void CMyRenderer::InitVertexMap()
	{
	//	int vertex = 0;
	iVertexMapTextures[0] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(0)		), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[1] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients  )), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[2] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients*2)), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[3] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients*3)), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[4] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients*4)), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[5] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients*5)), KSamplingResolution, KSamplingResolution );

	iLightProbe = LoadBasicPFMCubeMap("church_cubemap_32.pfm", &iProbeMapTextures[0] );

//	iLightProbe = LoadBasicPFMCubeMap("test3_cubemap_32.pfm", &iProbeMapTextures[0] );

	//iLightProbe = LoadBasicPFMCubeMap("test_cubemap_32.pfm", &iProbeMapTextures[0] );
	//iLightProbe = LoadBasicPFMCubeMap("church_cubemap_64.pfm", &iProbeMapTextures[0] );
	
	//iCubeTexture = LoadPFMCubeMap("church_cubemap_64.pfm");
	iCubeTexture = LoadPFMCubeMap
					(
					 "church_roof.tga"
					,"church_left.tga"
					,"church_front.tga"
					,"church_right.tga"
					,"church_floor.tga"
					,"church_back.tga"
					);

	if(	iLightProbe == NULL )
		{
		printf("No light probe.");
		exit(-1);
		}
	else
		{
		printf("Light probe ok.");
		//printf("P_: 0x%X\n", iLightProbe);
		//printf("[%f, %f, %f]\n", iLightProbe->iX, iLightProbe->iY, iLightProbe->iZ);
		}
	printf("[%d, %d, %d, %d, %d, %d]\n",iProbeMapTextures[0],iProbeMapTextures[1],iProbeMapTextures[2],iProbeMapTextures[3],iProbeMapTextures[4],iProbeMapTextures[5]);

	iTransformedLightProbe = new TVector3[ KSamplingTotalCoefficients ];
	CalculateTransformedLightProbe();

	//printf("-------------------------------------------------------------");
	//CalculateTransformedLightProbe();
	//printf("-------------------------------------------------------------");
	//CalculateTransformedLightProbe();
	//printf("-------------------------------------------------------------");
	//CalculateTransformedLightProbe();
	//printf("-------------------------------------------------------------");
	//CalculateTransformedLightProbe();
	//printf("-------------------------------------------------------------");
	//CalculateTransformedLightProbe();

	ChangeProbeMap();
	}


void CMyRenderer::InitHashTables()
	{
	for(int object=0, endI=iSceneGraph.size(); object<endI; object++)
		{
		int numberOfVertices = iSceneGraph.at(object)->iNumVertices;
		iSceneGraph.at(object)->iVisibilityHash.resize( numberOfVertices );

		for( int vertex=0;vertex<numberOfVertices; vertex++)
			{
			for(int coefficient=0; coefficient<KSamplingTotalCoefficients; coefficient++)
				{
				float value=iSceneGraph.at(object)->iVisibilityCoefficients.at(vertex).at(coefficient);
				//store the non-empty
				if( 0.0f != value )
					{
					//TSquare key( 0, 0, coefficient );
					iSceneGraph.at(object)->iVisibilityHash.at(vertex).insert( make_pair(coefficient, value) );
					}
				//else
				//	{
				//	}
				}
			//			printf("[%d: %d] = %d\n", object, vertex, iSceneGraph.at(object)->iVisibilityHash.at(vertex).size() );
			//			iSceneGraph.at(object)->iVisibilityCoefficients.at(vertex).clear();
			}
		//		iSceneGraph.at(object)->iVisibilityCoefficients.clear();
		}
	}

void CMyRenderer::ChangeProbeMap()
	{
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[0] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe->iX)) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[1] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients)->iX) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[2] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients*2)->iX) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[3] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients*3)->iX) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[4] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients*4)->iX) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[5] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients*5)->iX) );
	}



void CMyRenderer::ChangeVertexMap()
	{
	if(++iCubeMapVertex >= iSceneGraph.at(0)->iVisibilityCoefficients.size())
		{
		iCubeMapVertex = 0;
		}

#ifndef USE_FP_TEXTURES
	GLubyte* checkImage = new GLubyte[ KSamplingTotalCoefficients*4 ]; //amount of bytes! (ie. 1 float is 4 bytes)

	float* data = reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(0));
	float* data1 = data; //NASTY!


	if( iVisualDecomposition )
		{
		data1 = DecomposeVisibility();
		}

	for (int i=0;i<KSamplingTotalCoefficients;i++)
		{
		int val = 0xFF * *(data1+i);
		*(checkImage+i*4) =	(GLubyte)val; //((((i&0x8)==0)^((i*width&0x8))==0))*255;; //red
		*(checkImage+i*4+1) = (GLubyte)val; //green
		*(checkImage+i*4+2) = (GLubyte)val; //blue
		*(checkImage+i*4+3) = (GLubyte)0xFF; // alpha
		}
	//roof
	glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[0] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, KSamplingResolution, KSamplingResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	//left
	glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[1] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, KSamplingResolution, KSamplingResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage+KSamplingFaceCoefficients*4 );
	//front
	glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[2] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, KSamplingResolution, KSamplingResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage+KSamplingFaceCoefficients*4*2 );

	//right
	glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[3] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, KSamplingResolution, KSamplingResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage+KSamplingFaceCoefficients*4*3 );

	//floor
	glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[4] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, KSamplingResolution, KSamplingResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage+KSamplingFaceCoefficients*4*4 );

	//back
	glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[5] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, KSamplingResolution, KSamplingResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage+KSamplingFaceCoefficients*4*5 );

	delete[] checkImage;

	if( iVisualDecomposition )
		{
		delete[] data1;
		}

#else

	if( iVisualDecomposition )
		{
		float* data = reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(0));
		data = DecomposeVisibility();

		//roof
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[0] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, data );
		//left
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[1] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, data+KSamplingFaceCoefficients );
		//front
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[2] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, data+KSamplingFaceCoefficients*2 );
		//right
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[3] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, data+KSamplingFaceCoefficients*3 );
		//floor
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[4] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, data+KSamplingFaceCoefficients*4 );
		//back
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[5] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, data+KSamplingFaceCoefficients*5 );

		delete data;
		}
	else
		{
		//roof
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[0] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(0        )) );
		//left
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[1] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients  )) );
		//front
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[2] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients*2  )) );
		//right
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[3] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients*3  )) );
		//floor
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[4] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients*4  )) );
		//back
		glBindTexture( GL_TEXTURE_2D, iVertexMapTextures[5] );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(KSamplingFaceCoefficients*5  )) );
		}
#endif
	}

float* CMyRenderer::DecomposeVisibility()
	{
	return DecomposeVisibility( 0, iCubeMapVertex );
	}

float* CMyRenderer::DecomposeVisibility(int aObject, int aVertexIndex )
	{
#ifdef _DEBUG
	printf("Visibility %d ", aVertexIndex ) ;
#endif // _DEBUG
	float* data = reinterpret_cast<float*>( &iSceneGraph.at(aObject)->iVisibilityCoefficients.at(aVertexIndex).at(0));
	float *totaldata;
	float *FaceData[6];

	CMatrixNoColors *matrix1;
	CWavelet		*wavelet1;
	for (int i=0;i<6;i++)
		{
		matrix1= new CMatrixNoColors(data+KSamplingFaceCoefficients*i,KSamplingResolution,KSamplingResolution);
		wavelet1=new CWavelet(matrix1,KSamplingResolution,KSamplingResolution);
//		wavelet1->nonStandardDeconstruction();
		wavelet1->standardDeconstruction();
		FaceData[i]=wavelet1->returnFloat();
		delete matrix1;
		delete wavelet1;
		}

	int index(0);
	totaldata=new float[KSamplingFaceCoefficients*6];
	float *ptr=totaldata;
	for (int i=0;i<6;i++)
		{
		for (int j=0;j<KSamplingFaceCoefficients;j++)
			{
#ifdef USE_FP_TEXTURES
			*(FaceData[i]+j) *= 150.0f;
#endif
			*(totaldata++) = *(FaceData[i]+j);
			index++;
			}
		}
//	printf("Copied %d values. [first:%f, last:%f]\n", index, *ptr, *(ptr+(index-1)) ) ;
#ifdef _DEBUG
	printf("...done\n") ;
#endif // _DEBUG

	delete[] FaceData[0];
	delete[] FaceData[1];
	delete[] FaceData[2];
	delete[] FaceData[3];
	delete[] FaceData[4];
	delete[] FaceData[5];

	return ptr;
	}


void CMyRenderer::DecomposeLightProbeMap()
	{
	//printf("\n\n\nLight probe is getting compressed in wavelet basis");
	// roof
	CMatrix *RoofLightProbe= new CMatrix(iTransformedLightProbe, KSamplingResolution, KSamplingResolution);
	CWavelet *RoofWavelet=new CWavelet(RoofLightProbe,KSamplingResolution,KSamplingResolution);

	//left
	CMatrix *LeftLightProbe=new CMatrix(iTransformedLightProbe+KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *LeftWavelet=new CWavelet(LeftLightProbe,KSamplingResolution,KSamplingResolution);
	//front
	CMatrix *FrontLightProbe=new CMatrix(iTransformedLightProbe+2*KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *FrontWavelet=new CWavelet(FrontLightProbe,KSamplingResolution,KSamplingResolution);
	//right
	CMatrix *RightLightProbe=new CMatrix(iTransformedLightProbe+3*KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *RightWavelet=new CWavelet(RightLightProbe,KSamplingResolution,KSamplingResolution);
	//floor
	CMatrix *FloorLightProbe=new CMatrix(iTransformedLightProbe+4*KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *FloorWavelet=new CWavelet(FloorLightProbe,KSamplingResolution,KSamplingResolution);
	//back
	CMatrix *BackLightProbe=new CMatrix(iTransformedLightProbe+5*KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *BackWavelet=new CWavelet(BackLightProbe,KSamplingResolution,KSamplingResolution);

	//wavelet compression of all the faces of the cubemap Lightprobe

	//standard wavelet basis visualisation
	RoofWavelet->standardDeconstruction();
	LeftWavelet->standardDeconstruction();
	FrontWavelet->standardDeconstruction();
	RightWavelet->standardDeconstruction();
	FloorWavelet->standardDeconstruction();
	BackWavelet->standardDeconstruction();

	////non-standard wavelet basis visualisation
	//RoofWavelet->nonStandardDeconstruction();
	//LeftWavelet->nonStandardDeconstruction();
	//FrontWavelet->nonStandardDeconstruction();
	//RightWavelet->nonStandardDeconstruction();
	//FloorWavelet->nonStandardDeconstruction();
	//BackWavelet->nonStandardDeconstruction();
	//printf("\n\n\n lightprobe faces' wavelets generated");	

	float *f0=RoofWavelet->returnScaledFloat();
	float *f1=LeftWavelet->returnScaledFloat();
	float *f2=FrontWavelet->returnScaledFloat();
	float *f3=RightWavelet->returnScaledFloat();
	float *f4=FloorWavelet->returnScaledFloat();
	float *f5=BackWavelet->returnScaledFloat();

	delete RoofLightProbe;
	delete RoofWavelet;
	delete LeftLightProbe;
	delete LeftWavelet;
	delete FrontLightProbe;
	delete FrontWavelet;
	delete RightLightProbe;
	delete RightWavelet;
	delete FloorLightProbe;
	delete FloorWavelet;
	delete BackLightProbe;
	delete BackWavelet;

	//roof
	glBindTexture( GL_TEXTURE_2D, iProbeMapTextures[0] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, f0 );
	//left
	glBindTexture( GL_TEXTURE_2D, iProbeMapTextures[1] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, f1 );
	//front
	glBindTexture( GL_TEXTURE_2D, iProbeMapTextures[2] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, f2 );
	//right
	glBindTexture( GL_TEXTURE_2D, iProbeMapTextures[3] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, f3 );
	//floor
	glBindTexture( GL_TEXTURE_2D, iProbeMapTextures[4] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, f4 );
	//back
	glBindTexture( GL_TEXTURE_2D, iProbeMapTextures[5] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, f5 );

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(data->iX)) );
	delete []f0;
	delete []f1;
	delete []f2;
	delete []f3;
	delete []f4;
	delete []f5;
	}


float * CMyRenderer::DecomposeLightProbe()
{   
	//printf("\n\n\nLight probe is getting compressed in wavelet basis");
	// roof 
	CMatrix *RoofLightProbe= new CMatrix(iTransformedLightProbe, KSamplingResolution, KSamplingResolution);	
	CWavelet *RoofWavelet=new CWavelet(RoofLightProbe,KSamplingResolution,KSamplingResolution);

	//left 
	CMatrix *LeftLightProbe=new CMatrix(iTransformedLightProbe+KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *LeftWavelet=new CWavelet(LeftLightProbe,KSamplingResolution,KSamplingResolution);
	//front 
	CMatrix *FrontLightProbe=new CMatrix(iTransformedLightProbe+2*KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *FrontWavelet=new CWavelet(FrontLightProbe,KSamplingResolution,KSamplingResolution);
	//right 
	CMatrix *RightLightProbe=new CMatrix(iTransformedLightProbe+3*KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *RightWavelet=new CWavelet(RightLightProbe,KSamplingResolution,KSamplingResolution);
	//floor 
	CMatrix *FloorLightProbe=new CMatrix(iTransformedLightProbe+4*KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *FloorWavelet=new CWavelet(FloorLightProbe,KSamplingResolution,KSamplingResolution);
	//back
	CMatrix *BackLightProbe=new CMatrix(iTransformedLightProbe+5*KSamplingFaceCoefficients,KSamplingResolution, KSamplingResolution);
	CWavelet *BackWavelet=new CWavelet(BackLightProbe,KSamplingResolution,KSamplingResolution);

	//wavelet compression of all the faces of the cubemap Lightprobe

	//standard wavelet basis visualisation
	RoofWavelet->standardDeconstruction();
	LeftWavelet->standardDeconstruction();
	FrontWavelet->standardDeconstruction();
	RightWavelet->standardDeconstruction();
	FloorWavelet->standardDeconstruction();
	BackWavelet->standardDeconstruction();

	////non-standard wavelet basis visualisation
	//RoofWavelet->nonStandardDeconstruction();
	//LeftWavelet->nonStandardDeconstruction();
	//FrontWavelet->nonStandardDeconstruction();
	//RightWavelet->nonStandardDeconstruction();
	//FloorWavelet->nonStandardDeconstruction();
	//BackWavelet->nonStandardDeconstruction();
	//printf("\n\n\n lightprobe faces' wavelets generated");	

	float *f0,*f1,*f2,*f3,*f4,*f5;
	 f0=RoofWavelet->returnFloat();
	 f1=LeftWavelet->returnFloat();
	 f2=FrontWavelet->returnFloat();
	 f3=RightWavelet->returnFloat();
	 f4=FloorWavelet->returnFloat();
	 f5=BackWavelet->returnFloat();

	float *DecomposedLightProbe=new float[KSamplingTotalCoefficients*3];
	
		for (int iW=0, endIW=KSamplingFaceCoefficients*3; iW<endIW;iW++)
		{
			*(DecomposedLightProbe+iW)=*(f0+iW);
		}
		for (int iW=0, endIW=KSamplingFaceCoefficients*3; iW<endIW;iW++)
		{
			*(DecomposedLightProbe + iW + KSamplingFaceCoefficients)=*(f1+iW);
		}
		for (int iW=0, endIW=KSamplingFaceCoefficients*3; iW<endIW;iW++)
		{
			*(DecomposedLightProbe + iW + 2*KSamplingFaceCoefficients)=*(f2+iW);
		}
		for (int iW=0, endIW=KSamplingFaceCoefficients*3; iW<endIW;iW++)
		{
			*(DecomposedLightProbe + iW + 3*KSamplingFaceCoefficients)=*(f3+iW);
		}
		for (int iW=0, endIW=KSamplingFaceCoefficients*3; iW<endIW;iW++)
		{
			*(DecomposedLightProbe + iW + 4*KSamplingFaceCoefficients)=*(f4+iW);
		}
		for (int iW=0, endIW=KSamplingFaceCoefficients*3; iW<endIW;iW++)
		{
			*(DecomposedLightProbe + iW + 5*KSamplingFaceCoefficients)=*(f5+iW);
		}
	
	


	delete RoofLightProbe;
	delete RoofWavelet;
	delete LeftLightProbe;
	delete LeftWavelet;
	delete FrontLightProbe;
	delete FrontWavelet;
	delete RightLightProbe;
	delete RightWavelet;
	delete FloorLightProbe;
	delete FloorWavelet;
	delete BackLightProbe;
	delete BackWavelet;

	delete []f0;
	delete []f1;
	delete []f2;
	delete []f3;
	delete []f4;
	delete []f5;

	return DecomposedLightProbe;
}


// *************	RENDERING METHODS *********** /

/** \brief Method that specifies how the screen is rendered
*/

void CMyRenderer::RenderScene()
	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef( 0, 0, -4.0 );

	DrawCubemap();
	glMultMatrixf( (GLfloat*) iObjectRotationXForm );
	glPushMatrix();

	////OBJECT ROTATION

	glScalef( iScale, iScale, iScale );

	//DrawScene
	for(int i=0, endi=iSceneGraph.size(); i<endi; i++)
		{
		RenderObject( iSceneGraph.at(i) );
		}

	//Transparents:
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	glDepthMask( GL_FALSE );
	for(int i=0, endi=iSceneGraphTransparent.size(); i<endi; i++)
		{
		RenderObject( iSceneGraphTransparent.at(i) );
		}
	glDepthMask( GL_TRUE );
	glDisable(GL_BLEND);

	DrawVertexVisibilityMap();
	DrawLightProbe();

	//ShowLightDirection();

	ShowFPS();

	glutSwapBuffers();
	}



void CMyRenderer::DrawLightSphere()
	{
	//glTranslatef( 2, 0.5, -3.0 );

	glPointSize(2.0);
	glColor3f( 1, 1, 1 );
	glBegin( GL_POINTS );

	//	float func[3];
	//	float lightValue;
	//	float value;
	//	for(int i=0, j=iSampleData->iVector.size(); i<j; i++)
	//		{
	//		for( int j=0, endJ = iSampleData->iNumberOfFunctions; j<endJ; j++ )
	//			{
	//			value = iSampleData->iFunctionValues.at(i).at( j ) * iLightData->iRotatedSHSCoefficients.at( j );
	////			lightValue += iSampleData->iFunctionValues.at(i).at( j ) * iLightData->iLightCoefficients.at(j); //->iRotatedSHSCoefficients.at( j );
	//			lightValue += (value<200.0)?value:0;
	//			}
	//		lightValue *= 0.65;
	////		glColor3f( 0.5*lightValue, 0.2*lightValue, 0.1*lightValue );
	//		glColor3f( 0.1*lightValue, 0.1*lightValue, 0.7*lightValue );
	//
	//		//glVertex3fv( reinterpret_cast<float*>( &( iSampleData->iVector.at(i) ) ) );
	//		glVertex3f( iSampleData->iVector.at(i).iX*0.3, iSampleData->iVector.at(i).iY*0.3, iSampleData->iVector.at(i).iZ*0.3 );
	//		}
	glEnd();
	}

// Render a single object
// there are two implementations, one which draws every triangle separately
// and one that uses vertex-(,color-, and normal-) arrays
void CMyRenderer::RenderObject( CMesh* aMesh )
	{
	TTriangle t;

	//vertex count
	int vertCount(	 static_cast<int>( aMesh->iNumVertices    ) );
	//	int normalCount( static_cast<int>( aMesh->iFaceNormals.size() ) );

	TVector3  vx[3];
	//	TVector3  nv[3];
	TColorRGBA colors[3];
	//LightProbeWaveletHash();
	TIntColorHashTable::iterator colorItEnd=iLightProbeWaveletHash.end();

	//CALCULATE VERTEX COLORS
	if( 1.0 == aMesh->iMaterialColor.iA )
		{
		float visibility;
		aMesh->iVertexColors.resize(vertCount);

		TIntHashTable::iterator it;
		TIntHashTable::iterator itEnd;
		//THashTable::iterator probeIt;
		int index(0);
		TIntColorHashTable::iterator colorIt;

#ifdef USE_OPENMP
#pragma omp parallel for private( index, it, itEnd, colorIt )
#endif
		
		for (int vertex=0;vertex<vertCount;vertex++)
			{
			TColorRGBA color;

			//Browse through the hash
			//it		= aMesh->iVisibilityHash.at(vertex).begin();
			//itEnd	= aMesh->iVisibilityHash.at(vertex).end();
			it      = aMesh->iWaveletHash.at(vertex).begin();
			itEnd	= aMesh->iWaveletHash.at(vertex).end();

			colorIt= iLightProbeWaveletHash.begin();

			while( it != itEnd )
				{
				index = it->first;
				//TIntColorHashTable::iterator colorIt= iLightProbeWaveletHash.find(index);

				if ( colorIt != colorItEnd )
					{
					color.iR += it->second * (colorIt->second).iX;//  (iTransformedLightProbe+index)->iX;
					color.iG += it->second * (colorIt->second).iY;//(iTransformedLightProbe+index)->iY;
					color.iB += it->second * (colorIt->second).iZ;//(iTransformedLightProbe+index)->iZ;
					}
				it++;
				colorIt++;
				}
			//do the averaging
			aMesh->iVertexColors.at(vertex) = color/aMesh->iVisibilityHash.at(vertex).size();
			aMesh->iVertexColors.at(vertex) *= ( sqrtf( 32.0f )*sqrtf( 32.0f ) );
			}
		}

	//DRAW POLYGONS
	for (int triangleIndex=0, triangleCount=static_cast<int>(aMesh->iTriangles.size()); triangleIndex<triangleCount; triangleIndex++)
		{
		//Current triangle
		t = aMesh->iTriangles.at(triangleIndex);

		//VERTICES
		vx[0] = aMesh->iVertices.at(t.iV1);
		vx[1] = aMesh->iVertices.at(t.iV2);
		vx[2] = aMesh->iVertices.at(t.iV3);

		//NORMALS
		//nv[0] = aMesh->iVertexNormals.at(t.iV1);
		//nv[1] = aMesh->iVertexNormals.at(t.iV2);
		//nv[2] = aMesh->iVertexNormals.at(t.iV3);


		//Only opaque objects have PRT calculated
		if( 1.0 == aMesh->iMaterialColor.iA )
			{
			colors[0] = aMesh->iVertexColors.at(t.iV1);
			colors[1] = aMesh->iVertexColors.at(t.iV2);
			colors[2] = aMesh->iVertexColors.at(t.iV3);
			}
		else
			{
			colors[0] = 1.0f;
			colors[1] = 1.0f;
			colors[2] = 1.0f;
			}

		colors[0] *= aMesh->iMaterialColor;
		colors[1] *= aMesh->iMaterialColor;
		colors[2] *= aMesh->iMaterialColor;

		//DrawTriangle( &vx[0], &nv[0], &colors[0] );
		DrawTriangle( &vx[0], &colors[0] );
		}
	}

void CMyRenderer::PrecomputedRadianceTransfer()
	{
	printf("\nPrecomputed Radiance Transfer...\n");

	if ( ValidPRTDataExists( KWaveletDataFileName ) )
		{
		LoadPRTWaveletData();

		LightProbeWaveletHash();

		return;
		} 
	//OLD DATA TYPE TO NEW ONE
	else if ( ValidPRTDataExists( KHashDataFileName ) )
		{
		LoadPRTHashData();

		printf("CONVERTING TO NEW FORMAT...\n");
		printf("---------------------------\n");
		InitVertexMap();
		InitHashTables();
		//SavePRTHashData();
		InitWaveletHash();
		SavePRTWaveletData();

		printf("You can now start with the new data\n");
		exit(-1);
		return;
		}
	//OLD DATA TYPE TO NEW ONE
	else if ( ValidPRTDataExists( KDataFileName ) )
		{
		LoadPRTData();

		printf("CONVERTING TO NEW FORMAT...\n");
		printf("---------------------------\n");
		InitVertexMap();
		InitHashTables();
		SavePRTHashData();
		
		printf("You can now start with the new data\n");
		exit(-1);
		return;
		}
	else
		{
		PreCalculateDirectLight();
		InitVertexMap();
		InitHashTables();
		InitWaveletHash();

		LightProbeWaveletHash();

		//SavePRTData();
		//SavePRTHashData();
		//SavePRTWaveletData();

		//iTextures.push_back( 
		//				CreateTexture( 
		//					  reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(0).at(0) )
		//					, 32
		//					, 32
		//					)
		//				);

		//	WriteTGA( "test.tga", 30, 30, reinterpret_cast<char*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(0).at(0) ) );
		}
	}

bool CMyRenderer::ValidPRTDataExists( string filename )
	{
	std::ifstream infile( filename.c_str(), std::ios::in | std::ios::binary);
	if(!infile.is_open())
		{
		return false;
		}

	//Are these correct
	int numOfSamples;
	int numOfVertices;

	infile.read((char *)&numOfSamples,		sizeof(int));
	infile.read((char *)&numOfVertices,	sizeof(int));

	if( numOfSamples!= KSamplingTotalCoefficients || numOfVertices!=iVerticesInScene )
		{
		printf("Data mismatch with the existing data file...\n");
		infile.close();
		return false;
		}

	infile.close();
	return true;
	}

void CMyRenderer::LoadPRTData()
	{
	std::ifstream infile( KDataFileName.c_str(), std::ios::in | std::ios::binary);
	if(!infile.is_open())
		{
		printf("File access error while loading.\n");
		exit(-1);
		}

	//read the header (again!)
	int numOfSamples;
	int numOfTotalVertices;
	infile.read((char *)&numOfSamples,		sizeof(int));
	infile.read((char *)&numOfTotalVertices,	sizeof(int));

	printf("Loading from file (\"%s\")...\n", KDataFileName.c_str() );

	int datasize(0);

	//for all the objects in the scene...
	for(int i=0, endI=iSceneGraph.size(); i<endI; ++i)
		{
		CMesh* currentMesh = iSceneGraph.at( i );
		int numvertices=currentMesh->iNumVertices;
		currentMesh->iVisibilityCoefficients.clear();
		currentMesh->iVisibilityCoefficients.resize(numvertices);

		//...and for all the vertices in objects...
		float coefficient;
		for(int j=0; j<numvertices; ++j)
			{
			vector<float> vertexVisibilityCoefficients;
			//load the visibility coefficients
			for(int k=0;k<numOfSamples;k++)
				{
				infile.read((char *)&coefficient, sizeof(float));
				vertexVisibilityCoefficients.push_back( coefficient );
				datasize++;
				}
			currentMesh->iVisibilityCoefficients.at(j) = vertexVisibilityCoefficients;
			}
		}
	infile.close();
	printf("Loaded %d bytes from file OK.\n\n", datasize*sizeof(float));
	return;
	}

void CMyRenderer::SavePRTData()
	{
	printf("Saving PRT Data to a file(\"%s\").\n", KDataFileName.c_str() );
	std::ofstream outFile( KDataFileName.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	//Create header
	int numOfTotalVertices( iVerticesInScene );
	outFile.write((const char *)&KSamplingTotalCoefficients, sizeof(int));
	outFile.write((const char *)&numOfTotalVertices, sizeof(int));

	printf(" - Samples/Vertices/Objects: %d/%d/%d\n", KSamplingTotalCoefficients, numOfTotalVertices, iSceneGraph.size() );

	//for all the objects in the scene...
	for(int i=0, endI=iSceneGraph.size(); i<endI; ++i)
		{
		//		printf(" O: %d\n", i );
		CMesh* currentMesh = iSceneGraph.at( i );
		int numVertices=currentMesh->iNumVertices;

		//...and for all the vertices in objects...
		for(int j=0; j<numVertices; ++j)
			{
			//			printf("  V: %d/%d\n", j, numVertices );
			//load the visibility coefficients
			for(int k=0;k<KSamplingTotalCoefficients;k++)
				{
				//				printf("   S: %d/%d\n", k, KSamplingTotalCoefficients );
				outFile.write((char *)&currentMesh->iVisibilityCoefficients.at(j).at(k), sizeof(float));
				}
			}
		}
	outFile.close();
	printf("\nSaving OK.\n");
	}

void CMyRenderer::SavePRTHashData()
	{
	printf("Saving PRT Hash Data to a file(\"%s\").\n", KHashDataFileName.c_str() );
	std::ofstream outFile( KHashDataFileName.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	//Create header
	int numOfTotalVertices( iVerticesInScene );
	outFile.write((const char *)&KSamplingTotalCoefficients, sizeof(int));
	outFile.write((const char *)&numOfTotalVertices, sizeof(int));

	printf(" - Samples/Vertices/Objects: %d/%d/%d\n", KSamplingTotalCoefficients, numOfTotalVertices, iSceneGraph.size() );

	//for all the objects in the scene...
	for(int i=0, endI=iSceneGraph.size(); i<endI; ++i)
		{
		//		printf(" O: %d\n", i );
		CMesh* currentMesh = iSceneGraph.at( i );
		int numVertices=currentMesh->iNumVertices;

		TIntHashTable::iterator it;
		TIntHashTable::iterator itEnd;
		int index(0);

		//...and for all the vertices in objects...
		for(int j=0; j<numVertices; ++j)
			{

			//1. Write the number of elements in a hash table for the vertex
			int size=currentMesh->iVisibilityHash.at(j).size();
			outFile.write( (char *)&size, sizeof(int));

			//Browse through the hash
			int index=0;
			it		= currentMesh->iVisibilityHash.at(j).begin();
			itEnd	= currentMesh->iVisibilityHash.at(j).end();
			while( it != itEnd )
				{
				//write the "key"
				outFile.write( (char *) &(it->first), sizeof(int));
				//write the value
				outFile.write( (char *) &(it->second), sizeof(float));
				it++;
				index++;
				}
			}
		}
	outFile.close();
	printf("\nSaving HashData OK.\n");
	}


void CMyRenderer::LoadPRTHashData()
	{
	std::ifstream infile( KHashDataFileName.c_str(), std::ios::in | std::ios::binary);
	if(!infile.is_open())
		{
		printf("File access error while loading: %s.\n", KHashDataFileName.c_str() );
		exit(-1);
		}

	//read the header (again!)
	int numOfSamples;
	int numOfTotalVertices;
	infile.read((char *)&numOfSamples,		sizeof(int));
	infile.read((char *)&numOfTotalVertices,	sizeof(int));

	printf("Loading from file (\"%s\")...\n", KHashDataFileName.c_str() );

	int datasize(0);

	//for all the objects in the scene...
	for(int i=0, endI=iSceneGraph.size(); i<endI; ++i)
		{
		CMesh* currentMesh = iSceneGraph.at( i );
		int numvertices=currentMesh->iNumVertices;;
		currentMesh->iVisibilityCoefficients.clear();
		currentMesh->iVisibilityCoefficients.resize(numvertices);
		currentMesh->iVisibilityHash.resize( numvertices );

		int key(0);
		float value(0);

		//...and for all the vertices in objects...
		for(int j=0; j<numvertices; ++j)
			{
			int amountOfHashValues(0);
			infile.read((char *)&amountOfHashValues, sizeof(int));
			//printf("Hash(%d) = %d\n", j, amountOfHashValues);

			vector<float> vertexVisibilityCoefficients( KSamplingTotalCoefficients );

			//load the visibility coefficients
			for(int k=0;k<amountOfHashValues;k++)
				{
				infile.read((char *)&key, sizeof(int));
				infile.read((char *)&value, sizeof(float));

				//printf("[%d: %f]\n", key, value);

				vertexVisibilityCoefficients.at(key) = value;
				currentMesh->iVisibilityHash.at(j).insert( make_pair(key, value) );

				datasize++;
				}
			currentMesh->iVisibilityCoefficients.at(j) = vertexVisibilityCoefficients;
			}
		}
	infile.close();
	printf("Loaded %d bytes from file OK.\n\n", datasize*(sizeof(float) + sizeof(int)));
	return;
	}

void CMyRenderer::SavePRTWaveletData()
	{
	printf("Saving PRT Wavelet Data to a file(\"%s\").\n", KWaveletDataFileName.c_str() );
	std::ofstream outFile( KWaveletDataFileName.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	//Create header
//	printf("\n no of vertices: %d", iVerticesInScene);
	int numOfTotalVertices( iVerticesInScene );
//	printf("\n no of vertices(after): %d", numOfTotalVertices);
	outFile.write((const char *)&KSamplingTotalCoefficients, sizeof(int));
	outFile.write((const char *)&numOfTotalVertices, sizeof(int));

	printf(" - Samples/Vertices/Objects: %d/%d/%d\n", KSamplingTotalCoefficients, numOfTotalVertices, iSceneGraph.size() );

	//for all the objects in the scene...
	for(int i=0, endI=iSceneGraph.size(); i<endI; ++i)
		{
		//		printf(" O: %d\n", i );
		CMesh* currentMesh = iSceneGraph.at( i );
		int numVertices=currentMesh->iVertices.size();

		TIntHashTable::iterator it;		
		TIntHashTable::iterator itEnd;		
		int index(0);

		//...and for all the vertices in objects...
		for(int j=0; j<numVertices; ++j)
			{
			//1. Write the number of elements in a hash table for the vertex
			int size=currentMesh->iWaveletHash.at(j).size();
			outFile.write( (char *)&size, sizeof(int));

			//Browse through the hash
			int index=0;
			it		= currentMesh->iWaveletHash.at(j).begin();
			itEnd	= currentMesh->iWaveletHash.at(j).end();
			while( it != itEnd )
				{
				//write the "key"
				outFile.write( (char *) &(it->first), sizeof(int));
				//write the value
				outFile.write( (char *) &(it->second), sizeof(float));
				it++;
				index++;
				}
			}
		}
	outFile.close();
	printf("\nSaving Wavelet Data OK.\n");
	}


void CMyRenderer::LoadPRTWaveletData()
	{
	std::ifstream infile( KWaveletDataFileName.c_str(), std::ios::in | std::ios::binary);
	if(!infile.is_open())
		{
		printf("File access error while loading: %s.\n", KWaveletDataFileName.c_str() );
		exit(-1);
		}

	//read the header (again!)
	int numOfSamples;
	int numOfTotalVertices;
	infile.read((char *)&numOfSamples,		sizeof(int));
	infile.read((char *)&numOfTotalVertices,	sizeof(int));

	printf("Loading from file (\"%s\")...\n", KWaveletDataFileName.c_str() );

	int datasize(0);

	//for all the objects in the scene...
	for(int i=0, endI=iSceneGraph.size(); i<endI; ++i)
		{
		CMesh* currentMesh = iSceneGraph.at( i );
		int numvertices=currentMesh->iVertices.size();;
		currentMesh->iVisibilityCoefficients.clear();
		currentMesh->iVisibilityCoefficients.resize(numvertices);
		currentMesh->iVisibilityHash.resize( numvertices );
		currentMesh->iWaveletHash.resize(numvertices);

		int key(0);
		float value(0);

		//...and for all the vertices in objects...
		for(int j=0; j<numvertices; ++j)
			{
			int amountOfHashValues(0);
			infile.read((char *)&amountOfHashValues, sizeof(int));
			//printf("Hash(%d) = %d\n", j, amountOfHashValues);

			//vector<float> vertexVisibilityCoefficients( KSamplingTotalCoefficients );
			currentMesh->iWaveletHash.at(j).clear();

			//load the visibility coefficients
			for(int k=0;k<amountOfHashValues;k++)
				{
				infile.read((char *)&key, sizeof(int));
				infile.read((char *)&value, sizeof(float));

				//printf("[%d: %f]\n", key, value);

				//vertexVisibilityCoefficients.at(key) = value;
				//currentMesh->iVisibilityHash.at(j).insert( make_pair(key, value) );
				
				currentMesh->iWaveletHash.at(j).insert(make_pair(key,value));
				datasize++;
				}
			float *loaded=ReconstructVisibility(&currentMesh->iWaveletHash.at(j));

			vector<float> vertexVisibilityCoefficients( KSamplingTotalCoefficients );
			for (int i=0;i<KSamplingTotalCoefficients;i++)
				{
				vertexVisibilityCoefficients.at(i)=*(loaded+i);						
				}
			delete[] loaded;
			currentMesh->iVisibilityCoefficients.at(j) = vertexVisibilityCoefficients;
			}
		}
	infile.close();
	InitHashTables();
	printf("Loaded %d bytes from file OK.\n\n", datasize*(sizeof(float) + sizeof(int)));
	return;
	}


void CMyRenderer::PreCalculateDirectLight()
	{
	int numberOfSamples = InitializeSamplingData();

	printf("Start calculating the visibility co-efficients...\n");
	printf(" - Vertices in the scene: %d\n", iVerticesInScene );

	int numObjects = iSceneGraph.size(); //objects.size();
	CRay ray;

	time_t start1;
	time_t end1;
	struct tm* currenttime;
	time(&start1);
	currenttime=localtime(&start1);

	printf(" - Start Time: %s\n", asctime(currenttime) );

	//GENERATE COEFFs
	//Loop through the vertices and project the transfer function into SH space

	static int vertexCount(0);
	float dot(0.0);
	bool rayBlocked;

	printf("Calculating vertex:         " );

	CMesh* currentMesh;

	//FOR EACH OBJECT...
	for(int i=0; i<numObjects; ++i)
		{
		currentMesh = iSceneGraph.at( i );
		int numOfVertices =currentMesh->iNumVertices;

		currentMesh->iVisibilityCoefficients.resize(numOfVertices);

		//FOR EACH VERTEX...
#ifdef USE_OPENMP
#pragma omp parallel for shared(vertexCount) private( ray, rayBlocked, dot )
#endif
		for( int j=0; j<numOfVertices; ++j )
			{
			printf("\b\b\b\b\b\b\b\b%8d", ++vertexCount );

			//SAMPLES FOR EACH VERTEX
			// i is current object
			// j is current vertex
			// k is current sample
			vector<float> vertexVisibility;
			vertexVisibility.resize(iNumberOfSamples, 0.0f );

			for(int k=0; k<iNumberOfSamples; ++k)
				{
				//Calculate cosine term for this sample
				dot = iSampleData.at( k ).dot( currentMesh->iVertexNormals.at(j) );

				//UPPER HEMISPHERE?
				//Clamp to [0, 1]
				if( dot>0.0 )
					{
					//Fill in a RAY structure for this sample
					ray.Set( currentMesh->iVertices.at(j) + currentMesh->iVertexNormals.at(j)*KEpsilon*2.0
						, iSampleData.at(k) );

					//See if the ray is blocked by any object
					rayBlocked = IsRayBlocked( &ray );

					//NOT in the shadow:
					if(!rayBlocked)
						{
						vertexVisibility.at(k) = ( dot );
						}
					//SHADOWED
					//else
					//	{
					//	vertexVisibility.push_back( 0.0 );
					//	}
					}
				//else
				//	{
				//	vertexVisibility.push_back( 0.0 );
				//	}
				}
			//add visibility list to current vertex
			currentMesh->iVisibilityCoefficients.at(j) = vertexVisibility;
			}
		}

	//end time:
	time(&end1);
	currenttime=localtime(&end1);
	printf("\n - End Time: %s, Diff Total: %f\n", asctime(currenttime), difftime(end1,start1) );

	//CHECK
	for(int i=0; i<numObjects; ++i)
		{
		currentMesh = iSceneGraph.at( i );
		int numOfVertices =currentMesh->iNumVertices;

		//FOR EACH VERTEX...
//		for( int j=0; j<numOfVertices; ++j )
//			{
//			printf("Object: %d, Vert/Coeffs: %d / %d\n", i, j, currentMesh->iVisibilityCoefficients.at(j).size() );
//			}
		}
	printf("Pre-calc Ready.\n\n");
	}

bool CMyRenderer::IsRayBlocked( CRay* aRay )
	{
	for( int i=0, endI=iSceneGraph.size(); i<endI; i++ )
		{
		//Only reasonable to check if it hits the bounding box
		if( aRay->IntersectBoundingBoxAABB( &iSceneGraph.at(i)->iMin, &iSceneGraph.at(i)->iMax ) )
			{
			for( int j=0,endJ=iSceneGraph.at(i)->iTriangles.size(); j<endJ; j++ )
				{
				TTriangle triangle = iSceneGraph.at(i)->iTriangles.at(j);
				TVector3* list[3]={  &iSceneGraph.at(i)->iVertices.at( triangle.iV1 )
					,&iSceneGraph.at(i)->iVertices.at( triangle.iV2 )
					,&iSceneGraph.at(i)->iVertices.at( triangle.iV3 )
					};
					{
					if( aRay->IntersectTriangle( list, &iSceneGraph.at(i)->iFaceNormals.at(j) ) )
						{
						return true;
						}
					}

				}//end polygons
			}//end bb
		}//end scene
	return false;
	}



void CMyRenderer::DrawCubemap()
	{
	glDepthFunc(GL_ALWAYS);	// don't need to clear depth buffer

	glBindTexture(GL_TEXTURE_CUBE_MAP, iCubeTexture);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP);   

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); // Modelview
		{
		glLoadIdentity();
		glTranslatef( 0, 0, -2.5 );

		glScalef( iScale, iScale, iScale );

		glMatrixMode(GL_TEXTURE);
		glPushMatrix(); // Texture
			{
			glLoadIdentity();

			//Light rotation
			glMultMatrixf( (GLfloat*) iLightRotationXForm );

			//OBJECT ROTATION
			glMultMatrixf( (GLfloat*) iObjectRotationXFormInv );

			//DRAW THE CUBE
			for (int i = 0; i < 6; ++i)
				{
				glBegin(GL_QUADS);
 
				for (int j = 0; j < 4; ++j)
					{
					glNormal3fv( vertices[ faces[i][j] ] );
					glVertex3fv( vertices[ faces[i][j] ] );
					}
				glEnd();
				}
			}
			glPopMatrix(); // Texture
			glMatrixMode(GL_MODELVIEW);
		}
		glPopMatrix(); // Modelview

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_CUBE_MAP);

		glDepthFunc(GL_LESS);	// normal depth buffering
	}

void CMyRenderer::ActivateDecomposition()
	{
	iVisualDecomposition = true;
	DecomposeLightProbeMap();

	iCubeMapVertex--;
	ChangeVertexMap();
	}

void CMyRenderer::ActivateReconstruction()
	{
	iVisualDecomposition = false;
	iCubeMapVertex--;
	ChangeVertexMap();

	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[0] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe->iX)) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[1] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients)->iX) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[2] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients*2)->iX) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[3] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients*3)->iX) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[4] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients*4)->iX) );
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[5] ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, KSamplingResolution, KSamplingResolution, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(iTransformedLightProbe+KSamplingFaceCoefficients*5)->iX) );
	}

void CMyRenderer::DrawVertexVisibilityMap()
	{
	glDisable(GL_LIGHTING);

	glColor3f( 1.0f,1.0f,0.0);
	glTranslatef( iSceneGraph.at(0)->iVertices.at(iCubeMapVertex).iX, iSceneGraph.at(0)->iVertices.at(iCubeMapVertex).iY, iSceneGraph.at(0)->iVertices.at(iCubeMapVertex).iZ );
	glutSolidSphere(0.05f, 16,16);


	glDepthFunc(GL_ALWAYS);	// don't need to clear depth buffer
	//glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D( 0, iScreenWidth, 0, iScreenHeight );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//	glTranslatef(0.375f, 1.5f, 0.0f);

	glColor3f(1.0, 1.0, 1.0 );

	const int size = 64;
	const int size2 = size*2;
	const int translateY = iScreenHeight - (20 + size );
	const int translateX = iScreenWidth-(size2+20);

	glEnable( GL_TEXTURE_2D );

	//roof
	glBindTexture( GL_TEXTURE_2D,( iVertexMapTextures[0] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);		 glVertex2i( translateX,		translateY );
	glTexCoord2f(1.0f,1);	 glVertex2i( size+translateX,	translateY );
	glTexCoord2f(1.0f,0.0f); glVertex2i( size+translateX,	size+translateY );
	glTexCoord2f(0,0.0f);	 glVertex2i( translateX,		size+translateY );
	glEnd();

	//left
	glBindTexture( GL_TEXTURE_2D,( iVertexMapTextures[1] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);		glVertex2i(-size+translateX,	-size+translateY);
	glTexCoord2f(1.0f,1);	glVertex2i( translateX,			-size+translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i( translateX,			translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(-size+translateX,	translateY);
	glEnd();

	//front
	glBindTexture( GL_TEXTURE_2D,( iVertexMapTextures[2] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);		glVertex2i(translateX,		-size+translateY);
	glTexCoord2f(1.0f,1);	glVertex2i(size+translateX,	-size+translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i(size+translateX,	translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(translateX,		translateY);
	glEnd();

	//right
	glBindTexture( GL_TEXTURE_2D,( iVertexMapTextures[3] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);		glVertex2i(size+translateX,		-size+translateY);
	glTexCoord2f(1.0f,1);	glVertex2i(size2 + translateX,	-size+translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i(size2 + translateX,	translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(size+translateX,		translateY);
	glEnd();

	//floor
	glBindTexture( GL_TEXTURE_2D,( iVertexMapTextures[4] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);		glVertex2i(translateX,		-size2 +translateY);
	glTexCoord2f(1.0f,1);	glVertex2i(size+translateX,	-size2 +translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i(size+translateX,	-size+translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(translateX,		-size+translateY);
	glEnd();

	//back
	glBindTexture( GL_TEXTURE_2D,( iVertexMapTextures[5] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1);		glVertex2i(translateX,		-size2 -size +translateY);
	glTexCoord2f(1.0f,1);	glVertex2i(size+translateX,	-size2 - size +translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i(size+translateX,	-size2 + translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(translateX,		-size2 + translateY);
	glEnd();

	glDisable( GL_TEXTURE_2D );

	glDepthFunc(GL_LESS);	// normal depth buffering
	//glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	}

void CMyRenderer::DrawLightProbe()
	{
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_ALWAYS);	// don't need to clear depth buffer
	//glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D( 0, iScreenWidth, 0, iScreenHeight );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0, 1.0, 1.0 );

	const int size = 64;
	const int size2 = size*2;
	const int translateY = iScreenHeight - (50 + size*5 );
	const int translateX = iScreenWidth-(size2+20);

	glEnable( GL_TEXTURE_2D );

	//roof
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[0] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1.0f);		 glVertex2i( translateX,		translateY );
	glTexCoord2f(1.0f,1.0f);	glVertex2i( size+translateX,	translateY );
	glTexCoord2f(1.0f,0.0f);	glVertex2i( size+translateX,	size+translateY );
	glTexCoord2f(0,0.0f);		glVertex2i( translateX,		size+translateY );
	glEnd();

	//left
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[1] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1.0f);		glVertex2i(-size+translateX,	-size+translateY);
	glTexCoord2f(1.0f,1.0f);	glVertex2i( translateX,			-size+translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i( translateX,			translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(-size+translateX,	translateY);
	glEnd();

	//front
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[2] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1.0f);		glVertex2i(translateX,		-size+translateY);
	glTexCoord2f(1.0f,1.0f);	glVertex2i(size+translateX,	-size+translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i(size+translateX,	translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(translateX,		translateY);
	glEnd();

	//right
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[3] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1.0f);		glVertex2i(size+translateX,		-size+translateY);
	glTexCoord2f(1.0f,1.0f);	glVertex2i(size2 + translateX,	-size+translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i(size2 + translateX,	translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(size+translateX,		translateY);
	glEnd();

	//floor
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[4] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1.0f);		glVertex2i(translateX,		-size2 +translateY);
	glTexCoord2f(1.0f,1.0f);	glVertex2i(size+translateX,	-size2 +translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i(size+translateX,	-size+translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(translateX,		-size+translateY);
	glEnd();

	//back
	glBindTexture( GL_TEXTURE_2D,( iProbeMapTextures[5] ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,1.0f);		glVertex2i(translateX,		-size2 -size +translateY);
	glTexCoord2f(1.0f,1.0f);	glVertex2i(size+translateX,	-size2 - size +translateY);
	glTexCoord2f(1.0f,0.0f);glVertex2i(size+translateX,	-size2 + translateY);
	glTexCoord2f(0,0.0f);	glVertex2i(translateX,		-size2 + translateY);
	glEnd();

	glDisable( GL_TEXTURE_2D );

	glDepthFunc(GL_LESS);	// normal depth buffering
	//glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	}



void CMyRenderer::DrawTriangle( TVector3* aVx, TVector3* aNv, TColorRGBA aCol[3])
	{
	glBegin(iWireFrame);
	glColor4fv( &aCol[0].iR );
	glNormal3f(aNv[0].iX, aNv[0].iY, aNv[0].iZ);
	glVertex3f(aVx[0].iX, aVx[0].iY, aVx[0].iZ);

	glColor4fv( &aCol[1].iR );
	glNormal3f(aNv[1].iX, aNv[1].iY, aNv[1].iZ);
	glVertex3f(aVx[1].iX, aVx[1].iY, aVx[1].iZ);

	glColor4fv( &aCol[2].iR );
	glNormal3f(aNv[2].iX, aNv[2].iY, aNv[2].iZ);
	glVertex3f(aVx[2].iX, aVx[2].iY, aVx[2].iZ);
	glEnd();
	}

void CMyRenderer::DrawTriangle( TVector3* aVx, TColorRGBA aCol[3])
	{
	glBegin(iWireFrame);
	glColor4fv( &aCol[0].iR );
	glVertex3f(aVx[0].iX, aVx[0].iY, aVx[0].iZ);

	glColor4fv( &aCol[1].iR );
	glVertex3f(aVx[1].iX, aVx[1].iY, aVx[1].iZ);

	glColor4fv( &aCol[2].iR );
	glVertex3f(aVx[2].iX, aVx[2].iY, aVx[2].iZ);
	glEnd();
	}


//Resize the window for GLUT
//--------------------------
void CMyRenderer::ResizeScene(const int aWidth, const int aHeight)
	{
	iScreenWidth=aWidth;
	iScreenHeight=aHeight;

	glMatrixMode( GL_VIEWPORT );
	glViewport(0, 0, (GLsizei)iScreenWidth, (GLsizei)iScreenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLdouble)iScreenWidth/(GLdouble)iScreenHeight, 0.1, 10);
	glMatrixMode(GL_MODELVIEW);
	}



// Browses through the entire node and draws every mesh on its way
//  also the matrices are applied
void CMyRenderer::ApplySceneTransformations( CSceneNode* aNode )
	{
	// Push old transform (saving parent node's matrix)
	glPushMatrix();
	//----------------------
	// DIFFERENT NODE TYPES
	//----------------------
	switch(aNode->getNodeType())
		{
		//===============================
		// BASE NODE
		//===============================
		case CSceneNode::EBaseNode:
			{
			}
			break;
			//===============================
			// MESH NODE ->
			//    * NEEDS TO BE RENDERED *
			//===============================
		case CSceneNode::EMeshNode:
			{
			CSceneMesh* mNode=reinterpret_cast<CSceneMesh*>(aNode);

			CMesh* mesh = new CMesh( *mNode->GetMesh() );
			if(NULL!= mesh )
				{
				TransformMesh( mesh );

				//Transparent parts of the scene will be rendered seperately
				if( mesh->iMaterialColor.iA != 1.0 )
					{
					iSceneGraphTransparent.push_back( mesh );
					}
				else
					{
					iSceneGraph.push_back( mesh );
					}
				}
			}
			break;
			//===============================
			// ROTATION NODE
			//===============================
		case CSceneNode::ERotationNode:
			{
			CSceneRotation* rNode=reinterpret_cast<CSceneRotation*>(aNode);
			glRotatef( rNode->iAngles.iX, 1,0,0 );
			glRotatef( rNode->iAngles.iY, 0,1,0 );
			glRotatef( rNode->iAngles.iZ, 0,0,1 );
			}
			break;
			//===============================
			// TRANSLATION NODE
			//===============================
		case CSceneNode::ETranslationNode:
			{
			CSceneTranslation* tNode=reinterpret_cast<CSceneTranslation*>(aNode);
			glTranslatef(tNode->iX, tNode->iY, tNode->iZ);
			}
			break;
			// UNKNOWN NODE
		default:
			{
			}
			break;
		}
	//-------------------------------
	// BROWSE THE CHILDREN...
	//-------------------------------
	for(int i=0, j=aNode->getChildAmount(); i<j; i++)
		{
		ApplySceneTransformations( aNode->getChild(i) );
		}
	//-------------------------------
	// POP THE PARENT NODE'S MATRIX
	//-------------------------------
	glPopMatrix();
	}

void CMyRenderer::TransformMesh( CMesh* aMesh )
	{
	GLdouble modelViewMatrix[16];

	glGetDoublev( GL_MODELVIEW_MATRIX, modelViewMatrix );
	//printf("OGL modelViewMatrix:\n");
	//printf("%f\t%f\t%f\t%f\n",modelViewMatrix[0],modelViewMatrix[4],modelViewMatrix[8],modelViewMatrix[12]);
	//printf("%f\t%f\t%f\t%f\n",modelViewMatrix[1],modelViewMatrix[5],modelViewMatrix[9],modelViewMatrix[13]);
	//printf("%f\t%f\t%f\t%f\n",modelViewMatrix[2],modelViewMatrix[6],modelViewMatrix[10],modelViewMatrix[14]);
	//printf("%f\t%f\t%f\t%f\n\n",modelViewMatrix[3],modelViewMatrix[7],modelViewMatrix[11],modelViewMatrix[15]);
	for(int i=0, endi=aMesh->iNumVertices; i<endi; i++)
		{
		ApplyMultMatrixVect( modelViewMatrix, &aMesh->iVertices.at( i ) );
		}
	}

//NOTE: OPENGL matrices are column major
void CMyRenderer::ApplyMultMatrixVect(const double* aMatrix, TVector3* aVector)
	{
	//	printf("VEC:  [%f, %f %f]\n", aVector->iX, aVector->iY, aVector->iZ);
		aVector->set(																	// + transform
		aMatrix[0] * aVector->iX + aMatrix[4] * aVector->iY   + aMatrix[8] * aVector->iZ +aMatrix[12],
		aMatrix[1] * aVector->iX + aMatrix[5] * aVector->iY   + aMatrix[9] * aVector->iZ +aMatrix[13],
		aMatrix[2] * aVector->iX + aMatrix[6] * aVector->iY   + aMatrix[10] * aVector->iZ +aMatrix[14]
	);
	}

//NOTE: OPENGL matrices are column major
TVector3 CMyRenderer::MultMatrixVect(const float* aMatrix, const TVector3& aVector)
	{
	//	printf("VEC:  [%f, %f %f]\n", aVector->iX, aVector->iY, aVector->iZ);
	return TVector3(																	// + transform
		aMatrix[0] * aVector.iX + aMatrix[4] * aVector.iY   + aMatrix[8] * aVector.iZ +aMatrix[12],
		aMatrix[1] * aVector.iX + aMatrix[5] * aVector.iY   + aMatrix[9] * aVector.iZ +aMatrix[13],
		aMatrix[2] * aVector.iX + aMatrix[6] * aVector.iY   + aMatrix[10] * aVector.iZ +aMatrix[14]
		);
	}

void CMyRenderer::ShowFPS()
	{
	iFrame++;
	iTime=glutGet(GLUT_ELAPSED_TIME);
	if (iTime - iTimebase > 1000)
		{
		//sprintf( iFpsString, "LIGHTING: %s - FPS: %4.2f", iLightingModelName.c_str(), iFrame*1000.0/(iTime-iTimebase));
		sprintf( iFpsString, "LIGHTING: %s FPS: %4.2f", iLightingModelName.c_str(), iFrame*1000.0/(iTime-iTimebase));
		//printf( "FPS:%4.2f\n", iFrame*1000.0/(iTime-iTimebase));
		iTimebase = iTime;
		iFrame = 0;
		}

	//DRAW the text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D( 0.0f, iScreenWidth, 0.0f, iScreenHeight );

	glDisable(GL_LIGHTING);
	glDisable( GL_TEXTURE_2D );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.5, 0.5, 0.0 );

	glRasterPos2i( 10, 10 );
	for (int i=0, len=strlen(iFpsString)-1; i < len; i++)
		{
		if( 'A' <= iFpsString[i] &&  'Z' >= iFpsString[i] )
			glutBitmapCharacter( GLUT_BITMAP_9_BY_15, iFpsString[i] );
		else
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, iFpsString[i] );
		}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	}

void CMyRenderer::ApplyObjectRotations()
	{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLoadMatrixf( (GLfloat*) &iObjectRotationXForm );
	glRotatef( iObjectRotationAngle, iObjectRotationAxis.iX, iObjectRotationAxis.iY, iObjectRotationAxis.iZ );

	//Get the model transformations and inverse
	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) &iObjectRotationXForm );
	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) &iObjectRotationXFormInv );
	InverseMatrix( iObjectRotationXFormInv );
	glPopMatrix();
	}

void CMyRenderer::ApplyLightRotations()
	{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLoadMatrixf( (GLfloat*) &iLightRotationXForm );
	//glRotatef( iLightRotationAngle, iLightRotationAxis.iX, iLightRotationAxis.iY, iLightRotationAxis.iZ );
	glRotatef( iLightRotationAngle, 0, iLightRotationAxis.iY, 0 );

	//GET LIGHT MATRIX and its INVERSE
	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) &iLightRotationXForm );
	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) &iLightRotationXFormInv );
	InverseMatrix( iLightRotationXFormInv );
	
	CalculateTransformedLightProbe();
	ChangeProbeMap();
	glPopMatrix();
	}

void CMyRenderer::ObjectRotatingEvent()
	{
	ApplyObjectRotations();
	iObjectRotationAngle = 0.0f;
	iObjectRotationAxis.set(0,0,0);
	}

void CMyRenderer::LightRotatingEvent()
	{
	ApplyLightRotations();
	iLightRotationAngle = 0.0f;
	iLightRotationAxis.set(0,0,0);
	}

void CMyRenderer::InverseMatrix(float aMatrix[4][4]) const
	{
	float b[4][4] = {{1,0,0,0},{0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

	/////////////////////////////////////////////////////////////////
	// Taken from Numerical Recipes in C++;
	// void NR::gaussj(Mat_IO_DP &aMatrix, Mat_IO_DP &b)
	int i,icol,irow,j,k,l,ll;
	float big,dum,pivinv,temp;

	int n = 4;
	int m = 4;
	int indxc[4];
	int indxr[4];
	int ipiv[4];

	for (j=0;j<n;j++) ipiv[j]=0;
	for (i=0;i<n;i++) {
		big=0.0;
		for (j=0;j<n;j++)
			if (ipiv[j] != 1)
				for (k=0;k<n;k++) {
					if (ipiv[k] == 0) {
						if (fabs(aMatrix[j][k]) >= big) {
							big=fabs(aMatrix[j][k]);
							irow=j;
							icol=k;
							}
						}
					}
				++(ipiv[icol]);
				if (irow != icol) {
					for (l=0;l<n;l++) std::swap(aMatrix[irow][l],aMatrix[icol][l]);
					for (l=0;l<m;l++) std::swap(b[irow][l],b[icol][l]);
					}
				indxr[i]=irow;
				indxc[i]=icol;

				pivinv=1.0/aMatrix[icol][icol];
				aMatrix[icol][icol]=1.0;
				for (l=0;l<n;l++) aMatrix[icol][l] *= pivinv;
				for (l=0;l<m;l++) b[icol][l] *= pivinv;
				for (ll=0;ll<n;ll++)
					if (ll != icol) {
						dum=aMatrix[ll][icol];
						aMatrix[ll][icol]=0.0;
						for (l=0;l<n;l++) aMatrix[ll][l] -= aMatrix[icol][l]*dum;
						for (l=0;l<m;l++) b[ll][l] -= b[icol][l]*dum;
						}
		}
	for (l=n-1;l>=0;l--) {
		if (indxr[l] != indxc[l])
			for (k=0;k<n;k++)
				std::swap(aMatrix[k][indxr[l]],aMatrix[k][indxc[l]]);
		}
	}


void CMyRenderer::LightProbeWaveletHash()
{
	float *DecomposedLightProbe=DecomposeLightProbe();
	
	TVector3 zero(0.0,0.0,0.0);
	iLightProbeWaveletHash.clear();
	
	for(int coefficient=0,coefCOLOR=0, coefCOLORend=KSamplingTotalCoefficients*3; coefficient<KSamplingTotalCoefficients; coefficient++,coefCOLOR+=3)
	{
		TVector3 color( *(DecomposedLightProbe+coefCOLOR),*(DecomposedLightProbe+coefCOLOR+1),*(DecomposedLightProbe+coefCOLOR+2) );
		//store the non-empty

		//printf("\n %d)",coefficient);

//		if( zero != color )
		{
			//printf("Color=");
			//color.print();
			//TSquare key( 0, 0, coefficient );
			//printf("\n coeff=%d, value =%f", coefficient,value);
			iLightProbeWaveletHash.insert( make_pair(coefficient, color) );
		}
	}
}

void CMyRenderer::ShowLightDirection()
	{
	//////////////////////////////////////////////////////////////////////////
	glPushMatrix();
//	glLoadIdentity();
//	glTranslatef( 0, 0, -4.0 );
	glDisable( GL_DEPTH_TEST);
	glDisable( GL_CULL_FACE );
	float matrix[4][4] =  {
		{iLightRotationXForm[0][0], iLightRotationXForm[0][1], iLightRotationXForm[0][2], iLightRotationXForm [0][3]}
		,{iLightRotationXForm[1][0], iLightRotationXForm[1][1], iLightRotationXForm[1][2], iLightRotationXForm[1][3]}
		,{iLightRotationXForm[2][0], iLightRotationXForm[2][1], iLightRotationXForm[2][2], iLightRotationXForm[2][3]}
		,{iLightRotationXForm[3][0], iLightRotationXForm[3][1], iLightRotationXForm[3][2], iLightRotationXForm[3][3]}
		};

	InverseMatrix( matrix );
	glMultMatrixf( (GLfloat*) matrix  );
	glBegin( GL_TRIANGLES );
	glColor3f(1,0,0);
	glVertex3f(0,0,-2);
	glColor3f(1,1,0);
	glVertex3f(-0.2,0,1);
	glColor3f(1,1,0);
	glVertex3f(0.2,0,1);
	glEnd();

	glEnable( GL_DEPTH_TEST);
	glEnable( GL_CULL_FACE );

	glPopMatrix();
	glPushMatrix();
	//////////////////////////////////////////////////////////////////////////
	}


void CMyRenderer::InitWaveletHash()
	{
	for(int object=0, endI=iSceneGraph.size(); object<endI; object++)
		{
		int numberOfVertices = iSceneGraph.at(object)->iVertices.size();
		iSceneGraph.at(object)->iWaveletHash.resize( numberOfVertices );

		for( int vertex=0;vertex<numberOfVertices; vertex++)
			{
#ifdef _DEBUG
			printf("Object: %d, ", object );
#endif // _DEBUG
			float *faces=DecomposeVisibility(object, vertex); 
			iSceneGraph.at(object)->iWaveletHash.at(vertex).clear();

			for(int coefficient=0; coefficient < KSamplingTotalCoefficients; coefficient++)
				{
				float value= *(faces+coefficient);
				//store the non-empty
				if( 0.0f != value )
					{
					//TSquare key( 0, 0, coefficient );
					//printf("\n coeff=%d, value =%f", coefficient,value);
					iSceneGraph.at(object)->iWaveletHash.at(vertex).insert( make_pair(coefficient, value) );
					}
				//else
				//	{
				//	}
				}
			//			printf("[%d: %d] = %d\n", object, vertex, iSceneGraph.at(object)->iVisibilityHash.at(vertex).size() );
			//			iSceneGraph.at(object)->iVisibilityCoefficients.at(vertex).clear();
			}
		//		iSceneGraph.at(object)->iVisibilityCoefficients.clear();
		}
	//int i=0;
	}

float* CMyRenderer::ReconstructVisibility( TIntHashTable* aHashTable )
	{
	//float* data = reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(aVertexIndex).at(0));

	float *totaldata;
	float *FaceData[6];
	
	CMatrixNoColors *matrix;
	CWavelet *wavelet;
	for (int i=0;i<6;i++)
	{
	matrix=new CMatrixNoColors(aHashTable,KSamplingResolution,KSamplingResolution,i);
	wavelet=new CWavelet(matrix,KSamplingResolution,KSamplingResolution);
	wavelet->standardReconstruction();
	FaceData[i]=wavelet->returnFloat();
	delete matrix;
	delete wavelet;
	}

	int index(0);
	totaldata=new float[KSamplingFaceCoefficients*6];
	float *ptr=totaldata;
	for (int i=0;i<6;i++)
		{
		for (int j=0;j<KSamplingFaceCoefficients;j++)
			{
#ifdef USE_FP_TEXTURES
			*(FaceData[i]+j) *= 150.0f;
#endif
			*(totaldata++) = *(FaceData[i]+j);
			index++;
			}
		}
	//printf("Copied %d values. [first:%f, last:%f]\n", index, *ptr, *(ptr+(index-1)) ) ;

	delete[] FaceData[0];
	delete[] FaceData[1];
	delete[] FaceData[2];
	delete[] FaceData[3];
	delete[] FaceData[4];
	delete[] FaceData[5];

	return ptr;

	}

//EXTERNAL FUNCTIONS TO USE GLUT CALLBACKS
//-----------------------------------------------------
void RenderSceneWithRenderer()
	{
	if (CMyRenderer::iCurrentRenderer != NULL)
		CMyRenderer::iCurrentRenderer->RenderScene();
	}

void UpdateSceneWithRenderer()
	{
	if (CMyRenderer::iCurrentRenderer != NULL)
		CMyRenderer::iCurrentRenderer->UpdateScene();
	}


void ResizeSceneWithRenderer( int aWidth, int aHeight )
	{
	if (CMyRenderer::iCurrentRenderer != NULL)
		CMyRenderer::iCurrentRenderer->ResizeScene(aWidth, aHeight);
	}

