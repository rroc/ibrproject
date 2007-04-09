//INCLUDES
#include <fstream>
#include "CMyRenderer.h"
#include "Texture.h"
#include "CMeshLoader.h"
#include "CObjLoader.h"
#include <ctime>

#ifdef USE_OPENMP
#include <omp.h>
#endif

//INIT STATIC DATA
CMyRenderer* CMyRenderer::iCurrentRenderer = 0;

static const float KEpsilon( 0.0001 );

static const float	KObjectScale = 0.0045;
static const string KObjectName = "simple4"; //"testscene" (1.0); "monster" (0.3); "gt16k" (0.0045); //"simple4";
static const string KObjectFileName = KObjectName+".obj";

static const string KDataFileName	= KObjectName + "_coefficients.bin";
static const float KLightVectorSize( 3.0 );


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
, iRotationAnimation( 0, 0, 0)
, iCubeMapVertex(0)
, iWireFrame(GL_TRIANGLES)
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

	CMyRenderer::iCurrentRenderer = 0;
	}


void CMyRenderer::LoadTextures()
	{
	iTextures.push_back( LoadTGATexture("church_roof.tga") );
	iTextures.push_back( LoadTGATexture("church_left.tga") );
	iTextures.push_back( LoadTGATexture("church_front.tga") );
	iTextures.push_back( LoadTGATexture("church_right.tga") );
	iTextures.push_back( LoadTGATexture("church_floor.tga") );
	iTextures.push_back( LoadTGATexture("church_back.tga") );

	//iTextures.push_back( loadCubeMapTextures(
	//										  "church_roof.tga"
	//										, "church_left.tga"
	//										, "church_front.tga"
	//										, "church_right.tga"
	//										, "church_floor.tga"
	//										, "church_back.tga"
	//										) 
	//					); 
	}

//
/// Init function for the constructors
void CMyRenderer::InitMain()
	{
	glMatrixMode( GL_VIEWPORT );
	glViewport(0, 0, (GLsizei)iScreenWidth, (GLsizei)iScreenHeight);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	glOrtho( -1, 1, -1, 1, 0.01, 1000 );

	glMatrixMode(GL_MODELVIEW);

	//Construct the scenegraph
	CreateScene();
	
	LoadTextures();

	InitLights();

	PrecomputedRadianceTransfer();

	InitVertexMap();

	//LoadPFMTexture("room_toy_mirror.pfm");

	//	iLightVector = iLightData->GetLightVector();
	//glLightfv( GL_LIGHT0, GL_POSITION, reinterpret_cast<GLfloat*>(&iLightVector) );
	//iLightVector *= KLightVectorSize; // for drawing the vector!
 	}

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
			for(float u=0; u<1.0f; u+=diff )
				{
				switch(cube)
					{
					case 0: //roof
						iSampleData.push_back( TVector3( 
														   (u-0.5)*2.0f
														,  1.0f
														,  -(v-0.5)*-2.0f 
														) 
													);
						break;
					case 1: //left
						iSampleData.push_back( TVector3( 
														  -1.0f
														, -(v-0.5)*-2.0f
														, (u-0.5)*-2.0f 
														)
													);
						break;
					case 2: //front 
						iSampleData.push_back( TVector3( 
														   (u-0.5)* 2.0f
														,  -(v-0.5)*-2.0f
														, -1.0f 
														)
													);
						break;
					case 3: //right
						iSampleData.push_back( TVector3( 
														   1.0f
														,  -(v-0.5)*-2.0f
														,  (u-0.5)* 2.0f 
														)
													);
						break;
					case 4: //floor
						iSampleData.push_back( TVector3( 
														   (u-0.5)*2.0f
														, -1.0f
														,  -(v-0.5)*2.0f 
														)
													);
						break;
					case 5: //back
						iSampleData.push_back( TVector3( 
														  (u-0.5)*2.0f
														, -(v-0.5)*2.0f
														, 1.0f 
														)
													);
						break;
					default:
						//no other options
						break;
					}
//				printf("%d\t%f\t%f - [%f, %f, %f]\n", cube, u, v, iSampleData.back().iX, iSampleData.back().iY, iSampleData.back().iZ);
				index++;
				}
			}
		}
	printf("READY, with %d coefficients.\n\n", index);
	return index;
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

	//Transparents are rendered separately
	//	iCarObjectCount -= transparentCount;

	//Free the loader
	delete loader;
	loader = NULL;

	//	system("pause");
	//	printf("GOT: verts: %d, norms:%d tris:%d\n", iMeshList.at(0)->iVertices.size(), iMeshList.at(0)->iVertexNormals.size(), iMeshList.at(0)->iTriangles.size() );


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
		iVerticesInScene += iSceneGraph.at(i)->iVertices.size();
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
	const int offSet = KSamplingResolution*KSamplingResolution;

	//	int vertex = 0;
	iVertexMapTextures[0] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(0)		), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[1] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet  )), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[2] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*2)), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[3] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*3)), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[4] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*4)), KSamplingResolution, KSamplingResolution );
	iVertexMapTextures[5] = CreateTexture( reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*5)), KSamplingResolution, KSamplingResolution );

	iTextures.push_back( iVertexMapTextures[0] );
	iTextures.push_back( iVertexMapTextures[1] );
	iTextures.push_back( iVertexMapTextures[2] );
	iTextures.push_back( iVertexMapTextures[3] );
	iTextures.push_back( iVertexMapTextures[4] );
	iTextures.push_back( iVertexMapTextures[5] );
	}


