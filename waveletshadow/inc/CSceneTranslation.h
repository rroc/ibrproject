#ifndef CSCENE_TRANSLATION_H
#define CSCENE_TRANSLATION_H


#include "basic.h"
#include "TVector3.h"


/** \brief Class Scene Node
*
*/
class CSceneTranslation : public CSceneNode
	{
	public:
		CSceneTranslation()
			: CSceneNode()
			{
			iNodeType=ETranslationNode;
			};
		CSceneTranslation( TVector3 aVector )
			: CSceneNode()
			, iX(aVector.iX)
			, iY(aVector.iY)
			, iZ(aVector.iZ)
			{
			iNodeType=ETranslationNode;
			};
		void setX(float aX){ iX=aX; };
		void setY(float aY){ iY=aY; };
		void setZ(float aZ){ iZ=aZ; };
	public:
		float iX, iY, iZ;
	};

#endif