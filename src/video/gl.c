/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#include <lao.h>

#include <string.h>
#include <stdio.h>

#include "vbo.h"
#include "ubo.h"
#include "resource/resource.h"

#ifdef __WIN32__
// #include <GL/wgl.h>
#else
#include <GL/glx.h>
#endif

LaoBufferObjs lao_bos;

typedef void (*GLFuncPtr)(void);
GLFuncPtr get_proc_address(char *name) {
#ifdef __WIN32__
	return (GLFuncPtr)wglGetProcAddress(name);
#else
	return glXGetProcAddress((GLubyte *)name);
#endif
}

static void load_gl_functions(void);

void lao_gl_init(void) {
	load_gl_functions();
		
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

static void load_gl_functions(void) {
#ifdef __WIN32__
	glActiveLaoTexture = (PFNGLACTIVETEXTUREPROC)get_proc_address("glActiveLaoTexture");
	glBlendEquation = (PFNGLBLENDEQUATIONPROC)get_proc_address("glBlendEquation");
#endif
	
/* 	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)get_proc_address("glBindFramebuffer");	
 	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)get_proc_address("glGenFramebuffers");
 	glFramebufferLaoTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)get_proc_address("glFramebufferLaoTexture2D");
 	glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)get_proc_address("glDeleteFramebuffers");*/

	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)get_proc_address("glGenVertexArrays");
	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)get_proc_address("glDeleteVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)get_proc_address("glBindVertexArray");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)get_proc_address("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)get_proc_address("glDisableVertexAttribArray");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)get_proc_address("glVertexAttribPointer");
	
	glGenBuffers = (PFNGLGENBUFFERSPROC)get_proc_address("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)get_proc_address("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)get_proc_address("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)get_proc_address("glBufferSubData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)get_proc_address("glDeleteBuffers");
	glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)get_proc_address("glBindBufferRange");

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)get_proc_address("glCreateProgram");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)get_proc_address("glLinkProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)get_proc_address("glUseProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)get_proc_address("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)get_proc_address("glGetProgramInfoLog");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)get_proc_address("glDeleteProgram");

	glCreateShader = (PFNGLCREATESHADERPROC)get_proc_address("glCreateShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)get_proc_address("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)get_proc_address("glGetShaderInfoLog");
	glShaderSource = (PFNGLSHADERSOURCEPROC)get_proc_address("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)get_proc_address("glCompileShader");
	glAttachShader = (PFNGLATTACHSHADERPROC)get_proc_address("glAttachShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)get_proc_address("glDeleteShader");

	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)get_proc_address("glGetActiveUniform");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)get_proc_address("glGetUniformLocation");
	glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)get_proc_address("glUniformBlockBinding");
	glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)get_proc_address("glGetUniformBlockIndex");	
	
	glUniform1f = (PFNGLUNIFORM1FPROC)get_proc_address("glUniform1f");
	glUniform2f = (PFNGLUNIFORM2FPROC)get_proc_address("glUniform2f");
	glUniform3f = (PFNGLUNIFORM3FPROC)get_proc_address("glUniform3f");
	glUniform4f = (PFNGLUNIFORM4FPROC)get_proc_address("glUniform4f");

	glUniform1i = (PFNGLUNIFORM1IPROC)get_proc_address("glUniform1i");
	glUniform2i = (PFNGLUNIFORM2IPROC)get_proc_address("glUniform2i");
	glUniform3i = (PFNGLUNIFORM3IPROC)get_proc_address("glUniform3i");
	glUniform4i = (PFNGLUNIFORM4IPROC)get_proc_address("glUniform4i");
	
	glUniform2fv = (PFNGLUNIFORM2FVPROC)get_proc_address("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)get_proc_address("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)get_proc_address("glUniform4fv");
	
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)get_proc_address("glUniformMatrix4fv");
}
