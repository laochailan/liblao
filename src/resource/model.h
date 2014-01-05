#ifndef MODEL_H
#define MODEL_H

#include <lao.h>

struct LaoModel;

typedef int IVec3[3];

typedef struct _LaoObjFileData _LaoObjFileData;
struct _LaoObjFileData {
	Vec3 *xs;
	int xcount;
	
	Vec3 *normals;
	int ncount;
	
	Vec3 *texcoords;
	int tcount;
	
	IVec3 *indices;
	int icount;
};

void _lao_load_model(struct LaoModel *model, const char *filename);
void _lao_free_model(struct LaoModel *model);

#endif
