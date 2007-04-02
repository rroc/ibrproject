#include "CMeshLoader.h"
#include <string>
#include "index_t.h"

//========//
// macros //
//========//
#define VEC_Add(a, b, c)			((c)[0] = (a)[0] + (b)[0], (c)[1] = (a)[1] + (b)[1], (c)[2] = (a)[2] + (b)[2])
#define VEC_AddTo(a, b)				((a)[0] += (b)[0], (a)[1] += (b)[1], (a)[2] += (b)[2])					
#define VEC_AddScalar(a, b, c)	((c)[0] = (a)[0] + (b), (c)[1] = (a)[1] + (b), (c)[2] = (a)[2] + (b))
#define VEC_AddScalarTo(a, b)		((a)[0] += (b), (a)[1] += (b), (a)[2] += (b))					
#define VEC_Copy(a, b)				((b)[0] = (a)[0], (b)[1] += (a)[1], (b)[2] += (a)[2])					
#define VEC_Cross(a, b, c)			((c)[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1], (c)[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2], (c)[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0])
#define VEC_Divide(a, b, c)		((c)[0] = (a)[0] / (b)[0], (c)[1] = (a)[1] / (b)[1], (c)[2] = (a)[2] / (b)[2])
#define VEC_DivideBy(a, b)			((a)[0] /= (b)[0], (a)[1] /= (b)[1], (a)[2] /= (b)[2])
#define VEC_DivideByScalar(v, s)	((v)[0] /= s, (v)[1] /= s, (v)[2] /= s)	
#define VEC_DivideScalar(v, s, o)	((o)[0] = (v)[0] / s, (o)[1] = (v)[1] / s, (o)[2] = v[2] / s)				
#define VEC_DotProduct(a, b)		((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define VEC_Dot4(a, b)		      ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2] + (a)[3] * (b)[3])
#define VEC_MidPoint(a, b, c)		((c)[0] = (b)[0] + ((a)[0] - (b)[0]) * 0.5f, (c)[1] = (b)[1] + ((a)[1] - (b)[1]) * 0.5f, (c)[2] = (b)[2] + ((a)[2] - (b)[2]) * 0.5f)
#define VEC_Mult(a, b, c)			((c)[0] = (a)[0] * (b)[0], (c)[1] = (a)[1] * (b)[1], (c)[2] = (a)[2] * (b)[2])
#define VEC_MultBy(a, b)			((a)[0] *= (b)[0], (a)[1] *= (b)[1], (a)[2] *= (b)[2])
#define VEC_MultByScalar(v, s)	((v)[0] *= s, (v)[1] *= s, (v)[2] *= s)	
#define VEC_MultScalar(v, s, o)	((o)[0] = (v)[0] * s, (o)[1] = (v)[1] * s, (o)[2] = v[2] * s)				
#define VEC_Negate(a, b)			((b)[0] = -(a)[0], (b)[1] = -(a)[1], (b)[2] = -(a)[2])
#define VEC_Scale(v, s, o)			((o)[0] = (v)[0] * s, (o)[1] = (v)[1] * s, (o)[2] = v[2] * s)				
#define VEC_ScaleBy(v, s)			((v)[0] *= s, (v)[1] *= s, (v)[2] *= s)	
#define VEC_Set(v, x, y, z)		((v)[0] = x, (v)[1] = y, (v)[2] = z)
#define VEC_Subtract(a, b, c)		((c)[0] = (a)[0] - (b)[0], (c)[1] = (a)[1] - (b)[1], (c)[2] = (a)[2] - (b)[2])
#define VEC_SubtractFrom(a, b)	((a)[0] -= (b)[0], (a)[1] -= (b)[1], (a)[2] -= (b)[2])
#define VEC_Magnitude(v)			(sqrt((v)[0] * (v)[0] + (v)[1] * (v)[1] + (v)[2] * (v)[2]))
#define VEC_Normalize(v)         { double __mag = 1.0 / VEC_Magnitude(v); \
                                   (v)[0] *= __mag; (v)[1] *= __mag; (v)[2] *= __mag; }
#define VEC_Normalizef(v)        { float __mag = 1.0f / (float)VEC_Magnitude(v); \
                                   (v)[0] *= __mag; (v)[1] *= __mag; (v)[2] *= __mag; }


typedef int (*TSortFunc)(const void*, const void*);

int CompareLong(long* a, long* b)
	{
	if (*a > *b)
		{
		return 1;
		}
	if (*a < *b)
		{
		return -1;
		}
	return 0;  
	}

//FOR NORMAL CALCULATION
//---------------------------
int ComparePosition(float *a, float *b)
	{
	int i;

	for (i = 0; i < 3; i++)
		{
		if (a[i] > b[i])
			{
			return 1;
			}
		if (a[i] < b[i])
			{
			return -1;
			}
		}

	return 0;
	}

//------------------------------------------


#define FREE(p)   if (p){ free(p); p = NULL; }

CMeshLoader::CMeshLoader(void)
	{
	}

CMeshLoader::~CMeshLoader(void)
	{
	Free3dsData();
	if(NULL != iDataInfo )
		delete iDataInfo;
	}





void  CMeshLoader::Calculate3dsBoundingBox( T3dsInfo* aData )
	{
	for (int i = 0; i < aData->iMeshCount; i++)
		{
		CalculateMesh3dsBoundingBox(&aData->iMeshes[i]);

		if (i == 0)
			{
			memcpy(aData->iMax, aData->iMeshes[i].iMax, sizeof(float) * 3);
			memcpy(aData->iMin, aData->iMeshes[i].iMin, sizeof(float) * 3);
			}
		else
			{
			for (int j = 0; j < 3; j++)
				{
				if (aData->iMeshes[i].iMax[j] > aData->iMax[j])
					{
					aData->iMax[j] = aData->iMeshes[i].iMax[j];
					}
				if (aData->iMeshes[i].iMin[j] < aData->iMin[j])
					{
					aData->iMin[j] = aData->iMeshes[i].iMin[j];
					}
				}
			}
		}
	aData->iCenter[0] = aData->iMin[0] + (aData->iMax[0] - aData->iMin[0]) * 0.5f;
	aData->iCenter[1] = aData->iMin[1] + (aData->iMax[1] - aData->iMin[1]) * 0.5f;
	aData->iCenter[2] = aData->iMin[2] + (aData->iMax[2] - aData->iMin[2]) * 0.5f;
	}

