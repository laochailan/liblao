/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#include "util.h"

#include <stdlib.h>
#include <string.h>

void stralloc(char **dest, char *src) {
	*dest = malloc(strlen(src)+1);
	strcpy(*dest, src);
}