void CMyRenderer::ChangeVertexMap()
	{
	const int offSet = KSamplingResolution*KSamplingResolution;

	if(++iCubeMapVertex >= iSceneGraph.at(0)->iVisibilityCoefficients.size())
		{
		iCubeMapVertex = 0;
		}

	//roof
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[0]-1 ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(0        )) );
	//left
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[1]-1 ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet  )) );
	//front
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[2]-1 ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*2  )) );
	//right
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[3]-1 ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*3  )) );
	//floor
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[4]-1 ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*4  )) );
	//back
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[5]-1 ) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*5  )) );
	}


// *************	RENDERING METHODS *********** /

/** \brief Method that specifies how the screen is rendered
*/

void CMyRenderer::RenderScene()
	{
	if( abs(iRotationAnimation.iX) > 0.01 || abs(iRotationAnimation.iY) > 0.05 )
		{
		iRotationAnimation.iX *= 0.7;//0.3;
		iRotationAnimation.iY *= 0.9;
		}
	iSceneRotation->iAngles.iX += iRotationAnimation.iX;
	iSceneRotation->iAngles.iY += iRotationAnimation.iY;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	

	glTranslatef( 0, 0, -4.0 );
	glRotatef( iSceneRotation->iAngles.iX, 1,0,0 );
	glRotatef( iSceneRotation->iAngles.iY, 0,1,0 );
	//	glRotatef( iSceneRotation->iAngles.iZ, 0,0,1 );
	

	glPushMatrix();
	DrawCubemap();
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

	//
	//DrawAxis
	/*	glPopMatrix();
	glTranslatef( 1.0, 0.5, -2.5);
	glColor3f( 0, 0, 1 );
	glBegin( GL_LINES );

	//	glVertex3f( 0, 0, 0 );
	//	glVertex3f( iLightVector.iX, iLightVector.iY, iLightVector.iZ );

	glColor3f( 1, 0, 0 );
	glVertex3f( 0, 0, 0 );
	glVertex3f( 1, 0, 0  );

	glColor3f( 1, 1, 0 );
	glVertex3f( 0, 0, 0 );
	glVertex3f( 0, 1, 0  );

	glColor3f( 0, 1, 0 ); 
	glVertex3f( 0, 0, 0 );
	glVertex3f( 0, 0, 1  );
	glEnd();

	DrawLightSphere();
	*/
	DrawMap();

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

	//vertice count
	int vertCount(	 static_cast<int>( aMesh->iVertices.size()    ) );
	int normalCount( static_cast<int>( aMesh->iFaceNormals.size() ) );

	TVector3  vx[3];
	TVector3  nv[3];
	TColorRGBA colors[3];
	float brightness(1.0);

	//Only opague objects have PRT calculated
	if( 1.0 == aMesh->iMaterialColor.iA )
		{
		/*
		iTempVertexColors.clear();
		for(int i=0, endi=aMesh->iVertices.size(); i<endi; i++)
		{
		brightness=0.0;

		for(int j=0, endj = iSampleData->iNumberOfFunctions; j<endj; ++j)
		{
		brightness += iLightData->iRotatedSHSCoefficients.at( j ) * aMesh->iVertexShadowedLightCoefficients.at( i ).at( j ); // (number of vertices, number of functions)
		}
		//			brightness /= iSampleData->iNumberOfFunctions;
		brightness *= 1.5; //pow((brightness), 4 );
		brightness *= brightness;
		brightness *= brightness;
		brightness *= 1.5;

		iTempVertexColors.push_back( brightness );
		}
		*/
		}

	//Go through the Mesh Polygon by polygon
	for (int triangleIndex=0, triangleCount=static_cast<int>(aMesh->iTriangles.size()); triangleIndex<triangleCount; triangleIndex++)
		{
		//Current triangle
		t = aMesh->iTriangles.at(triangleIndex);

		//check that there is enough vertices in the vertices list
//		if( (t.iV1<=vertCount ) && (t.iV2<=vertCount) && (t.iV3<=vertCount) )
			{
			//VERTICES
			vx[0] = aMesh->iVertices.at(t.iV1);
			vx[1] = aMesh->iVertices.at(t.iV2);
			vx[2] = aMesh->iVertices.at(t.iV3);

			//NORMALS
			nv[0] = aMesh->iVertexNormals.at(t.iV1);
			nv[1] = aMesh->iVertexNormals.at(t.iV2);
			nv[2] = aMesh->iVertexNormals.at(t.iV3);

			colors[0] = aMesh->iMaterialColor;
			colors[1] = aMesh->iMaterialColor;
			colors[2] = aMesh->iMaterialColor;
			DrawTriangle( &vx[0], &nv[0], &colors[0] );
			}
		}
	}