void  CMeshLoader::CalculateMesh3dsBoundingBox( T3dsMesh* aMesh )
	{
	for( int i = 0; i < aMesh->iVertCount; i++ )
		{
		if( i == 0 )
			{
			memcpy(aMesh->iMax, aMesh->iVertices[i], sizeof(float) * 3);
			memcpy(aMesh->iMin, aMesh->iVertices[i], sizeof(float) * 3);
			}
		else
			{
			for(int j = 0; j < 3; j++)
				{
				if (aMesh->iVertices[i][j] > aMesh->iMax[j])
					{
					aMesh->iMax[j] = aMesh->iVertices[i][j];
					}
				if (aMesh->iVertices[i][j] < aMesh->iMin[j])
					{
					aMesh->iMin[j] = aMesh->iVertices[i][j];
					}
				}
			}
		}

	aMesh->iCenter[0] = aMesh->iMin[0] + (aMesh->iMax[0] - aMesh->iMin[0]) * 0.5f;
	aMesh->iCenter[1] = aMesh->iMin[1] + (aMesh->iMax[1] - aMesh->iMin[1]) * 0.5f;
	aMesh->iCenter[2] = aMesh->iMin[2] + (aMesh->iMax[2] - aMesh->iMin[2]) * 0.5f;
	}


void CMeshLoader::Calculate3dsNormals( T3dsInfo* model)
	{
	int i;

	for (i = 0; i < model->iMeshCount; i++)
		{
		CalculateMeshNormals(&model->iMeshes[i]);
		}
	}


//====================================================================//
// compute normals for this mesh taking smoothing groups into account //
//====================================================================//
void CMeshLoader::CalculateMeshNormals( T3dsMesh* aMesh )
	{
	int	      i, j, k;
	int         normCount;
	long        (*triRefs)[50];
	float       (*faceNorms)[3];
	float		   v1[3];
	float		   v2[3];

	//======================//
	// allocate the normals //
	//======================//
	aMesh->iNormals = (float(*)[3])malloc(sizeof(float) * 3 * aMesh->iTriCount * 3);
	if (aMesh->iNormals == NULL)
		{
		return;
		}
	memset(aMesh->iNormals, 0, sizeof(float) * 3 * aMesh->iTriCount * 3);

	if (aMesh->iSmooth == NULL)
		{
		for (i = 0; i < aMesh->iTriCount; i++)
			{
			VEC_Subtract(aMesh->iVertices[aMesh->iTriangles[i][1]], aMesh->iVertices[aMesh->iTriangles[i][0]], v1);
			VEC_Subtract(aMesh->iVertices[aMesh->iTriangles[i][2]], aMesh->iVertices[aMesh->iTriangles[i][0]], v2);
			VEC_Cross(v1, v2, aMesh->iNormals[i * 3]);
			VEC_Normalizef(aMesh->iNormals[i * 3]);
			memcpy(aMesh->iNormals[i * 3 + 1], aMesh->iNormals[i * 3], sizeof(float) * 3);
			memcpy(aMesh->iNormals[i * 3 + 2], aMesh->iNormals[i * 3], sizeof(float) * 3);
			}
		return;
		}


	index_t	vertIndex;
	int		vert = 0;
	int		result = 0;

	//============================================//
	// index the vertices based on position alone //
	//============================================//
	indexArray( &vertIndex, (char*)aMesh->iVertices, sizeof(float) * 3, aMesh->iVertCount, (TSortFunc)ComparePosition );

	//============================================================================//
	// build a table that links each vertex to all triangles that use said vertex // 
	//============================================================================//
	triRefs = (long(*)[50])malloc(sizeof(long) * 50 * vertIndex.count);
	if (triRefs == NULL)
		{
		return;
		}
	memset(triRefs, 0, sizeof(unsigned long) * 50 * vertIndex.count);		

	for (i = 0; i < aMesh->iTriCount; i++)
		{
		for (j = 0; j < 3; j++)
			{
			vert = indexFind(&vertIndex, aMesh->iVertices[aMesh->iTriangles[i][j]], &result);
			if (triRefs[vert][0] < 48)
				{
				triRefs[vert][0]++;
				triRefs[vert][triRefs[vert][0]] = i;
				}
			}
		}

	//========================================//
	// allocate a buffer for the flat normals //
	//========================================//
	faceNorms = (float(*)[3])malloc(sizeof(float) * 3 * aMesh->iTriCount);
	if ((faceNorms != NULL) && (triRefs != NULL))
		{
		memset(faceNorms, 0, sizeof(float) * 3 * aMesh->iTriCount);

		//==============================================//
		// go through every triangle to find its normal //
		//==============================================//
		for (i = 0; i < aMesh->iTriCount; i++)
			{
			VEC_Subtract(aMesh->iVertices[aMesh->iTriangles[i][1]], aMesh->iVertices[aMesh->iTriangles[i][0]], v1);
			VEC_Subtract(aMesh->iVertices[aMesh->iTriangles[i][2]], aMesh->iVertices[aMesh->iTriangles[i][0]], v2);
			VEC_Cross(v1, v2, faceNorms[i]);
			VEC_Normalizef(faceNorms[i]);
			}

		for (i = 0; i < aMesh->iTriCount; i++)
			{
			for (j = 0; j < 3; j++)
				{
				vert = indexFind(&vertIndex, aMesh->iVertices[aMesh->iTriangles[i][j]], &result);

				normCount = 0;
				for (k = 1; k <= triRefs[vert][0]; k++)
					{
					if (aMesh->iSmooth[i] == aMesh->iSmooth[triRefs[vert][k]])
						{
						VEC_AddTo(aMesh->iNormals[i * 3 + j], faceNorms[triRefs[vert][k]]);
						normCount++;
						}
					}

				VEC_DivideByScalar(aMesh->iNormals[i * 3 + j], (float)normCount);
				VEC_Normalizef(aMesh->iNormals[i * 3 + j]);
				}
			}
		}

	indexFree(&vertIndex);

	//==========================================//
	// free up the local buffers that were used //
	//==========================================//
	if (triRefs)
		{
		free(triRefs);
		}

	if (faceNorms)
		{
		free(faceNorms);
		}
	}











