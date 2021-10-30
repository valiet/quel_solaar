#include <math.h>
#include <stdio.h>

#include "forge.h"
#include "confuse.h"

extern void sui_draw_3d_line_gl(float start_x, float start_y,  float start_z, float end_x, float end_y, float end_z, float red, float green, float blue, float alpha);

float c_compute_ik_matrix(float *start, float *end, float length, float *dir, float *knee, float *foot, float *leg_m, float *knee_m)
{
	float vec[3], vec2[3], vec3[3], dist, f, inv_aspect, aspect = 0.5;
	float matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	uint i;
	for(i = 0; i < 16; i++)
		leg_m[i] = matrix[i];
	for(i = 0; i < 16; i++)
		knee_m[i] = matrix[i];
	inv_aspect = 1.0 - aspect;

	vec[0] = end[0] - start[0];
	vec[1] = end[1] - start[1];
	vec[2] = end[2] - start[2];
	dist = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] /= dist;
	vec[1] /= dist;
	vec[2] /= dist;

	f = vec[0] * dir[0] + vec[1] * dir[1] + vec[2] * dir[2];
	vec2[0] = dir[0] - vec[0] * f;
	vec2[1] = dir[1] - vec[1] * f;
	vec2[2] = dir[2] - vec[2] * f;
	f = sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1] + vec2[2] * vec2[2]);
	vec2[0] /= f;
	vec2[1] /= f;
	vec2[2] /= f;

	if(dist < length)
	{
		knee[0] = dir[0] + start[0];
		knee[1] = dir[1] + start[1];
		knee[2] = dir[2] + start[2];
		for(i = 0; i < 5; i++)
		{
			vec3[0] = knee[0] - end[0];
			vec3[1] = knee[1] - end[1];
			vec3[2] = knee[2] - end[2];
			f = sqrt(vec3[0] * vec3[0] + vec3[1] * vec3[1] + vec3[2] * vec3[2]);
			knee[0] = end[0] + vec3[0] / f * length * inv_aspect;
			knee[1] = end[1] + vec3[1] / f * length * inv_aspect;
			knee[2] = end[2] + vec3[2] / f * length * inv_aspect;

			vec3[0] = knee[0] - start[0];
			vec3[1] = knee[1] - start[1];
			vec3[2] = knee[2] - start[2];
			f = sqrt(vec3[0] * vec3[0] + vec3[1] * vec3[1] + vec3[2] * vec3[2]);
			knee[0] = start[0] + vec3[0] / f * length * aspect;
			knee[1] = start[1] + vec3[1] / f * length * aspect;
			knee[2] = start[2] + vec3[2] / f * length * aspect;
		}

		foot[0] = end[0];
		foot[1] = end[1];
		foot[2] = end[2];
		leg_m[4] = (start[0] - knee[0]) / (length * aspect);
		leg_m[5] = (start[1] - knee[1]) / (length * aspect);
		leg_m[6] = (start[2] - knee[2]) / (length * aspect);
		leg_m[0] = vec2[1] * vec[2] - vec2[2] * vec[1];
		leg_m[1] = vec2[2] * vec[0] - vec2[0] * vec[2];
		leg_m[2] = vec2[0] * vec[1] - vec2[1] * vec[0];
		leg_m[8] = leg_m[6] * leg_m[1] - leg_m[5] * leg_m[2];
		leg_m[9] = leg_m[4] * leg_m[2] - leg_m[6] * leg_m[0];
		leg_m[10] = leg_m[5] * leg_m[0] - leg_m[4] * leg_m[1];

		
		knee_m[0] = leg_m[0];
		knee_m[1] = leg_m[1];
		knee_m[2] = leg_m[2];
		knee_m[4] = (knee[0] - foot[0]) / (length * inv_aspect);
		knee_m[5] = (knee[1] - foot[1]) / (length * inv_aspect);
		knee_m[6] = (knee[2] - foot[2]) / (length * inv_aspect);
		knee_m[8] = knee_m[6] * knee_m[1] - knee_m[5] * knee_m[2];
		knee_m[9] = knee_m[4] * knee_m[2] - knee_m[6] * knee_m[0];
		knee_m[10] = knee_m[5] * knee_m[0] - knee_m[4] * knee_m[1];

	}else
	{
		knee[0] = start[0] + vec[0] * length * aspect;
		knee[1] = start[1] + vec[1] * length * aspect;
		knee[2] = start[2] + vec[2] * length * aspect;
		foot[0] = start[0] + vec[0] * length;
		foot[1] = start[1] + vec[1] * length;
		foot[2] = start[2] + vec[2] * length;
		leg_m[0] = vec2[2] * vec[1] - vec2[1] * vec[2];
		leg_m[1] = vec2[0] * vec[2] - vec2[2] * vec[0];
		leg_m[2] = vec2[1] * vec[0] - vec2[0] * vec[1];

		leg_m[0] = vec2[1] * vec[2] - vec2[2] * vec[1];
		leg_m[1] = vec2[2] * vec[0] - vec2[0] * vec[2];
		leg_m[2] = vec2[0] * vec[1] - vec2[1] * vec[0];

		leg_m[3] = 0;
		leg_m[4] = -vec[0];
		leg_m[5] = -vec[1];
		leg_m[6] = -vec[2];
		leg_m[7] = 0;
		leg_m[8] = vec2[0];
		leg_m[9] = vec2[1];
		leg_m[10] = vec2[2];
		leg_m[11] = 0;

		for(i = 0; i < 12; i++)
			knee_m[i] = leg_m[i];

	}
	knee_m[3] = leg_m[3] = 0;
	knee_m[7] = leg_m[7] = 0;
	knee_m[11] = leg_m[11] = 0;
	leg_m[12] = start[0];
	leg_m[13] = start[1];
	leg_m[14] = start[2];
	leg_m[15] = 1;
	knee_m[12] = knee[0];
	knee_m[13] = knee[1];
	knee_m[14] = knee[2];
	knee_m[15] = 1;
	return dist;
}

