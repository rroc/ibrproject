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

#ifndef TGA_H
#define TGA_H

#include "basic.h"

/* OpenGL texture info */
typedef struct
{
  GLsizei width;
  GLsizei height;

  GLenum format;
  GLint	internalFormat;
  GLuint id;

  GLubyte *texels;

} gl_texture_t;


#pragma pack(push, 1)

/* tga header */
typedef struct
{
  GLubyte id_lenght;          /* size of image id */
  GLubyte colormap_type;      /* 1 is has a colormap */
  GLubyte image_type;         /* compression type */

  short	cm_first_entry;       /* colormap origin */
  short	cm_length;            /* colormap length */
  GLubyte cm_size;            /* colormap size */

  short	x_origin;             /* bottom left x coord origin */
  short	y_origin;             /* bottom left y coord origin */

  short	width;                /* picture width (in pixels) */
  short	height;               /* picture height (in pixels) */

  GLubyte pixel_depth;        /* bits per pixel: 8, 16, 24 or 32 */
  GLubyte image_descriptor;   /* 24 bits = 0x00; 32 bits = 0x80 */

} tga_header_t;

#pragma pack(pop)

void GetTextureInfo (tga_header_t* header, gl_texture_t* texinfo);
void ReadTGA8bits (FILE* fp, GLubyte* colormap, gl_texture_t* texinfo);
void ReadTGA16bits (FILE* fp, gl_texture_t* texinfo);
void ReadTGA24bits (FILE* fp, gl_texture_t* texinfo);
void ReadTGA32bits (FILE* fp, gl_texture_t* texinfo);
void ReadTGAgray8bits (FILE* fp, gl_texture_t* texinfo);
void ReadTGAgray16bits (FILE* fp, gl_texture_t* texinfo);
void ReadTGA8bitsRLE (FILE* fp, GLubyte* colormap, gl_texture_t* texinfo);
void ReadTGA16bitsRLE (FILE* fp, gl_texture_t* texinfo);
void ReadTGA24bitsRLE (FILE* fp, gl_texture_t* texinfo);
void ReadTGA32bitsRLE (FILE* fp, gl_texture_t* texinfo);
void ReadTGAgray8bitsRLE (FILE* fp, gl_texture_t* texinfo);
void ReadTGAgray16bitsRLE (FILE* fp, gl_texture_t* texinfo);
gl_texture_t*  ReadTGAFile (const char* filename);
GLuint loadTGATexture(const char* filename);
GLuint loadCubeMapTextures( const char* filename1, const char* filename2, const char* filename3, const char* filename4, const char* filename5, const char* filename6);


void WriteTGA( const char* aFileName, int width, int height, char* data );

#endif
