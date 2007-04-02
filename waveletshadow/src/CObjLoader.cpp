#include "CObjLoader.h"


ObjModel::ObjModel()
	: iNumNormal(0)
	, iNumTexCoord(0)
	, iNumTriangle(0)
	, iNumVertex(0)
	{
	this->iNormalArray.clear();
	this->iTexCoordArray.clear();
	this->iTriangleArray.clear();
	this->iVertexArray.clear();
	}

ObjModel::~ObjModel()  
	{
	iNormalArray.clear();
	iTexCoordArray.clear();
	iTriangleArray.clear();
	iVertexArray.clear();
	}

ObjModel::ObjModel(const ObjModel &aCopy)  
	{
	//make room for the new data
	iNumNormal		= aCopy.iNumNormal;
	iNumTexCoord	= aCopy.iNumTexCoord;
	iNumTriangle	= aCopy.iNumTriangle;
	iNumVertex		= aCopy.iNumVertex;

	iNormalArray.clear();
	iTexCoordArray.clear();
	iTriangleArray.clear();
	iVertexArray.clear();

	//Copy data
	for(int i = 0; i < iNumNormal; i++)
		iNormalArray.push_back( aCopy.iNormalArray.at(i) );

	for(int i = 0; i < iNumTexCoord; i++)
		iTexCoordArray.push_back( aCopy.iTexCoordArray.at(i) );

	for(int i = 0; i < iNumTriangle; i++)
		iTriangleArray.push_back( aCopy.iTriangleArray.at(i) );

	for(int i = 0; i < iNumVertex; i++)
		iVertexArray.push_back( aCopy.iVertexArray.at(i) );
	}	


//Object Loader
//--------------
CObjLoader::CObjLoader()  
	{
	}

CObjLoader::~CObjLoader()  
	{
	FreeObj();
	}

void CObjLoader::FreeObj()  
	{
	for(int i=0, j=iMeshes.size(); i<j; i++)
		{
		if( iMeshes.at(i) != NULL ) 
			{
			delete iMeshes.at(i);
			iMeshes.at(i) = NULL;
			}
		}
	iMeshes.clear();
	iObj = NULL;
	}

