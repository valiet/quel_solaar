#include <math.h>
#include "forge.h"
#include "confuse.h"
/*
void c_sim_edge(PhyVertiex *a, PhyVertiex *b, float length)
{
	float f, vec[3];
	vec[0] = a->pos[0] - b->pos[0];	
	vec[1] = a->pos[1] - b->pos[1];
	vec[2] = a->pos[2] - b->pos[2];
	length = length * length;
	length = length / (vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + length) - 0.5;
	vec[0] *= length;
	vec[1] *= length;
	vec[2] *= length;
	a->pos[0] += vec[0];
	a->pos[1] += vec[1];	
	a->pos[2] += vec[2];
	b->pos[0] -= vec[0];
	b->pos[1] -= vec[1];
	b->pos[2] -= vec[2];	
}
*/
void c_sim_ik(float *m1, float *m2)
{


}

void c_sim_joint(float *m1, float *m2)
{

}

void c_sim_joint2(float *m1, float *m2)
{

}

void c_sim_run(CCharDesc *d, CCharacter *c, float delta, float *gravity)
{
	float f;
	uint i;
/*	for(i = 0; i < C_CM_COUNT + C_MAX_INVENTORY_COUNT; i++)
	{
		c->matrix[i][12] += gravity[0] * delta;
		c->matrix[i][13] += gravity[1] * delta;
		c->matrix[i][14] += gravity[2] * delta;
	}

	for(i = 0; i < d->leg_config; i++)
	{
		f = 1.0 - 2.0 * (float)(i % 2);
		joint[0] = c->matrix[C_CM_BUTT + i / 2][12] + c->matrix[C_CM_BUTT + i / 2][0] * f;
		joint[1] = c->matrix[C_CM_BUTT + i / 2][13] + c->matrix[C_CM_BUTT + i / 2][1] * f;
		joint[2] = c->matrix[C_CM_BUTT + i / 2][14] + c->matrix[C_CM_BUTT + i / 2][2] * f;
		joint[0] = c->matrix[C_CM_BUTT + i / 2][12] + c->matrix[C_CM_BUTT + i / 2][0] * f;
		joint[1] = c->matrix[C_CM_BUTT + i / 2][13] + c->matrix[C_CM_BUTT + i / 2][1] * f;
		joint[2] = c->matrix[C_CM_BUTT + i / 2][14] + c->matrix[C_CM_BUTT + i / 2][2] * f;
*/
	for(i = C_CM_ARM_LA_DOWN; i <= C_CM_LEG_RC_DOWN; i += 2)
	{
/*		c->matrix[i][4] += gravity[0] * delta;
		c->matrix[i][5] += gravity[1] * delta;
		c->matrix[i][6] += gravity[2] * delta;*/
		c->matrix[i][12] += gravity[0] * delta;
		c->matrix[i][13] += gravity[1] * delta;
		c->matrix[i][14] += gravity[2] * delta;
	}

	for(i = C_CM_ARM_LA_UP; i < C_CM_LEG_RC_DOWN; i += 2)
	{
		float start[3], dir[3], knee[3], foot[3], end[3];
		end[0] = c->matrix[i + 1][12] + c->matrix[i + 1][4];
		end[1] = c->matrix[i + 1][13] + c->matrix[i + 1][5];
		end[2] = c->matrix[i + 1][14] + c->matrix[i + 1][6];
		start[0] = c->matrix[i][12];
		start[1] = c->matrix[i][13];
		start[2] = c->matrix[i][14];
		dir[0] = c->matrix[i + 1][12];
		dir[1] = c->matrix[i + 1][13];
		dir[2] = c->matrix[i + 1][14];
		c_compute_ik_matrix(start, end, d->legs_size, dir, knee, foot, c->matrix[i], c->matrix[i + 1]);
	}
	/* add gravity */
	/* move all points acording to springs */
	/* rectify matrices */
}