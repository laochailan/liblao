#include "util.h"

#include <stdlib.h>
#include <string.h>

void stralloc(char **dest, char *src) {
	*dest = malloc(strlen(src)+1);
	strcpy(*dest, src);
}
