/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include "texture.h"
#include "shader.h"
#include "model.h"

typedef struct _LaoResources _LaoResources;
struct _LaoResources {
	LaoTexture *textures;
	int ntex;
	
	LaoShaderProg *shaders;
	int nsha;
	
	LaoModel *models;
	int nmodels;
};

extern _LaoResources _lao_resources;

void _lao_recurse_dir(const char *path, const char *suffix, void (*loader)(void*, const char*), void *dest);
char *_lao_get_res_name(const char *base, const char *path);

#endif
