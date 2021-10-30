#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "betray.h"
#include "relinquish.h"

void c_execute_component_module_random_init_pos(float *pos, float *center, float size, uint *seed);

void c_execute_component_module_particle_emitter_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	static uint seed = 0;
	float *output, *position, *pos, *target, *size, tmp[3], *center, *speed, *age, f, time, last_time;
	uint i,  j, k, count = 1, position_stride, position_count, target_stride, target_count, size_stride, size_count, speed_stride, speed_count, age_stride, age_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	type = &entity->stack_types[c->references[5]]; 
	count = type->size; 
	output = &stack[type->pos];

	position = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_POS, &position_stride, &position_count)];
	target = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &target_stride, &target_count)];
	size = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &size_stride, &size_count)];
	speed = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &speed_stride, &speed_count)];
	age = &stack[c_entity_stack_get(entity, c->references[4], C_TYPE_FLOAT, &age_stride, &age_count)];

	/*
	size = c_type_size_get(type->type);
	a_count *= size;
	b_count *= size;
	
	for(i = 0; i < count; i++)
	{
		f = f_randf(instance->seed * count + i);
		for(k = 0; k < size; k++)
		{
			tmp = a[(i % b_count) * b_stride + k];
		//	printf("RANDOM %f %f %f", tmp, (b[(i % b_count) * b_stride + k] - tmp), f);
			output[i * size + k] = tmp + (b[(i % b_count) * b_stride + k] - tmp) * f;
			i++;
		}
	}*/
	session->delta_time;
	instance->time / age[(i % age_count) * age_stride] - ((float)i / (float)count);

/*	BInputState *input;
	input = betray_get_input_state();
	if(input->pointers[0].pointer_x > 0)
		return;*/

	for(i = 0; i < count; i++)
	{
		time = instance->time / (age[(i % age_count) * age_stride] * 30.0) - ((float)i / (float)count);
		if(time < 0)
		{
			pos = &position[(i % position_count) * position_stride];
			output[i * 7 + 0] = pos[0];
			output[i * 7 + 1] = pos[1];
			output[i * 7 + 2] = pos[2];
			output[i * 7 + 6] = time;
		}else
		{
			last_time = (instance->time - session->delta_time) / (age[(i % age_count) * age_stride] * 30.0) - ((float)i / (float)count);

			if(last_time < 0.0 || (uint)time != (uint)last_time)
			{
				float dist = 10.0;
				f = size[(i % size_count) * size_stride];
				center = &target[(i % target_count) * target_stride];
				pos = &position[(i % position_count) * position_stride];
				output[i * 7 + 0] = pos[0];
				output[i * 7 + 1] = pos[1];
				output[i * 7 + 2] = pos[2];
	
				while(dist > 1.0)
				{
					tmp[0] = f_randnf(seed++);
					tmp[1] = f_randnf(seed++);
					tmp[2] = f_randnf(seed++);
					dist = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
				}
				tmp[0] = tmp[0] * f + center[0] - pos[0]; 
				tmp[1] = tmp[1] * f + center[1] - pos[1]; 
				tmp[2] = tmp[2] * f + center[2] - pos[2];
				f = sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]) / speed[(i % speed_count) * speed_stride];
				output[i * 7 + 3] = tmp[0] / f;
				output[i * 7 + 4] = tmp[1] / f;
				output[i * 7 + 5] = tmp[2] / f;
				output[i * 7 + 6] = time - (float)((uint)time);
			}
			output[i * 7 + 0] += output[i * 7 + 3] * session->delta_time;
			output[i * 7 + 1] += output[i * 7 + 4] * session->delta_time;
			output[i * 7 + 2] += output[i * 7 + 5] * session->delta_time;
			if(output[i * 7 + 6] <= 1.0)
				output[i * 7 + 6] = time - (float)((uint)time);
		}
	}
}