//
// PSP SDK does not support cin so the c-style loading is used
//
int CMeshLoader::Load3Ds( char* aFilename, const float aScale )
	{
	iScale = aScale;
	iDataInfo = new T3dsInfo();

	//read file in binary form
	iFile = fopen( aFilename, "rb");

	//FILE OPENED OK
	if( iFile )
		{
		printf("  File Found. )\n" );

		T3dsChunk chunk;
		//		unsigned char byte[4]; 

		fseek( iFile, 0, SEEK_SET);
		chunk = ReadChunk();
		//fread( &chunk, sizeof( unsigned char ), 2, iFile );
		//fread( &byte, sizeof( unsigned char ), 4, iFile );
		//chunk.iLength = *(reinterpret_cast<unsigned long*>(&byte) );

		//		printf("  chunk: 0x%x, %d (%d)\n", chunk.iId, chunk.iLength, sizeof( T3dsChunk ) );
		//printf("  chunk: %x %x %x %x %x %x (%d)\n", byte[0], byte[1],byte[2], byte[3],byte[4], byte[5], sizeof( unsigned char  ) );
		/*		for(int i=0;i<15;i++)
		{
		//			fread( &chunk, sizeof( sizeof( unsigned short )+sizeof( unsigned long ) ), 1, iFile );
		fread( &chunk, sizeof( unsigned char ), 2, iFile );
		fread( &byte, sizeof( unsigned char ), 4, iFile );
		chunk.iLength = *(reinterpret_cast<unsigned long*>(&byte) );

		//			printf("  chunk: 0x%x, %d (%d)\n", chunk.iId, chunk.iLength, sizeof( T3dsChunk ) );
		//printf("  chunk: %x %x %x %x %x %x (%d)\n", byte[0], byte[1],byte[2], byte[3],byte[4], byte[5], sizeof( unsigned char  ) );
		}
		return 1;
		*/
		//main 3ds (the main)
		if( 0x4d4d == chunk.iId )
			{
			printf("  3ds file\n");
			ReadChunkArray( chunk.iLength - 6,  &CMeshLoader::ReadMain3ds );
			}
		//all done, close the file
		fclose( iFile );
		}
	//file not found... exit gracefully
	else
		{
		fclose( iFile );
		printf("  Mesh File: \"%s\" NOT Found. )\n", aFilename );
		return(-1);
		}

	printf("  Looking inside the 3ds data...\n");

	//Prepare object
	for (int i = 0; i < iDataInfo->iMeshCount; i++)
		{
		RemoveDegenerates( &iDataInfo->iMeshes[i] );
		SortTriangles( &iDataInfo->iMeshes[i] );
		}

	iDataInfo->iVertCount = 0;
	iDataInfo->iTriCount = 0;
	for (int i = 0; i < iDataInfo->iMeshCount; i++)
		{
		iDataInfo->iVertCount += iDataInfo->iMeshes[i].iVertCount;
		iDataInfo->iTriCount  += iDataInfo->iMeshes[i].iTriCount;
		}
	printf("  Proceed with normals.");

	Calculate3dsBoundingBox( iDataInfo );
	Calculate3dsNormals( iDataInfo );
	//	Calculate3dsTangentSpace(output);

	printf("  Data Ready. (Meshes:%d, Vertices:%d, Triangles:%d)\n", iDataInfo->iMeshCount, iDataInfo->iVertCount, iDataInfo->iTriCount);
	return iDataInfo->iMeshCount;
	}


//Extract the 3ds data to mesh
CMesh* CMeshLoader::GetMesh( int aIndex )
	{
	if( iDataInfo->iMeshCount <= aIndex )
		{
		printf("  Mesh out of bounds:(Max: %d, Wanted: %d)\n", iDataInfo->iMeshCount, aIndex );
		return NULL;
		}
	CMesh* mesh = new CMesh();
	mesh->iName = iDataInfo->iMeshes[aIndex].iName;
	mesh->iMin.iX = iDataInfo->iMeshes[aIndex].iMin[0];
	mesh->iMax.iX = iDataInfo->iMeshes[aIndex].iMax[0];
	mesh->iMin.iY = iDataInfo->iMeshes[aIndex].iMin[1];
	mesh->iMax.iY = iDataInfo->iMeshes[aIndex].iMax[1];
	mesh->iMin.iZ = iDataInfo->iMeshes[aIndex].iMin[2];
	mesh->iMax.iZ = iDataInfo->iMeshes[aIndex].iMax[2];

	printf("MESH:\"%s\"(%d),MAT:\"%s\"(%d),\tVRT:      ", mesh->iName.c_str(), aIndex, iDataInfo->iMaterials[iDataInfo->iMeshes[aIndex].iGroups[0].iMat].iName, iDataInfo->iMeshes[aIndex].iGroups[0].iMat );

	mesh->iMaterialColor = TColorRGBA( 
				  iDataInfo->iMaterials[iDataInfo->iMeshes[aIndex].iGroups[0].iMat].iDiffuse[0]
				, iDataInfo->iMaterials[iDataInfo->iMeshes[aIndex].iGroups[0].iMat].iDiffuse[1]
				, iDataInfo->iMaterials[iDataInfo->iMeshes[aIndex].iGroups[0].iMat].iDiffuse[2]
				, iDataInfo->iMaterials[iDataInfo->iMeshes[aIndex].iGroups[0].iMat].iDiffuse[3]
				);
//	printf("  Mesh Color: (%f, %f, %f, %f)\n", mesh->iMaterialColor.iR, mesh->iMaterialColor.iG, mesh->iMaterialColor.iB, mesh->iMaterialColor.iA );

	//COPY VERTICES
//	printf("  Copy vertices & normals:             ");
	TVector3 vectorData;
	for( int i=0, j=iDataInfo->iMeshes[aIndex].iVertCount; i<j; i++)
		{
		printf("\b\b\b\b\b\b%6d", i+1 );
		//Texture coordinates
		//container.iTextureCoords.set( 
		//	( *iDataInfo->iMeshes[aIndex].iTexCoords+(i*2) )[0]
		//, ( *iDataInfo->iMeshes[aIndex].iTexCoords+(i*2) )[1]
		//);
		//Vertices
		vectorData.set(  ( *iDataInfo->iMeshes[aIndex].iVertices+(i*3) )[0]
						,( *iDataInfo->iMeshes[aIndex].iVertices+(i*3) )[1]
						,( *iDataInfo->iMeshes[aIndex].iVertices+(i*3) )[2]
						);
		mesh->iVertices.push_back( vectorData );

		//normals
		vectorData.set(   ( *iDataInfo->iMeshes[aIndex].iNormals+(i*3) )[0]
						, ( *iDataInfo->iMeshes[aIndex].iNormals+(i*3) )[1]
						, ( *iDataInfo->iMeshes[aIndex].iNormals+(i*3) )[2]
						);
		mesh->iVertexNormals.push_back( vectorData );
		}

	//COPY TRIANGLES
	printf(",TRI:        ");
	for( int i=0, j=iDataInfo->iMeshes[aIndex].iTriCount; i<j; i++ )
		{
		printf("\b\b\b\b\b\b\b\b%8d", i+1 );
		mesh->iTriangles.push_back( TTriangle(
			  ( *iDataInfo->iMeshes[aIndex].iTriangles+(i*3) )[0] 
			, ( *iDataInfo->iMeshes[aIndex].iTriangles+(i*3) )[1]
			, ( *iDataInfo->iMeshes[aIndex].iTriangles+(i*3) )[2] )
			);
		}
	printf(".\n");
	return mesh;	
	}

