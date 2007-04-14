#ifndef CMYRENDERER_H
#define CMYRENDERER_H

#include "basic.h"

#include "CMesh.h"
#include "CBall.h"
#include "CIcosahedron.h"
#include "CBox.h"

#include "CSceneNode.h"
#include "CSceneMesh.h"
#include "CSceneRotation.h"
#include "CSceneTranslation.h"

#include "CRay.h"

//CONSTANTS
static const int KSamplingResolution(32); // this is used as the cubemap resolution ie. (64x64x6)
static const int KSamplingFaceCoefficients( KSamplingResolution*KSamplingResolution );
static const int KSamplingTotalCoefficients( KSamplingResolution*KSamplingResolution*6 );

//CLASS DECLARATION

/** \brief Rendering class.
*
* This class contains all the functions necessary for rendering.
*/
class CMyRenderer
	{
	//PUBLIC FUNCTIONS
	//------------------
	public:

		//CONSTRUCTORS
		//------------------
		/**
		* Default constructor.
		* Initializes private data, and sets the window size to ( aWidth, aHeight ).
		* @param aWidth width of the screen
		* @param aHeight height of the screen
		*/
		CMyRenderer( const int aWidth, const int aHeight );

		//DESTRUCTOR
		//------------------
		/**
		* Default destructor.
		*/
		~CMyRenderer();


		//MEMBER FUNCTIONS
		///Renders a scene. This function is called from OpenGL, and renders a scene.
		void RenderScene();

		///Resize the rendering window
		void ResizeScene(const int aWidth, const int aHeight);

		///Rotate lightcoefficients
		void RotateLights( float aChangeTheta, float aChangePhi );

		void UpdateScene();

		void ChangeVertexMap();
		void DecomposeLightProbeMap();



		//GETTERS
		//---------------------------------------
		/**
		* Get a width of the screen.
		* @return iScreenWidth
		*/
		int Width() const
			{ return iScreenWidth; };

		/**
		* Get a height of the screen.
		* @return iScreenHeight
		*/
		int Height() const
			{ return iScreenHeight; };


		void ActivateDecomposition();
		void ActivateReconstruction();

		void RotatingStarted();
		void RotatingFinished( float aSpeed );

	//PRIVATE FUNCTIONS
	//------------------
	private:
		//Constructors will call this one
		void InitMain();
		void InitLights();
		void InitVertexMap();
		void InitHashTables();
		void InitWaveletHash();

		//Constructors will call this one
		void CreateScene();
		void LoadTextures();

		void ApplyRotations();

		void RenderObject(CMesh* aMesh);
		void DrawTriangle( TVector3* aVx, TVector3* aNv, TColorRGBA aCol[3]);
		void DrawTriangle( TVector3* aVx, TColorRGBA aCol[3]);
		void DrawCubemap();

		//per Vertex cubemap
		void DrawMap();
		void DrawProbe();

		float* DecomposeVisibility();
		float* DecomposeVisibility(int aVertexIndex);


		//float* ReconstructVisibility();
		float* ReconstructVisibility( TIntHashTable* aHash );



		void DrawLightSphere();

		void DrawSceneNode( CSceneNode* aNode );
		void ApplySceneTransformations( CSceneNode* aNode );
		
		void PrecomputedRadianceTransfer();
		void PreCalculateDirectLight();
		bool ValidPRTDataExists(string filename);
		
		void LoadPRTData();
		void LoadPRTHashData();
		void LoadPRTWaveletData();

		void SavePRTData();
		void SavePRTHashData();
		void SavePRTWaveletData();

		void TransformMesh( CMesh* aMesh );
		void MultMatrixVect(const double aMatrix[16], TVector3* aVector);

		bool IsRayBlocked( CRay* aRay );
		void ShowFPS();

		int InitializeSamplingData();

	//PUBLIC STATIC DATA
	//------------------
	public:
		/// A static pointer to the current renderer object.
		/// This is used to be able to pass rendering method to OpenGL.
		static CMyRenderer* iCurrentRenderer;
		CSceneRotation* iSceneRotation;
		TVector3 iRotationAnimation;

		TVector3	iRotationAxis;
		float		iRotationAngle;
		float		iRotationAngleChange;

		GLenum iWireFrame;

		float iScale;

	//PRIVATE DATA
	//------------------
	private:
		bool iRotationFinished;

		float iRotationXForm[16];

		int iScreenHeight;	//< The height of the screen
		int iScreenWidth;	//< The width of the screen

		vector<int> iTextures;
		int iVertexMapTextures[6];
		int iProbeMapTextures[6];

		TVector3* iLightProbe;

		CSceneNode* iScene;
		vector<CMesh*> iMeshList;
		vector<CMesh*> iSceneGraph;
		vector<CMesh*> iSceneGraphTransparent;

		int iObjectsInScene;
		int iVerticesInScene;

		int iCubeMapVertex;
		int	iObjectCount;

		bool iVisualDecomposition;

		//The sampling vectors for raytracing
		vector<TVector3>			iSampleData;

		//FPS
		int			iFrame;
		int			iTime;
		int			iTimebase;
		char		iFpsString[ 80 ];
		std::string	iLightingModelName;
	};



//EXTERNAL FUNCTIONS
//Reference to rendering function. Used to pass the render object's rendering function to OpenGL.
extern void RenderSceneWithRenderer();
//Reference to rendering function. Used to update variables while idle.
extern void UpdateSceneWithRenderer();
//Reference to resize function. Used to pass the render object's resize function to OpenGL.
extern void ResizeSceneWithRenderer( int aWidth, int aHeight );

#endif


