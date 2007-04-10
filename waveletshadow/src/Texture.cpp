/*
* tga.c -- tga texture loader
* last modification: dec. 15, 2005
*
* Copyright (c) 2005 David HENRY
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* gcc -Wall -ansi -L/usr/X11R6/lib -lGL -lGLU -lglut tga.c -o tga
*/

#include "Texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>


#ifdef __WIN32
#include <GL/glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glu.h>
#endif

/* texture id for exemple */
GLuint texId = 0;

#ifdef __APPLE__
short SwapTwoBytes(short w)
	{
	short tmp;
	tmp =  (w & 0x00FF);
	tmp = ((w & 0xFF00) >> 0x08) | (tmp << 0x08);
	return(tmp);
	}

int SwapFourBytes(int dw)
	{
	int tmp;
	tmp =  (dw & 0x000000FF);
	tmp = ((dw & 0x0000FF00) >> 0x08) | (tmp << 0x08);
	tmp = ((dw & 0x00FF0000) >> 0x10) | (tmp << 0x08);
	tmp = ((dw & 0xFF000000) >> 0x18) | (tmp << 0x08);
	return(tmp);
	}
#endif

void GetTextureInfo (tga_header_t *header, gl_texture_t *texinfo)
	{
	texinfo->width = header->width;
	texinfo->height = header->height;

	switch (header->image_type)
		{
		case 3:  /* grayscale 8 bits */
		case 11: /* grayscale 8 bits (RLE) */
			{
			if (header->pixel_depth == 8)
				{
				texinfo->format = GL_LUMINANCE;
				texinfo->internalFormat = 1;
				}
			else /* 16 bits */
				{
				texinfo->format = GL_LUMINANCE_ALPHA;
				texinfo->internalFormat = 2;
				}

			break;
			}

		case 1:  /* 8 bits color index */
		case 2:  /* BGR 16-24-32 bits */
		case 9:  /* 8 bits color index (RLE) */
		case 10: /* BGR 16-24-32 bits (RLE) */
			{
			/* 8 bits and 16 bits images will be converted to 24 bits */
			if (header->pixel_depth <= 24)
				{
				texinfo->format = GL_RGB;
				texinfo->internalFormat = 3;
				}
			else /* 32 bits */
				{
				texinfo->format = GL_RGBA;
				texinfo->internalFormat = 4;
				}

			break;
			}
		}
	}


void ReadTexture8bits (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo)
	{
	int i;
	GLubyte color;

	for (i = 0; i < texinfo->width * texinfo->height; ++i)
		{
		/* read index color byte */
		color = (GLubyte)fgetc (fp);

		/* convert to RGB 24 bits */
		texinfo->texels[(i * 3) + 2] = colormap[(color * 3) + 0];
		texinfo->texels[(i * 3) + 1] = colormap[(color * 3) + 1];
		texinfo->texels[(i * 3) + 0] = colormap[(color * 3) + 2];
		}
	}


void ReadTexture16bits (FILE *fp, gl_texture_t *texinfo)
	{
	int i;
	unsigned short color;

	for (i = 0; i < texinfo->width * texinfo->height; ++i)
		{
		/* read color word */
		color = fgetc (fp) + (fgetc (fp) << 8);

		/* convert BGR to RGB */
		texinfo->texels[(i * 3) + 0] = (GLubyte)(((color & 0x7C00) >> 10) << 3);
		texinfo->texels[(i * 3) + 1] = (GLubyte)(((color & 0x03E0) >>  5) << 3);
		texinfo->texels[(i * 3) + 2] = (GLubyte)(((color & 0x001F) >>  0) << 3);
		}
	}


void ReadTexture24bits (FILE *fp, gl_texture_t *texinfo)
	{
	int i;

	for (i = 0; i < texinfo->width * texinfo->height; ++i)
		{
		/* read and convert BGR to RGB */
		texinfo->texels[(i * 3) + 2] = (GLubyte)fgetc (fp);
		texinfo->texels[(i * 3) + 1] = (GLubyte)fgetc (fp);
		texinfo->texels[(i * 3) + 0] = (GLubyte)fgetc (fp);
		}
	}