void c_execute_component_module_particle_burst_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	static uint seed = 0;
	float *output, *position, *pos, *target, *size, tmp[3], *center, *speed, *age, *spread, f, time, last_time;
	uint i,  j, k, count = 1, position_stride, position_count, target_stride, target_count, size_stride, size_count, speed_stride, speed_count, age_stride, age_count, spread_stride, spread_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	type = &entity->stack_types[c->references[6]]; 
	count = type->size; 
	output = &stack[type->pos];

	position = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_POS, &position_stride, &position_count)];
	target = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &target_stride, &target_count)];
	size = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &size_stride, &size_count)];
	speed = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &speed_stride, &speed_count)];
	age = &stack[c_entity_stack_get(entity, c->references[4], C_TYPE_FLOAT, &age_stride, &age_count)];
	spread = &stack[c_entity_stack_get(entity, c->references[5], C_TYPE_FLOAT, &spread_stride, &spread_count)];

	for(i = 0; i < count; i++)
	{
		f = age[(i % age_count) * age_stride] * 30.0;
		time = (instance->time - session->delta_time - 0.001 - spread[(i % spread_count) * spread_stride] * f * ((float)i / (float)count)) / f;
		if(time < 0)
		{
			pos = &position[(i % position_count) * position_stride];
			output[i * 7 + 0] = pos[0];
			output[i * 7 + 1] = pos[1];
			output[i * 7 + 2] = pos[2];
			output[i * 7 + 6] = time;
		}else if(time > 1.0)
		{
			if(time > output[i * 7 + 6])
				output[i * 7 + 6] = time;
		}else
		{ 
			if(time >= 0.0 && output[i * 7 + 6] < 0)
			{
				float dist = 10.0;
				f = size[(i % size_count) * size_stride];
				center = &target[(i % target_count) * target_stride];
				pos = &position[(i % position_count) * position_stride];
				output[i * 7 + 0] = pos[0];
				output[i * 7 + 1] = pos[1];
				output[i * 7 + 2] = pos[2];
	
				while(dist > 1.0)
				{
					tmp[0] = f_randnf(seed++);
					tmp[1] = f_randnf(seed++);
					tmp[2] = f_randnf(seed++);
					dist = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
				}
				tmp[0] = tmp[0] * f + center[0] - pos[0]; 
				tmp[1] = tmp[1] * f + center[1] - pos[1]; 
				tmp[2] = tmp[2] * f + center[2] - pos[2];
				f = sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]) / speed[(i % speed_count) * speed_stride];
				output[i * 7 + 3] = tmp[0] / f;
				output[i * 7 + 4] = tmp[1] / f;
				output[i * 7 + 5] = tmp[2] / f;

				f = time /  (time - output[i * 7 + 6]) * session->delta_time;
				output[i * 7 + 0] += output[i * 7 + 3] * f;
				output[i * 7 + 1] += output[i * 7 + 4] * f;
				output[i * 7 + 2] += output[i * 7 + 5] * f;
				output[i * 7 + 6] = time;
			}else
			{
				output[i * 7 + 0] += output[i * 7 + 3] * session->delta_time;
				output[i * 7 + 1] += output[i * 7 + 4] * session->delta_time;
				output[i * 7 + 2] += output[i * 7 + 5] * session->delta_time;
				output[i * 7 + 6] = time;
			}
		}
	}
}

	static CTypeType particle_reemitter_param_types[5] = {C_TYPE_PARTICLE, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *particle_reemitter_param_names[5] = {"Source", "Amount", "Spread", "Speed", "Age"};

void c_execute_component_module_particle_reemitter_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	static uint seed = 0;
	float *output, *particle, *size, *amount, tmp[3], *center, *speed, *age, f, time, last_time, dist;
	uint i,  j, k, count = 1, add, particle_stride, particle_count, amount_stride, amount_count, size_stride, size_count, speed_stride, speed_count, age_stride, age_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[5]]; 
	count = type->size; 
	output = &stack[type->pos];

	particle = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &particle_stride, &particle_count)];
	amount = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &amount_stride, &amount_count)];
	size = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &size_stride, &size_count)];
	speed = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &speed_stride, &speed_count)];
	age = &stack[c_entity_stack_get(entity, c->references[4], C_TYPE_FLOAT, &age_stride, &age_count)];

	for(j = i = 0; i < particle_count; i++)
	{
		f = particle[i * particle_stride + 6];
		if(f > 1.0 && f < 2.0)
		{
			add = (uint)((float)count * amount[(i % amount_count) * amount_stride]);
			for(k = 0; k < add && j < count; j++)
			{
				if(output[j * 7 + 6] > 1.0)
				{
					output[j * 7 + 0] = particle[i * particle_stride + 0];
					output[j * 7 + 1] = particle[i * particle_stride + 1];
					output[j * 7 + 2] = particle[i * particle_stride + 2];
					dist = 10.0;
					while(dist > 1.0)
					{
						tmp[0] = f_randnf(seed++);
						tmp[1] = f_randnf(seed++);
						tmp[2] = f_randnf(seed++);
						dist = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
					}
					f = size[(i % size_count) * size_stride];
					tmp[0] = tmp[0] * f; 
					tmp[1] = tmp[1] * f; 
					tmp[2] = tmp[2] * f;
					f = sqrt(particle[i * particle_stride + 3] * particle[i * particle_stride + 3] + 
								particle[i * particle_stride + 4] * particle[i * particle_stride + 4] + 
								particle[i * particle_stride + 5] * particle[i * particle_stride + 5]) / (1.0 - f);
					tmp[0] += particle[i * particle_stride + 3] / f; 
					tmp[1] += particle[i * particle_stride + 4] / f; 
					tmp[2] += particle[i * particle_stride + 5] / f;
					f = sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]) / speed[(i % speed_count) * speed_stride];
					output[j * 7 + 3] = tmp[0] / f;
					output[j * 7 + 4] = tmp[1] / f;
					output[j * 7 + 5] = tmp[2] / f;
					output[j * 7 + 6] = 0.0;
					k++;
				}
			}
			particle[i * particle_stride + 6] = 2.5;
		}
	}
	
	instance->time / age[(i % age_count) * age_stride] - ((float)i / (float)count);
	for(i = 0; i < count; i++)
	{
		output[i * 7 + 0] += output[i * 7 + 3] * session->delta_time;
		output[i * 7 + 1] += output[i * 7 + 4] * session->delta_time;
		output[i * 7 + 2] += output[i * 7 + 5] * session->delta_time;
		output[i * 7 + 6] += age[(i % age_count) * age_stride] * 30.0 * session->delta_time;
	}
}


/*	static CTypeType particle_globe_param_types[5] = {C_TYPE_PARTICLE, C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *particle_globe_param_names[5] = {"Source", "Position", "Direction", "Radius", "Speed"};
*/
void c_execute_component_module_particle_globe_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	static uint seed = 0;
	float *output, *position, *pos, *direction, *radius, tmp[3], vec[3], *center, *speed, *age, f, f2, time, last_time, *p, *v, delta;
	uint i,  j, k, count = 1, position_stride, position_count, direction_stride, direction_count, radius_stride, radius_count, speed_stride, speed_count, age_stride, age_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	type = &entity->stack_types[c->references[4]]; 
	count = type->size; 
	output = &stack[type->pos];

	position = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_POS, &position_stride, &position_count)];
	direction = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &direction_stride, &direction_count)];
	radius = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &radius_stride, &radius_count)];
	speed = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &speed_stride, &speed_count)];

	delta = session->delta_time / 2.0;
	for(i = 0; i < count; i++)
	{
		p = &position[(i % position_count) * position_stride];
		tmp[0] = output[i * 7 + 0] - p[0];
		tmp[1] = output[i * 7 + 1] - p[1];
		tmp[2] = output[i * 7 + 2] - p[2];
		f = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
		f2 = radius[(i % radius_count) * radius_stride];
		if(f > f2 * f2)
		{
			float dist = 10.0;	
			while(dist > 1.0)
			{
				tmp[0] = f_randnf(seed++);
				tmp[1] = f_randnf(seed++);
				tmp[2] = f_randnf(seed++);
				dist = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
			}
			v = &direction[(i % direction_count) * direction_stride];
			vec[0] = v[0] - p[0];
			vec[1] = v[1] - p[1];
			vec[2] = v[2] - p[2];
			if(vec[0] * tmp[0] + vec[1] * tmp[1] + vec[2] * tmp[2] > 0.0)
			{
				tmp[0] = -tmp[0];
				tmp[1] = -tmp[1];
				tmp[2] = -tmp[2];
			}
			dist = sqrt(dist) / f2;
			output[i * 7 + 0] = p[0] + tmp[0] / dist;
			output[i * 7 + 1] = p[1] + tmp[1] / dist;
			output[i * 7 + 2] = p[2] + tmp[2] / dist;
			f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / speed[(i % speed_count) * speed_stride] / f2;
			output[i * 7 + 3] = vec[0] / f;
			output[i * 7 + 4] = vec[1] / f;
			output[i * 7 + 5] = vec[2] / f;
			output[i * 7 + 6] = 0;
		}
		output[i * 7 + 0] += output[i * 7 + 3] * session->delta_time;
		output[i * 7 + 1] += output[i * 7 + 4] * session->delta_time;
		output[i * 7 + 2] += output[i * 7 + 5] * session->delta_time;
		output[i * 7 + 6] += delta * speed[(i % speed_count) * speed_stride];
	}
}

