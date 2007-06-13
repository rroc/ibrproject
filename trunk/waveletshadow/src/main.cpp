#include "basic.h"
#include "CMyRenderer.h"
#include "CMyUiEvents.h"
#include "CMatrix.h"
#include "CMatrixNoColors.h"
#include "CWavelet.h"

#include "THashtable.h"

//CONSTANTS
const int KWindowPositionX = 100;
const int KWindowPositionY = 100;
const int KWindowWidth  = 800;//1920;//800;
const int KWindowHeight = 600;//1200;//600;


void initGLUTOpenGL(int argc, char **argv)
	{
	//Init Window
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition( KWindowPositionX, KWindowPositionY );
	glutInitWindowSize( KWindowWidth, KWindowHeight );
	glutCreateWindow("WaveletShadows");

	//Set some Glut functions
	glutKeyboardFunc( ProcessNormalKeysWithUi );
	glutSpecialFunc(  ProcessCursorKeysWithUi );
	glutMouseFunc(    ProcessMouseEventWithUi );
	glutMotionFunc(   ProcessMouseMotionEventWithUi );

	glutDisplayFunc( RenderSceneWithRenderer );
	glutIdleFunc(    UpdateSceneWithRenderer );
	glutReshapeFunc( ResizeSceneWithRenderer );

//	glEnable(GL_BLEND);
//	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_SMOOTH); //GL_FLAT | GL_SMOOTH;
	//glShadeModel( GL_FLAT ); // | GL_SMOOTH;

	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glEnable( GL_POLYGON_SMOOTH );

	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST ); // Texture mapping perspective correction
    //glEnable(GL_TEXTURE_2D); // Texture mapping ON
	// select modulate to mix texture with color for shading
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glPolygonMode (GL_FRONT, GL_FILL); // Polygon rasterization mode (polygon filled)
	glEnable(GL_CULL_FACE); // Enable the back face culling

	//glDepthRange( 0, 1);
    glEnable(GL_DEPTH_TEST); // Enable the depth test (z-buffer)

	GLenum err = glewInit();
	printf("Glew Init: %s\n", (GLEW_OK==err?"OK" : "FAIL"));
	}


int main(int argc, char **argv)
	{
		//float m1[4][4]={  {40, 40, 40, 39 } , {40, 29, 40, 9} , {140, 140, 40, 9} , {139, 139, 4, 4}  };
		////float m1[1][4]= {{9,7,3,5}};
		//float *p=&m1[0][0];
		//CMatrixNoColors matrix1(p,4,4);
		////matrix1.print();
		//
		//CWavelet wavelet1(&matrix1, 4,4);
		//wavelet1.print();
		//wavelet1.standardDeconstruction();

		//wavelet1.standardReconstruction();

		//wavelet1.print();
		//
		//
	initGLUTOpenGL( argc, argv );

	CMyRenderer* renderer = new CMyRenderer( KWindowWidth, KWindowHeight );
	CMyRenderer::iCurrentRenderer = renderer;

	CMyUiEvents* ui = new CMyUiEvents( renderer );
	CMyUiEvents::iCurrentUi = ui;

	glutFullScreen();
	glutMainLoop();
	return 0;
	}
