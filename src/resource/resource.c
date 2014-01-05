/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#include "resource.h"

#include <lao.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "video/vbo.h"
#include "video/ubo.h"

_LaoResources _lao_resources;

char *_lao_get_res_name(const char *base, const char *path) {
	const char *beg, *end;
	char *res;
		
	beg = strstr(path, base);
	if(!beg)
		beg = path;
	else
		beg += strlen(base)+1;
	
	end = strrchr(path, '.');

	res = malloc(end-beg+1);
	strncpy(res, beg, end-beg);
	res[end-beg] = 0;
	
	return res;
}

void _lao_recurse_dir(const char *path, const char *suffix, void (*loader)(void*, const char*), void *dest) {
	DIR *dir = opendir(path);
	struct stat statbuf;
	struct dirent *dp;
	
	if(dir == NULL)
		lao_log_fatal("recurse_dir(): can't open directory '%s'", path);
	
	
	while((dp = readdir(dir)) != NULL) {
		char *buf = malloc(strlen(path) + strlen(dp->d_name)+2);
		strcpy(buf, path);
		strcat(buf, "/");
		strcat(buf, dp->d_name);
				
		stat(buf, &statbuf);
		
		if(S_ISDIR(statbuf.st_mode) && dp->d_name[0] != '.')
			_lao_recurse_dir(buf, suffix, loader, dest);
		else if(strcmp(dp->d_name+strlen(dp->d_name)-strlen(suffix), suffix) == 0)
			loader(dest, buf);
		
		free(buf);
	}
	
	closedir(dir);
}

static void add_texture(void *dest, const char *path) {
	_LaoResources *res = dest;
	
	res->textures = realloc(res->textures, sizeof(LaoTexture)*(++res->ntex));
	_lao_load_texture(&res->textures[res->ntex-1], path);
}

static void add_model(void *dest, const char *path) {
	_LaoResources *res = dest;
	
	res->models = realloc(res->models, sizeof(LaoModel)*(++res->nmodels));
	_lao_load_model(&res->models[res->nmodels-1], path);
}

void lao_resources_load(const char *basepath) {
	int maxl, l;
	char *buf;

	maxl = sizeof(LAO_GFX_PATH);
	l = sizeof(LAO_MODEL_PATH);
	if(l > maxl)
		maxl = l;
	l = sizeof(LAO_SHADER_PATH);
	if(l > maxl)
		maxl = l;
	l = strlen(basepath);
	buf = malloc(l + maxl+1);
	strcpy(buf, basepath);
	buf[l] = '/';

	memset(&_lao_resources, 0, sizeof(_LaoResources));
	_lao_vbo_init(&lao_bos.vbo, LAO_VBO_SIZE);
	lao_log_msg("loading textures...");
	strcpy(buf+l+1, LAO_GFX_PATH);
	_lao_recurse_dir(buf, ".png", add_texture, &_lao_resources);
	lao_log_msg("loading models...");
	strcpy(buf+l+1, LAO_MODEL_PATH);
	_lao_recurse_dir(buf, ".obj", add_model, &_lao_resources);
	
	strcpy(buf+l+1, LAO_SHADER_PATH);
	_lao_load_shaders(buf, &_lao_resources.shaders, &_lao_resources.nsha);
}

void lao_resources_free(void) {
	int i;
	_lao_vbo_free(&lao_bos.vbo);
	
	for(i = 0; i < _lao_resources.ntex; i++)
		_lao_free_texture(&_lao_resources.textures[i]);
	
	for(i = 0; i < _lao_resources.nsha; i++)
		_lao_free_shader(&_lao_resources.shaders[i]);
	
	for(i = 0; i < _lao_resources.nmodels; i++)
		_lao_free_model(&_lao_resources.models[i]);
}