void ReadTexture32bits (FILE *fp, gl_texture_t *texinfo)
	{
	int i;

	for (i = 0; i < texinfo->width * texinfo->height; ++i)
		{
		// read and convert BGRA to RGBA
		texinfo->texels[(i * 4) + 2] = (GLubyte)fgetc (fp);
		texinfo->texels[(i * 4) + 1] = (GLubyte)fgetc (fp);
		texinfo->texels[(i * 4) + 0] = (GLubyte)fgetc (fp);
		texinfo->texels[(i * 4) + 3] = (GLubyte)fgetc (fp);

		//RGBA->RGBA
		//texinfo->texels[(i * 4) + 0] = (GLubyte)fgetc (fp);
		//texinfo->texels[(i * 4) + 1] = (GLubyte)fgetc (fp);
		//texinfo->texels[(i * 4) + 2] = (GLubyte)fgetc (fp);
		//texinfo->texels[(i * 4) + 3] = (GLubyte)fgetc (fp);

		}
	}


void ReadTexturegray8bits (FILE *fp, gl_texture_t *texinfo)
	{
	int i;

	for (i = 0; i < texinfo->width * texinfo->height; ++i)
		{
		/* read grayscale color byte */
		texinfo->texels[i] = (GLubyte)fgetc (fp);
		}
	}


void ReadTexturegray16bits (FILE *fp, gl_texture_t *texinfo)
	{
	int i;

	for (i = 0; i < texinfo->width * texinfo->height; ++i)
		{
		/* read grayscale color + alpha channel bytes */
		texinfo->texels[(i * 2) + 0] = (GLubyte)fgetc (fp);
		texinfo->texels[(i * 2) + 1] = (GLubyte)fgetc (fp);
		}
	}


void ReadTexture8bitsRLE (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo)
	{
	int i, size;
	GLubyte color;
	GLubyte packet_header;
	GLubyte *ptr = texinfo->texels;

	while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 3)
		{
		/* read first byte */
		packet_header = (GLubyte)fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
			{
			/* run-length packet */
			color = (GLubyte)fgetc (fp);

			for (i = 0; i < size; ++i, ptr += 3)
				{
				ptr[0] = colormap[(color * 3) + 2];
				ptr[1] = colormap[(color * 3) + 1];
				ptr[2] = colormap[(color * 3) + 0];
				}
			}
		else
			{
			/* non run-length packet */
			for (i = 0; i < size; ++i, ptr += 3)
				{
				color = (GLubyte)fgetc (fp);

				ptr[0] = colormap[(color * 3) + 2];
				ptr[1] = colormap[(color * 3) + 1];
				ptr[2] = colormap[(color * 3) + 0];
				}
			}
		}
	}


void ReadTexture16bitsRLE (FILE *fp, gl_texture_t *texinfo)
	{
	int i, size;
	unsigned short color;
	GLubyte packet_header;
	GLubyte *ptr = texinfo->texels;

	while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 3)
		{
		/* read first byte */
		packet_header = fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
			{
			/* run-length packet */
			color = fgetc (fp) + (fgetc (fp) << 8);

			for (i = 0; i < size; ++i, ptr += 3)
				{
				ptr[0] = (GLubyte)(((color & 0x7C00) >> 10) << 3);
				ptr[1] = (GLubyte)(((color & 0x03E0) >>  5) << 3);
				ptr[2] = (GLubyte)(((color & 0x001F) >>  0) << 3);
				}
			}
		else
			{
			/* non run-length packet */
			for (i = 0; i < size; ++i, ptr += 3)
				{
				color = fgetc (fp) + (fgetc (fp) << 8);

				ptr[0] = (GLubyte)(((color & 0x7C00) >> 10) << 3);
				ptr[1] = (GLubyte)(((color & 0x03E0) >>  5) << 3);
				ptr[2] = (GLubyte)(((color & 0x001F) >>  0) << 3);
				}
			}
		}
	}


void ReadTexture24bitsRLE (FILE *fp, gl_texture_t *texinfo)
	{
	int i, size;
	GLubyte rgb[3];
	GLubyte packet_header;
	GLubyte *ptr = texinfo->texels;

	while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 3)
		{
		/* read first byte */
		packet_header = (GLubyte)fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
			{
			/* run-length packet */
			fread (rgb, sizeof (GLubyte), 3, fp);

			for (i = 0; i < size; ++i, ptr += 3)
				{
				ptr[0] = rgb[2];
				ptr[1] = rgb[1];
				ptr[2] = rgb[0];
				}
			}
		else
			{
			/* non run-length packet */
			for (i = 0; i < size; ++i, ptr += 3)
				{
				ptr[2] = (GLubyte)fgetc (fp);
				ptr[1] = (GLubyte)fgetc (fp);
				ptr[0] = (GLubyte)fgetc (fp);
				}
			}
		}
	}


