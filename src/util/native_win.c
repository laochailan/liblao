/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#include "native.h"

#include <direct.h>

int lao_chdir(const char *path) {
	return _chdir(path);
}
