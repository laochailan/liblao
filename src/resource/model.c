/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#include "model.h"

#include <lao.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "video/vbo.h"
#include "resource.h"

static void parse_obj(const char *filename, _LaoObjFileData *data) {
	FILE *fp = fopen(filename, "rb");
	
	char line[256];
	Vec3 buf;
	char mode;
	int linen = 0;
	
	memset(data, 0, sizeof(_LaoObjFileData));
	
	while(fgets(line, sizeof(line), fp)) {
		char *first;
		
		linen++;	
		
		first = strtok(line, " \n");
				
		if(strcmp(first, "v") == 0)
			mode = 'v';
		else if(strcmp(first, "vt") == 0)
			mode = 't';
		else if(strcmp(first, "vn") == 0)
			mode = 'n';
		else if(strcmp(first, "f") == 0)
			mode = 'f';
		else
			mode = 0;
		
		if(mode != 0 && mode != 'f') {			
			buf[0] = atof(strtok(NULL, " \n"));
			buf[1] = atof(strtok(NULL, " \n"));
			if(mode != 't')
				buf[2] = atof(strtok(NULL, " \n"));
						
			switch(mode) {
			case 'v':
				data->xs = realloc(data->xs, sizeof(Vec3)*(++data->xcount));
				memcpy(data->xs[data->xcount-1], buf, sizeof(Vec3));
				break;
			case 't':
				data->texcoords = realloc(data->texcoords, sizeof(Vec3)*(++data->tcount));
				memcpy(data->texcoords[data->tcount-1], buf, sizeof(Vec3));
				break;
			case 'n':
				data->normals = realloc(data->normals, sizeof(Vec3)*(++data->ncount));
				memcpy(data->normals[data->ncount-1], buf, sizeof(Vec3));
				break;
			}
		} else if(mode == 'f') {
			char *segment, *seg;
			int j = 0, jj;
			IVec3 ibuf;
			memset(ibuf, 0, sizeof(ibuf));
			
			while((segment = strtok(NULL, " \n"))) {
				seg = segment;
				j++;				
				
				jj = 0;
				while(jj < 3) {
					ibuf[jj] = atoi(seg);
					jj++;
					
					while(*seg != '\0' && *(++seg) != '/');
					
					if(*seg == '\0')
						break;
					else
						seg++;
				}
				
				if(strstr(segment, "//")) {
					ibuf[2] = ibuf[1];
					ibuf[1] = 0;
				}
				
				if(jj == 0 || jj > 3 || segment[0] == '/')
					lao_log_fatal("parse_obj(): OBJ file '%s:%d': Parsing error: Corrupt face definition", filename,linen);
				
				data->indices = realloc(data->indices, sizeof(IVec3)*(++data->icount));
				memcpy(data->indices[data->icount-1], ibuf, sizeof(IVec3));
			}

			if(j != 3)
				lao_log_fatal("parse_obj(): OBJ file '%s:%d': Mesh must be triangular.", filename, linen);	
		}
	}
	
	fclose(fp);
}
	
static void bad_reference_error(const char *filename, const char *aux, int n) {
	lao_log_fatal("_lao_load_model(): OBJ file '%s': Index %d: bad %s index reference", filename, n, aux);
}

void _lao_load_model(LaoModel *m, const char *filename) {
	_LaoObjFileData data;
	unsigned int i;
	
	LaoVertex *verts;
	
	memset(m, 0, sizeof(LaoModel));	
	m->name = _lao_get_res_name(LAO_MODEL_PATH, filename);
		
	parse_obj(filename, &data);
	
	m->count = data.icount;
	
	verts = calloc(data.icount, sizeof(LaoVertex));
	memset(verts, 0, data.icount*sizeof(LaoVertex));
	for(i = 0; i < data.icount; i++) {
		int xi, ni, ti;
		
		xi = data.indices[i][0]-1;
		if(xi < 0 || xi >= data.xcount)
			bad_reference_error(filename, "vertex", i);
		
		memcpy(verts[i].x, data.xs[xi], sizeof(Vec3));
				
		if(data.tcount) {
			ti = data.indices[i][1]-1;
			if(ti < 0 || ti >= data.tcount)
				bad_reference_error(filename, "texcoord", i);
			
			verts[i].s = data.texcoords[ti][0];
			verts[i].t = data.texcoords[ti][1];
		}
		
		if(data.ncount) {
			ni = data.indices[i][2]-1;
			if(ni < 0 || ni >= data.ncount)
				bad_reference_error(filename, "normal", ni);
			
			memcpy(verts[i].n, data.normals[ni], sizeof(Vec3));
		}
	}
	
	_lao_vbo_bind_model(&lao_bos.vbo, m, verts);
	
	lao_log_msg("%s -> %s", filename, m->name);
	
	free(verts);
	free(data.xs);
	free(data.normals);
	free(data.texcoords);
	free(data.indices);
}

LaoModel *lao_get_model(const char *name) {
	int i;
	for(i = 0; i < _lao_resources.nmodels; i++)
		if(strcmp(_lao_resources.models[i].name, name) == 0)
			return &_lao_resources.models[i];

	
	lao_log_fatal("lao_get_model(): cannot load model '%s'", name);
	return NULL;
}

void lao_draw_model_p(LaoModel *model) {	
	glDrawArrays(GL_TRIANGLES, model->offset, model->count);	
}

void lao_draw_model(const char *name) {
	lao_draw_model_p(lao_get_model(name));
}

void _lao_free_model(LaoModel *model) {
	free(model->name);
	_lao_vbo_unbind_model(&lao_bos.vbo, model);
}
