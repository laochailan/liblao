/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (C) 2014, Lukas Weber <laochailan@web.de>
 */

#include <lao.h>
#include <math.h>
#include <string.h>

const Mat3 _identity3 = {
	{1, 0, 0},
	{0, 1, 0},
	{0, 0, 1}
};

const Mat4 _identity4 = {
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1}
};

void vec3_add(Vec3 dest, const Vec3 a, const Vec3 b) {
	dest[0] = a[0] + b[0];
	dest[1] = a[1] + b[1];
	dest[2] = a[2] + b[2];
}

float dot(const Vec3 a, const Vec3 b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void cross(Vec3 dest, const Vec3 a, const Vec3 b) {
	dest[0] = a[1]*b[2]-a[2]*b[1];
	dest[1] = a[2]*b[0]-a[0]*b[2];
	dest[2] = a[0]*b[1]-a[1]*b[0];
}	
	
float vec3_length(const Vec3 a) {
	return sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
}

void vec3_norm(Vec3 a) {
	float l = vec3_length(a);
	vec3_scale(a,1.0f/l);
}

void vec3_scale(Vec3 a, float f) {
	a[0] *= f;
	a[1] *= f;
	a[2] *= f;
}

void transpose3(Mat3 dest, const Mat3 m) {
	int i, j;
	
	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			dest[i][j] = m[j][i];
}

void transpose4(Mat4 dest, const Mat4 m) {
	int i, j;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			dest[i][j] = m[j][i];
}

void matmul3(Mat3 dest, const Mat3 a, const Mat3 b) {
	int i,j,k;
	for(j = 0; j < 3; j++) {
		for(i = 0; i < 3; i++) {
			dest[j][i] = 0;
			for(k = 0; k < 3; k++)
				dest[j][i] += a[k][i] * b[j][k];
		}
	}
}

void matmul4(Mat4 dest, const Mat4 a, const Mat4 b) {
	int i,j,k;
	for(j = 0; j < 4; j++) {
		for(i = 0; i < 4; i++) {
			dest[j][i] = 0;
			for(k = 0; k < 4; k++)
				dest[j][i] += a[k][i] * b[j][k];
		}
	}
}

void matvec3(Vec3 dest, const Mat3 m, const Vec3 v) {
	int i, j;
	for(j = 0; j < 3; j++) {
		dest[j] = 0;
		for(i = 0; i < 3; i++)
			dest[j] += m[i][j] * v[i];
	}
}

void matpadding(Mat4 dest, const Mat3 m) {
	int i;
	memset(dest, 0, sizeof(Mat4));
	
	for(i = 0; i < 3; i++)
		memcpy(dest+i*4, m+i*3, sizeof(float)*3);

	dest[3][3] = 1.0f;
}

void lao_set_perspective(Mat4 mat, float ratio, float angle, float n, float f) {
	float h = 1.0/tan(angle);
	float w = h/ratio;
	
	memset(mat, 0, sizeof(Mat4));
	
	mat[0][0] = w;
	mat[1][1] = h;
	mat[2][2] = (f+n)/(n-f);
	mat[2][3] = -1.0;
	mat[3][2] = (2*f*n)/(n-f);
}
