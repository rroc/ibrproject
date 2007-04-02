#ifndef TCOLORRGBA_H
#define TCOLORRGBA_H

#include "basic.h"

/** \brief RGBA color class
*
*/
class TColorRGBA
	{
	public:
		// Constructors
		TColorRGBA()
			:iR(1), iG(1), iB(1), iA(1){};
		TColorRGBA(GLfloat _r , GLfloat _g, GLfloat _b)
			:iR(_r), iG(_g), iB(_b), iA(1){};
		TColorRGBA(GLfloat _r , GLfloat _g, GLfloat _b, GLfloat _a)
			:iR(_r), iG(_g), iB(_b), iA(_a){};

		// Init to gray shade
		TColorRGBA(float _g)
			:iR(_g), iG(_g), iB(_g), iA(1){};
		//Copy constructor
		TColorRGBA(const TColorRGBA& _c)
			:iR(_c.iR), iG(_c.iG), iB(_c.iB), iA(1){};

		//destructor
		~TColorRGBA(void){};

		inline
		TColorRGBA randomColors()
			{
			//TColorRGBA c;
			this->iR = float(rand()%256)/255;
			this->iG = float(rand()%256)/255;
			this->iB = float(rand()%256)/255;
			this->iA = 1.0f;
			return *this;
			};

		inline
		TColorRGBA randomColors( int aSeed )
			{
			srand( aSeed );
			return randomColors();
			};

		//PUBLIC OPERATORS
		TColorRGBA operator-(){ return TColorRGBA(-iR, -iG, -iB); };
		TColorRGBA& operator+=( GLfloat aV ){ iR+=aV; iG+=aV; iB+=aV; return *this; };
		TColorRGBA& operator+=( const TColorRGBA& aV ){ iR+=aV.iR; iG+=aV.iG; iB+=aV.iB; return *this; };
		TColorRGBA& operator-=( const TColorRGBA& aV ){ iR-=aV.iR; iG-=aV.iG; iB-=aV.iB; return *this; };
		TColorRGBA& operator-=( GLfloat aV ){ iR-=aV; iG-=aV; iB-=aV; return *this; };
		TColorRGBA& operator*=( GLfloat aVal ){ iR*=aVal; iG*=aVal; iB*=aVal; return *this; };
		TColorRGBA& operator*=( TColorRGBA aVal ){ iR*=aVal.iR; iG*=aVal.iG; iB*=aVal.iB; return *this; };
		TColorRGBA& operator/=( GLfloat aVal ){ iR/=aVal; iG/=aVal; iB/=aVal; return *this; };

		TColorRGBA operator+( const TColorRGBA& aV2 ) { return TColorRGBA(*this)+=aV2; };
		TColorRGBA operator+( GLfloat aV2 ) { return TColorRGBA(*this)+=aV2; };
		TColorRGBA operator-( const TColorRGBA& aV2 ) { return TColorRGBA(*this)-=aV2; };
		TColorRGBA operator-( GLfloat aV2 ) { return TColorRGBA(*this)-=aV2; };
		TColorRGBA operator*( GLfloat aVal ) { return TColorRGBA(*this)*=aVal; };
		TColorRGBA operator*( TColorRGBA aVal ) { return TColorRGBA(*this)*=aVal; };
		TColorRGBA operator/( GLfloat aVal ) { return TColorRGBA(*this)/=aVal; };
		bool operator==( const TColorRGBA& aV ) { return ( (iR==aV.iR)&&(iG==aV.iG)&&(iB==aV.iB) )? true:false;; };
		bool operator!=( const TColorRGBA& aV ) { return !(*this==aV); };

	public:
		GLfloat iR, iG, iB, iA;
	};


#endif