//Extract the 3ds data to mesh
CMesh* CObjLoader::GetMesh( int aIndex, float aScale )
	{
	//select the group
	iObj = iMeshes.at( aIndex );

	CMesh* mesh = new CMesh();

	mesh->iMin.set( 0,0,0 );
	mesh->iMax.set( 0,0,0 );

	mesh->iName = iObj->iName;

	//	printf("MESH:\"%s\"(%d),MAT:\"%s\"(%d),\tVRT:      ", mesh->iName.c_str(), aIndex, iDataInfo->iMaterials[iDataInfo->iMeshes[aIndex].iGroups[0].iMat].iName, iDataInfo->iMeshes[aIndex].iGroups[0].iMat );
	mesh->iMaterialColor = iObj->iObjectColor;

	//COPY VERTICES
	printf("COPYING Object(%d):\"%s\" v:%d, n:%d, tx:%d, t:%d\n", aIndex, mesh->iName.c_str(), iObj->iNumVertex, iObj->iNumNormal, iObj->iNumTexCoord, iObj->iNumTriangle); 
	printf("- Mesh Color: (%f, %f, %f, %f)\n", mesh->iMaterialColor.iR, mesh->iMaterialColor.iG, mesh->iMaterialColor.iB, mesh->iMaterialColor.iA );
	printf("- Copy vertices:             ");
	TVector3 vectorData;
//	printf("\n");
	TVector3 objMin( iObj->iVertexArray[0].X, iObj->iVertexArray[0].Y, iObj->iVertexArray[0].Z );
	TVector3 objMax( iObj->iVertexArray[0].X, iObj->iVertexArray[0].Y, iObj->iVertexArray[0].Z );
	for( int i=0, j=iObj->iNumVertex; i<j; i++)
		{
		printf("\b\b\b\b\b\b%6d", i+1 );
		//Texture coordinates
		//container.iTextureCoords.set( 
		//	( *iDataInfo->iMeshes[aIndex].iTexCoords+(i*2) )[0]
		//, ( *iDataInfo->iMeshes[aIndex].iTexCoords+(i*2) )[1]
		//);
		//Vertices
		vectorData.set(  iObj->iVertexArray[i].X * aScale
			,iObj->iVertexArray[i].Y * aScale
			,iObj->iVertexArray[i].Z * aScale
			);
		mesh->iVertices.push_back( vectorData );

		//Mins and Maxs
		if( objMin.iX > vectorData.iX ) objMin.iX = vectorData.iX;
		if( objMax.iX < vectorData.iX ) objMax.iX = vectorData.iX;
		if( objMin.iY > vectorData.iY ) objMin.iY = vectorData.iY;
		if( objMax.iY < vectorData.iY ) objMax.iY = vectorData.iY;
		if( objMin.iZ > vectorData.iZ ) objMin.iZ = vectorData.iZ;
		if( objMax.iZ < vectorData.iZ ) objMax.iZ = vectorData.iZ;

//		printf("[%f, %f, %f]\n", vectorData.iX, vectorData.iY, vectorData.iZ );


		//normals
		/*
		vectorData.set(   iObj->iNormalArray[i].X
		, iObj->iNormalArray[i].Y
		, iObj->iNormalArray[i].Z
		);
		mesh->iVertexNormals.push_back( vectorData );
		*/
		}
	mesh->iMin = objMin;
	mesh->iMax = objMax;


	//COPY TRIANGLES
	int v1, v2, v3;
	int n1, n2, n3;

	printf(", TRI:        ");
//	printf("\n");

	//Fill normals empty
	TVector3 empty(0,0,0);
	mesh->iVertexNormals.resize( iObj->iNumVertex, empty );
	int verticesAdded(0);
	for( int i=0, j=iObj->iNumTriangle; i<j; i++ )
		{
		printf("\b\b\b\b\b\b\b\b%8d", i+1 );

		//VertexIndex indices
		v1 = iObj->iTriangleArray.at(i).VertexIndex[0];
		v2 = iObj->iTriangleArray.at(i).VertexIndex[1];
		v3 = iObj->iTriangleArray.at(i).VertexIndex[2];
//		printf("v[%d, %d, %d]\n", v1,v2,v3);

		//normal indices
		n1 = iObj->iTriangleArray.at(i).NormalIndex[0];
		n2 = iObj->iTriangleArray.at(i).NormalIndex[1];
		n3 = iObj->iTriangleArray.at(i).NormalIndex[2];
//		printf("n[%d, %d, %d]\n\n", n1,n2,n3);

		//Normal 1
		//---------
		vectorData.set(   iObj->iNormalArray.at(n1).X
						, iObj->iNormalArray.at(n1).Y
						, iObj->iNormalArray.at(n1).Z
						);
		if(		mesh->iVertexNormals.at( v1 ) != empty
			&&  mesh->iVertexNormals.at( v1 ) != vectorData )
			{
//			printf("d1(%d)",v1);
			//this vertex has to be duplicated, and another normal added
			mesh->iVertices.push_back( mesh->iVertices.at(v1) );
			v1 = ( mesh->iVertices.size() - 1 );
			mesh->iVertexNormals.push_back( vectorData );
			verticesAdded++;
			}
		else
			{
			mesh->iVertexNormals.at( v1 ).set( vectorData );
			}

		//Normal 2
		//---------
		vectorData.set(   iObj->iNormalArray.at(n2).X
						, iObj->iNormalArray.at(n2).Y
						, iObj->iNormalArray.at(n2).Z
						);
		if(		mesh->iVertexNormals.at( v2 ) != empty
			&&  mesh->iVertexNormals.at( v2 ) != vectorData )
			{
//			printf("d2(%d)",v2);
			//this vertex has to be duplicated, and another normal added
			mesh->iVertices.push_back( mesh->iVertices.at(v2) );
			v2 = ( mesh->iVertices.size() - 1 );
			mesh->iVertexNormals.push_back( vectorData );
			verticesAdded++;
			}
		else
			{
			mesh->iVertexNormals.at( v2 ).set( vectorData );
			}

		//Normal 3
		//---------
		vectorData.set(   iObj->iNormalArray.at(n3).X
						, iObj->iNormalArray.at(n3).Y
						, iObj->iNormalArray.at(n3).Z
						);
		if(		mesh->iVertexNormals.at( v3 ) != empty
			&&  mesh->iVertexNormals.at( v3 ) != vectorData )
			{
//			printf("d3(%d)",v3);
			//this vertex has to be duplicated, and another normal added
			mesh->iVertices.push_back( mesh->iVertices.at(v3) );
			v3 = ( mesh->iVertices.size() - 1 );
			mesh->iVertexNormals.push_back( vectorData );
			verticesAdded++;
			}
		else
			{
			mesh->iVertexNormals.at( v3 ).set( vectorData );
			}

		mesh->iTriangles.push_back( TTriangle( v1, v2, v3 ) );
//		printf("\nT: v[%d, %d, %d] n[%d, %d, %d]\n", v1, v2, v3, n1, n2, n3 );
		}

	printf("\nVertices Added: %d\nTotal: %d\n---------------------------------\n", verticesAdded, mesh->iVertices.size() );
//	system("pause");
	return mesh;	
	}