void CMyRenderer::PrecomputedRadianceTransfer()
	{
	printf("\nPrecomputed Radiance Transfer...\n");

	if ( ValidPRTDataExists() )
		{
		LoadPRTData();
		return;
		} 
	else
		{
		PreCalculateDirectLight();
		SavePRTData();

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

bool CMyRenderer::ValidPRTDataExists()
	{
	std::ifstream infile( KDataFileName.c_str(), std::ios::in | std::ios::binary);
	if(!infile.is_open())
		{
		return false;
		}

	//Are these correct
	int numOfSamples;
	int numOfVertices;

	infile.read((char *)&numOfSamples,		sizeof(int));
	infile.read((char *)&numOfVertices,	sizeof(int));

	if( numOfSamples!= (KSamplingResolution*KSamplingResolution*6) || numOfVertices!=iVerticesInScene )
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
		int numvertices=currentMesh->iVertices.size();;
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
	int numOfSamples( KSamplingResolution*KSamplingResolution*6 );
	int numOfTotalVertices( iVerticesInScene );
	outFile.write((const char *)&numOfSamples, sizeof(int));
	outFile.write((const char *)&numOfTotalVertices, sizeof(int));

	printf(" - Samples/Vertices/Objects: %d/%d/%d\n", numOfSamples, numOfTotalVertices, iSceneGraph.size() );

	//for all the objects in the scene...
	for(int i=0, endI=iSceneGraph.size(); i<endI; ++i)
		{
//		printf(" O: %d\n", i );
		CMesh* currentMesh = iSceneGraph.at( i );
		int numVertices=currentMesh->iVertices.size();

		//...and for all the vertices in objects...
		for(int j=0; j<numVertices; ++j)
			{
//			printf("  V: %d/%d\n", j, numVertices );
			//load the visibility coefficients
			for(int k=0;k<numOfSamples;k++)
				{
//				printf("   S: %d/%d\n", k, numOfSamples );
				outFile.write((char *)&currentMesh->iVisibilityCoefficients.at(j).at(k), sizeof(float));
				}
			}
		}
	outFile.close();
	printf("\nSaving OK.\n");
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

#ifdef USE_OPENMP
	omp_set_num_threads( 3 );
#endif

	//FOR EACH OBJECT...
	for(int i=0; i<numObjects; ++i)
		{
		currentMesh = iSceneGraph.at( i );
		int numOfVertices =currentMesh->iVertices.size();

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
			vertexVisibility.resize(numberOfSamples, 0.0f );

			for(int k=0; k<numberOfSamples; ++k)
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
		int numOfVertices =currentMesh->iVertices.size();

		//FOR EACH VERTEX...
		for( int j=0; j<numOfVertices; ++j )
			{
			printf("Object: %d, Vert/Coeffs: %d / %d\n", i, j, currentMesh->iVisibilityCoefficients.at(j).size() );
			}
		}
	printf("Pre-calc Ready.\n\n");
	}


void CMyRenderer::RotateLights( float aChangeTheta, float aChangePhi )
	{
	//printf("ROT LIGHT: %f, %f\n", aChangeTheta, aChangePhi );
	//iLightData->RotateCoefficients( aChangeTheta, aChangePhi );
	//iLightVector = iLightData->GetLightVector();

	////let opengl know about the new light vector as well
	//glLightfv( GL_LIGHT0, GL_POSITION, reinterpret_cast<GLfloat*>(&iLightVector) );

	//iLightVector *= KLightVectorSize; // for drawing the vector!
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
					if( aRay->IntersectTriangle2( list, &iSceneGraph.at(i)->iFaceNormals.at(j) ) )
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
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	
	glDepthFunc(GL_ALWAYS);	// don't need to clear depth buffer
	//glDisable(GL_CULL_FACE);
	//glDisable(GL_LIGHTING );

	//USING MULTIPLE TEXTURES
	const float size(3.5f);
	const float maxEps(0.999);
	const float minEps(0.001);

	//roof
	glBindTexture( GL_TEXTURE_2D, iTextures.at(0) );
	glBegin(GL_QUADS);
	glTexCoord2f(minEps,maxEps);	glVertex3f(-size,size,size);
	glTexCoord2f(minEps,minEps);	glVertex3f(-size,size,-size);
	glTexCoord2f(maxEps,minEps);	glVertex3f(size,size,-size);
	glTexCoord2f(maxEps,maxEps);	glVertex3f(size,size,size);
	glEnd();

	//left
	glBindTexture( GL_TEXTURE_2D, iTextures.at(1) );
	glBegin(GL_QUADS);
	glTexCoord2f(minEps,maxEps);	glVertex3f(-size,size,size);
	glTexCoord2f(minEps,minEps);	glVertex3f(-size,-size,size);
	glTexCoord2f(maxEps,minEps);	glVertex3f(-size,-size,-size);
	glTexCoord2f(maxEps,maxEps);	glVertex3f(-size,size,-size);
	glEnd();

	//front 
	glBindTexture( GL_TEXTURE_2D, iTextures.at(2) );
	glBegin(GL_QUADS);
	glTexCoord2f(minEps,maxEps);	glVertex3f(-size,size,-size);
	glTexCoord2f(minEps,minEps);	glVertex3f(-size,-size,-size);
	glTexCoord2f(maxEps,minEps);	glVertex3f(size,-size,-size);
	glTexCoord2f(maxEps,maxEps);	glVertex3f(size,size,-size);
	glEnd();

	//right
	glBindTexture( GL_TEXTURE_2D, iTextures.at(3) );
	glBegin(GL_QUADS);
	glTexCoord2f(minEps,maxEps);	glVertex3f(size,size,-size);
	glTexCoord2f(minEps,minEps);	glVertex3f(size,-size,-size);
	glTexCoord2f(maxEps,minEps);	glVertex3f(size,-size,size);
	glTexCoord2f(maxEps,maxEps);	glVertex3f(size,size,size);
	glEnd();

	//fLoor
	glBindTexture( GL_TEXTURE_2D, iTextures.at(4) );
	glBegin(GL_QUADS);
	glTexCoord2f(minEps,maxEps);	glVertex3f(-size,-size,-size);
	glTexCoord2f(minEps,minEps);	glVertex3f(-size,-size,size);
	glTexCoord2f(maxEps,minEps);	glVertex3f(size,-size,size);
	glTexCoord2f(maxEps,maxEps);	glVertex3f(size,-size,-size);
	glEnd();

	//back
	glBindTexture( GL_TEXTURE_2D, iTextures.at(5) );
	glBegin(GL_QUADS);
	glTexCoord2f(minEps,maxEps);	glVertex3f(-size,-size,size);
	glTexCoord2f(minEps,minEps);	glVertex3f(-size,size,size);
	glTexCoord2f(maxEps,minEps);	glVertex3f(size,size,size);
	glTexCoord2f(maxEps,maxEps);	glVertex3f(size,-size,size);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);	// normal depth buffering

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_LIGHTING );
	}



