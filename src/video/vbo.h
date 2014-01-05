#ifndef VBO_H
#define VBO_H

#include "resource/model.h"

typedef struct _LaoVBOBinding _LaoVBOBinding;
struct _LaoVBOBinding {
	int start;
	int end;
	int id;
};

typedef unsigned int GLuint;
struct LaoVBO;
struct LaoModel;
struct LaoVertex;

void _lao_vbo_init(struct LaoVBO *vbo, int size);
void _lao_vbo_bind_model(struct LaoVBO *vbo, struct LaoModel *m, struct LaoVertex *verts);
void _lao_vbo_unbind_model(struct LaoVBO *vbo, struct LaoModel *m);
void _lao_vbo_free(struct LaoVBO *vbo);

#endif
