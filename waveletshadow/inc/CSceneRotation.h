#ifndef CSCENE_ROTATION_H
#define CSCENE_ROTATION_H


#include "basic.h"
#include "TVector3.h"
#include "CMesh.h"


/** \brief Class Scene Node
*
*/
class CSceneRotation : public CSceneNode
	{
	public:
		CSceneRotation()
			: CSceneNode()
			,iAngles()
			{
			iNodeType=ERotationNode;
			};

		CSceneRotation( const TVector3& aAngles )
			: CSceneNode()
			,iAngles(aAngles)
			{
			iNodeType=ERotationNode;
			};

		void setAngleX(float aAngle){ iAngles.iX=aAngle; };
		void setAngleY(float aAngle){ iAngles.iY=aAngle; };
		void setAngleZ(float aAngle){ iAngles.iZ=aAngle; };

		void SetRotation(const TVector3& aAngles)
			{
			iAngles = aAngles;
			}
		void Rotate(const TVector3& aAngles)
			{
			iAngles += aAngles;
			}

	public:
		TVector3 iAngles;
	};
#endif