void ReadTexture32bitsRLE (FILE *fp, gl_texture_t *texinfo)
	{
	int i, size;
	GLubyte rgba[4];
	GLubyte packet_header;
	GLubyte *ptr = texinfo->texels;

	while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 4)
		{
		/* read first byte */
		packet_header = (GLubyte)fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
			{
			/* run-length packet */
			fread (rgba, sizeof (GLubyte), 4, fp);

			for (i = 0; i < size; ++i, ptr += 4)
				{
				ptr[0] = rgba[2];
				ptr[1] = rgba[1];
				ptr[2] = rgba[0];
				ptr[3] = rgba[3];
				}
			}
		else
			{
			/* non run-length packet */
			for (i = 0; i < size; ++i, ptr += 4)
				{
				ptr[2] = (GLubyte)fgetc (fp);
				ptr[1] = (GLubyte)fgetc (fp);
				ptr[0] = (GLubyte)fgetc (fp);
				ptr[3] = (GLubyte)fgetc (fp);
				}
			}
		}
	}


void ReadTexturegray8bitsRLE (FILE *fp, gl_texture_t *texinfo)
	{
	int i, size;
	GLubyte color;
	GLubyte packet_header;
	GLubyte *ptr = texinfo->texels;

	while (ptr < texinfo->texels + (texinfo->width * texinfo->height))
		{
		/* read first byte */
		packet_header = (GLubyte)fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
			{
			/* run-length packet */
			color = (GLubyte)fgetc (fp);

			for (i = 0; i < size; ++i, ptr++)
				*ptr = color;
			}
		else
			{
			/* non run-length packet */
			for (i = 0; i < size; ++i, ptr++)
				*ptr = (GLubyte)fgetc (fp);
			}
		}
	}


void ReadTexturegray16bitsRLE (FILE *fp, gl_texture_t *texinfo)
	{
	int i, size;
	GLubyte color, alpha;
	GLubyte packet_header;
	GLubyte *ptr = texinfo->texels;

	while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 2)
		{
		/* read first byte */
		packet_header = (GLubyte)fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
			{
			/* run-length packet */
			color = (GLubyte)fgetc (fp);
			alpha = (GLubyte)fgetc (fp);

			for (i = 0; i < size; ++i, ptr += 2)
				{
				ptr[0] = color;
				ptr[1] = alpha;
				}
			}
		else
			{
			/* non run-length packet */
			for (i = 0; i < size; ++i, ptr += 2)
				{
				ptr[0] = (GLubyte)fgetc (fp);
				ptr[1] = (GLubyte)fgetc (fp);
				}
			}
		}
	}