void c_execute_component_module_particle_globe_display_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *position, *direction, *radius;
	uint count = 1, position_stride, position_count, direction_stride, direction_count, radius_stride, radius_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	type = &entity->stack_types[c->references[4]]; 
	count = type->size; 
	output = &stack[type->pos];

	position = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_POS, &position_stride, &position_count)];
	direction = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &direction_stride, &direction_count)];
	radius = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &radius_stride, &radius_count)];
	if(position_count == 1 && radius_count == 1)
		c_preview_draw_sphere(position[0], position[1], position[2], radius[0]);
	if(position_count == 1 && direction_count == 1)
		c_preview_draw_line(position[0], position[1], position[2], direction[0], direction[1], direction[2]);
}


void c_execute_component_module_particle_gravity_force_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *position, *pos, *target, *strength, *p, *t, f, time, last_time, vec[3];
	uint i, seed, j, k, output_stride, output_count, position_stride, position_count, target_stride, target_count, strength_stride, strength_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;

	output = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &output_stride, &output_count)];

	position = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &position_stride, &position_count)];
	target = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_POS, &target_stride, &target_count)];
	strength = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &strength_stride, &strength_count)];


	if(position_count == 1 && target_count == 1)
	{
		vec[0] = target[0] - position[0];
		vec[1] = target[1] - position[1];
		vec[2] = target[2] - position[2];
		f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / 10.0;
		vec[0] /= f;
		vec[1] /= f;
		vec[2] /= f;
		for(j = 0; j < output_count; j++)
		{
			f = strength[(j % strength_count) * strength_stride];
			output[j * output_stride + 3] -= vec[0] * f * session->delta_time;
			output[j * output_stride + 4] -= vec[1] * f * session->delta_time;
			output[j * output_stride + 5] -= vec[2] * f * session->delta_time;
		}
	}else
	{
		for(j = 0; j < output_count; j++)
		{
			p = &position[(j % position_count) * position_stride];
			t = &target[(j % target_count) * target_stride];
			vec[0] = t[0] - p[0];
			vec[1] = t[1] - p[1];
			vec[2] = t[2] - p[2];
			f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / 10.0;
			vec[0] /= f;
			vec[1] /= f;
			vec[2] /= f;

			f = strength[(j % strength_count) * strength_stride];
			output[j * output_stride + 3] -= vec[0] * f * session->delta_time;
			output[j * output_stride + 4] -= vec[1] * f * session->delta_time;
			output[j * output_stride + 5] -= vec[2] * f * session->delta_time;
		}
	}
}



void c_execute_component_module_particle_drag_force_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *position, *pos, *target, *strength, *speed, *p, *t, f, s, vec[3];
	uint i, seed, j, k, output_stride, output_count, position_stride, position_count, target_stride, target_count, strength_stride, strength_count, speed_stride, speed_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;

	output = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &output_stride, &output_count)];

	position = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &position_stride, &position_count)];
	target = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_POS, &target_stride, &target_count)];
	strength = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &strength_stride, &strength_count)];
	speed = &stack[c_entity_stack_get(entity, c->references[4], C_TYPE_FLOAT, &speed_stride, &speed_count)];


	if(position_count == 1 && target_count == 1)
	{
		vec[0] = target[0] - position[0];
		vec[1] = target[1] - position[1];
		vec[2] = target[2] - position[2];
		f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / 10.0;
		vec[0] /= f;
		vec[1] /= f;
		vec[2] /= f;
		for(j = 0; j < output_count; j++)
		{
			f = strength[(j % strength_count) * strength_stride] * session->delta_time;
			s = speed[(j % speed_count) * speed_stride];
			output[j * output_stride + 3] = output[j * output_stride + 3] + (vec[0] * s - output[j * output_stride + 3]) * f;
			output[j * output_stride + 4] = output[j * output_stride + 4] + (vec[1] * s - output[j * output_stride + 4]) * f;
			output[j * output_stride + 5] = output[j * output_stride + 5] + (vec[2] * s - output[j * output_stride + 5]) * f;
		}
	}else
	{
		for(j = 0; j < output_count; j++)
		{
			p = &position[(j % position_count) * position_stride];
			t = &target[(j % target_count) * target_stride];
			vec[0] = t[0] - p[0];
			vec[1] = t[1] - p[1];
			vec[2] = t[2] - p[2];
			f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / 10.0;
			vec[0] /= f;
			vec[1] /= f;
			vec[2] /= f;

			f = strength[(j % strength_count) * strength_stride] * session->delta_time;
			s = speed[(j % speed_count) * speed_stride];
			output[j * output_stride + 3] = output[j * output_stride + 3] + (vec[0] * s - output[j * output_stride + 3]) * f;
			output[j * output_stride + 4] = output[j * output_stride + 4] + (vec[1] * s - output[j * output_stride + 4]) * f;
			output[j * output_stride + 5] = output[j * output_stride + 5] + (vec[2] * s - output[j * output_stride + 5]) * f;
		}
	}
}


void c_execute_component_module_particle_gravity_well_force_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *position, *pos, *target, *strength, *p, *t, f, time, last_time, vec[3];
	uint i, seed, j, k, count = 1, position_stride, position_count, target_stride, target_count, strength_stride, strength_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;


	position = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_POS, &position_stride, &position_count)];
	target = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &target_stride, &target_count)];
	strength = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &strength_stride, &strength_count)];

	for(i = 0; i < entity->stack_type_count; i++)
	{
		if(entity->stack_types[i].type == C_TYPE_PARTICLE)
		{
			output = &((float *)instance->stack)[entity->stack_types[i].pos];
			if(position_count == 1 && target_count == 1)
			{
				vec[0] = target[0] - position[0];
				vec[1] = target[1] - position[1];
				vec[2] = target[2] - position[2];
				f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / 10.0;
				vec[0] /= f;
				vec[1] /= f;
				vec[2] /= f;
				for(j = 0; j < entity->stack_types[i].size; j++)
				{
					f = strength[(j % strength_count) * strength_stride];
					output[j * 7 + 3] -= vec[0] * f * session->delta_time;
					output[j * 7 + 4] -= vec[1] * f * session->delta_time;
					output[j * 7 + 5] -= vec[2] * f * session->delta_time;
				}
			}else
			{
				for(j = 0; j < entity->stack_types[i].size; j++)
				{
					p = &position[(j % position_count) * position_stride];
					t = &target[(j % target_count) * target_stride];
					vec[0] = t[0] - p[0];
					vec[1] = t[1] - p[1];
					vec[2] = t[2] - p[2];
					f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / 10.0;
					vec[0] /= f;
					vec[1] /= f;
					vec[2] /= f;

					f = strength[(j % strength_count) * strength_stride];
					output[j * 7 + 3] -= vec[0] * f * session->delta_time;
					output[j * 7 + 4] -= vec[1] * f * session->delta_time;
					output[j * 7 + 5] -= vec[2] * f * session->delta_time;
				}
			}
		}
	}
}