void CMyRenderer::DrawMap()
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
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[0]-1 ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);		 glVertex2i( translateX,		translateY );
	glTexCoord2f(1.0f,0);	 glVertex2i( size+translateX,	translateY );
	glTexCoord2f(1.0f,1.0f); glVertex2i( size+translateX,	size+translateY );
	glTexCoord2f(0,1.0f);	 glVertex2i( translateX,		size+translateY );
	glEnd();

	//left
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[1]-1 ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);		glVertex2i(-size+translateX,	-size+translateY);
	glTexCoord2f(1.0f,0);	glVertex2i( translateX,			-size+translateY);
	glTexCoord2f(1.0f,1.0f);glVertex2i( translateX,			translateY);
	glTexCoord2f(0,1.0f);	glVertex2i(-size+translateX,	translateY);
	glEnd();

	//front
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[2]-1 ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);		glVertex2i(translateX,		-size+translateY);
	glTexCoord2f(1.0f,0);	glVertex2i(size+translateX,	-size+translateY);
	glTexCoord2f(1.0f,1.0f);glVertex2i(size+translateX,	translateY);
	glTexCoord2f(0,1.0f);	glVertex2i(translateX,		translateY);
	glEnd();

	//right
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[3]-1 ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);		glVertex2i(size+translateX,		-size+translateY);
	glTexCoord2f(1.0f,0);	glVertex2i(size2 + translateX,	-size+translateY);
	glTexCoord2f(1.0f,1.0f);glVertex2i(size2 + translateX,	translateY);
	glTexCoord2f(0,1.0f);	glVertex2i(size+translateX,		translateY);
	glEnd();

	//floor
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[4]-1 ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);		glVertex2i(translateX,		-size2 +translateY);
	glTexCoord2f(1.0f,0);	glVertex2i(size+translateX,	-size2 +translateY);
	glTexCoord2f(1.0f,1.0f);glVertex2i(size+translateX,	-size+translateY);
	glTexCoord2f(0,1.0f);	glVertex2i(translateX,		-size+translateY);
	glEnd();

	//back
	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[5]-1 ) );
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);		glVertex2i(translateX,		-size2 -size +translateY);
	glTexCoord2f(1.0f,0);	glVertex2i(size+translateX,	-size2 - size +translateY);
	glTexCoord2f(1.0f,1.0f);glVertex2i(size+translateX,	-size2 + translateY);
	glTexCoord2f(0,1.0f);	glVertex2i(translateX,		-size2 + translateY);
	glEnd();

	glDisable( GL_TEXTURE_2D );

	glDepthFunc(GL_LESS);	// normal depth buffering
	//glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	}


