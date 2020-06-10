//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
#ifndef NANOVG_GL_H
#define NANOVG_GL_H

#include <nanovg/nanovg.h>

#ifdef __cplusplus
extern "C" {
#endif

// Create flags

enum NVGcreateFlags {
	// Flag indicating if geometry based anti-aliasing is used (may not be needed when using MSAA).
	NVG_ANTIALIAS 		= 1<<0,
	// Flag indicating if strokes should be drawn using stencil buffer. The rendering will be a little
	// slower, but path overlaps (i.e. self-intersecting or sharp turns) will be drawn just once.
	NVG_STENCIL_STROKES	= 1<<1,
	// Flag indicating that additional debug checks are done.
	NVG_DEBUG 			= 1<<2,
};

// #if defined NANOVG_GL2_IMPLEMENTATION
// #  define NANOVG_GL2 1
// #  define NANOVG_GL_IMPLEMENTATION 1
// #elif defined NANOVG_GL3_IMPLEMENTATION
// #  define NANOVG_GL3 1
// #  define NANOVG_GL_IMPLEMENTATION 1
// #  define NANOVG_GL_USE_UNIFORMBUFFER 1
// #elif defined NANOVG_GLES2_IMPLEMENTATION
// #  define NANOVG_GLES2 1
// #  define NANOVG_GL_IMPLEMENTATION 1
// #elif defined NANOVG_GLES3_IMPLEMENTATION
// #  define NANOVG_GLES3 1
// #  define NANOVG_GL_IMPLEMENTATION 1
// #endif

#define NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GL3 1
#define NANOVG_GL_IMPLEMENTATION 1
#define NANOVG_GL_USE_UNIFORMBUFFER 1


#define NANOVG_GL_USE_STATE_FILTER (1)

// Creates NanoVG contexts for different OpenGL (ES) versions.
// Flags should be combination of the create flags above.

#if defined NANOVG_GL2

NVGcontext* nvgCreateGL2(int flags);
void nvgDeleteGL2(NVGcontext* ctx);

int nvglCreateImageFromHandleGL2(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
GLuint nvglImageHandleGL2(NVGcontext* ctx, int image);

#endif

#if defined NANOVG_GL3

NVGcontext* nvgCreateGL3(int flags);
void nvgDeleteGL3(NVGcontext* ctx);

int nvglCreateImageFromHandleGL3(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
GLuint nvglImageHandleGL3(NVGcontext* ctx, int image);

#endif

#if defined NANOVG_GLES2

NVGcontext* nvgCreateGLES2(int flags);
void nvgDeleteGLES2(NVGcontext* ctx);

int nvglCreateImageFromHandleGLES2(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
GLuint nvglImageHandleGLES2(NVGcontext* ctx, int image);

#endif

#if defined NANOVG_GLES3

NVGcontext* nvgCreateGLES3(int flags);
void nvgDeleteGLES3(NVGcontext* ctx);

int nvglCreateImageFromHandleGLES3(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
GLuint nvglImageHandleGLES3(NVGcontext* ctx, int image);

#endif

// These are additional flags on top of NVGimageFlags.
enum NVGimageFlagsGL {
	NVG_IMAGE_NODELETE			= 1<<16,	// Do not delete GL texture handle.
};

#ifdef __cplusplus
}
#endif

#endif /* NANOVG_GL_H */

