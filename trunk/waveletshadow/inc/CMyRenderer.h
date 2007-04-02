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

		void UpdateScene();

		//MEMBER FUNCTIONS
		///Renders a scene. This function is called from OpenGL, and renders a scene.
		void RenderScene();

		///Resize the rendering window
		void ResizeScene(const int aWidth, const int aHeight);

		///Rotate lightcoefficients
		void RotateLights( float aChangeTheta, float aChangePhi );

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


	//PRIVATE FUNCTIONS
	//------------------
	private:
		//Constructors will call this one
		void InitMain();
		void InitLights();

		//Constructors will call this one
		void CreateScene();
		void LoadTextures();


		void RenderObject(CMesh* aMesh);
		void DrawTriangle( TVector3* aVx, TVector3* aNv, TColorRGBA aCol[3]);
		void DrawCubemap();

		void DrawLightSphere();

		void DrawSceneNode( CSceneNode* aNode );
		void ApplySceneTransformations( CSceneNode* aNode );
		void PreCalculateDirectLight();

		void TransformMesh( CMesh* aMesh );
		void MultMatrixVect(const double aMatrix[16], TVector3* aVector);

		bool IsRayBlocked( CRay* aRay );
		void ShowFPS();

	//PUBLIC STATIC DATA
	//------------------
	public:
		/// A static pointer to the current renderer object.
		/// This is used to be able to pass rendering method to OpenGL.
		static CMyRenderer* iCurrentRenderer;
		CSceneRotation* iSceneRotation;
		TVector3 iRotationAnimation;

	//PRIVATE DATA
	//------------------
	private:
		int iScreenHeight;	//< The height of the screen
		int iScreenWidth;	//< The width of the screen

		vector<int> iTextures;

		CSceneNode* iScene;
		vector<CMesh*> iMeshList;
		vector<CMesh*> iSceneGraph;
		vector<CMesh*> iSceneGraphTransparent;

		int iObjectsInScene;
		int iVerticesInScene;

		int	iCarObjectCount;

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


