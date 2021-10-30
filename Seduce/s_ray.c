
#include <math.h>
#include <stdlib.h>
#include "forge.h"

#define ME_EPSILON 0.000001
//#define ME_TEST_CULL

int me_geo_raycast_intersect_triangle(float orig[3], float dir[3], float vert0[3], float vert1[3], float vert2[3], float *t, float *u, float *v)
{
	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det, inv_det;

	edge1[0] = vert1[0] - vert0[0];
	edge1[1] = vert1[1] - vert0[1];
	edge1[2] = vert1[2] - vert0[2]; 
	edge2[0] = vert2[0] - vert0[0];
	edge2[1] = vert2[1] - vert0[1];
	edge2[2] = vert2[2] - vert0[2];
	pvec[0] = dir[1] * edge2[2] - dir[2] * edge2[1];
	pvec[1] = dir[2] * edge2[0] - dir[0] * edge2[2];
	pvec[2] = dir[0] * edge2[1] - dir[1] * edge2[0];
	det = edge1[0] * pvec[0] + edge1[1] * pvec[1] + edge1[2] * pvec[2];

#ifdef ME_TEST_CULL   
	if (det < ME_EPSILON)
		return FALSE;

	tvec[0] = orig[0] - vert0[0];
	tvec[1] = orig[1] - vert0[1];
	tvec[2] = orig[2] - vert0[2];

	*u = tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2];
	if (*u < 0.0 || *u > det)
		return FALSE;

	qvec[0] = tvec[1] * edge1[2] - tvec[2] * edge1[1];
	qvec[1] = tvec[2] * edge1[0] - tvec[0] * edge1[2];
	qvec[2] = tvec[0] * edge1[1] - tvec[1] * edge1[0];
	*v = dir[0] * qvec[0] + dir[1] * qvec[1] + dir[2] * qvec[2];
	if (*v < 0.0 || *u + *v > det)
		return FALSE;

	*t = edge2[0] * qvec[0] + edge2[1] * qvec[1] + edge2[2] * qvec[2];
	inv_det = 1.0 / det;
	*t *= inv_det;
	*u *= inv_det;
	*v *= inv_det;
#else  
	if (det > -ME_EPSILON && det < ME_EPSILON)
		return FALSE;
	inv_det = 1.0 / det;

	tvec[0] = orig[0] - vert0[0];
	tvec[1] = orig[1] - vert0[1];
	tvec[2] = orig[2] - vert0[2];

	*u = (tvec[0] * pvec[0] + tvec[1] * pvec[1] + tvec[2] * pvec[2]) * inv_det;
	if (*u < 0.0 || *u > 1.0)
		return FALSE;

	qvec[0] = tvec[1] * edge1[2] - tvec[2] * edge1[1];
	qvec[1] = tvec[2] * edge1[0] - tvec[0] * edge1[2];
	qvec[2] = tvec[0] * edge1[1] - tvec[1] * edge1[0];

	*v = (dir[0] * qvec[0] + dir[1] * qvec[1] + dir[2] * qvec[2]) * inv_det;
	if (*v < 0.0 || *u + *v > 1.0)
		return FALSE;

	*t = (edge2[0] * qvec[0] + edge2[1] * qvec[1] + edge2[2] * qvec[2]) * inv_det;
#endif
	return TRUE;
}
/*
int me_geo_raycast_intersect_triangle(float orig[3], float dir[3], float vert0[3], float vert1[3], float vert2[3], float *t, float *u, float *v)
{
	f_project3f(float *output, float *plane_pos, float *normal, float *pos, float *vector);
	if()
}
*/
boolean ray_test(float *start, float *end, float *vertex, uint vertex_stride, uint poly_count, uint *out_poly, float *out_pos, float *out_normal, float closest)
{
	float vec[3], f, t, u, v;
	uint i;
	vec[0] = end[0] - start[0];
	vec[1] = end[1] - start[1];
	vec[2] = end[2] - start[2];
/*	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] /= f;
	vec[1] /= f;
	vec[2] /= f;*/
	for(i = 0; i < poly_count / 3; i++)
	{
/*		if(me_geo_raycast_intersect_triangle(start, vec, &vertex[0], 
									&vertex[3], 
									&vertex[6], &t, &u, &v))*/

		if(me_geo_raycast_intersect_triangle(start, vec, &vertex[i * 3 * vertex_stride], 
									&vertex[i * 3 * vertex_stride + vertex_stride], 
									&vertex[i * 3 * vertex_stride + vertex_stride * 2], &t, &u, &v) && t > closest)
		{
			return TRUE;
		}
	}
	return FALSE;
}