//void CMyRenderer::DrawMap()
//	{
//	glDisable(GL_LIGHTING);
//
//	if(iCubeMapVertex > iSceneGraph.at(0)->iVisibilityCoefficients.size())
//		{
//		iCubeMapVertex = 0;
//		}
//
//	glColor3f( 1.0f,1.0f,0.0);
//	glTranslatef( iSceneGraph.at(0)->iVertices.at(iCubeMapVertex).iX, iSceneGraph.at(0)->iVertices.at(iCubeMapVertex).iY, iSceneGraph.at(0)->iVertices.at(iCubeMapVertex).iZ );
//	glutSolidSphere(0.05f, 16,16);
//
//
//	glDepthFunc(GL_ALWAYS);	// don't need to clear depth buffer
//	//glDisable(GL_DEPTH_TEST);
//
//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//
//	gluOrtho2D( 0, iScreenWidth, 0, iScreenHeight );
//
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadIdentity();
//
//	//	glTranslatef(0.375f, 1.5f, 0.0f);
//
//	glColor3f(1.0, 1.0, 1.0 );
//
//	const int size = 64;
//	const int size2 = size*2;
//	const int translateY = iScreenHeight - (20 + size );
//	const int translateX = iScreenWidth-(size2+20);
//
//	glEnable( GL_TEXTURE_2D );
//
//	int offSet = KSamplingResolution*KSamplingResolution;
//	//roof
//	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[0]-1 ) );
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(0        )) );
//
//	glBegin(GL_QUADS);
//	glTexCoord2f(0,0);		 glVertex2i( translateX,		translateY );
//	glTexCoord2f(1.0f,0);	 glVertex2i( size+translateX,	translateY );
//	glTexCoord2f(1.0f,1.0f); glVertex2i( size+translateX,	size+translateY );
//	glTexCoord2f(0,1.0f);	 glVertex2i( translateX,		size+translateY );
//	glEnd();
//
//	//left
//	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[1]-1 ) );
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet  )) );
//	glBegin(GL_QUADS);
//	glTexCoord2f(0,0);		glVertex2i(-size+translateX,	-size+translateY);
//	glTexCoord2f(1.0f,0);	glVertex2i( translateX,			-size+translateY);
//	glTexCoord2f(1.0f,1.0f);glVertex2i( translateX,			translateY);
//	glTexCoord2f(0,1.0f);	glVertex2i(-size+translateX,	translateY);
//	glEnd();
//
//	//front
//	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[2]-1 ) );
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*2  )) );
//	glBegin(GL_QUADS);
//	glTexCoord2f(0,0);		glVertex2i(translateX,		-size+translateY);
//	glTexCoord2f(1.0f,0);	glVertex2i(size+translateX,	-size+translateY);
//	glTexCoord2f(1.0f,1.0f);glVertex2i(size+translateX,	translateY);
//	glTexCoord2f(0,1.0f);	glVertex2i(translateX,		translateY);
//	glEnd();
//
//	//right
//	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[3]-1 ) );
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*3  )) );
//	glBegin(GL_QUADS);
//	glTexCoord2f(0,0);		glVertex2i(size+translateX,		-size+translateY);
//	glTexCoord2f(1.0f,0);	glVertex2i(size2 + translateX,	-size+translateY);
//	glTexCoord2f(1.0f,1.0f);glVertex2i(size2 + translateX,	translateY);
//	glTexCoord2f(0,1.0f);	glVertex2i(size+translateX,		translateY);
//	glEnd();
//
//	//floor
//	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[4]-1 ) );
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*4  )) );
//	glBegin(GL_QUADS);
//	glTexCoord2f(0,0);		glVertex2i(translateX,		-size2 +translateY);
//	glTexCoord2f(1.0f,0);	glVertex2i(size+translateX,	-size2 +translateY);
//	glTexCoord2f(1.0f,1.0f);glVertex2i(size+translateX,	-size+translateY);
//	glTexCoord2f(0,1.0f);	glVertex2i(translateX,		-size+translateY);
//	glEnd();
//
//	//back
//	glBindTexture( GL_TEXTURE_2D, iTextures.at( iVertexMapTextures[5]-1 ) );
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, KSamplingResolution, KSamplingResolution, 1, GL_LUMINANCE, GL_FLOAT, reinterpret_cast<float*>( &iSceneGraph.at(0)->iVisibilityCoefficients.at(iCubeMapVertex).at(offSet*5  )) );
//	glBegin(GL_QUADS);
//	glTexCoord2f(0,0);		glVertex2i(translateX,		-size2 -size +translateY);
//	glTexCoord2f(1.0f,0);	glVertex2i(size+translateX,	-size2 - size +translateY);
//	glTexCoord2f(1.0f,1.0f);glVertex2i(size+translateX,	-size2 + translateY);
//	glTexCoord2f(0,1.0f);	glVertex2i(translateX,		-size2 + translateY);
//	glEnd();
//
//	glDisable( GL_TEXTURE_2D );
//
//	glDepthFunc(GL_LESS);	// normal depth buffering
//	//glEnable(GL_DEPTH_TEST);
//
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();
//
//	glMatrixMode(GL_MODELVIEW);
//	glPopMatrix();
//	}



