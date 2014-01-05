/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#ifndef SHADER_H
#define SHADER_H

#include <lao.h>

struct LaoUniform {
	char *name;
	GLint location;
};

typedef struct _LaoShaderObj _LaoShaderObj;
struct _LaoShaderObj {
	char **links;
	int nlinks;
	
	GLuint obj;
	
	char *name;	
	GLenum type;
	int main;
};

typedef struct _LaoShaderContext _LaoShaderContext;
struct _LaoShaderContext {
	_LaoShaderObj *shaders;
	int nshaders;
	
	char **hname;
	char **hsrc;
	int nheaders;
};

void _lao_load_shaders(char *path, LaoShaderProg **dest, int *nsha);

void _lao_compile_shader(_LaoShaderContext *sc, char *path);
void _lao_link_shaders(LaoShaderProg **dest, int *nsha, _LaoShaderContext *sc);

void _lao_free_shadercontext(_LaoShaderContext *sc);
void _lao_free_shader(LaoShaderProg *sha);
void _lao_free_shaderobj(_LaoShaderObj *sha);

void _lao_cache_uniforms(LaoShaderProg *sha);

#endif
