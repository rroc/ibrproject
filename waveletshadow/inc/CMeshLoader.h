#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "CMesh.h"

//CONSTANTS
static const int K3dsStringSize(64);

//STRUCTURES
struct T3dsChunk
	{
	unsigned short	iId;
	unsigned long	iLength;
	};

struct T3dsMaterial
	{
	char     iName[32];
	float    iAmbient[4];
	float    iDiffuse[4];
	float    iSpecular[4];
	float    iEmission[4];
	float    iShininess;
	char     iTexture[32];
	};

struct T3dsGroup
	{
	char     iName[32];
	int      iMat;
	int      iStart;
	int      iSize;
	long*	 iTris;
	};

struct T3dsTriangle
	{
	long     iMat;
	long     iVerts[3];
	long     iIndex;
	long     iSmooth;
	};

struct T3dsMesh
	{
	char           iName[32];
	int            iVertCount;
	int            iTexCoordCount;
	int            iTriCount;
	int            iGroupCount;
	
	float          (*iVertices)[3];
	float          (*iNormals)[3];
	float          (*iTexCoords)[2];
	float          (*iTangentSpace)[9];
	long           (*iTriangles)[3];
	T3dsGroup	   *iGroups;
	long           *iSmooth;
	
	float          iAxis[3][3];
	float          iPosition[3];
	float          iMin[3];
	float          iMax[3];
	float          iCenter[3];	
	};

struct T3dsInfo
	{
	int            iMaterialCount;
	int            iMeshCount;
	int            iVertCount;
	int            iTriCount;
	T3dsMaterial  *iMaterials;
	T3dsMesh      *iMeshes;
	float          iMin[3];
	float          iMax[3];
	float          iCenter[3];
	};
	
class CMeshLoader
	{
	//PUBLIC FUNCTIONS
	public:
		CMeshLoader(void);
		~CMeshLoader(void);

		int		Load3Ds( char* aFilename, const float aScale );
		CMesh*	GetMesh( int aIndex );

		void Free3dsData();


	//PRIVATE FUNCTIONS
	private:
		T3dsChunk ReadChunk();

		void ReadString(char* aString);
		
		void ReadChunkArray(long aLength,  void (CMeshLoader::*aCallback)(T3dsChunk*) );
		void ReadChunkArray(long aLength,  void (CMeshLoader::*aCallback)(T3dsChunk*, void*), void* aData);

		void InspectChunkArray(long aLength, void (CMeshLoader::*aCallback)(T3dsChunk*) );
		void InspectChunkArray(long aLength, void (CMeshLoader::*aCallback)(T3dsChunk*, void*), void* aData);

		void ReadMain3ds(T3dsChunk* aChunk );
		void InspectEdit3ds(T3dsChunk* aChunk );
		void InspectEditObject(T3dsChunk* aChunk );
		void ReadEdit3ds(T3dsChunk* aChunk );
		void ReadEditObject(T3dsChunk* aChunk );
		void ReadTriangleObject(T3dsChunk* aChunk, void* output);
		void ReadVertexList(T3dsMesh* output);
		void ReadFaceList(T3dsMesh* output, long aLength);
		void InspectFaceSubs(T3dsChunk* aChunk, void* output);
		void ReadFaceSubs(T3dsChunk* aChunk, void* output);
		void ReadTextureCoordinates(T3dsMesh* output);
		void ReadLocalAxis(T3dsMesh* output);
		void ReadMaterialGroup(T3dsGroup* aGroup);
		void ReadMaterial(T3dsChunk* aChunk, void* aMaterial);
		void ReadTexture(T3dsChunk* aChunk, void* aMaterial);
		void ReadColorChunk(T3dsChunk* aChunk, void* aColor);
		void ReadPercentageChunk(T3dsChunk* aChunk, void* aValue);

		void RemoveDegenerates(T3dsMesh* aMesh);
		void SortTriangles(T3dsMesh* aData);
		int  SortByMaterial(T3dsTriangle* a, T3dsTriangle* b);

		void Calculate3dsNormals( T3dsInfo* aData );
		void CalculateMeshNormals( T3dsMesh* aMesh );

//		void Calculate3dsTangentSpace( T3dsMesh* aMesh );
		void Calculate3dsBoundingBox( T3dsInfo* aData );
		void CalculateMesh3dsBoundingBox( T3dsMesh* aMesh );

	//PRIVATE DATA
	private:
		float		iScale;
		
		T3dsInfo*	iDataInfo;
		char		iFileStringData[ K3dsStringSize ];
		FILE*		iFile;		
	};
