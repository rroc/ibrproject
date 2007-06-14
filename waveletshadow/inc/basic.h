#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>
#include <limits>

class TSquare
	{
	public:
		TSquare(  int aX, int aY )
			: x(aX)
			, y(aY)
			{
			//empty on purpose
			}
	public:
		int x; //x-coord
		int y; //y-coord
	};


#ifndef M_PI
static const float M_PI        (3.14159265358979323846);
#endif

static const float  TWOPI (M_PI*2.0);
static const float  FOURPI (M_PI*4.0);
static const float  KDegreeToRadian (M_PI/180.0);

#define USE_OPENMP
#define USE_FP_TEXTURES
