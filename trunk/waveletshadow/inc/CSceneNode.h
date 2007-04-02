#ifndef CSCENE_NODE_H
#define CSCENE_NODE_H


#include "basic.h"

/** \brief Class Scene Node
*
*/
class CSceneNode
	{
	public:

		enum TNodeType
			{
			EBaseNode=0,
			EMeshNode,
			ERotationNode,
			ETranslationNode,
			//last value
			EUndefined
			};
	public:
		CSceneNode()
			: iNodeType(EBaseNode)
			{};
		~CSceneNode()
			{
			for(int i=0,j=static_cast<int>(iChildren.size());i<j;i++)
				delete iChildren.at(i);
			iChildren.clear();
			};
		CSceneNode*     getChild(int aIndex)const 
			{
			return iChildren.at(aIndex); 
			};
		int getChildAmount()const 
			{
			return static_cast<int>(iChildren.size()); 
			};
		TNodeType	getNodeType()const
			{
			return iNodeType;
			}
		CSceneNode*	addChild(CSceneNode* aNode)
			{
			iChildren.push_back( aNode );
			return aNode;
			};
	protected:
		std::vector<CSceneNode*> iChildren;
		TNodeType       iNodeType;

		//Later maybe: enable/disable children
	};

#endif