void CMeshLoader::Free3dsData()
	{
	int i, j;

	printf("  Free 3ds\n");
	if (iDataInfo->iMeshes)
		{
		for (i = 0; i < iDataInfo->iMeshCount; i++)
			{
			FREE(iDataInfo->iMeshes[i].iVertices);
			FREE(iDataInfo->iMeshes[i].iTriangles);
			FREE(iDataInfo->iMeshes[i].iNormals);
			FREE(iDataInfo->iMeshes[i].iTangentSpace);
			FREE(iDataInfo->iMeshes[i].iTexCoords);
			FREE(iDataInfo->iMeshes[i].iSmooth);

			if (iDataInfo->iMeshes[i].iGroups)
				{
				for (j = 0; j < iDataInfo->iMeshes[i].iGroupCount; j++)
					{
					FREE(iDataInfo->iMeshes[i].iGroups[j].iTris);
					}
				FREE(iDataInfo->iMeshes[i].iGroups);
				}
			}
		free(iDataInfo->iMeshes);
		}

	FREE(iDataInfo->iMaterials);

	memset(iDataInfo, 0, sizeof(T3dsInfo));
	printf("  Mesh Freed.\n");
	}



void CMeshLoader::RemoveDegenerates(T3dsMesh* aMesh)
	{
	int i;
	int j;
	int k;
	int l;
	long* found;

	for (i = 0; i < aMesh->iTriCount; i++)
		{
		if ((aMesh->iTriangles[i][0] >= aMesh->iVertCount) ||
			(aMesh->iTriangles[i][1] >= aMesh->iVertCount) ||
			(aMesh->iTriangles[i][2] >= aMesh->iVertCount))
			{
			i = i;
			}
		}

	for (i = 0;i < aMesh->iGroupCount; i++)
		{
		qsort(aMesh->iGroups[i].iTris, aMesh->iGroups[i].iSize, sizeof(long), (TSortFunc)CompareLong);
		}

	for (i = 0; i < aMesh->iTriCount; i++)
		{
		if (((aMesh->iTriangles[i][0] == aMesh->iTriangles[i][1]) ||
			(aMesh->iTriangles[i][0] == aMesh->iTriangles[i][2]) ||
			(aMesh->iTriangles[i][1] == aMesh->iTriangles[i][2])) ||

			((aMesh->iVertices[aMesh->iTriangles[i][0]][0] == aMesh->iVertices[aMesh->iTriangles[i][1]][0]) &&
			(aMesh->iVertices[aMesh->iTriangles[i][0]][1] == aMesh->iVertices[aMesh->iTriangles[i][1]][1]) &&
			(aMesh->iVertices[aMesh->iTriangles[i][0]][2] == aMesh->iVertices[aMesh->iTriangles[i][1]][2])) ||

			((aMesh->iVertices[aMesh->iTriangles[i][0]][0] == aMesh->iVertices[aMesh->iTriangles[i][2]][0]) &&
			(aMesh->iVertices[aMesh->iTriangles[i][0]][1] == aMesh->iVertices[aMesh->iTriangles[i][2]][1]) &&
			(aMesh->iVertices[aMesh->iTriangles[i][0]][2] == aMesh->iVertices[aMesh->iTriangles[i][2]][2])) ||

			((aMesh->iVertices[aMesh->iTriangles[i][1]][0] == aMesh->iVertices[aMesh->iTriangles[i][2]][0]) &&
			(aMesh->iVertices[aMesh->iTriangles[i][1]][1] == aMesh->iVertices[aMesh->iTriangles[i][2]][1]) &&
			(aMesh->iVertices[aMesh->iTriangles[i][1]][2] == aMesh->iVertices[aMesh->iTriangles[i][2]][2])))
			{
			if (i != (aMesh->iTriCount - 1))
				{
				memmove(&aMesh->iTriangles[i], &aMesh->iTriangles[i+1], sizeof(long) * 3 * (aMesh->iTriCount - i - 1));
				}

			for (j = 0; j < aMesh->iGroupCount; j++)
				{
				found = (long*)bsearch(&i, aMesh->iGroups[j].iTris, aMesh->iGroups[j].iSize, sizeof(long), (TSortFunc)CompareLong);

				if (found != NULL)
					{
					k = ((int)found - (int)aMesh->iGroups[j].iTris) / sizeof(long);

					if (k < aMesh->iGroups[j].iSize - 1)
						{
						memmove(&aMesh->iGroups[j].iTris[k], &aMesh->iGroups[j].iTris[k+1], sizeof(long) * (aMesh->iGroups[j].iSize - k - 1));
						}
					aMesh->iGroups[j].iSize--;

					for (l = k; l < aMesh->iGroups[j].iSize; l++)
						{
						aMesh->iGroups[j].iTris[l]--;
						}
					}
				}
			aMesh->iTriCount--;
			i--;
			}
		}
	}