gl_texture_t* ReadTextureFile (const char *filename)
	{
	FILE *fp;
	gl_texture_t *texinfo;
	tga_header_t header;
	GLubyte *colormap = NULL;

	fp = fopen (filename, "rb");
	if (!fp)
		{
		fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
		return NULL;
		}

	/* read header */
	fread (&header, sizeof (tga_header_t), 1, fp);

#ifdef __APPLE__
	header.cm_first_entry = SwapTwoBytes( header.cm_first_entry );
	header.cm_length      = SwapTwoBytes( header.cm_length );
	header.x_origin       = SwapTwoBytes( header.x_origin );
	header.y_origin       = SwapTwoBytes( header.y_origin );
	header.width          = SwapTwoBytes( header.width );
	header.height         = SwapTwoBytes( header.height );
#endif


	texinfo = (gl_texture_t *)malloc (sizeof (gl_texture_t));
	GetTextureInfo (&header, texinfo);
	fseek (fp, header.id_lenght, SEEK_CUR);

	/* memory allocation */
	texinfo->texels = (GLubyte *)malloc (sizeof (GLubyte) *
		texinfo->width * texinfo->height * texinfo->internalFormat);
	if (!texinfo->texels)
		{
		free (texinfo);
		return NULL;
		}

	/* read color map */
	if (header.colormap_type)
		{
		/* NOTE: color map is stored in BGR format */
		colormap = (GLubyte *)malloc (sizeof (GLubyte)
			* header.cm_length * (header.cm_size >> 3));
		fread (colormap, sizeof (GLubyte), header.cm_length
			* (header.cm_size >> 3), fp);
		}

	/* read image data */
	switch (header.image_type)
		{
		case 0:
			/* no data */
			break;

		case 1:
			/* uncompressed 8 bits color index */
			ReadTexture8bits (fp, colormap, texinfo);
			break;

		case 2:
			/* uncompressed 16-24-32 bits */
			switch (header.pixel_depth)
				{
				case 16:
					ReadTexture16bits (fp, texinfo);
					break;

				case 24:
					ReadTexture24bits (fp, texinfo);
					break;

				case 32:
					ReadTexture32bits (fp, texinfo);
					break;
				}

			break;

		case 3:
			/* uncompressed 8 or 16 bits grayscale */
			if (header.pixel_depth == 8)
				ReadTexturegray8bits (fp, texinfo);
			else /* 16 */
				ReadTexturegray16bits (fp, texinfo);

			break;

		case 9:
			/* RLE compressed 8 bits color index */
			ReadTexture8bitsRLE (fp, colormap, texinfo);
			break;

		case 10:
			/* RLE compressed 16-24-32 bits */
			switch (header.pixel_depth)
				{
				case 16:
					ReadTexture16bitsRLE (fp, texinfo);
					break;

				case 24:
					ReadTexture24bitsRLE (fp, texinfo);
					break;

				case 32:
					ReadTexture32bitsRLE (fp, texinfo);
					break;
				}

			break;

		case 11:
			/* RLE compressed 8 or 16 bits grayscale */
			if (header.pixel_depth == 8)
				ReadTexturegray8bitsRLE (fp, texinfo);
			else /* 16 */
				ReadTexturegray16bitsRLE (fp, texinfo);
			break;

		default:
			/* image type is not correct */
			fprintf (stderr, "error: unknown Texture image type %i!\n", header.image_type);
			free (texinfo->texels);
			free (texinfo);
			texinfo = NULL;
			break;
		}

	/* no longer need colormap data */
	if (colormap)
		free (colormap);

	fclose (fp);
	return texinfo;
	}


GLuint LoadTGATexture (const char *filename)
	{
	gl_texture_t *tga_tex = NULL;
	GLuint tex_id = 0;

	tga_tex = ReadTextureFile (filename);

	if (tga_tex && tga_tex->texels)
		{
		/* generate texture */
		glGenTextures (1, &tga_tex->id);
		glBindTexture (GL_TEXTURE_2D, tga_tex->id);

		/* setup some parameters for texture filters and mipmapping */
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);/*GL_CLAMP, GL_REPEAT*/
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		glTexImage2D( GL_TEXTURE_2D, 0, tga_tex->internalFormat,
			tga_tex->width, tga_tex->height, 0, tga_tex->format,
			GL_UNSIGNED_BYTE, tga_tex->texels);

		gluBuild2DMipmaps (GL_TEXTURE_2D, tga_tex->internalFormat,
			tga_tex->width, tga_tex->height,
			tga_tex->format, GL_UNSIGNED_BYTE, tga_tex->texels);

		tex_id = tga_tex->id;

		/* OpenGL has its own copy of texture data */
		free (tga_tex->texels);
		free (tga_tex);
		}

	return tex_id;
	}

void WriteTGATexture(const char* aFileName, int width, int height, char* data )
	{
		FILE *outputFile;
		outputFile = fopen(aFileName, "wb");
		if(outputFile==NULL) return;

		tga_header_t header;
		header.id_lenght		= 0x00; //byte
		header.colormap_type	= 0x00; //byte
		header.image_type		= 0x02; //byte

		header.cm_first_entry	= 0x0000; //short
		header.cm_length		= 0x0000; //short
		header.cm_size			= 0x00; //byte

		header.x_origin			= 0x0000; //short
		header.y_origin			= 0x0000; //short

		header.width			= width; //short
		header.height			= height;//short

		header.pixel_depth		= 0x20; //byte (32bits = 0x20)
		header.image_descriptor = 0x08; //byte

		fwrite( &header, sizeof(tga_header_t),1,outputFile );

		fwrite( data, 4, width*height, outputFile );
		fclose(outputFile);
	}

