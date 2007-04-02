/*-----------------------------------------------------------------//
// April 10, 2005                                                  //
//                                Copyright (C) 2005  Justin Walsh //
//                                                                 //
//  This code is Released Under the GNU GPL                        //
//       http://www.gnu.org/copyleft/gpl.html                      //
//                                                                 //
//  Shout out to GDNet+ member: Ranger_One                         //
//-----------------------------------------------------------------*/

#ifndef CObjLoader_H
#define CObjLoader_H

#include <fstream>
#include <string>
#include <sstream>
#include "CMesh.h"
#include "TColorRGBA.h"

//STRUCTURES
struct ObjVertex
	{
	float X, Y, Z;
	};

struct ObjTexCoord
	{
	float U, V;
	};

struct ObjTriangle
	{
	int VertexIndex[3];
	int NormalIndex[3];
	int TexCoordIndex[3];
	};


//MODEL CLASS
class ObjModel 
	{
	public:

		ObjModel();
		~ObjModel();

		ObjModel(const ObjModel& aCopy);

	public:
		std::string iName;
		int iNumVertex; 
		int iNumNormal;
		int iNumTexCoord;
		int iNumTriangle;

		std::vector<ObjVertex>	 iVertexArray;
		std::vector<ObjVertex>	 iNormalArray;
		std::vector<ObjTexCoord> iTexCoordArray;
		std::vector<ObjTriangle> iTriangleArray;
		TColorRGBA				 iObjectColor;
	};

//LOADER CLASS
class CObjLoader  
	{
	public:
		CObjLoader();
		~CObjLoader();

		int LoadObj( std::string aFile );
		CMesh*	GetMesh(  int aIndex, float aScale );
		void FreeObj();

	protected:
		string		iFileName;
		ObjModel*	iObj;

		std::vector<ObjModel*> iMeshes;
		int ReadData();
	};

#endif