//reads an ASCII string from 3ds file
void CMeshLoader::ReadString(char* aString)
	{
	//	printf("  Reading string\n");
	int i = 0;
	do
		{
		fread(&aString[i], sizeof(char), 1, iFile);
		} while ( aString[i++] != '\0');
	}

//reads a chunk(of defined size) from 3ds file
void CMeshLoader::ReadChunkArray(long aLength,  void (CMeshLoader::*aCallback)(T3dsChunk*) )
	{
	T3dsChunk	chunk;
	long        start; 
	long        pos = 0;

	do
		{
		// store the position of the this chunk
		start = ftell(iFile);

		// read in the sub chunk //
		//fread(&chunk, sizeof(T3dsChunk), 1, iFile);
		chunk = ReadChunk();

		// Callback with subchunk id //

		//CMeshLoader meshLoader;
		//(meshLoader.*aCallback) ( &chunk );
		(this->*aCallback) ( &chunk );

		// set the position the next sub chunk //
		fseek(iFile, start + chunk.iLength, SEEK_SET);

		// account for this chunk in the position //
		pos += chunk.iLength;
		} while (pos < aLength);
	}

//reads a chunk(of defined size) from 3ds file
void CMeshLoader::ReadChunkArray(long aLength,  void (CMeshLoader::*aCallback)(T3dsChunk*, void*), void* aData)
	{
	T3dsChunk	chunk;
	long        start; 
	long        pos = 0;

	do
		{
		// store the position of the this chunk
		start = ftell(iFile);

		// read in the sub chunk //
		//fread(&chunk, sizeof(T3dsChunk), 1, iFile);
		chunk = ReadChunk();

		// Callback with subchunk id //
		//CMeshLoader meshLoader;
		//(meshLoader.*aCallback) (&chunk, aData);
		(this->*aCallback) (&chunk, aData);

		// set the position the next sub chunk //
		fseek(iFile, start + chunk.iLength, SEEK_SET);

		// account for this chunk in the position //
		pos += chunk.iLength;
		} while (pos < aLength);
	}





//evaluate the data that was read
void CMeshLoader::InspectChunkArray(long aLength,  void (CMeshLoader::*aCallback)(T3dsChunk*) )
	{
	T3dsChunk	chunk;
	long        start; 
	long        chunkStart;
	long        pos = 0;

	// record the position of the chunk //
	chunkStart = ftell(iFile);

	do
		{
		// store the position of the this sub chunk //
		start = ftell(iFile);

		// read in the sub chunk //
		//fread(&chunk, sizeof(T3dsChunk), 1, iFile);
		chunk = ReadChunk();

		// Callback with subchunk id //
		//CMeshLoader meshLoader;
		//(meshLoader.*aCallback) (&chunk);
		(this->*aCallback) (&chunk);

		// set the position the next sub chunk //
		fseek(iFile, start + chunk.iLength, SEEK_SET);

		// account for this chunk in the position //
		pos += chunk.iLength;
		} while (pos < aLength);

		// set the position to the start of this chunk //
		fseek(iFile, chunkStart, SEEK_SET);
	}

//evaluate the data that was read
void CMeshLoader::InspectChunkArray(long aLength,  void (CMeshLoader::*aCallback)(T3dsChunk*, void*), void* aData)
	{
	T3dsChunk	chunk;
	long        start; 
	long        chunkStart;
	long        pos = 0;

	// record the position of the chunk //
	chunkStart = ftell(iFile);

	do
		{
		// store the position of the this sub chunk //
		start = ftell(iFile);

		// read in the sub chunk //
		//fread(&chunk, sizeof(T3dsChunk), 1, iFile);
		chunk = ReadChunk();

		// Callback with subchunk id //
		//CMeshLoader meshLoader;
		//(meshLoader.*aCallback) (&chunk, aData);
		(this->*aCallback) (&chunk, aData);

		// set the position the next sub chunk //
		fseek(iFile, start + chunk.iLength, SEEK_SET);

		// account for this chunk in the position //
		pos += chunk.iLength;
		} while (pos < aLength);

		// set the position to the start of this chunk //
		fseek(iFile, chunkStart, SEEK_SET);
	}








//SPECIFIC CHUNK READERS AND INSPECTORS

void CMeshLoader::ReadMain3ds(T3dsChunk* aChunk)
	{
	int i, j, k;

	//	printf("  ReadMain3ds: 0x%x\n", aChunk->iId);

	switch (aChunk->iId)
		{
		case 0x3d3d: // Edit3ds
			InspectChunkArray(aChunk->iLength - 6, &CMeshLoader::InspectEdit3ds );

			if (iDataInfo->iMeshCount)
				{
				iDataInfo->iMeshes = (T3dsMesh*)malloc(sizeof(T3dsMesh) * iDataInfo->iMeshCount);
				memset(iDataInfo->iMeshes, 0, sizeof(T3dsMesh) * iDataInfo->iMeshCount);
				}

			if (iDataInfo->iMaterialCount)
				{
				iDataInfo->iMaterials = (T3dsMaterial*)malloc(sizeof(T3dsMaterial) * iDataInfo->iMaterialCount);
				memset(iDataInfo->iMaterials, 0, sizeof(T3dsMaterial) * iDataInfo->iMaterialCount);
				}

			iDataInfo->iMeshCount = 0;
			iDataInfo->iMaterialCount = 0;

			ReadChunkArray(aChunk->iLength - 6, &CMeshLoader::ReadEdit3ds);

			for (i = 0; i < iDataInfo->iMeshCount; i++)
				{
				for (j = 0; j < iDataInfo->iMeshes[i].iGroupCount; j++)
					{
					for (k = 0; k < iDataInfo->iMaterialCount; k++)
						{
						if (!strcmp(iDataInfo->iMeshes[i].iGroups[j].iName, iDataInfo->iMaterials[k].iName)) 
							{
							iDataInfo->iMeshes[i].iGroups[j].iMat = k;
							break;
							}
						}
					}
				}
			break;
		default:
			break;
		}
	}