int CObjLoader::LoadObj(string file)  
	{
//	FreeObj();
	iFileName = file;
	return ReadData();
	}



int CObjLoader::ReadData()  
	{
	printf("Start loading...\n");
	string buffer;
	string matFileName;

	iObj = new ObjModel();
	iMeshes.clear();

	ifstream input(iFileName.c_str());
	//make sure we got the file opened up ok...
	if( !input.is_open() )
		{
		printf("\nERROR FILE:\"%s\" not found!\n", iFileName.c_str() );
		return -1;
		}

//	printf("File found ok...\nCheck Data");

	//Setup our ObjModel arrays...
	//read one line at a time of the file...
/*	while( !input.eof() )  
		{
		//			printf(".");
		getline(input, buffer);

		if(buffer.substr(0,2) == "vn")
			iObj->iNumNormal++;
		else if(buffer.substr(0,2) == "vt")
			iObj->iNumTexCoord++;
		else if(buffer.substr(0,1) == "v")
			iObj->iNumVertex++;
		else if(buffer.substr(0,1) == "f")
			iObj->iNumTriangle++;

		}
*/

	//Make the arrays the right size...
	//iObj->iNormalArray =   new ObjVertex[iObj->iNumNormal];
	//iObj->iTexCoordArray = new ObjTexCoord[iObj->iNumTexCoord];
	//iObj->iTriangleArray = new ObjTriangle[iObj->iNumTriangle];
	//iObj->iVertexArray =   new ObjVertex[iObj->iNumVertex];

//	printf("\nDATA: vert: %d, norm: %d, tri: %d", iObj->iNumVertex, iObj->iNumNormal, iObj->iNumTriangle);

	//close the file...
//	input.close();

	//reopen it...
//	input.open(iFileName->c_str());

//	input.clear();

	//make sure we got the file opened up ok...
//	if( !input.is_open() )
//		return -1;


	printf("\nRead in  data\n");

	ObjTriangle tri;
	iObj->iName = "empty";

	int vertCount(0);
	int normCount(0);
	int texcCount(0);
	int vertCount2(0);
	int normCount2(0);
	int texcCount2(0);

	//read in line by line
	while( !input.eof() )  
		{
		//printf(".");
		getline(input, buffer);
		istringstream line(buffer);
		string temp;
		string f1, f2, f3;

		//Group name
		if(buffer.substr(0,1) == "g")
			{
//			printf("g ");
			if(iObj->iName != "empty")
				{
				vertCount2 = vertCount;
				normCount2 = normCount;
				texcCount2 = texcCount;

				iObj->iNumNormal	= iObj->iNormalArray.size();
				iObj->iNumTexCoord	= iObj->iTexCoordArray.size();
				iObj->iNumVertex	= iObj->iVertexArray.size();
				iObj->iNumTriangle	= iObj->iTriangleArray.size();

				iMeshes.push_back( iObj );
				iObj = new ObjModel();
				}
			line >> temp >> f1;
			iObj->iName = f1;
			}
		//vertex normals
		else if(buffer.substr(0,2) == "vn")  
			{
//			printf("vn ");
			normCount++;
			line >> temp >> f1 >> f2 >> f3;
			ObjVertex norm = {atof(f1.c_str()),atof(f2.c_str()),atof(f3.c_str())};
			iObj->iNormalArray.push_back( norm );

			//iObj->iNormalArray[nC].X = atof(f1.c_str());
			//iObj->iNormalArray[nC].Y = atof(f2.c_str());
			//iObj->iNormalArray[nC].Z = atof(f3.c_str());
			//sscanf(buffer.c_str(), "vn %f %f %f", iObj->iNormalArray[nC].X, 
			//					   iObj->iNormalArray[nC].Y, iObj->iNormalArray[nC].Z);
			}
		//vertex texturecoords
		else if(buffer.substr(0,2) == "vt")  
			{
//			printf("vt ");
			texcCount++;
			line >> temp >> f1 >> f2;
			ObjTexCoord texc = { atof(f1.c_str()), atof(f2.c_str()) };
			iObj->iTexCoordArray.push_back( texc );

			//iObj->iTexCoordArray[tC].U = atof(f1.c_str());
			//iObj->iTexCoordArray[tC].V = atof(f2.c_str());
			//sscanf(buffer.c_str(), "vt %f %f", iObj->iTexCoordArray[tC].U, 
			//					   iObj->iTexCoordArray[tC].V);
			}
		//vertices
		else if(buffer.substr(0,1) == "v")  
			{
//			printf("v ");
			vertCount++;
			line >> temp >> f1 >> f2 >> f3;

			ObjVertex vert = {atof(f1.c_str()),atof(f2.c_str()),atof(f3.c_str())};
			iObj->iVertexArray.push_back( vert );

			//iObj->iVertexArray[vC].X = atof(f1.c_str());
			//iObj->iVertexArray[vC].Y = atof(f2.c_str());
			//iObj->iVertexArray[vC].Z = atof(f3.c_str());
			//sscanf(buffer.c_str(), "v %f %f %f", iObj->iVertexArray[vC].X, 
			//					   iObj->iVertexArray[vC].Y, iObj->iVertexArray[vC].Z);
			}

		//faces (triangles)
		else if(buffer.substr(0,1) == "f")  
			{
//			printf("f ");
			line >> temp >> f1 >> f2 >> f3;

			int sPos = 0;
			int ePos = sPos;
			string temp2;

			ePos = f1.find_first_of("/");

			//we have a line with the format of "f %d/%d/%d %d/%d/%d %d/%d/%d"
			if(ePos != string::npos)  
				{
				temp2 = f1.substr(sPos, ePos - sPos);
				tri.VertexIndex[0] = (atoi(temp2.c_str()) - 1)		-vertCount2;

				sPos = ePos+1;
				ePos = f1.find("/", sPos);
				temp2 = f1.substr(sPos, ePos - sPos);
				tri.TexCoordIndex[0] = (atoi(temp2.c_str()) - 1)	-texcCount2;

				sPos = ePos+1;
				ePos = f1.length();
				temp2 = f1.substr(sPos, ePos - sPos);
				tri.NormalIndex[0] = (atoi(temp2.c_str()) - 1)		-normCount2;
				}

			sPos = 0;
			ePos = f2.find_first_of("/");
			//we have a line with the format of "f %d/%d/%d %d/%d/%d %d/%d/%d"
			if(ePos != string::npos)  
				{
				temp2 = f2.substr(sPos, ePos - sPos);
				tri.VertexIndex[1] = (atoi(temp2.c_str()) - 1)		-vertCount2;

				sPos = ePos + 1;
				ePos = f2.find("/", sPos+1);
				temp2 = f2.substr(sPos, ePos - sPos);
				tri.TexCoordIndex[1] = (atoi(temp2.c_str()) - 1)	-texcCount2;

				sPos = ePos + 1;
				ePos = f2.length();
				temp2 = f2.substr(sPos, ePos - sPos);
				tri.NormalIndex[1] = (atoi(temp2.c_str()) - 1)		-normCount2;
				}

			sPos = 0;
			ePos = f3.find_first_of("/");
			//we have a line with the format of "f %d/%d/%d %d/%d/%d %d/%d/%d"
			if(ePos != string::npos)  
				{
				temp2 = f3.substr(sPos, ePos - sPos);
				tri.VertexIndex[2] = (atoi(temp2.c_str()) - 1)		-vertCount2;

				sPos = ePos + 1;
				ePos = f3.find("/", sPos+1);
				temp2 = f3.substr(sPos, ePos - sPos);
				tri.TexCoordIndex[2] = (atoi(temp2.c_str()) - 1)	-texcCount2;

				sPos = ePos + 1;
				ePos = f3.length();
				temp2 = f3.substr(sPos, ePos - sPos);
				tri.NormalIndex[2] = (atoi(temp2.c_str()) - 1)		-normCount2;
				}
			iObj->iTriangleArray.push_back( tri );
			}
		//material filename
		else if(buffer.substr(0,6) == "mtllib")  
			{
//			printf("mtllib ");
			line >> temp >> f1;
			matFileName = f1;
			}
		//material
		else if( (buffer.substr(0,6) == "usemtl" ) && (0<matFileName.length()) )
			{
//			printf("usemtl ");

			//read desired meterial name
			line >> temp >> f1;

			ifstream inputMat( matFileName.c_str() );

			if( !inputMat.is_open() )
				{
				printf("\nERROR: material file not found!\n" );
//				return -1;
				}
			else
				{
//				printf("Material file: %s\n", matFileName.c_str() );
				while( !inputMat.eof() )  
					{
//					printf(",");
					getline(inputMat, buffer);
					istringstream line(buffer);

					//Material type
					if(buffer.substr(0,6) == "newmtl")
						{
//						printf("newmtl ");
						line >> temp >> f2;

						//Correct material name
						if( f1 == f2 )
							{
//							printf("Match: %s\n", f2.c_str() );
							while( !inputMat.eof() )  
								{
								getline(inputMat, buffer);
								istringstream line(buffer);

								//read diffuse color
								if(buffer.substr(0,2) == "Kd")
									{
//									printf("kd");
									line >> temp >> f1 >> f2 >> f3;
									iObj->iObjectColor.iR = atof(f1.c_str());
									iObj->iObjectColor.iG = atof(f2.c_str());
									iObj->iObjectColor.iB = atof(f3.c_str());
									}
								//dissolve (=alpha)
								else if(buffer.substr(0,1) == "d")
									{
//									printf("d"); 
									line >> temp >> f1;
									iObj->iObjectColor.iA = atof(f1.c_str());
									break;
									}
								//already at the next material
								else if(buffer.substr(0,6) == "newmtl")
									{
									break;
									}
								}
							}
						}
					}
				}
			inputMat.close();
			}

		}// end while read
//	printf("e");
	input.close();

	if(iObj->iName != "empty")
		{
		iObj->iNumNormal = iObj->iNormalArray.size();
		iObj->iNumTexCoord = iObj->iTexCoordArray.size();
		iObj->iNumVertex = iObj->iVertexArray.size();
		iObj->iNumTriangle = iObj->iTriangleArray.size();

		iMeshes.push_back( iObj );
		}
	//all should be good
	printf("\n Read OK.\n");

	return iMeshes.size();
	}


/*                                                                 //
//-----------------------------------------------------------------*/