void c_execute_component_module_turbulence_force_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	static float rand_array[32];
	static boolean init = FALSE;
	float *output, *position, *scale, *strength, *p, *t, f, f2, time, last_time, vec[3], tmp[3];
	uint i, seed, j, k, l, m, output_stride, output_count, scale_stride, scale_count, strength_stride, strength_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;
	if(!init)
	{
		for(i = 0; i < 32; i++)
			rand_array[i] = f_randnf(i);
	}


	output = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &output_stride, &output_count)];
	scale = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &scale_stride, &scale_count)];
	strength = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &strength_stride, &strength_count)];

	output_count *= output_stride;
	for(j = 0; j < output_count; j += output_stride)
	{
		f = scale[(j % scale_count) * scale_stride] + 0.01;
		f2 = strength[(j % strength_count) * strength_stride] * session->delta_time;
		k = (uint)((output[j + 0] + output[j + 1] * 34 + output[j + 2] * 743) / f); 
		output[j + 3] -= rand_array[k % 32] * f2;
		output[j + 4] -= rand_array[(k + 3) % 32] * f2;
		output[j + 5] -= rand_array[(k + 9) % 32] * f2;
	}
/*	for(j = 0; j < output_count; j++)
	{
		f = scale[(j % scale_count) * scale_stride] + 0.01;
		k = (uint)(output[j * output_stride + 0] / f); 
		k += (uint)(output[j * output_stride + 1] / f) * 34; 
		k += (uint)(output[j * output_stride + 2] / f) * 743; 
		f = strength[(j % strength_count) * strength_stride] * session->delta_time;
		vec[0] = f_randnf(k++) * f;
		vec[1] = f_randnf(k++) * f;
		vec[2] = f_randnf(k) * f;
		output[j * output_stride + 3] -= vec[0];
		output[j * output_stride + 4] -= vec[1];
		output[j * output_stride + 5] -= vec[2];
	}*/

}
/*
	static CTypeType wave_force_param_types[3] = {C_TYPE_PARTICLE, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *wave_force_param_names[3] = {"Particles", "Frequency", "Strength"};
*/

void c_execute_component_module_wave_force_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *position, *frequency, *strength, *p, *t, f, x, y, time, last_time, vec[3], tmp[3], a[3], b[3], cross[3] = {1, 0.5, 0.4};
	uint i, seed, j, k, output_stride, output_count, frequency_stride, frequency_count, strength_stride, strength_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;

	f_normalize3f(cross);

	output = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &output_stride, &output_count)];

	frequency = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &frequency_stride, &frequency_count)];
	strength = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &strength_stride, &strength_count)];

	for(j = 0; j < output_count; j++)
	{
		tmp[0] = output[j * output_stride + 3]; 
		tmp[1] = output[j * output_stride + 4]; 
		tmp[2] = output[j * output_stride + 5];
		f = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
		if(f > 0.00001)
		{
			f = sqrt(f);
			tmp[0] /= f;
			tmp[1] /= f;
			tmp[2] /= f;
			f_cross3f(a, cross, tmp);
			f_cross3f(b, a, tmp);
			f = (output[j * output_stride + 6] + PI * 2.0) / (frequency[(j % frequency_count) * frequency_stride] + 0.01);
			x = sin(f);			
			y = cos(f);
			f = strength[(j % strength_count) * strength_stride] * session->delta_time;
			x *= f;
			y *= f;
			output[j * output_stride + 3] += x * a[0] + y * b[0];
			output[j * output_stride + 4] += x * a[1] + y * b[1];
			output[j * output_stride + 5] += x * a[2] + y * b[2];
		}
	}
}


	/*static CTypeType follow_force_param_types[3] = {C_TYPE_POS, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *follow_force_param_names[3] = {"Goal", "Speed", "Turn"};
	*/
void c_execute_component_module_follow_force_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *goal, *speed, *turn, *p, s, f, time, last_time, vec[3], tmp[3], tmp2[3];
	uint i, seed, j, k, output_stride, output_count, goal_stride, goal_count, speed_stride, speed_count, turn_stride, turn_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;

	output = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &output_stride, &output_count)];

	goal = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &goal_stride, &goal_count)];
	speed = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &speed_stride, &speed_count)];
	turn = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &turn_stride, &turn_count)];

	for(j = 0; j < output_count; j++)
	{
		s = sqrt(output[j * output_stride + 3] * output[j * output_stride + 3] + output[j * output_stride + 4] * output[j * output_stride + 4] + output[j * output_stride + 5] * output[j * output_stride + 5]);
		tmp[0] = output[j * output_stride + 3] / s; 
		tmp[1] = output[j * output_stride + 4] / s; 
		tmp[2] = output[j * output_stride + 5] / s; 
		p = &goal[(j % goal_count) * goal_stride];
		vec[0] = p[0] - output[j * output_stride + 0];
		vec[1] = p[1] - output[j * output_stride + 1];
		vec[2] = p[2] - output[j * output_stride + 2];
		f_normalize3f(vec);
		f = vec[0] * tmp[0] + vec[1] * tmp[1] + vec[2] * tmp[2];
		if(f < 0)
		{
			vec[0] = vec[0] - tmp[0] * f;
			vec[1] = vec[1] - tmp[1] * f;
			vec[2] = vec[2] - tmp[2] * f;
			f_normalize3f(vec);
		}
		f = session->delta_time * turn[(j % turn_count) * turn_stride] * 100.0;
				
		vec[0] = tmp[0] + (vec[0] - tmp[0]) * f; 
		vec[1] = tmp[1] + (vec[1] - tmp[1]) * f; 
		vec[2] = tmp[2] + (vec[2] - tmp[2]) * f; 

		f = s + (speed[(j % speed_count) * speed_stride] - s) * f;
		output[j * output_stride + 3] = vec[0] / f; 
		output[j * output_stride + 4] = vec[1] / f; 
		output[j * output_stride + 5] = vec[2] / f; 
	}

}