//EDIT_3DS
void CMeshLoader::ReadEdit3ds(T3dsChunk* aChunk)
	{	
	//	printf("  ReadEdit3ds: 0x%x\n", aChunk->iId);
	switch (aChunk->iId)
		{
		case 0x4000: // EDIT_OBJECT
			//			printf("  edit obj\n");
			ReadString(iFileStringData);
			ReadChunkArray(aChunk->iLength - 6 + strlen(iFileStringData), &CMeshLoader::ReadEditObject);
			break;
		case 0xAFFF: // EDIT_MATERIAL
			//			printf("  material\n");
			ReadChunkArray(aChunk->iLength - 6, &CMeshLoader::ReadMaterial, static_cast<void*>(&iDataInfo->iMaterials[iDataInfo->iMaterialCount++]) );
			break;
		default:
			break;
		};
	}

void CMeshLoader::InspectEdit3ds(T3dsChunk* aChunk)
	{
	//	printf("  InspectEdit3ds: 0x%x\n", aChunk->iId);
	switch (aChunk->iId)
		{
		case 0xAFFF: // EDIT_MATERIAL
			//			printf("  material++\n");
			iDataInfo->iMaterialCount++;
			break;
		case 0x4000: // EDIT_OBJECT
			//			printf("  edit obj\n");
			ReadString(iFileStringData);
			InspectChunkArray(aChunk->iLength - 6 + strlen(iFileStringData), &CMeshLoader::InspectEditObject);
			break;
		};
	}


//EDIT_OBJECT
void CMeshLoader::ReadEditObject(T3dsChunk* aChunk)
	{
	//	printf("  ReadEditObject: 0x%x\n", aChunk->iId);
	switch (aChunk->iId)
		{
		case 0x4100: // TRIANGLE_OBJECT
			//			printf("  triangle obj\n");
			strcpy(iDataInfo->iMeshes[iDataInfo->iMeshCount].iName, iFileStringData);
//			printf("Reading Mesh: %s\n", iFileStringData );
			ReadChunkArray(aChunk->iLength - 6, &CMeshLoader::ReadTriangleObject, &iDataInfo->iMeshes[iDataInfo->iMeshCount++]);
			break;
		};
	}

void CMeshLoader::InspectEditObject(T3dsChunk* aChunk)
	{
	//	printf("  InspectEditObject: 0x%x\n", aChunk->iId);
	switch (aChunk->iId)
		{
		case 0x4100: // EDIT_OBJECT
			//			printf("  edit obj++\n");
			iDataInfo->iMeshCount++;
			break;
		};
	}

//MESH ITEMS

void CMeshLoader::ReadTriangleObject(T3dsChunk* aChunk, void* aOutput)
	{
	T3dsMesh* output = static_cast<T3dsMesh*>( aOutput );
	switch (aChunk->iId)
		{
		case 0x4110: // VERTEX_LIST
			//			printf("  vert list\n");
			ReadVertexList(output);
			break;
		case 0x4120: // FACE_LIST
			//			printf("  face list\n");
			ReadFaceList(output, aChunk->iLength - 6);
			break;
		case 0x4140: // MAPPING_COORDS
			//			printf("  tx coords\n");
			ReadTextureCoordinates(output);
			break;
		case 0x4160: // LOCAL_AXIS
			//			printf("  local axis\n");
			ReadLocalAxis(output);
			break;
		case 0x4170: // TEXTURE_INFO
			//			printf("  tx info\n");
			//ReadTextureInfo((char*)(pData + lPos + 6));
			break;
		default:
			break;
		};
	}

void CMeshLoader::ReadVertexList(T3dsMesh* output)
	{
	unsigned short	shNumCoords;
	fread(&shNumCoords, sizeof(unsigned short), 1, iFile);

	//	printf("---------------\nVertList: %d (float size: %d)\n", shNumCoords, sizeof(float));

	output->iVertCount = shNumCoords;
	output->iVertices = (float(*)[3])malloc(sizeof(float) * 3 * output->iVertCount);

	float x,y,z;

	for(int i=0; i<output->iVertCount;i++)
		{
		fread( &x, sizeof(float), 1, iFile);
		fread( &y, sizeof(float), 1, iFile);
		fread( &z, sizeof(float), 1, iFile);

		//printf("- %f %f %f\n",x ,y ,z );

		(*output->iVertices+(i*3))[0] = x*iScale;
		(*output->iVertices+(i*3))[1] = y*iScale;
		(*output->iVertices+(i*3))[2] = z*iScale;
		}
	//	printf("---------------\n");
	}

void CMeshLoader::ReadFaceList(T3dsMesh* output, long aLength)
	{
	long pos = 6;
	int               i;
	unsigned short		shNumFaces;

	fread(&shNumFaces, sizeof(unsigned short), 1, iFile);
	pos += sizeof(unsigned short);

	output->iTriCount = shNumFaces;
	output->iTriangles = (long(*)[3])malloc(sizeof(long) * 3 * output->iTriCount);

	unsigned short verts[4];

	for (i = 0; i < shNumFaces; i++)
		{
		fread(verts, sizeof(short), 4, iFile);
		for (int j = 0; j < 3; j++)
			{
			output->iTriangles[i][j] = (long)verts[j];
			}
		}

	pos += sizeof(unsigned short) * 4 * shNumFaces;

	if (pos < aLength)
		{
		InspectChunkArray(pos - 6, &CMeshLoader::InspectFaceSubs, output );

		if (output->iGroupCount)
			{
			output->iGroups = (T3dsGroup*)malloc(sizeof(T3dsGroup) * output->iGroupCount);
			memset(output->iGroups, 0, sizeof(T3dsGroup) * output->iGroupCount);
			if (output->iGroups)
				{
				output->iGroupCount = 0;
				ReadChunkArray(pos - 6, &CMeshLoader::ReadFaceSubs, output);
				}
			}
		}
	}

