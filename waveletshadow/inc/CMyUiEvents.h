#ifndef MYUIEVENTS_JOHANNES_H
#define MYUIEVENTS_JOHANNES_H

#include "Basic.h"
#include "CMyRenderer.h"

//CLASS DECLARATION

/** \brief User interface class.
*
* This class is for functions of user interaction.
*/
class CMyUiEvents
	{
	//PUBLIC FUNCTIONS
	public:
		//CONSTRUCTORS
		//CMyUiEvents();
		CMyUiEvents(CMyRenderer* aRenderer);

		//DESTRUCTOR
		~CMyUiEvents();

		//MEMBER FUNCTIONS
		void ProcessNormalKeys( unsigned char key, TVector3 aPoint );
		void ProcessCursorKeys( int key, TVector3 aPoint );
		void ProcessMouseEvent( int button, int state, TVector3 aPoint );
		void ProcessMouseMotionEvent( TVector3 aPoint );

		TVector3 trackBall( TVector3 aPoint );

	//PUBLIC DATA
	public:
		static CMyUiEvents* iCurrentUi;

	//PRIVATE DATA
	private:
		enum TMouseDownStatus
			{
			EMouseUpLeft = 0,
			EMouseUpRight,
			EMouseDownLeft,
			EMouseDownRight
			};

		CMyRenderer* iRenderer;
		TMouseDownStatus iMouseButtonDown;
		
		TVector3 iPreviousRotationPoint;
		TVector3 iCurrentRotationPoint;

		TVector3 iPreviousZoomPoint;
		TVector3 iCurrentZoomPoint;

		TVector3 iScreenSize;

		float iSpeed;
	};

extern void ProcessNormalKeysWithUi( unsigned char key, int x, int y );
extern void ProcessCursorKeysWithUi( int key, int x, int y );
extern void ProcessMouseEventWithUi( int button, int state, int x, int y);
extern void ProcessMouseMotionEventWithUi( int x, int y );
#endif
