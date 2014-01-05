#include "vbo.h"

#include <lao.h>
#include <string.h>
#include <stdlib.h>

void _lao_vbo_init(LaoVBO *vbo, int size) {
	memset(vbo, 0, sizeof(LaoVBO));	
	vbo->size = size;
	
	glGenBuffers(1, &vbo->vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LaoVertex)*size, NULL, GL_STREAM_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LaoVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LaoVertex), (void *) sizeof(Vec3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(LaoVertex), (void *) (2*sizeof(Vec3)));
}

void _lao_vbo_bind_model(LaoVBO *vbo, LaoModel *m, LaoVertex *verts) {
	int size;
	LaoListElement *e;
	_LaoVBOBinding *b;
	
	int offset;
	
	if(m->bindid != 0)
		lao_log_fatal("_lao_vbo_bind_model(): can't bind model more than once.");
	
	size = m->count;
	
	for(e = vbo->alloc.first; e && e->next; e = e->next)
		if(((_LaoVBOBinding *) e->next->data)->start - ((_LaoVBOBinding *) e->data)->end >= size)
			break;
	
	offset = e == NULL ? 0 : ((_LaoVBOBinding *) e->data)->end;

	if(offset+size > vbo->size)
		lao_log_fatal("_lao_vbo_bind_model(): VBO too small. %d > %d", offset+size, vbo->size); /* TODO: make vbo dynamically sized. */
			
	b = malloc(sizeof(_LaoVBOBinding));
	b->start = offset;
	b->end = b->start + size;
	b->id = ++vbo->ids;
	
	m->bindid = b->id;
	m->offset = offset;
		
	lao_list_insert(&vbo->alloc, e, b);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(LaoVertex)*b->start, sizeof(LaoVertex)*size, verts);
}

void _lao_vbo_unbind_model(LaoVBO *vbo, LaoModel *m) {
	LaoListElement *e;
	
	for(e = vbo->alloc.first; e; e = e->next) {
		if(((_LaoVBOBinding *) e->data)->id == m->bindid) {			
			m->bindid = 0;
			m->offset = 0;
			
			lao_list_remove(&vbo->alloc, e);
			return;
		}
	}
}

void _lao_vbo_free(LaoVBO *vbo) {
	lao_list_free(&vbo->alloc);
	glDeleteBuffers(1, &vbo->vbo);
}