GLuint CreateTexture(float* data, int width, int height )
	{
	GLuint texId; 
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

#ifndef USE_FP_TEXTURES
	GLubyte* checkImage = new GLubyte[ width*height*4 ];

	for (int i=0,endI=width*height;i<endI;i++)
		{
		int val = 0xFF * *(data+i);
		*(checkImage+i*4) =	(GLubyte)val; //((((i&0x8)==0)^((i*width&0x8))==0))*255;; //red
		*(checkImage+i*4+1) = (GLubyte)val; //green
		*(checkImage+i*4+2) = (GLubyte)val; //blue
		*(checkImage+i*4+3) = (GLubyte)0xFF; // alpha
		}
	//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	delete[] checkImage;
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 1, GL_LUMINANCE, GL_FLOAT, data );
#endif
	printf("Created a luminance texture: %d\n",texId);
	return(texId);
	}


GLuint CreateTexture(TVector3* data, int width, int height )
	{
	GLuint texId; 
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

#ifndef USE_FP_TEXTURES
	GLubyte* checkImage = new GLubyte[ width*height*4 ];

	for (int i=0,endI=width*height;i<endI;i++)
		{
//		printf("[%f, %f, %f]\n", (data+i)->iX, (data+i)->iY, (data+i)->iZ);
		*(checkImage+i*4)   = (GLubyte)0xFF * pow((data+i)->iX, 0.7f); //red
		*(checkImage+i*4+1) = (GLubyte)0xFF * pow((data+i)->iY, 0.7f); //green
		*(checkImage+i*4+2) = (GLubyte)0xFF * pow((data+i)->iZ, 0.7f); //blue
		*(checkImage+i*4+3) = (GLubyte)0xFF; // alpha

		if( pow((data+i)->iX, 0.7f) > 1.0 )
			*(checkImage+i*4) = 0xFF; 
		if( pow((data+i)->iY, 0.7f) > 1.0 )
			*(checkImage+i*4+1) = 0xFF; 
		if( pow((data+i)->iZ, 0.7f) > 1.0 )
			*(checkImage+i*4+2) = 0xFF; 
		}

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	delete[] checkImage;
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 1, GL_RGB, GL_FLOAT, reinterpret_cast<float*>(&(data->iX)) );
#endif
	printf("Created a color texture: %d\n",texId);
	return(texId);
	}


GLuint LoadPFMTexture( string filename )
	{
	std::ifstream infile( filename.c_str(), std::ios::in | std::ios::binary);
	if(!infile.is_open())
		{
		printf("File access error while loading: %s\n", filename.c_str());
		exit(-1);
		}

	//read the header
	char id;
	char type;
	int channels(1);
	int width(0);
	int height(0);

	infile.read((char *)&id,	sizeof(char));
	infile.read((char *)&type,	sizeof(char));
	if (type == 'F')
		{
		channels=3;
		}	
	infile >> width;
	infile >> height;
	printf("ID:%c, Type:%c, Size: %dx%d\n", id, type, width, height );

	float byteOrder(0);
	infile >> byteOrder;
	infile.ignore(1);//ignore the newline

	if (byteOrder<0)
		{
		//little-endian
		printf("Little endian (%f)\n", byteOrder);
		}
	else
		{
		//big-endian
		printf("Big endian (%f)\n", byteOrder);
		printf("Big endian not supported atm.\n");
		exit(-1);
		}

	//read the data
	int size = width*height*channels;
	float* data = new float[size];	
	infile.read((char*) data, sizeof(float)*size);	

	infile.close();

	vector<TVector3> imageData;
	for(int i=0;i<size; i+=3)
		{
		TVector3 vec( *(data), *(data+1), *(data+2) );
		imageData.push_back( vec );
		printf("[%f, %f, %f]\n", vec.iX, vec.iY, vec.iZ);
		}
	delete[] data;
	imageData.clear();
	}