/*
void c_execute_component_module_follow_force_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *goal, *speed, *turn, *p, s, f, time, last_time, vec[3], tmp[3], tmp2[3];
	uint i, seed, j, k, output_stride, output_count, goal_stride, goal_count, speed_stride, speed_count, turn_stride, turn_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;

	output = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &output_stride, &output_count)];

	goal = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &goal_stride, &goal_count)];
	speed = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &speed_stride, &speed_count)];
	turn = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &turn_stride, &turn_count)];

	for(j = 0; j < output_count; j++)
	{
		s = sqrt(output[j * output_stride + 3] * output[j * output_stride + 3] + output[j * output_stride + 4] * output[j * output_stride + 4] + output[j * output_stride + 5] * output[j * output_stride + 5]);
		tmp[0] = output[j * output_stride + 3] / s; 
		tmp[1] = output[j * output_stride + 4] / s; 
		tmp[2] = output[j * output_stride + 5] / s; 
		p = &goal[(j % goal_count) * goal_stride];
		vec[0] = p[0] - output[j * output_stride + 0];
		vec[1] = p[1] - output[j * output_stride + 1];
		vec[2] = p[2] - output[j * output_stride + 2];
		f_normalize3f(vec);
		f = vec[0] * tmp[0] + vec[1] * tmp[1] + vec[2] * tmp[2];
		if(f < 0)
		{
			vec[0] = vec[0] - tmp[0] * f;
			vec[1] = vec[1] - tmp[1] * f;
			vec[2] = vec[2] - tmp[2] * f;
			f_normalize3f(vec);
		}
		f = session->delta_time * turn[(j % turn_count) * turn_stride] * 100.0;
				
		vec[0] = tmp[0] + (vec[0] - tmp[0]) * f; 
		vec[1] = tmp[1] + (vec[1] - tmp[1]) * f; 
		vec[2] = tmp[2] + (vec[2] - tmp[2]) * f; 

		f = s + (speed[(j % speed_count) * speed_stride] - s) * f;
		output[j * output_stride + 3] = vec[0] / f; 
		output[j * output_stride + 4] = vec[1] / f; 
		output[j * output_stride + 5] = vec[2] / f; 
	}

}*/

void c_execute_component_module_sepatate_force_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *output2, *strength, *scale, *p, s, s2, f, time, last_time, vec[3], tmp[3], tmp2[3];
	uint i, seed, j, k, output_stride, output_count,  output2_stride, output2_count,  strength_stride, strength_count, scale_stride, scale_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;
	
	output = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &output_stride, &output_count)];
	output2 = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_PARTICLE, &output2_stride, &output2_count)];

	strength = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &strength_stride, &strength_count)];
	scale = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &scale_stride, &scale_count)];

	if(output == output2)
	{
		if(scale_count == 1 && strength_count == 1)
		{
			s = scale[0] + scale[0];
			s2 = strength[0] + strength[0];
			s2 *= session->delta_time * s;

			for(i = 0; i < output_count; i++)
			{
				for(j = 0; j < i; j++)
				{
					tmp[0] = (output[j * output_stride + 0] - output[i * output_stride + 0]) / s; 
					tmp[1] = (output[j * output_stride + 1] - output[i * output_stride + 1]) / s; 
					tmp[2] = (output[j * output_stride + 2] - output[i * output_stride + 2]) / s;
					f = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
					if(f < 3.0 * 3.0)
					{
						f_normalize3f(tmp);
						f = s2 / (1.0 + f) - 1.0 / (1.0 / 9.0);
						output[i * output_stride + 0] -= tmp[0] * f; 
						output[i * output_stride + 1] -= tmp[1] * f; 
						output[i * output_stride + 2] -= tmp[2] * f; 
						output[j * output_stride + 0] -= tmp[0] * f; 
						output[j * output_stride + 1] -= tmp[1] * f; 
						output[j * output_stride + 2] -= tmp[2] * f; 
					}
				}
			}
		}else
		{
			for(i = 0; i < output_count; i++)
			{
				for(j = 0; j < i; j++)
				{
					s = scale[(j % scale_count) * scale_stride] + scale[(i % scale_count) * scale_stride];
					tmp[0] = (output[j * output_stride + 0] - output[i * output_stride + 0]) / s; 
					tmp[1] = (output[j * output_stride + 1] - output[i * output_stride + 1]) / s; 
					tmp[2] = (output[j * output_stride + 2] - output[i * output_stride + 2]) / s;
					f = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
					if(f < 3.0 * 3.0)
					{
						f_normalize3f(tmp);
						f = 1.0 / (1.0 + f) - 1.0 / (1.0 / 9.0);
						f *= strength[(j % strength_count) * strength_stride] + strength[(i % strength_count) * strength_stride];
						f *= session->delta_time * s;
						output[i * output_stride + 0] -= tmp[0] * f; 
						output[i * output_stride + 1] -= tmp[1] * f; 
						output[i * output_stride + 2] -= tmp[2] * f; 
						output[j * output_stride + 0] -= tmp[0] * f; 
						output[j * output_stride + 1] -= tmp[1] * f; 
						output[j * output_stride + 2] -= tmp[2] * f; 
					}
				}
			}
		}
	}else
	{
		if(scale_count == 1 && strength_count == 1)
		{
			s = scale[0] + scale[0];
			s2 = strength[0] + strength[0];
			s2 *= session->delta_time * s;
			for(i = 0; i < output2_count; i++)
			{
				for(j = 0; j < output_count; j++)
				{
					tmp[0] = (output[j * output_stride + 0] - output2[i * output2_stride + 0]) / s; 
					tmp[1] = (output[j * output_stride + 1] - output2[i * output2_stride + 1]) / s; 
					tmp[2] = (output[j * output_stride + 2] - output2[i * output2_stride + 2]) / s;
					f = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
					if(f < 3.0 * 3.0)
					{
						f_normalize3f(tmp);
						f = s2 / (1.0 + f) - 1.0 / (1.0 / 9.0);
						output2[i * output2_stride + 0] -= tmp[0] * f; 
						output2[i * output2_stride + 1] -= tmp[1] * f; 
						output2[i * output2_stride + 2] -= tmp[2] * f; 
						output[j * output_stride + 0] -= tmp[0] * f; 
						output[j * output_stride + 1] -= tmp[1] * f; 
						output[j * output_stride + 2] -= tmp[2] * f; 
					}
				}
			}
		}else
		{
			for(i = 0; i < output2_count; i++)
			{
				for(j = 0; j < output_count; j++)
				{
					s = scale[(j % scale_count) * scale_stride] + scale[(i % scale_count) * scale_stride];
					tmp[0] = (output[j * output_stride + 0] - output[i * output_stride + 0]) / s; 
					tmp[1] = (output[j * output_stride + 1] - output[i * output_stride + 1]) / s; 
					tmp[2] = (output[j * output_stride + 2] - output[i * output_stride + 2]) / s;
					f = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
					if(f < 3.0 * 3.0)
					{
						f_normalize3f(tmp);
						f = 1.0 / (1.0 + f) - 1.0 / (1.0 / 9.0);
						f *= strength[(j % strength_count) * strength_stride] + strength[(i % strength_count) * strength_stride];
						f *= session->delta_time * s;
						output[i * output_stride + 0] -= tmp[0] * f; 
						output[i * output_stride + 1] -= tmp[1] * f; 
						output[i * output_stride + 2] -= tmp[2] * f; 
						output[j * output_stride + 0] -= tmp[0] * f; 
						output[j * output_stride + 1] -= tmp[1] * f; 
						output[j * output_stride + 2] -= tmp[2] * f; 
					}
				}
			}
		}
	}
}

