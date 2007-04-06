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

#include "tga.h"
#include <stdio.h>
#include <stdlib.h>

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

void
GetTextureInfo (tga_header_t *header, gl_texture_t *texinfo)
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


void
ReadTGA8bits (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo)
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


void
ReadTGA16bits (FILE *fp, gl_texture_t *texinfo)
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


void
ReadTGA24bits (FILE *fp, gl_texture_t *texinfo)
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


void
ReadTGA32bits (FILE *fp, gl_texture_t *texinfo)
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


void
ReadTGAgray8bits (FILE *fp, gl_texture_t *texinfo)
	{
	int i;

	for (i = 0; i < texinfo->width * texinfo->height; ++i)
		{
		/* read grayscale color byte */
		texinfo->texels[i] = (GLubyte)fgetc (fp);
		}
	}


void
ReadTGAgray16bits (FILE *fp, gl_texture_t *texinfo)
	{
	int i;

	for (i = 0; i < texinfo->width * texinfo->height; ++i)
		{
		/* read grayscale color + alpha channel bytes */
		texinfo->texels[(i * 2) + 0] = (GLubyte)fgetc (fp);
		texinfo->texels[(i * 2) + 1] = (GLubyte)fgetc (fp);
		}
	}


void
ReadTGA8bitsRLE (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo)
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


void
ReadTGA16bitsRLE (FILE *fp, gl_texture_t *texinfo)
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


void
ReadTGA24bitsRLE (FILE *fp, gl_texture_t *texinfo)
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


void
ReadTGA32bitsRLE (FILE *fp, gl_texture_t *texinfo)
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


void
ReadTGAgray8bitsRLE (FILE *fp, gl_texture_t *texinfo)
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


void
ReadTGAgray16bitsRLE (FILE *fp, gl_texture_t *texinfo)
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


gl_texture_t *
ReadTGAFile (const char *filename)
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
			ReadTGA8bits (fp, colormap, texinfo);
			break;

		case 2:
			/* uncompressed 16-24-32 bits */
			switch (header.pixel_depth)
				{
				case 16:
					ReadTGA16bits (fp, texinfo);
					break;

				case 24:
					ReadTGA24bits (fp, texinfo);
					break;

				case 32:
					ReadTGA32bits (fp, texinfo);
					break;
				}

			break;

		case 3:
			/* uncompressed 8 or 16 bits grayscale */
			if (header.pixel_depth == 8)
				ReadTGAgray8bits (fp, texinfo);
			else /* 16 */
				ReadTGAgray16bits (fp, texinfo);

			break;

		case 9:
			/* RLE compressed 8 bits color index */
			ReadTGA8bitsRLE (fp, colormap, texinfo);
			break;

		case 10:
			/* RLE compressed 16-24-32 bits */
			switch (header.pixel_depth)
				{
				case 16:
					ReadTGA16bitsRLE (fp, texinfo);
					break;

				case 24:
					ReadTGA24bitsRLE (fp, texinfo);
					break;

				case 32:
					ReadTGA32bitsRLE (fp, texinfo);
					break;
				}

			break;

		case 11:
			/* RLE compressed 8 or 16 bits grayscale */
			if (header.pixel_depth == 8)
				ReadTGAgray8bitsRLE (fp, texinfo);
			else /* 16 */
				ReadTGAgray16bitsRLE (fp, texinfo);
			break;

		default:
			/* image type is not correct */
			fprintf (stderr, "error: unknown TGA image type %i!\n", header.image_type);
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


GLuint
loadTGATexture (const char *filename)
	{
	gl_texture_t *tga_tex = NULL;
	GLuint tex_id = 0;

	tga_tex = ReadTGAFile (filename);

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

void WriteTGA(const char* aFileName, int width, int height, char* data )
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


GLuint loadCubeMapTextures(
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
	texture1 = ReadTGAFile (filename1);
	gl_texture_t *texture2 = NULL;
	texture2 = ReadTGAFile (filename2);
	gl_texture_t *texture3 = NULL;
	texture3 = ReadTGAFile (filename3);
	gl_texture_t *texture4 = NULL;
	texture4 = ReadTGAFile (filename4);
	gl_texture_t *texture5 = NULL;
	texture5 = ReadTGAFile (filename5);
	gl_texture_t *texture6 = NULL;
	texture6 = ReadTGAFile (filename6);

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