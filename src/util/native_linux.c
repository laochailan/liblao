#include <lao.h>
#include <unistd.h>

int lao_chdir(const char *path) {
	return chdir(path);
}
