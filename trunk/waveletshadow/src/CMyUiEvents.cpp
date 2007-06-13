
//INCLUDE FILES
#include "CMyUiEvents.h"

//CONSTANTS
const float KAngleChangeAmount = 0.005f;
const float KLightChangeAmount = 0.1f;

const float KScaleChangeAmount = 1.0f;
const float KFocusAreaAmount = 1.0f;

//INIT STATIC DATA
CMyUiEvents* CMyUiEvents::iCurrentUi = 0;

//Constructor, saves a pointer to the renderer
CMyUiEvents::CMyUiEvents( CMyRenderer* aRenderer )
: iRenderer( aRenderer )
, iScreenSize( glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT), 0)
, iPreviousObjectRotationPoint(0,0,0)
, iPreviousZoomPoint(0,0,0)
, iCurrentObjectRotationPoint(0,0,0)
, iCurrentZoomPoint(0,0,0)
	{
	}

//Destructor
CMyUiEvents::~CMyUiEvents()
	{
	}

//Used to handle the keyboard input (ASCII Characters)
void CMyUiEvents::ProcessNormalKeys(  unsigned char key, TVector3 aPoint  )
	{
	switch (key)
		{
		//ESC
		case 27:
			exit(0);
			break;
			//SPACE
		case 32 :
//			iRenderer->ObjectRotatingEvent();
			break;
			//ENTER
		case 13:
			iRenderer->ChangeVertexMap();
			break;
		case 'd':
		case 'D':
			iRenderer->ActivateDecomposition();
			break;
		case 'r':
		case 'R':
			iRenderer->ActivateReconstruction();
			break;
		case 'w':
		case 'W':
			if(iRenderer->iWireFrame == GL_TRIANGLES)
				{
				iRenderer->iWireFrame = GL_LINE_LOOP;
				}
			else
				{
				iRenderer->iWireFrame = GL_TRIANGLES;
				}			
			break;
			//undefined key
		default:
			break;
		}
	}

// Used to handle the keyboard input (not ASCII Characters)
void CMyUiEvents::ProcessCursorKeys(  int key, TVector3 aPoint  )
	{
	}

void CMyUiEvents::ProcessMouseEvent(  int button, int state, TVector3 aPoint  )
	{
	switch (button)
		{
		case GLUT_LEFT_BUTTON:
			//MOUSE DOWN
			if(GLUT_DOWN == state)
				{		
				//MOVE LIGHT PROBE
				if( GLUT_ACTIVE_CTRL == glutGetModifiers())
					{
					iMouseButtonDown = EMouseCTRLDownLeft;
					iPreviousLightRotationPoint = trackBall( aPoint );
					}
				//MOVE SCENE
				else
					{
					iMouseButtonDown = EMouseDownLeft;
					iPreviousObjectRotationPoint = trackBall( aPoint );
					}
				}

			//MOUSE UP
			else
				{
				iMouseButtonDown = EMouseUpLeft;
				}
			break;

			//ZOOM
		case GLUT_RIGHT_BUTTON:
			if(GLUT_DOWN == state)
				{
				iMouseButtonDown = EMouseDownRight;
				iPreviousZoomPoint = aPoint;
				}
			else
				{
				iMouseButtonDown = EMouseUpRight;
				}
			break;
		default:
			break;
		}
	}

void CMyUiEvents::ProcessMouseMotionEvent(  TVector3 aPoint  )
	{
	//ROTATE SCENE
	if( EMouseDownLeft == iMouseButtonDown)
		{
		iCurrentObjectRotationPoint = trackBall( aPoint );
		TVector3 direction = iCurrentObjectRotationPoint - iPreviousObjectRotationPoint;
		iObjectRotationSpeed = direction.length();
		if( iObjectRotationSpeed > 0.0001 ) // If little movement - do nothing.
			{
			iRenderer->iObjectRotationAngle = iObjectRotationSpeed * 15.0f;
			iRenderer->iObjectRotationAxis = (iPreviousObjectRotationPoint.cross( iCurrentObjectRotationPoint )).normalize();
			iRenderer->ObjectRotatingEvent();
			}
		}
	//ROTATE LIGHT
	else if ( EMouseCTRLDownLeft == iMouseButtonDown )
		{
		iCurrentLightRotationPoint = trackBall( aPoint );
		TVector3 direction = iCurrentLightRotationPoint - iPreviousLightRotationPoint;
		iLightRotationSpeed = direction.length();
		if( iLightRotationSpeed > 0.0001 ) // If little movement - do nothing.
			{
			iRenderer->iLightRotationAngle = iLightRotationSpeed * 15.0f;
			iRenderer->iLightRotationAxis = (iPreviousLightRotationPoint.cross( -iCurrentLightRotationPoint )).normalize();
			iRenderer->LightRotatingEvent();
			}
		}
	//ZOOM
	else if( EMouseDownRight == iMouseButtonDown)
		{
		int diff = aPoint.iY - iPreviousZoomPoint.iY;
		float zoomFactor = diff * 0.01f;
		float sum = iRenderer->iScale + zoomFactor;

		//check range
		if(sum<0.6f)
			{
			iRenderer->iScale = 0.6f;
			}
		else if(sum>2.7f)
			{
			iRenderer->iScale = 2.5f;
			}
		else
			{
			iRenderer->iScale = sum;
			}
		iPreviousZoomPoint = aPoint;
		}
	}

TVector3 CMyUiEvents::trackBall( TVector3 aPoint )
	{
	TVector3 trackBallPoint( 
		( 2.0f*aPoint.iX - iScreenSize.iX ) / iScreenSize.iX
		, ( 2.0f*aPoint.iY - iScreenSize.iY ) / iScreenSize.iY
		, 0.0
		);

	float d = trackBallPoint.length();
	d = (d<1.0f)? d : 1.0f;
	trackBallPoint.iZ = sqrtf(1.001f - d*d);

	//needs to be normalized (only d was clipped)
	trackBallPoint.normalize(); 
	return trackBallPoint;
	}



//EXTERNAL FUNCTIONS TO USE GLUT CALLBACKS
void ProcessNormalKeysWithUi( unsigned char key, int x, int y )
	{
	if (CMyUiEvents::iCurrentUi != 0 )
		CMyUiEvents::iCurrentUi->ProcessNormalKeys( key, TVector3(x, y, 0)  );
	}

void ProcessCursorKeysWithUi( int key, int x, int y )
	{
	if (CMyUiEvents::iCurrentUi != 0 )
		CMyUiEvents::iCurrentUi->ProcessCursorKeys( key, TVector3(x, y, 0) );
	}
void ProcessMouseEventWithUi( int button, int state, int x, int y)
	{
	if (CMyUiEvents::iCurrentUi != 0 )
		CMyUiEvents::iCurrentUi->ProcessMouseEvent( button, state, TVector3(x, y, 0) );
	}

void ProcessMouseMotionEventWithUi( int x, int y )
	{
	if (CMyUiEvents::iCurrentUi != 0 )
		CMyUiEvents::iCurrentUi->ProcessMouseMotionEvent( TVector3(x, y, 0) );
	}