//FACE_SUBS
void CMeshLoader::ReadFaceSubs(T3dsChunk* aChunk, void* aOutput)
	{
	T3dsMesh* output = static_cast<T3dsMesh*>( aOutput );
	switch (aChunk->iId)
		{
		case 0x4130: // MATERIAL_GROUP
			ReadMaterialGroup(&output->iGroups[output->iGroupCount++]);
			break;
		case 0x4150: // SMOOTH_GROUP
			output->iSmooth = (long*)malloc(sizeof(long) * output->iTriCount);
			if (output->iSmooth)
				{
				fread(output->iSmooth, sizeof(long), output->iTriCount, iFile);
				}
			break;
		};
	}

void CMeshLoader::InspectFaceSubs(T3dsChunk* aChunk, void* aOutput)
	{
	T3dsMesh* output = static_cast<T3dsMesh*>( aOutput );
	switch (aChunk->iId)
		{
		case 0x4130: // MATERIAL_GROUP
			output->iGroupCount++;
			break;
		};
	}

void CMeshLoader::ReadLocalAxis(T3dsMesh* output)
	{
	fread(output->iAxis, sizeof(float), 9, iFile);
	fread(output->iPosition, sizeof(float), 3, iFile);
	}

//TEXTURE & MATERIALS
void CMeshLoader::ReadTextureCoordinates(T3dsMesh* output)
	{
	unsigned short	shNumCoords;

	fread(&shNumCoords, sizeof(unsigned short), 1, iFile);
	output->iTexCoordCount = shNumCoords;

	output->iTexCoords = (float(*)[2])malloc(sizeof(float) * 2 * output->iTexCoordCount);
	fread(output->iTexCoords, sizeof(float), 2 * output->iTexCoordCount, iFile);
	}


void CMeshLoader::ReadTexture(T3dsChunk* aChunk, void* aMaterial)
	{
	T3dsMaterial* material = static_cast<T3dsMaterial*>( aMaterial );

	switch (aChunk->iId)
		{
		case 0xa300: // MAT_MAP_NAME
			ReadString(material->iTexture);
			break;
		case 0xa351: // MAT_MAP_TILING
			break;
		case 0xa354: // MAT_MAP_U_SCALE
			break;
		case 0xa356: // MAT_MAP_V_SCALE
			break;
		case 0xa358: // MAT_MAP_U_OFFSET
			break;
		case 0xa35a: // MAT_MAP_V_OFFSET
			break;
		case 0xa35c: // MAT_MAP_V_ANG
			break;
		};
	}

void CMeshLoader::ReadColorChunk(T3dsChunk* aChunk, void* aColor)
	{
	float* color = static_cast<float*>( aColor );

	unsigned char chRGB[3];
	switch (aChunk->iId)
		{
		case 0x0010: // COLOR_F
			fread(color, sizeof(float), 3, iFile);
			break;
		case 0x0011: // COLOR_24
			fread(chRGB, sizeof(char), 3, iFile);

			color[0] = float(chRGB[0]) / 256.0f;
			color[1] = float(chRGB[1]) / 256.0f;
			color[2] = float(chRGB[2]) / 256.0f;
			break;
		};
	}


void CMeshLoader::ReadMaterialGroup(T3dsGroup* aGroup)
	{
	unsigned short	numFaces;
	unsigned short face;

	ReadString(aGroup->iName);

//	printf("Reading Material Group: %s\n", aGroup->iName );

	fread(&numFaces, sizeof(unsigned short), 1, iFile);

	aGroup->iTris = (long*)malloc(numFaces * sizeof(long));

	if (aGroup->iTris)
		{
		memset(aGroup->iTris, 0, numFaces * sizeof(long));

		aGroup->iSize = numFaces;
		aGroup->iMat = 0;

		for (int i = 0; i < numFaces; i++)
			{
			fread(&face, sizeof(unsigned short), 1, iFile);
			aGroup->iTris[i] = face;
			}
		}
	}

void CMeshLoader::ReadMaterial(T3dsChunk* aChunk, void* aMaterial)
	{
	T3dsMaterial* material = static_cast<T3dsMaterial*>( aMaterial );

	switch (aChunk->iId)
		{
		case 0xa000: // MAT_NAME
			ReadString(material->iName);
//			printf("Reading Material: %s\n", material->iName );
			break;
		case 0xa010: // MAT_AMBIENT
			ReadChunkArray(aChunk->iLength - 6, &CMeshLoader::ReadColorChunk, material->iAmbient);
			material->iAmbient[3] = 1.0f;
			break;
		case 0xa020: // MAT_DIFFUSE
			ReadChunkArray(aChunk->iLength - 6, &CMeshLoader::ReadColorChunk, material->iDiffuse);
			material->iDiffuse[3] = 1.0f;
			break;
		case 0xa030: // MAT_SPECULAR
			ReadChunkArray(aChunk->iLength - 6, &CMeshLoader::ReadColorChunk, material->iSpecular);
			material->iSpecular[3] = 1.0f;
			break;
		case 0xa040: // MAT_SHININESS
			ReadChunkArray(aChunk->iLength - 6, &CMeshLoader::ReadPercentageChunk, &material->iShininess);
			material->iShininess *= 140.0f;
			break;
		case 0xa041: // MAT_SHIN2PCT
			break;
		case 0xa042: // MAT_SHIN3PCT
			break;
		case 0xa050: // MAT_TRANSPANENCY
			break;
		case 0xa080: // MAT_EMISSION
			ReadChunkArray(aChunk->iLength - 6, &CMeshLoader::ReadColorChunk, material->iEmission);
			material->iEmission[3] = 1.0f;
			break;
		case 0xa200: // MAT_TEXMAP
			ReadChunkArray(aChunk->iLength - 6, &CMeshLoader::ReadTexture, aMaterial);
			break;
		default:
			break;
		};
	}

