/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#include "ubo.h"

#include <lao.h>
#include <math.h>

#include "resource/resource.h"
#include "resource/shader.h"

void lao_create_ubo(GLuint *ubo, const char *name, int size, void *data) {
	int j;
	glGenBuffers(1, ubo);
	
	glBindBuffer(GL_UNIFORM_BUFFER, *ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STREAM_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for(j = 0; j < _lao_resources.nsha; j++)
		glUniformBlockBinding(_lao_resources.shaders[j].prog, glGetUniformBlockIndex(_lao_resources.shaders[j].prog, name), 1);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, *ubo, 0, size);
}