float c_compute_ik_matrix_old(float *start, float *end, float length, float *dir, float *knee, float *foot, float *leg_m, float *knee_m)
{
	float vec[3], vec2[3], bend, dist, f;
	float matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	uint i;
	for(i = 0; i < 16; i++)
		leg_m[i] = matrix[i];
	for(i = 0; i < 16; i++)
		knee_m[i] = matrix[i];

	vec[0] = end[0] - start[0];
	vec[1] = end[1] - start[1];
	vec[2] = end[2] - start[2];
	dist = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] /= dist;
	vec[1] /= dist;
	vec[2] /= dist;

	f = vec[0] * dir[0] + vec[1] * dir[1] + vec[2] * dir[2];
	vec2[0] = dir[0] - vec[0] * f;
	vec2[1] = dir[1] - vec[1] * f;
	vec2[2] = dir[2] - vec[2] * f;
	f = sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1] + vec2[2] * vec2[2]);
	vec2[0] /= f;
	vec2[1] /= f;
	vec2[2] /= f;

	if(dist < length)
	{
		bend = sqrt(length * 0.5 * length * 0.5 - dist * dist * 0.25);
		knee[0] = start[0] + dist * 0.5 * vec[0] + vec2[0] * bend;
		knee[1] = start[1] + dist * 0.5 * vec[1] + vec2[1] * bend;
		knee[2] = start[2] + dist * 0.5 * vec[2] + vec2[2] * bend;
		foot[0] = end[0];
		foot[1] = end[1];
		foot[2] = end[2];
		leg_m[0] = vec2[1] * vec[2] - vec2[2] * vec[1];
		leg_m[1] = vec2[2] * vec[0] - vec2[0] * vec[2];
		leg_m[2] = vec2[0] * vec[1] - vec2[1] * vec[0];
		leg_m[4] = (start[0] - knee[0]) / (length * 0.5);
		leg_m[5] = (start[1] - knee[1]) / (length * 0.5);
		leg_m[6] = (start[2] - knee[2]) / (length * 0.5);
		leg_m[8] = leg_m[6] * leg_m[1] - leg_m[5] * leg_m[2];
		leg_m[9] = leg_m[4] * leg_m[2] - leg_m[6] * leg_m[0];
		leg_m[10] = leg_m[5] * leg_m[0] - leg_m[4] * leg_m[1];

		knee_m[0] = leg_m[0];
		knee_m[1] = leg_m[1];
		knee_m[2] = leg_m[2];
		knee_m[4] = (knee[0] - foot[0]) / (length * 0.5);
		knee_m[5] = (knee[1] - foot[1]) / (length * 0.5);
		knee_m[6] = (knee[2] - foot[2]) / (length * 0.5);
		knee_m[8] = knee_m[6] * knee_m[1] - knee_m[5] * knee_m[2];
		knee_m[9] = knee_m[4] * knee_m[2] - knee_m[6] * knee_m[0];
		knee_m[10] = knee_m[5] * knee_m[0] - knee_m[4] * knee_m[1];

	}else
	{
		knee[0] = start[0] + vec[0] * length * 0.5;
		knee[1] = start[1] + vec[1] * length * 0.5;
		knee[2] = start[2] + vec[2] * length * 0.5;
		foot[0] = start[0] + vec[0] * length;
		foot[1] = start[1] + vec[1] * length;
		foot[2] = start[2] + vec[2] * length;
		leg_m[0] = vec2[2] * vec[1] - vec2[1] * vec[2];
		leg_m[1] = vec2[0] * vec[2] - vec2[2] * vec[0];
		leg_m[2] = vec2[1] * vec[0] - vec2[0] * vec[1];
		leg_m[3] = 0;
		leg_m[4] = -vec[0];
		leg_m[5] = -vec[1];
		leg_m[6] = -vec[2];
		leg_m[7] = 0;
		leg_m[8] = -vec2[0];
		leg_m[9] = -vec2[1];
		leg_m[10] = -vec2[2];
		leg_m[11] = 0;

		for(i = 0; i < 12; i++)
			knee_m[i] = leg_m[i];

	}
	knee_m[3] = leg_m[3] = 0;
	knee_m[7] = leg_m[7] = 0;
	knee_m[11] = leg_m[11] = 0;
	leg_m[12] = start[0];
	leg_m[13] = start[1];
	leg_m[14] = start[2];
	leg_m[15] = 1;
	knee_m[12] = knee[0];
	knee_m[13] = knee[1];
	knee_m[14] = knee[2];
	knee_m[15] = 1;
	return dist;
}