void CMeshLoader::ReadPercentageChunk(T3dsChunk* aChunk, void* aValue)
	{
	float* value = static_cast<float*>( aValue );

	short shPercent;

	switch (aChunk->iId)
		{
		case 0x0030: // INT_PERCENTAGE (short)
			fread(&shPercent, sizeof(short), 1, iFile);
			*value = (float)shPercent;
			break;
		case 0x0031: // FLOAT_PERCENTAGE
			fread(value, sizeof(float), 1, iFile);
			break;
		default:
			break;
		};

	*value /= 100.0f;
	}


// sorts triangles by group and then by smoothing group //
//======================================================//
void CMeshLoader::SortTriangles(T3dsMesh* aData)
	{
	//	printf("  sort triangles\n");
	int            result = 0;
	int            next = 0;
	int            nextNext = 0;
	int            i, j;
	T3dsTriangle*  tris;
	T3dsTriangle*  temp;
	int            groupCount;

	if (aData->iGroupCount == 0)
		{
		aData->iGroups = (T3dsGroup*)malloc(sizeof(T3dsGroup));
		if (aData->iGroups)
			{
			aData->iGroupCount = 1;
			memset(&aData->iGroups[0], 0, sizeof(T3dsGroup));
			aData->iGroups[0].iMat = -1;
			aData->iGroups[0].iStart = 0;
			aData->iGroups[0].iSize = aData->iTriCount;
			aData->iGroups[0].iTris = (long*)malloc(sizeof(long) * aData->iTriCount);

			for (i = 0; i < aData->iGroups[0].iSize; i++)
				{
				aData->iGroups[0].iTris[i] = i;
				}
			}
		if (aData->iSmooth == NULL)
			{
			return;
			}
		}

	tris = (T3dsTriangle*)malloc(aData->iTriCount * sizeof(T3dsTriangle));
	temp = (T3dsTriangle*)malloc(aData->iTriCount * sizeof(T3dsTriangle));

	for (i = 0; i < aData->iTriCount; i++)
		{
		memcpy(tris[i].iVerts, aData->iTriangles[i], sizeof(long) * 3);
		tris[i].iMat = -1;
		tris[i].iIndex = i;

		if (aData->iSmooth)
			{
			tris[i].iSmooth = aData->iSmooth[i];
			}
		else
			{
			tris[i].iSmooth = 0;
			}
		}

	for (i = 0; i < aData->iGroupCount; i++)
		{
		for (j = 0; j < aData->iGroups[i].iSize; j++)
			{
			tris[aData->iGroups[i].iTris[j]].iMat = aData->iGroups[i].iMat;
			}
		}

	//====================================//
	// now sort the triangles by material //
	//====================================//
	//qsort(tris, aData->iTriCount, sizeof(T3dsTriangle), (int (__cdecl *)(const void *,const void *))SortByMaterial);
	i = 0;
	while (i < aData->iTriCount)
		{
		result = 0;
		next = i + 1;
		while ((next < aData->iTriCount) && (result == 0))
			{
			result = SortByMaterial(&tris[i], &tris[next]);
			next++;
			}
		if (next == aData->iTriCount)
			{
			break;
			}
		next--;

		if (result > 0)
			{
			result = 0;
			nextNext = next + 1;
			while ((nextNext < aData->iTriCount) && (result == 0))
				{
				result = SortByMaterial(&tris[next], &tris[nextNext]);
				nextNext++;
				}
			if (result != 0)
				{
				nextNext--;
				}

			//==========================================//
			// copy the less than portion into a buffer //
			//==========================================//
			memcpy(temp, &tris[next], (nextNext - next) * sizeof(T3dsTriangle));

			//=====================================//
			// move the greater than portion ahead //
			//=====================================//
			memmove(&tris[(nextNext - next) + i], &tris[i], (next - i) * sizeof(T3dsTriangle));

			//====================================================//
			// copy the less than portion back in from the buffer //
			//====================================================//
			memcpy(&tris[i], temp, (nextNext - next) * sizeof(T3dsTriangle));

			//===================================//
			// start at the begining of the list //
			//===================================//
			i = 0;
			}
		else if (result < 0)
			{
			i = next;
			}
		else
			{
			break;
			}
		}

	groupCount = 1;
	aData->iGroups[0].iMat = tris[0].iMat;
	aData->iGroups[0].iStart = 0;
	aData->iGroups[0].iSize = 0;
	for (i = 0; i < aData->iTriCount; i++)
		{
		memcpy(aData->iTriangles[i], tris[i].iVerts, sizeof(long) * 3);
		if (aData->iSmooth)
			{
			aData->iSmooth[i] = tris[i].iSmooth;
			}

		if (aData->iGroups[groupCount - 1].iMat != tris[i].iMat)
			{
			aData->iGroups[groupCount].iMat = tris[i].iMat;
			aData->iGroups[groupCount].iStart = i;
			aData->iGroups[groupCount].iSize = 0;
			groupCount++;
			}

		aData->iGroups[groupCount-1].iSize++;
		}

	if (tris)
		{
		free(tris);
		}
	if (temp)
		{
		free(temp);
		}
	}

int CMeshLoader::SortByMaterial(T3dsTriangle* a, T3dsTriangle* b)
	{
	if (a->iMat > b->iMat)
		{
		return 1;
		}
	if (a->iMat < b->iMat)
		{
		return -1;
		}

	if (a->iSmooth > b->iSmooth)
		{
		return 1;
		}
	if (a->iSmooth < b->iSmooth)
		{
		return -1;
		}
	return 0;
	}

T3dsChunk CMeshLoader::ReadChunk()
	{
	T3dsChunk chunk;
	unsigned char byte[4]; 

	fread( &chunk, sizeof( unsigned char ), 2, iFile );
	fread( &byte, sizeof( unsigned char ), 4, iFile );
	chunk.iLength = *(reinterpret_cast<unsigned long*>(&byte) );

	return chunk;
	}

