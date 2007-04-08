
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
	{
	iRenderer = aRenderer;
	}

//Destructor
CMyUiEvents::~CMyUiEvents()
	{
	}

//Used to handle the keyboard input (ASCII Characters)
void CMyUiEvents::ProcessNormalKeys(unsigned char key, int x, int y)
	{
    switch (key)
		{
		//ESC
		case 27:
			exit(0);
			break;
		//SPACE
		case 32 :
			iRenderer->iRotationAnimation.iX = 0;
			iRenderer->iRotationAnimation.iY = 0;
			break;
		//ENTER
		case 13:
			iRenderer->ChangeVertexMap();
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
void CMyUiEvents::ProcessCursorKeys(int key, int x, int y)
	{
	}

void CMyUiEvents::ProcessMouseEvent( int button, int state, int x, int y )
	{
	switch (button)
		{
		//Set point of rotation
		case GLUT_LEFT_BUTTON:
//			printf("LEFT\n");
			if(GLUT_DOWN == state)
				{
				iMouseButtonDown = EMouseDownLeft;
				iMouseY = y;
				iMouseX = x;
				}
			else
				{
				iRenderer->iRotationAnimation.iX += ( (iMouseY-y) * KAngleChangeAmount );
				iRenderer->iRotationAnimation.iY += ( (iMouseX-x) * KAngleChangeAmount );
				iMouseY = y;
				iMouseX = x;
				iMouseButtonDown = EMouseUpLeft;
				}
			break;
		//Set mesh position
		case GLUT_RIGHT_BUTTON:
//			printf("RIGHT\n");
			if(GLUT_DOWN == state)
				{
//				printf("DOWN\n");
				iMouseButtonDown = EMouseDownRight;
				iMouseY = y;
				iMouseX = x;
				}
			else
				{
//				printf("UP\n");
				iMouseButtonDown = EMouseUpRight;
				}
			break;
		default:
			break;
		}

	}

void CMyUiEvents::ProcessMouseMotionEvent( int x, int y )
	{
/*
//	printf("MouseMoving: %d\n", iMouseButtonDown);
	if( EMouseDownLeft == iMouseButtonDown)
			{
//			printf("LeftB\n");
			iRenderer->iRotationAnimation.iX += ( (iMouseY-y) * KAngleChangeAmount*20 );
			iRenderer->iRotationAnimation.iY += ( (iMouseX-x) * KAngleChangeAmount );
			iMouseY = y;
			iMouseX = x;
			}
	else if( EMouseDownRight == iMouseButtonDown)
			{
//			printf("RightB\n");
			iRenderer->RotateLights( (iMouseX-x)*KLightChangeAmount, (iMouseY-y)*KLightChangeAmount );
			iMouseY = y;
			iMouseX = x;
			}
*/
	}






//EXTERNAL FUNCTIONS TO USE GLUT CALLBACKS
void ProcessNormalKeysWithUi( unsigned char key, int x, int y )
	{
	if (CMyUiEvents::iCurrentUi != 0 )
		CMyUiEvents::iCurrentUi->ProcessNormalKeys( key, x, y  );
	}

void ProcessCursorKeysWithUi( int key, int x, int y )
	{
	if (CMyUiEvents::iCurrentUi != 0 )
		CMyUiEvents::iCurrentUi->ProcessCursorKeys( key, x, y );
	}
void ProcessMouseEventWithUi( int button, int state, int x, int y)
	{
	if (CMyUiEvents::iCurrentUi != 0 )
		CMyUiEvents::iCurrentUi->ProcessMouseEvent( button, state, x, y );
	}

void ProcessMouseMotionEventWithUi( int x, int y )
	{
	if (CMyUiEvents::iCurrentUi != 0 )
		CMyUiEvents::iCurrentUi->ProcessMouseMotionEvent( x, y );
	}