TVector3* LoadPFMCubeMap( string filename, int aTextureIds[6] )
	{
	FILE *infile = fopen(filename.c_str(), "rb");

	int fileType = fgetc(infile);
	int formatRGB = fgetc(infile);
	int junk = fgetc(infile); //newline

	if ((fileType != 'P') || ((formatRGB != 'F') && (formatRGB != 'f')))
		{
		fclose(infile);
		printf("Not a valid pfm file: %s\n", filename.c_str());
		exit(-1);
		}

	formatRGB = (formatRGB == 'F');		// 'F' = RGB,  'f' = monochrome
	int width, height;
	fscanf(infile, "%d %d%c", &width, &height, &junk);
	if ((width <= 0) || (height <= 0))
		{
		fclose(infile);
		printf("Size is 0 x 0\n");
		exit(-1);
		}

	float scalefactor;
	fscanf(infile, "%f%c", &scalefactor, &junk);

	int widthFace = width/3;
	int heightFace = height/4;
	int faceSize = widthFace*heightFace;

	TVector3* data = new TVector3[width*height];

	if (!data)
		{
		fclose(infile);
		printf("Out of Memory. (too large PFM file?)\n");
		exit(-1);
		}
	if (scalefactor > 0.0)
		{
		printf("No MSB support yet.\n");
		exit(-1);
		}

	fileType = width * (formatRGB ? 3 : 1);
	float *fbuf = new float[fileType];

	printf("Face: %d x %d\n", widthFace, heightFace );

	TVector3 *cur = data;
	for(int j = 0; j < height; j++)
		{
		if (fread(fbuf, sizeof(float), fileType, infile) != (size_t) fileType)
			{
			fclose(infile);
			delete fbuf;
			return 0;
			}

		float *temp = fbuf;
		for (int i = 0; i < width; i++)
			{
			if(
				//BACK & FLOOR
				(j<heightFace*2 && i>=widthFace && i<widthFace*2)
				//LEFT,FRONT,RIGHT
				|| (j>=heightFace*2 && j<heightFace*3) 
				//ROOF
				|| (j>=heightFace*3 && i>=widthFace && i<widthFace*2) 
				)
				{
				if (formatRGB)
					{
					cur->iX = *temp++;
					cur->iY = *temp++;
					cur->iZ = *temp++;
//#ifdef _DEBUG
//					printf("(%d,%d): [%f, %f, %f]\n",i,j,cur->iX,cur->iY,cur->iZ);
//#endif
					}
				else			// black and white
					{
					float c;
					c = *temp++;
					cur->iX = cur->iY = cur->iZ = c;
					}
				cur++;
				}
			//empty
			else
				{
				if (formatRGB)			// color
					{
					temp+=3;
					}
				else
					{
					temp++;
					}
				}
			}
		}
	delete fbuf;
	fclose(infile);


	//FIX center row
	TVector3* fixer = (data+faceSize*2); //first row should be ok
	TVector3* fixed  = new TVector3[faceSize*3];
	TVector3* first  = fixed;
	TVector3* second = fixed+faceSize;
	TVector3* third  = fixed+faceSize*2;

	for (int j=0;j<heightFace;j++)
		{
		for (int i=0; i<width;i++)
			{
//			printf("%d: ",i);
			if(i<widthFace)
				{
//				printf("third");
				*third++ = *fixer;
				}
			else if(i>=widthFace && i<widthFace*2)
				{
//				printf("second");
				*second++ = *fixer;
				}
			else
				{
//				printf("first");
				*first++ = *fixer;
				}
//			printf(" (%f,%f,%f)\n",fixer->iX,fixer->iY,fixer->iZ);
			fixer++;
			}
		}

	//int dOffSet = (widthFace*heightFace*6) -1;
	//printf("FIRST/LAST: [%f, %f, %f] / [%f, %f, %f]\n", data->iX,data->iY,data->iZ,  (data+dOffSet)->iX,(data+dOffSet)->iY,(data+dOffSet)->iZ );

//	printf("[%f, %f, %f] - [%f, %f, %f]", data->iX,data->iY,data->iZ,  (data+dOffSet)->iX,(data+dOffSet)->iY,(data+dOffSet)->iZ );
	first  = fixed;
	//second = fixed+faceSize;
	//third  = fixed+faceSize*2;
	TVector3* temp = (data+faceSize*2);
	for( int i=0; i<faceSize*3; i++)
		{
		*temp++ = *first++;
		}

	int offSet = widthFace*heightFace;

	first  = data;
	second = data+offSet;
	third  = data+offSet*2;
	TVector3* fourth = data+offSet*3;
	TVector3* fifth = data+offSet*4;
	TVector3* sixth = data+offSet*5;
	TVector3* tempVec = fixed;

	//SWAP
	for(int i=0;i<offSet;i++)
		{
		//1<->6
		*(tempVec+i) = *(first+i);
		*(first+i) = *(sixth+i);
		*(sixth+i) = *(tempVec+i);

		//2<->5
		*(tempVec+i) = *(second+i);
		*(second+i)= *(fifth+i);
		*(fifth+i) = *(tempVec+i);

		//3<->4
		*(tempVec+i) = *(third+i);
		*(third+i) = *(fourth+i);
		*(fourth+i) = *(tempVec+i);
		}

	aTextureIds[0] = CreateTexture( data, widthFace, heightFace );
	aTextureIds[1] = CreateTexture( data+offSet,   widthFace, heightFace );
	aTextureIds[2] = CreateTexture( data+offSet*2, widthFace, heightFace );
	aTextureIds[3] = CreateTexture( data+offSet*3, widthFace, heightFace );
	aTextureIds[4] = CreateTexture( data+offSet*4, widthFace, heightFace );
	aTextureIds[5] = CreateTexture( data+offSet*5, widthFace, heightFace );

	delete[] fixed;
//	delete[] data; //Will be freed by the caller

//	printf("[%f, %f, %f] - 0x%X\n", data->iX,data->iY,data->iZ, data );
	return data;
	}



