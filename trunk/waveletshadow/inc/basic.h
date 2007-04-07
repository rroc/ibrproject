#pragma once

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>


class TSquare
	{
	public:
		TSquare( int aL, int aX, int aY )
			: l(aL)
			, x(aX)
			, y(aY)
			{
			//empty on purpose
			}
	public:
		int l; //level
		int x; //x-coord
		int y; //y-coord
	};


#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

#define TWOPI M_PI*2.0
#define FOURPI M_PI*4.0
#define KDegreeToRadian M_PI/180.0

#define USE_OPENMP
