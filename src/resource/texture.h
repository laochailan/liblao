/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <lao.h>

void _lao_load_texture(LaoTexture *tex, const char *filename);
void _lao_free_texture(LaoTexture *tex);

#endif