/*
	static CTypeType rotate_force_param_types[5] = {C_TYPE_PARTICLE, C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *rotate_force_param_names[5] = {"Particles", "Axis A", "Axis B", "Speed", "Scale"};
	
*/

void c_execute_component_module_rotate_force_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *axis_a, *axis_b, *speed, *scale, *p, s, s2, f, f2, f3, time, last_time, vec[3], tmp[3], tmp2[3], tmp3[3];
	uint i, seed, j, k, output_stride, output_count,  axis_a_stride, axis_a_count, axis_b_stride, axis_b_count, speed_stride, speed_count, scale_stride, scale_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;
	
	output = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &output_stride, &output_count)];
	axis_a = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &axis_a_stride, &axis_a_count)];
	axis_b = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_POS, &axis_b_stride, &axis_b_count)];
	speed = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &speed_stride, &speed_count)];
	scale = &stack[c_entity_stack_get(entity, c->references[4], C_TYPE_FLOAT, &scale_stride, &scale_count)];
	
	vec[0] = axis_a[0] - axis_b[0];
	vec[1] = axis_a[1] - axis_b[1];
	vec[2] = axis_a[2] - axis_b[2];
	f_normalize3f(vec);
	for(i = 0; i < output_count; i++)
	{
		tmp[0] = (output[i * output_stride + 0] - axis_a[0]); 
		tmp[1] = (output[i * output_stride + 1] - axis_a[1]); 
		tmp[2] = (output[i * output_stride + 2] - axis_a[2]);
		f = vec[0] * tmp[0] + vec[1] * tmp[1] + vec[2] * tmp[2];
		tmp[0] -= vec[0] * f; 
		tmp[1] -= vec[1] * f; 
		tmp[2] -= vec[2] * f; 
		f_cross3f(tmp2, tmp, vec);
		f = speed[(i % speed_count) * speed_stride];
		f2 = scale[(i % scale_count) * scale_stride];
/*		output[i * output_stride + 3] = output[i * output_stride + 3] + ((tmp2[0] - tmp[0] * f2) * f) * session->delta_time;
		output[i * output_stride + 4] = output[i * output_stride + 4] + ((tmp2[1] - tmp[1] * f2) * f) * session->delta_time;
		output[i * output_stride + 5] = output[i * output_stride + 5] + ((tmp2[2] - tmp[2] * f2) * f) * session->delta_time;*/
		output[i * output_stride + 3] = output[i * output_stride + 3] * (1.0 - session->delta_time) + ((tmp2[0] - tmp[0] * f2) * f) * session->delta_time;
		output[i * output_stride + 4] = output[i * output_stride + 4] * (1.0 - session->delta_time) + ((tmp2[1] - tmp[1] * f2) * f) * session->delta_time;
		output[i * output_stride + 5] = output[i * output_stride + 5] * (1.0 - session->delta_time) + ((tmp2[2] - tmp[2] * f2) * f) * session->delta_time;
	}
/*	for(i = 0; i < output_count; i++)
	{
		tmp[0] = (output[i * output_stride + 0] - axis_a[0]); 
		tmp[1] = (output[i * output_stride + 1] - axis_a[1]); 
		tmp[2] = (output[i * output_stride + 2] - axis_a[2]);
		f = tmp[0] * tmp[0] + vec[1] * tmp[1] + vec[2] * tmp[2];
		tmp[0] -= vec[0] * f; 
		tmp[1] -= vec[1] * f; 
		tmp[2] -= vec[2] * f; 
		f_cross3f(tmp2, tmp, vec);
		f = speed[(i % speed_count) * speed_stride];
		output[i * output_stride + 0] += tmp2[0] * f * session->delta_time;
		output[i * output_stride + 1] += tmp2[1] * f * session->delta_time;
		output[i * output_stride + 2] += tmp2[2] * f * session->delta_time;

	}*/
	
}


void c_execute_component_module_rotate_force_display_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *axis_a, *axis_b;
	uint  axis_a_stride, axis_a_count, axis_b_stride, axis_b_count;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;	
	axis_a = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &axis_a_stride, &axis_a_count)];
	axis_b = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_POS, &axis_b_stride, &axis_b_count)];
	if(axis_a_count == 1 && axis_b_count == 1)
		c_preview_draw_axis(axis_a[0], axis_a[1], axis_a[2], axis_b[0], axis_b[1], axis_b[2]);
}

boolean c_execute_collision_func(float *pos, float *next, float *output_hit_pos, float *output_normal)
{
	if(next[1] < 0.0)
	{
		output_hit_pos[0] = next[0];
		output_hit_pos[1] = next[1];
		output_hit_pos[2] = next[2];
		output_normal[0] = 0;
		output_normal[1] = 1;
		output_normal[2] = 0;
		return TRUE;
	}else
		return FALSE;
} 