void CMyRenderer::DrawTriangle( TVector3* aVx, TVector3* aNv, TColorRGBA aCol[3])
	{
	glBegin(iWireFrame);
	glColor4fv( &aCol[0].iR );
//	glNormal3f(aNv[0].iX, aNv[0].iY, aNv[0].iZ);
	glVertex3f(aVx[0].iX, aVx[0].iY, aVx[0].iZ);

	glColor4fv( &aCol[1].iR );
//	glNormal3f(aNv[1].iX, aNv[1].iY, aNv[1].iZ);
	glVertex3f(aVx[1].iX, aVx[1].iY, aVx[1].iZ);

	glColor4fv( &aCol[2].iR );
//	glNormal3f(aNv[2].iX, aNv[2].iY, aNv[2].iZ);
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
	for(int i=0, endi=aMesh->iVertices.size(); i<endi; i++)
		{
		MultMatrixVect( modelViewMatrix, &aMesh->iVertices.at( i ) );
		}
	}


void CMyRenderer::MultMatrixVect(const double aMatrix[16], TVector3* aVector)
	{
	//	printf("VEC:  [%f, %f %f]\n", aVector->iX, aVector->iY, aVector->iZ);
	aVector->set(																	// + transform
		aMatrix[0] * aVector->iX + aMatrix[4] * aVector->iY   + aMatrix[8] * aVector->iZ +aMatrix[12],
		aMatrix[1] * aVector->iX + aMatrix[5] * aVector->iY   + aMatrix[9] * aVector->iZ +aMatrix[13],
		aMatrix[2] * aVector->iX + aMatrix[6] * aVector->iY   + aMatrix[10] * aVector->iZ +aMatrix[14]
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

