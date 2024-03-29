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
#include "THashtable.h"

#include "CRay.h"

#include "CCubeVectorDecomposition.h"

//CONSTANTS
static const int KSamplingResolution(32); // this is used as the cubemap resolution ie. (64x64x6)
static const int KSamplingFaceCoefficients( KSamplingResolution*KSamplingResolution );
static const int KSamplingTotalCoefficients( KSamplingResolution*KSamplingResolution*6 );
static const int KWaveletDescale(( sqrtf( KSamplingTotalCoefficients ) * sqrtf( KSamplingTotalCoefficients ) ));
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

		void UpdateScene();

		void ChangeVertexMap();
		void ChangeProbeMap();

		void DecomposeLightProbeMap();
		float * DecomposeLightProbe();



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

		void ObjectRotatingEvent();
		void LightRotatingEvent();

		TVector3 CubeToVector( int aCube, float aU, float aV );
		TVector3 VectorToCube(const TVector3& aVector);

	//PRIVATE FUNCTIONS
	//------------------
	private:
		//Constructors will call this one
		void InitMain();
		void InitLights();
		void InitVertexMap();
		//void InitHashTables();
		void InitWaveletHash();

		//Constructors will call this one
		void CreateScene();
		void LoadTextures();

		void ApplyObjectRotations();
		void ApplyLightRotations();

		void RenderObject(CMesh* aMesh);
		void DrawTriangle( TVector3* aVx, TVector3* aNv, TColorRGBA aCol[3]);
		void DrawTriangle( TVector3* aVx, TColorRGBA aCol[3]);
		void DrawCubemap();

		//per Vertex cubemap
		void DrawVertexVisibilityMap();
		void DrawLightProbe();

		float* DecomposeVisibility();
		float* DecomposeVisibility(int aObject, int aVertexIndex);
		void InitLightProbeWaveletHash();


		//float* ReconstructVisibility();
		void ReconstructVisibility( std::vector<float>* aVisCoefficients, TIntHashTable* aHash );

		void DrawLightSphere();

		void DrawSceneNode( CSceneNode* aNode );
		void ApplySceneTransformations( CSceneNode* aNode );
		
		void PrecomputedRadianceTransfer();
		void PreCalculateDirectLight();
		bool ValidPRTDataExists(string filename);
		
		//void LoadPRTData();
		//void LoadPRTHashData();
		void LoadPRTWaveletData();

		//void SavePRTData();
		//void SavePRTHashData();
		void SavePRTWaveletData();

		void TransformMesh( CMesh* aMesh );

		void ApplyMultMatrixVect(const double* aMatrix, TVector3* aVector);
		TVector3 MultMatrixVect(const float* aMatrix, const TVector3& aVector);

		bool IsRayBlocked( CRay* aRay );
		void ShowFPS();

		void ShowLightDirection();

		int InitializeSamplingData();
		void CalculateTransformedLightProbe();

		//TVector3 CubeToVector( int aCube, float aU, float aV );
		//TVector3 VectorToCube(const TVector3& aVector);

		void InverseMatrix( float aMatrix[4][4]) const;

	//PUBLIC STATIC DATA
	//------------------
	public:
		/// A static pointer to the current renderer object.
		/// This is used to be able to pass rendering method to OpenGL.
		static CMyRenderer* iCurrentRenderer;
		CSceneRotation*		iSceneRotation;

		TVector3	iObjectRotationAxis;
		float		iObjectRotationAngle;

		TVector3	iLightRotationAxis;
		float		iLightRotationAngle;

		GLenum iWireFrame;

		float iScale;

	//PRIVATE DATA
	//------------------
	private:

		bool  iObjectRotationFinished;
		float iObjectRotationXForm[4][4];
		float iObjectRotationXFormInv[4][4];
		float iObjectRotationAngleChange;

		float iLightRotationXForm[4][4];
		float iLightRotationXFormInv[4][4];

		int iScreenHeight;	//< The height of the screen
		int iScreenWidth;	//< The width of the screen

		int iVertexMapTextures[6];
		int iProbeMapTextures[6];

		TVector3* iLightProbe;
		TIntColorHashTable iLightProbeWaveletHash;
//		TVector3* iDecomposedLightProbe;

		CCubeVectorDecomposition* iDecomposedLightProbe;

		TVector3* iTransformedLightProbe;
		int		  iCubeTexture;

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
		int			iNumberOfSamples;

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


