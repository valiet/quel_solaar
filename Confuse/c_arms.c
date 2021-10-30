#include <math.h>
#include "forge.h"
#include "confuse.h"

void c_character_matrix_aim(float *matrix, float *aim, float *normal, float *pos, float scale)
{
	float f, n[3];
	matrix[8] = aim[0];
	matrix[9] = aim[1];
	matrix[10] = aim[2];
	f = matrix[8] * normal[0] + matrix[9] * normal[1] + matrix[10] * normal[2];
	n[0] = normal[0] - matrix[8] * f;
	n[1] = normal[1] - matrix[9] * f;
	n[2] = normal[2] - matrix[10] * f;
	f = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	matrix[4] = n[0] / f;
	matrix[5] = n[1] / f;
	matrix[6] = n[2] / f;
	matrix[0] = matrix[5] * matrix[10] - matrix[6] * matrix[9];
	matrix[1] = matrix[6] * matrix[8] - matrix[4] * matrix[10];
	matrix[2] = matrix[4] * matrix[9] - matrix[5] * matrix[8];
	matrix[0] *= scale;
	matrix[1] *= scale;
	matrix[2] *= scale;
	matrix[3] = 0.0;
	matrix[4] *= scale;
	matrix[5] *= scale;
	matrix[6] *= scale;
	matrix[7] = 0.0;
	matrix[8] *= scale;
	matrix[9] *= scale;
	matrix[10] *= scale;
	matrix[11] = 0.0;
	matrix[12] = pos[0];
	matrix[13] = pos[1];
	matrix[14] = pos[2];
	matrix[15] = 1.0;
}
/*
float c_character_action_retimer(float time, float value_a, float value_b, float value_c, float end)
{
	float inv, b;
	if(time > end)
		return 0;
	time /= end;
	inv = 1.0 - time;
	b = (value_b * inv + value_c * time);
	return ((value_a * inv + value_b * time) * inv + b * time) * inv + (b * inv + value_c * inv * time) * time;
}
*/
void c_character_arm(float *base, float *matrix, float sholder_dist, float *goal, float length, float *elbow, float *up_matrix, float *down_matrix, float *hand)
{
	float pos[3], f, vec[3];
	static uint blink, i;
	pos[0] = base[0] + matrix[0] * sholder_dist;
	pos[1] = base[1] + matrix[1] * sholder_dist;
	pos[2] = base[2] + matrix[2] * sholder_dist;
	vec[0] = elbow[0] - pos[0];
	vec[1] = elbow[1] - pos[1];
	vec[2] = elbow[2] - pos[2];
	c_compute_ik_matrix(pos, goal, length, vec, &down_matrix[12], hand, up_matrix, down_matrix);
	for(i = 0; i < 11; i++)
	{
		up_matrix[i] *= length;
		down_matrix[i] *= length;
	}
}

/*
void c_character_get_inventory_pos(CCharacter *c, CCharDesc *desc, float *hand, float *matrix, uint tool)
{
	float f;
	f = ((float)tool - 1.5) * 0.3;
	hand[0] = c->matrix[C_CM_BELLY][12] + c->matrix[C_CM_BELLY][8] * 0.2 + c->matrix[C_CM_BELLY][0] * f;
	hand[1] = c->matrix[C_CM_BELLY][13] + c->matrix[C_CM_BELLY][9] * 0.2 + c->matrix[C_CM_BELLY][1] * f;
	hand[2] = c->matrix[C_CM_BELLY][14] + c->matrix[C_CM_BELLY][10] * 0.2 + c->matrix[C_CM_BELLY][2] * f;
	if(matrix != NULL)
	{
		uint i;
		f = desc->arms_size / desc->butt_size;
		matrix[0] = c->matrix[C_CM_BELLY][0] / f;
		matrix[1] = c->matrix[C_CM_BELLY][1] / f;
		matrix[2] = c->matrix[C_CM_BELLY][2] / f;
		matrix[4] = c->matrix[C_CM_BELLY][8] / f;
		matrix[5] = c->matrix[C_CM_BELLY][9] / f;
		matrix[6] = c->matrix[C_CM_BELLY][10] / f;
		matrix[8] = -c->matrix[C_CM_BELLY][4] / f;
		matrix[9] = -c->matrix[C_CM_BELLY][5] / f;
		matrix[10] = -c->matrix[C_CM_BELLY][6] / f;
		matrix[12] = hand[0];
		matrix[13] = hand[1];
		matrix[14] = hand[2];
	}
}

void c_character_get_idle_pos(CCharacter *c, CCharDesc *desc, float *hand)
{
	hand[0] = c->matrix[C_CM_BELLY][12] + c->matrix[C_CM_BELLY][8] * 0.1 + c->matrix[C_CM_BELLY][0] * 1;
	hand[1] = c->matrix[C_CM_BELLY][13] + c->matrix[C_CM_BELLY][9] * 0.1 + c->matrix[C_CM_BELLY][1] * 1;
	hand[2] = c->matrix[C_CM_BELLY][14] + c->matrix[C_CM_BELLY][10] * 0.1 + c->matrix[C_CM_BELLY][2] * 1;
}

void c_matrix_multiply(float *a, float *b, float *output)
{
	output[0] = (b[0] * a[0]) + (b[1] * a[4]) + (b[2] * a[8]) + (b[3] * a[12]);
	output[4] = (b[4] * a[0]) + (b[5] * a[4]) + (b[6] * a[8]) + (b[7] * a[12]);
	output[8] = (b[8] * a[0]) + (b[9] * a[4]) + (b[10] * a[8]) + (b[11] * a[12]);
	output[12] = (b[12] * a[0]) + (b[13] * a[4]) + (b[14] * a[8]) + (b[15] * a[12]);

	output[1] = (b[0] * a[1]) + (b[1] * a[5]) + (b[2] * a[9]) + (b[3] * a[13]);
	output[5] = (b[4] * a[1]) + (b[5] * a[5]) + (b[6] * a[9]) + (b[7] * a[13]);
	output[9] = (b[8] * a[1]) + (b[9] * a[5]) + (b[10] * a[9]) + (b[11] * a[13]);
	output[13] = (b[12] * a[1]) + (b[13] * a[5]) + (b[14] * a[9]) + (b[15] * a[13]);

	output[2] = (b[0] * a[2]) + (b[1] * a[6]) + (b[2] * a[10]) + (b[3] * a[14]);
	output[6] = (b[4] * a[2]) + (b[5] * a[6]) + (b[6] * a[10]) + (b[7] * a[14]);
	output[10] = (b[8] * a[2]) + (b[9] * a[6]) + (b[10] * a[10]) + (b[11] * a[14]);
	output[14] = (b[12] * a[2]) + (b[13] * a[6]) + (b[14] * a[10]) + (b[15] * a[14]);

	output[3] = (b[0] * a[3]) + (b[1] * a[7]) + (b[2] * a[11]) + (b[3] * a[15]);
	output[7] = (b[4] * a[3]) + (b[5] * a[7]) + (b[6] * a[11]) + (b[7] * a[15]);
	output[11] = (b[8] * a[3]) + (b[9] * a[7]) + (b[10] * a[11]) + (b[11] * a[15]);
	output[15] = (b[12] * a[3]) + (b[13] * a[7]) + (b[14] * a[11]) + (b[15] * a[15]);
}

float c_character_retimer(float time, float a, float b)
{
	float invt;
	return time;
	invt = (1.0 - time);
	return ((a * invt) * time + (a * time + b * invt) * invt) * time + ((a * time + b * invt) * time + (b * time + invt) * invt) * invt;
}*/