void c_execute_component_module_particle_collision_die_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *particle, *scale, *strength, *p, *t, f, time, last_time, vec[3], tmp[3];
	uint i, j, k, seed, particle_stride, particle_count, strength_stride, strength_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;

	particle = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &particle_stride, &particle_count)];

	for(i = 0; i < particle_count; i++)
	{
		vec[0] = particle[i * particle_stride] + particle[i * particle_stride + 3] * session->delta_time;
		vec[1] = particle[i * particle_stride + 1] + particle[i * particle_stride + 4] * session->delta_time;
		vec[2] = particle[i * particle_stride + 2] + particle[i * particle_stride + 5] * session->delta_time;
		if(c_execute_collision_func(&particle[i * particle_stride], vec, &particle[i * particle_stride], tmp))
		{
			f = particle[i * particle_stride + 3] * tmp[0] +
				particle[i * particle_stride + 4] * tmp[1] +
				particle[i * particle_stride + 5] * tmp[2];
			if(f < 0)
			{
				f *= 2.0;
				particle[i * particle_stride + 3] -= f * tmp[0];
				particle[i * particle_stride + 4] -= f * tmp[1];
				particle[i * particle_stride + 5] -= f * tmp[2];
			}
			particle[i * particle_stride + 6] = 1.0001;
		}
	}
}


void c_execute_component_module_particle_collision_bounce_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *particle, *scale, *strength, *p, *t, f, time, last_time, vec[3], tmp[3];
	uint i, j, k, seed, particle_stride, particle_count, strength_stride, strength_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;
	
	particle = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &particle_stride, &particle_count)];
	strength = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &strength_stride, &strength_count)];

	for(i = 0; i < particle_count; i++)
	{
		vec[0] = particle[i * particle_stride] + particle[i * particle_stride + 3] * session->delta_time;
		vec[1] = particle[i * particle_stride + 1] + particle[i * particle_stride + 4] * session->delta_time;
		vec[2] = particle[i * particle_stride + 2] + particle[i * particle_stride + 5] * session->delta_time;
		if(c_execute_collision_func(&particle[i * particle_stride], vec, &particle[i * particle_stride], tmp))
		{
			f = particle[i * particle_stride + 3] * tmp[0] +
				particle[i * particle_stride + 4] * tmp[1] +
				particle[i * particle_stride + 5] * tmp[2];
			if(f < 0)
			{
				f *= 2.0;
				particle[i * particle_stride + 3] -= f * tmp[0];
				particle[i * particle_stride + 4] -= f * tmp[1];
				particle[i * particle_stride + 5] -= f * tmp[2];
				f = strength[(i % strength_count) * strength_stride];
				particle[i * particle_stride + 3] *= f;
				particle[i * particle_stride + 4] *= f;
				particle[i * particle_stride + 5] *= f;
			}
		}
	}
}


void c_execute_component_module_particle_vector_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *particle, *scale, *strength, *p, *t, f, time, last_time, vec[3], tmp[3];
	uint i, j, k, seed, count = 1, particle_stride, particle_count, strength_stride, strength_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;

	type = &entity->stack_types[c->references[2]]; 
	count = type->size; 
	output = &stack[type->pos];

	particle = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &particle_stride, &particle_count)];
	strength = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &strength_stride, &strength_count)];

	for(i = 0; i < count; i++)
	{
		f = strength[(i % strength_count) * strength_stride];
		p = &particle[i * particle_stride];
		output[i * 3 + 0] = p[0] + p[3] * f; 
		output[i * 3 + 1] = p[1] + p[4] * f; 
		output[i * 3 + 2] = p[2] + p[5] * f; 
	}
}


void c_execute_component_module_particle_matrix_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *particle, *scale, *position, *target, *p, *t, *m, f, time, last_time, vec[3], vec2[3];
	uint i, j, k, seed, count = 1, particle_stride, particle_count, scale_stride, scale_count, position_stride, position_count, target_stride, target_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	seed = instance->seed;

	type = &entity->stack_types[c->references[4]]; 
	count = type->size; 
	output = &stack[type->pos];

	particle = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &particle_stride, &particle_count)];
	scale = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &scale_stride, &scale_count)];
	position = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_POS, &position_stride, &position_count)];
	target = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_POS, &target_stride, &target_count)];

	for(i = 0; i < count; i++)
	{
		p = &position[(i % position_count) * position_stride];
		t = &target[(i % target_count) * target_stride];
		vec[0] = t[0] - p[0]; 
		vec[1] = t[1] - p[1]; 
		vec[2] = t[2] - p[2];
		p = &particle[(i % particle_count) * particle_stride];
		m = &output[i * 16];
		f_matrixxyf(m, NULL, &p[3], vec);
		f = scale[(i % scale_count) * scale_stride];
		m[0] *= f;
		m[1] *= f;
		m[2] *= f;
		m[4] *= f;
		m[5] *= f;
		m[6] *= f;
		m[8] *= f;
		m[9] *= f;
		m[10] *= f;
		m[12] = p[0];
		m[13] = p[1];
		m[14] = p[2];
	}
}


void c_execute_component_module_particle_normal_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	static uint seed = 0;
	float *output, *particle, tmp[3], *p, *center, f, time;
	uint i,  j, k, count = 1, particle_stride, particle_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	type = &entity->stack_types[c->references[1]]; 
	count = type->size; 
	output = &stack[type->pos];

	particle = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &particle_stride, &particle_count)];

	for(i = 0; i < count; i++)
	{
		f = 0;
		center = &particle[(i % particle_count) * particle_stride];
		
		tmp[0] = tmp[1] = tmp[2] = 0;
		time = center[6];
		for(j = 1; j < 5; j++)
		{
			p = &particle[((i + particle_count + j) % particle_count) * particle_stride];
			if(p[6] > time)
				break;
			tmp[0] += p[0];
			tmp[1] += p[1];
			tmp[2] += p[2];
			f++;
		}
		for(j = 1; j < 5; j++)
		{
			p = &particle[((i + particle_count - j) % particle_count) * particle_stride];
			if(p[6] < time)
				break;
			tmp[0] += p[0];
			tmp[1] += p[1];
			tmp[2] += p[2];
			f++;
		}
		output[i * 3 + 0] = center[0] - tmp[0] / f;
		output[i * 3 + 1] = center[1] - tmp[1] / f;
		output[i * 3 + 2] = center[2] - tmp[2] / f;
	}
}




