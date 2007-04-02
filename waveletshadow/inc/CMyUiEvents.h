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
		void ProcessNormalKeys( unsigned char key, int x, int y );
		void ProcessCursorKeys( int key, int x, int y );
		void ProcessMouseEvent( int button, int state, int x, int y );
		void ProcessMouseMotionEvent( int x, int y );

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
		int iMouseX;
		int iMouseY;
	};

extern void ProcessNormalKeysWithUi( unsigned char key, int x, int y );
extern void ProcessCursorKeysWithUi( int key, int x, int y );
extern void ProcessMouseEventWithUi( int button, int state, int x, int y);
extern void ProcessMouseMotionEventWithUi( int x, int y );
#endif