float c_compute_ik(float *start, float *end, float length, float *dir, float *knee, float *foot)
{
	float vec[3], vec2[3], bend, dist, f;
	float matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	vec[0] = end[0] - start[0];
	vec[1] = end[1] - start[1];
	vec[2] = end[2] - start[2];
	dist = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] /= dist;
	vec[1] /= dist;
	vec[2] /= dist;

	f = vec[0] * dir[0] + vec[1] * dir[1] + vec[2] * dir[2];
	vec2[0] = dir[0] - vec[0] * f;
	vec2[1] = dir[1] - vec[1] * f;
	vec2[2] = dir[2] - vec[2] * f;
	f = sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1] + vec2[2] * vec2[2]);
	vec2[0] /= f;
	vec2[1] /= f;
	vec2[2] /= f;

	if(dist < length)
	{
		bend = sqrt(length * 0.5 * length * 0.5 - dist * dist * 0.25);
		knee[0] = start[0] + dist * 0.5 * vec[0] + vec2[0] * bend;
		knee[1] = start[1] + dist * 0.5 * vec[1] + vec2[1] * bend;
		knee[2] = start[2] + dist * 0.5 * vec[2] + vec2[2] * bend;
		foot[0] = end[0];
		foot[1] = end[1];
		foot[2] = end[2];

	}else
	{
		knee[0] = start[0] + vec[0] * length * 0.5;
		knee[1] = start[1] + vec[1] * length * 0.5;
		knee[2] = start[2] + vec[2] * length * 0.5;
		foot[0] = start[0] + vec[0] * length;
		foot[1] = start[1] + vec[1] * length;
		foot[2] = start[2] + vec[2] * length;
	}
	return dist;
}
