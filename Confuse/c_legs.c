#include <math.h>
#include "forge.h"
#include "confuse.h"

#define C_MAX_STEP_SPEED 10.0
#define C_MAX_STEP_LENGTH 0.4


boolean (*c_foot_plant_pos_func)(float *goal, float *hip, float *vector, float *normal, float leg_length, void *user) = NULL;

void c_character_plant_pos_func_set(boolean (*func)(float *goal, float *hip, float *vector, float *normal, float leg_length, void *user))
{
	c_foot_plant_pos_func = func;
}

void c_character_leg(CIKLeg *leg, float *pos, float *center, float *dir, float *normal, float speed,  float *knee, float length, float height, float *leg_matrix, float *knee_matrix, boolean step, float delta)
{
	float tmp[3], output[3],  f, inv;
	uint i;

	tmp[0] = leg->foot_goal[0] - leg->foot_pos[0];
	tmp[1] = leg->foot_goal[1] - leg->foot_pos[1];
	tmp[2] = leg->foot_goal[2] - leg->foot_pos[2];

	delta *= 2;
	if(leg->timer > 1.0 || length * length * 0.01 * 0.01 > tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2])
	{
		tmp[0] = dir[0];
		tmp[1] = dir[1];
		tmp[2] = dir[2];		
		if(step)
		{
			output[0] = leg->foot_pos[0] = leg->foot_goal[0];
			output[1] = leg->foot_pos[1] = leg->foot_goal[1];
			output[2] = leg->foot_pos[2] = leg->foot_goal[2];
			leg->timer = 0;

			leg->speed = speed / (length * 4.0 * C_MAX_STEP_LENGTH * 0.5);
			if(leg->speed < 0.5)
				leg->speed = 0.5;
			if(leg->speed > 2.5)
				leg->speed = 2.5;
			f = speed / (leg->speed) * 0.75;
			tmp[0] = dir[0] * f - normal[0] * length;
			tmp[1] = dir[1] * f - normal[1] * length;
			tmp[2] = dir[2] * f - normal[2] * length;
			leg->tmp = 1;
			if(!c_foot_plant_pos_func(leg->foot_goal, center, tmp, normal, length, NULL))
			{ 
				leg->foot_goal[0] = center[0] - normal[0] * length * 0.4;
				leg->foot_goal[1] = center[1] - normal[1] * length * 0.4;
				leg->foot_goal[2] = center[2] - normal[2] * length * 0.4;
				leg->timer = 1.01;
				leg->tmp = 0;
			}
		}else
		{
			if(leg->speed < speed / (length * 4.0 * C_MAX_STEP_LENGTH * 0.5))
				leg->speed = speed / (length * 4.0 * C_MAX_STEP_LENGTH * 0.5);
			if(leg->speed < 0.5)
				leg->speed = 0.5;
			if(leg->speed > 2.5)
				leg->speed = 2.5;
			leg->timer += 0.5 * leg->speed * delta;
			output[0] = leg->foot_goal[0];
			output[1] = leg->foot_goal[1];
			output[2] = leg->foot_goal[2];	
		}
	}else
	{
		if(leg->speed < speed / (length * 4.0 * C_MAX_STEP_LENGTH * 0.5))
			leg->speed = speed / (length * 4.0 * C_MAX_STEP_LENGTH * 0.5);
		if(leg->speed < 0.5)
			leg->speed = 0.5;
		if(leg->speed > 2.5)
			leg->speed = 2.5;
		leg->timer += 0.5 * leg->speed * delta;
		f = leg->timer;
		if(f > 1.0)
			f = 1.0;
		inv = 1.0 - f;
		output[0] = leg->foot_goal[0] * f + leg->foot_pos[0] * inv;
		output[1] = leg->foot_goal[1] * f + leg->foot_pos[1] * inv;
		output[2] = leg->foot_goal[2] * f + leg->foot_pos[2] * inv;
		f *= inv * 0.5; 
		inv = 1.0 - f;
		output[0] = pos[0] * f + output[0] * inv;
		output[1] = pos[1] * f + output[1] * inv;
		output[2] = pos[2] * f + output[2] * inv;
	}
	c_compute_ik_matrix(pos, output, length, knee, &knee_matrix[12], tmp, leg_matrix, knee_matrix);

	for(i = 0; i < 11; i++)
	{
		leg_matrix[i] *= length;
		knee_matrix[i] *= length;
	}
}