/*
Target

Read out
*/

void c_execute_component_module_particles_init(CSession *session)
{
	static CTypeType particle_emitter_param_types[6] = {C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *particle_emitter_param_names[6] = {"Pos", "Target", "Spread", "Speed", "Age", "Emmit Time"};
	
	static CTypeType particle_reemitter_param_types[5] = {C_TYPE_PARTICLE, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *particle_reemitter_param_names[5] = {"Source", "Amount", "Spread", "Speed", "Age"};

	static CTypeType particle_globe_param_types[4] = {C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *particle_globe_param_names[4] = {"Position", "Direction", "Radius", "Speed"};

	static CTypeType gravity_force_param_types[4] = {C_TYPE_PARTICLE, C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT};
	static char *gravity_force_param_names[4] = {"Particles", "Pos", "Target", "Strength"};
	static CTypeType drag_force_param_types[5] = {C_TYPE_PARTICLE, C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *drag_force_param_names[5] = {"Particles", "Pos", "Target", "Strength", "Speed"};
	static CTypeType turbulence_force_param_types[3] = {C_TYPE_PARTICLE, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *turbulence_force_param_names[3] = {"Particles", "Scale", "Strength"};

	static CTypeType wave_force_param_types[3] = {C_TYPE_PARTICLE, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *wave_force_param_names[3] = {"Particles", "Frequency", "Strength"};

	static CTypeType follow_force_param_types[4] = {C_TYPE_PARTICLE, C_TYPE_POS, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *follow_force_param_names[4] = {"Particles", "Goal", "Speed", "Turn"};
	static CTypeType sepatate_force_param_types[4] = {C_TYPE_PARTICLE, C_TYPE_PARTICLE, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *sepatate_force_param_names[4] = {"Particles", "Particles", "Strength", "Scale"};

	static CTypeType rotate_force_param_types[5] = {C_TYPE_PARTICLE, C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *rotate_force_param_names[5] = {"Particles", "Axis A", "Axis B", "Speed", "Scale"};
	

	static CTypeType particle_collision_bounce_param_types[2] = {C_TYPE_PARTICLE, C_TYPE_FLOAT};
	static char *particle_collision_bounce_param_names[2] = {"Collision", "Bounce"};

	static CTypeType particle_vector_param_types[2] = {C_TYPE_PARTICLE, C_TYPE_FLOAT};
	static char *particle_vector_param_names[2] = {"Particles", "Distance"};

	static CTypeType particle_matrix_param_types[4] = {C_TYPE_PARTICLE, C_TYPE_FLOAT, C_TYPE_POS, C_TYPE_POS};
	static char *particle_matrix_param_names[4] = {"Particles", "Size", "Pos", "Target"};
	confuse_component_add(session, "particle emitter", particle_emitter_param_types, particle_emitter_param_names, 5, C_TYPE_PARTICLE, -1, c_execute_component_module_particle_emitter_func, C_CC_OBJECT, -1, NULL);	
	confuse_component_add(session, "particle explosion", particle_emitter_param_types, particle_emitter_param_names, 6, C_TYPE_PARTICLE, -1, c_execute_component_module_particle_burst_func, C_CC_OBJECT, -1, NULL);	
	confuse_component_add(session, "particle re-emit", particle_reemitter_param_types, particle_reemitter_param_names, 5, C_TYPE_PARTICLE, -1, c_execute_component_module_particle_reemitter_func, C_CC_OBJECT, -1, NULL);	
	confuse_component_add(session, "particle globe", particle_globe_param_types, particle_globe_param_names, 4, C_TYPE_PARTICLE, -1, c_execute_component_module_particle_globe_func, C_CC_OBJECT, -1, c_execute_component_module_particle_globe_display_func);	



	confuse_component_add(session, "Gravity force", gravity_force_param_types, gravity_force_param_names, 4, -1, -1, c_execute_component_module_particle_gravity_force_func, C_CC_FORCE, -1, NULL);	
	confuse_component_add(session, "Drag force", drag_force_param_types, drag_force_param_names, 5, -1, -1, c_execute_component_module_particle_drag_force_func, C_CC_FORCE, -1, NULL);	

	confuse_component_add(session, "Turbulence force", turbulence_force_param_types, turbulence_force_param_names, 3, -1, -1, c_execute_component_module_turbulence_force_func, C_CC_FORCE, -1, NULL);	
	confuse_component_add(session, "Wave force", wave_force_param_types, wave_force_param_names, 3, -1, -1, c_execute_component_module_wave_force_func, C_CC_FORCE, -1, NULL);	
	confuse_component_add(session, "Follow force", follow_force_param_types, follow_force_param_names, 4, -1, -1, c_execute_component_module_follow_force_func, C_CC_FORCE, -1, NULL);	

	confuse_component_add(session, "Sepatate force", sepatate_force_param_types, sepatate_force_param_names, 4, -1, -1, c_execute_component_module_sepatate_force_func, C_CC_FORCE, -1, NULL);	

	confuse_component_add(session, "Rotate force", rotate_force_param_types, rotate_force_param_names, 5, -1, -1, c_execute_component_module_rotate_force_func, C_CC_FORCE, -1, c_execute_component_module_rotate_force_display_func);	


	confuse_component_add(session, "Colission Bounce", particle_collision_bounce_param_types, particle_collision_bounce_param_names, 2, -1, -1, c_execute_component_module_particle_collision_bounce_func, C_CC_FORCE, -1, NULL);	
	confuse_component_add(session, "Colission Die", particle_collision_bounce_param_types, particle_collision_bounce_param_names, 1, -1, -1, c_execute_component_module_particle_collision_die_func, C_CC_FORCE, -1, NULL);	

	confuse_component_add(session, "Particle Vector", particle_vector_param_types, particle_vector_param_names, 2, C_TYPE_POS, -1, c_execute_component_module_particle_vector_func, C_CC_PROCESS, -1, NULL);	
	confuse_component_add(session, "Particle Matrix", particle_matrix_param_types, particle_matrix_param_names, 4, C_TYPE_MATRIX, -1, c_execute_component_module_particle_matrix_func, C_CC_PROCESS, -1, NULL);	

	confuse_component_add(session, "Particle Normal", particle_matrix_param_types, particle_matrix_param_names, 1, C_TYPE_POS, -1, c_execute_component_module_particle_normal_func, C_CC_PROCESS, -1, NULL);	

	
}