GLuint LoadCubeMapTextures(
						   const char *filename1
						   , const char *filename2
						   , const char *filename3
						   , const char *filename4
						   , const char *filename5
						   , const char *filename6
						   )
	{
	GLuint tex_id = 0;

	gl_texture_t *texture1 = NULL;
	texture1 = ReadTextureFile (filename1);
	gl_texture_t *texture2 = NULL;
	texture2 = ReadTextureFile (filename2);
	gl_texture_t *texture3 = NULL;
	texture3 = ReadTextureFile (filename3);
	gl_texture_t *texture4 = NULL;
	texture4 = ReadTextureFile (filename4);
	gl_texture_t *texture5 = NULL;
	texture5 = ReadTextureFile (filename5);
	gl_texture_t *texture6 = NULL;
	texture6 = ReadTextureFile (filename6);

	if(
		(texture1 && texture1->texels)
		&&(texture2 && texture2->texels)
		&&(texture3 && texture3->texels)
		&&(texture4 && texture4->texels)
		&&(texture5 && texture5->texels)
		&&(texture6 && texture6->texels)
		)
		{
		glGenTextures(1, &texture1->id);
		tex_id = texture1->id;
		texture2->id = tex_id;
		texture3->id = tex_id;
		texture4->id = tex_id;
		texture5->id = tex_id;
		texture6->id = tex_id;

		glBindTexture(GL_TEXTURE_CUBE_MAP,tex_id);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT
			, 0
			, texture1->internalFormat
			, texture1->width
			, texture1->height
			, 0
			, texture1->format
			, GL_UNSIGNED_BYTE
			, texture1->texels);
		glBindTexture(GL_TEXTURE_CUBE_MAP,tex_id);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT
			, 0
			, texture2->internalFormat
			, texture2->width
			, texture2->height
			, 0
			, texture2->format
			, GL_UNSIGNED_BYTE
			, texture2->texels);
		glBindTexture(GL_TEXTURE_CUBE_MAP,tex_id);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT
			, 0
			, texture3->internalFormat
			, texture3->width
			, texture3->height
			, 0
			, texture3->format
			, GL_UNSIGNED_BYTE
			, texture3->texels);
		glBindTexture(GL_TEXTURE_CUBE_MAP,tex_id);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT
			, 0
			, texture4->internalFormat
			, texture4->width
			, texture4->height
			, 0
			, texture4->format
			, GL_UNSIGNED_BYTE
			, texture4->texels);
		glBindTexture(GL_TEXTURE_CUBE_MAP,tex_id);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT
			, 0
			, texture5->internalFormat
			, texture5->width
			, texture5->height
			, 0
			, texture5->format
			, GL_UNSIGNED_BYTE
			, texture5->texels);
		glBindTexture(GL_TEXTURE_CUBE_MAP,tex_id);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
			, 0
			, texture6->internalFormat
			, texture6->width
			, texture6->height
			, 0
			, texture6->format
			, GL_UNSIGNED_BYTE
			, texture6->texels);

		/* OpenGL has its own copy of texture data */
		free (texture1->texels);
		free (texture1);
		free (texture2->texels);
		free (texture2);
		free (texture3->texels);
		free (texture3);
		free (texture4->texels);
		free (texture4);
		free (texture5->texels);
		free (texture5);
		free (texture6->texels);
		free (texture6);
		}

	return tex_id;
	}

