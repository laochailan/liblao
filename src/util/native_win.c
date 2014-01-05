#include "native.h"

#include <direct.h>

int lao_chdir(const char *path) {
	return _chdir(path);
}
